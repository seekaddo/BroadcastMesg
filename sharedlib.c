#define _GNU_SOURCE
//
// Created by Addo Dennis on 15/05/2017.
//


/*!
 * @file sharedlib.c
 *
 * Beispiel 3
 *@brief Using Semaphores and Shared memory to simulate Server/Client message(sender/receiver)
 *
 * @author Dennis Addo ic16b026
 * @author Robert Niedermaier ic16b089
 * @details More information about the project can be found here
 * @see URL:https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel3.html
 *
 * @date 15/05/2017
 *
 * @version 1.0
 *
 * @todo All implentations must be contained in one method structure unless these functions
 * @     are required by other programs/methods
 * @todo ALL errors must be the GNU C EXIT_SUCCESS and FAILURE macros
 * @todo perror and sterror is used to display error informations, functions and error-types
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <sem182.h>
#include <ctype.h>
#include "sharedlib.h"





/**
 * --------------------------------------------------------------- globals --
 */


/*! SPERM   This the permission for the shared memory 0700 or 0740*/
#define SPERM (S_IRUSR | S_IWUSR | S_IXUSR )

/*! SHM_KEY   This the permission for the shared memory */
#define SHM_KEY ((getuid() * 100) + 0)

/*! SEM_KEY   This the key for the semaphores */
#define SEM_KEY ((getuid() * 100) + 1)

/*! programmName   global pointer to hold the program name */
static const char *programmName = NULL;




//static shmseg *shmsg_ptr = NULL;
/*! shmssize_g   global value for the shared memory buffer size*/
static size_t shmssize_g = 0;


/*!
 * @brief       convert string to long
 * @param       ops     The passed number as a string
 * @result      The converted value in long otherwise it exit with failure 1.
 */
static size_t process_nums(const char *ops);


/*!
 * @brief       process the command options arguments.
 *              This function take a argc and argv from
 *              the main method and process the passed arguments
 * @param       argc    The number of arguments.
 * @param       argv     Array of char pointer to the passed
 *                       arguments.
 *@param        shmsizeptr  pointer to the size of the shared memory
 *                       the passed requested size to be copied here
 *
 * @result      An int ( 0 on success and 1 on failure).
 */
int args_parser(int argc, char *argv[], size_t *shmsizeptr) {
    
    
    int opt;
    long s = 0;
    programmName = argv[0];
    
    
    if (argc < 2) {
        fprintf(stderr, "%s ringbuffer size must be specified\n"
                "Usage: %s [-m] <ringbuffer size>\n", programmName, programmName);
        
        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }
    
    
    while ((opt = getopt(argc, argv, ":m:")) != -1) {
        
        switch (opt) {
            case 'm' :
                //fprintf(stderr,"first passed value is %s\n",optarg);
                s = process_nums(optarg);
                
                break;
            default:
                fprintf(stderr, "Usage: %s [-m] <ringbuffer size>\n", programmName);
                exit(EXIT_FAILURE);
                
        }
        
        
    }
    
    if (optind < argc) {
        fprintf(stderr, "%s: Non-option argument: %s\n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, optarg, programmName);
        *shmsizeptr = 0;
        exit(EXIT_FAILURE);
        
    }
    

#if defined(__unix__) && defined(__i386__)
    struct shminfo buf;
    if(shmctl(0, IPC_INFO, (struct shmid_ds *) &buf) < 0){
        fprintf(stderr,"shmctl: %s %s \n",programmName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    
    //fprintf(stderr,"size passed after processing is %lld ca-> %lu\n",s,((buf.shmall / 1024) * (1024 *4)));
    
    if ((unsigned long) s >= (buf.shmall * 4) ) {
        fprintf(stderr, "%s: specified shared memory size too big\n"
                "Usage: %s [-m] <ringbuffer size> \n",programmName,programmName);
        
        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }
    
#endif

    *shmsizeptr = (size_t) s;
    shmssize_g = *shmsizeptr;
    
    return 0;
    
    
}


/*!
 * @brief       convert string to long
 *              This function takes a number string and convert it
 *              to long integer value. It is a simple wrapper for the strtol function
 * @param       ops     The passed number as a string
 * @result      The convert value in long otherwise it exit with failure 1.
 */
static size_t process_nums(const char *ops) {
    
#if 1
    
    char *end;
    long val = strtol(ops, &end, 10);
    
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)) {
        
        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops, programmName);
        exit(EXIT_FAILURE);
    }
    
    if (val <= 0) {
        
        fprintf(stderr, "%s: Invalid specified shared memory size : ringbuffer size <= %zu \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, SIZE_MAX, programmName);
        
        exit(EXIT_FAILURE);
    } else if (*end != '\0') {
        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops, programmName);
        exit(EXIT_FAILURE);
    }
    
    return (size_t) val;
#endif
    
#if 0
    long long v;
    long long l;
    int n = sscanf(ops, "%lld%lln", &v, &l);
    
    
    if (l <= 0) {
        fprintf(stderr, "%s: specified shared memory size too small :ringbuffer size <= %zu \n"
                "Usage: %s [-m] <ringbuffer elements>\n",programmName , SIZE_MAX,programmName);
        
        return EXIT_FAILURE;
    }else if ( l >= SIZE_MAX) {
        
        fprintf(stderr, "%s: specified shared memory size too big: ringbuffer size <= %zu \n"
                "Usage: %s [-m] <ringbuffer elements>\n",programmName,SIZE_MAX,programmName);
        
        return EXIT_FAILURE;
    }
    
    
    if (l != len) {
        
        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops,programmName);
        exit(EXIT_FAILURE);
        
    } else if (n == EOF) {
        
        fprintf(stderr, "%sCannot pass the value <%s> to strtol \n"
                "sscanf:%s\n "
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops, strerror(errno),programmName);
        
        exit(EXIT_FAILURE);
    }
    return (size_t) v;
#endif
    
    
}


/*!
 * @brief       initialize / create all the required
 *              resources needed - shared memory/ semaphores
 *              This function takes a specided size of the shared memory
 *              the mode (read only / read and write mode) and a pointer to
 *              the shared memory segment.
 * @param       shmsize    The weight of the chicken.
 * @param       mode     The sauce for cooking. Could be a
 *                        NULL pointer.
 *@param        shmsg
 *
 * @result      An int ( 0 on success and -1 on failure).
 */
int shmseg_easy_init(const size_t *shmsize, const int mode, shmseg *shmsg) {
    
    shmsg->semid[0] = -1; //write access semaphore
    shmsg->semid[1] = -1; //read access semaphore
    shmsg->shmid = -1;
    shmsg->shmbff = (void *) -1;
    
    
    
    for (int i = 0; i < 2; ++i) {
        
        if ((shmsg->semid[i] = seminit(SEM_KEY + i, 0700, (i % 2 == 0 ? *shmsize :0) )) == -1) {
            
            if (errno == EEXIST) {
                if ((shmsg->semid[i] = semgrab(SEM_KEY + i)) == -1) {
                    fprintf(stderr, "semgrab: %s %s \n", programmName, strerror(errno));
                    return -1;
                }
            } else {
                fprintf(stderr, "seminit: %s %s \n", programmName, "initialization error");
                return -1;
            }
            
        }
    }
    
    
    if ((shmsg->shmid = shmget(SHM_KEY, *shmsize * sizeof(int), IPC_CREAT | SPERM)) == -1) {
        fprintf(stderr, "shmget: %s %s\n", programmName, strerror(errno));
        //shmseg_easy_clean(shmsg);
        return -1;
        
    }
    
    
    if ((shmsg->shmbff = shmat(shmsg->shmid, NULL, mode)) == (void *) -1) {
        fprintf(stderr, "shmat: %s %s\n", programmName, "problem with shared memory attachment");
        //shmseg_easy_clean(shmsg);
        return -1;
    }
    
    
    return 0;
    
}


/*!
 * @brief       write the byte specified by c to the shared memory buffer.
 *              This function takes int pointer interpreted as
 *              a char that is to be written to the specified shared
 *              memory segment's shared  buffer
 * @param       c    The char to be written.
 * @param       shmsgptr    The segment to put the char.
 *
 * @result      An int ( 0 on success and -1 on failure).
 */
int shmseg_easy_write(int *c, shmseg *shmsgptr) {
    
    static size_t i = 0;
    int l;
    
    
    do {
        l = P(shmsgptr->semid[0]);
    } while (l == -1 && errno == EINTR);
    
    if (l == -1) {
        fprintf(stderr, " P() : %s %s \n", programmName, strerror(errno));
        shmseg_easy_close(shmsgptr);
        return -1;
    }
    
    shmsgptr->shmbff[(i++) % shmssize_g] = *c;
    
    l = 0;
    
    do {
        
        l = V(shmsgptr->semid[1]);
    } while (l == -1 && errno == EINTR);
    
    
    if (l == -1) {
        fprintf(stderr, " V() : %s %s \n", programmName, strerror(errno));
        shmseg_easy_close(shmsgptr);
        return -1;
    }
    
    
    return 0;
    
}


/*!
 * @brief       read a char/integer value from the shared memory buffer.
 *              This function takes a pointer to the shared memory
 *              segment and read from it. It checks if it has read access P()
 *              if yes then it reads from the buffer and release the segment to
 *              another person. V() and returns the read value.
 * @param       shmsg    The shared memory segment to read from.
 *
 * @result      An int ( the read value on success and -1 on failure).
 */
int shmseg_easy_read(shmseg *shmsg) {
    
    int data = 0, l;
    static size_t indx = 0;
    
    
    do {
        errno = 0;
        l = P(shmsg->semid[1]);
        
    } while (l == -1 && errno == EINTR);
    
    
    if (l == -1) {
        fprintf(stderr, " P() : %s %s \n", programmName, strerror(errno));
        shmseg_easy_close(shmsg);
        return -1;
    }
    
    
    data = shmsg->shmbff[(indx++) % shmssize_g];
    
    l = 0;
    
    do {
        l = V(shmsg->semid[0]);
        
    } while (l == -1 && errno == EINTR);
    
    if (l == -1) {
        
        fprintf(stderr, " V() : %s %s \n", programmName, strerror(errno));
        shmseg_easy_close(shmsg);
        return -1;
    }
    
    return data;
    
    
}


/*!
 * @brief       delete all the semaphores and (if detached by others) shared memory.
 *              This function takes a pointer to the shared memory segment to
 *              delete. try to delete the semaphores and detach the shared memory if is
 *              not already done and then delete the shared memory buffer.
 * @param       shmsg_ptr    The shared memory segment to delete.
 *
 * @result      An int ( 0 on success and 1 on failure).
 */
int shmseg_easy_close(shmseg *shmsg_ptr) {
    
    if (shmsg_ptr->semid[0] != -1 && shmsg_ptr->semid[1] != -1) {
        for (int i = 0; i < 2; ++i) {
            if (semrm(shmsg_ptr->semid[i]) == -1) {
                fprintf(stderr, "semrm: %s %s\n", programmName, strerror(errno));
                return EXIT_FAILURE;
            }
        }
        
        shmsg_ptr->semid[0] = -1;
        shmsg_ptr->semid[1] = -1;
    }
    
    if (shmsg_ptr->shmbff != (void *) -1) {
        errno = 0;
        if (shmdt(shmsg_ptr->shmbff) == -1) {
            fprintf(stderr, "shmdt: %s %s \n", programmName, strerror(errno));
            return EXIT_FAILURE;
        }
    }
    
    if (shmsg_ptr->shmbff != (void *) -1) {
        if (shmctl(shmsg_ptr->shmid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "%s ->  shmctl: %s \n", programmName, strerror(errno));
            return EXIT_FAILURE;
        }
    }
    
    shmsg_ptr->shmbff = (void *) -1;
    shmsg_ptr->shmid = -1;
    shmsg_ptr->semid[0] = -1;
    shmsg_ptr->semid[1] = -1;
    
    return EXIT_SUCCESS;
    
}



