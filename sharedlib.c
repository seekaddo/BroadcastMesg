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


/*! @define SPERM   This the permission for the shared memory 0700 or 0740*/
#define SPERM (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP)

/*! @define SHM_KEY   This the permission for the shared memory */
#define SHM_KEY ((getuid() * 100) + 0)

/*! @define SEM_KEY   This the key for the semaphores */
#define SEM_KEY ((getuid() * 100) + 1)

/*! @var programmName   global pointer to hold the program name */
static const char *programmName = NULL;







//static shmseg *shmsg_ptr = NULL;
/*! @var shmssize_g   global value for the shared memory buffer size*/
static size_t shmssize_g = 0;


/*!
 * @function    shmseg_easy_clean
 * @brief       delete the semaphores and detach the shared memory
 * @param       shmsgptr    A pointer to the shared memory segment
 * @result      An int ( 0 on success and 1 on failure).
*/
static int shmseg_easy_clean(shmseg *shmsgptr);

/*!
 * @function    process_nums
 * @brief       convert string to long
 * @param       ops     The passed number as a string
 * @param       size    The size of the passed string
 * @result      The converted value in long otherwise it exit with failure 1.
*/
static long process_nums(const char *ops, size_t size);


/*!
 * @function    args_parser
 * @brief       process the command options arguments.
 *
 * @discussion  This function take a argc and argv from
 *              the main method and process the passed arguments
 * @param       argc    The number of arguments.
 * @param       argv     Array of char pointer to the passed
 *                       arguments.
 *@param        shmsize  pointer to the size of the shared memory
 *                       the passed requested size to be copied here
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
int args_parser(int argc, char *argv[], size_t *shmsizeptr) {


    int opt;
    long long s = 0;
    programmName = argv[0];


    if (argc < 2) {
        fprintf(stderr, "%1$s ringbuffer size must be specified\n"
                "Usage: %1$s [-m] <ringbuffer size>\n", programmName);

        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }


    while ((opt = getopt(argc, argv, ":m:")) != -1) {

        switch (opt) {
            case 'm' :
                s = process_nums(optarg, strlen(optarg));

                break;
            default:
                fprintf(stderr, "Usage: %s [-m] <ringbuffer size>\n", programmName);
                exit(EXIT_FAILURE);

        }


    }

    if (optind < argc) {
        fprintf(stderr, "%2$s: Non-option argument: %1$s\n"
                "Usage: %2$s [-m] <ringbuffer elements>\n", optarg, programmName);
        *shmsizeptr = 0;
        exit(EXIT_FAILURE);

    }


#if defined(__unix__)
    struct shminfo buf;
    if(shmctl(0, IPC_INFO, (struct shmid_ds *) &buf) < 0){
        fprintf(stderr,"shmctl: %s %s \n",programmName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((unsigned long long) s > buf.shmmax) {
        fprintf(stderr, "%2$s: specified shared memory size too big\n"
                "ringbuffer size <= %1$lu \n",buf.shmmax,programmName);

        *shmsizeptr = 0;
        return EXIT_FAILURE;
    } else if (s < 1) {
        fprintf(stderr, "%2$s: specified shared memory size too small\n"
                "ringbuffer size <= %1$lu \n",buf.shmmax,programmName);


        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }

#elif __APPLE__

    if ((unsigned long long) s > (SIZE_MAX * 2)) {
        fprintf(stderr, "%2$s: specified shared memory size too big \n"
                "ringbuffer size <= %1$llu \n", (SIZE_MAX * 2), programmName);
        *shmsizeptr = 0;
        return EXIT_FAILURE;
    } else if (s < 1) {
        fprintf(stderr, "%2$s: specified shared memory size too small \n"
                "ringbuffer size <= %1$llu \n", (SIZE_MAX * 2), programmName);

        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }
#endif

    *shmsizeptr = (size_t) s;
    shmssize_g = *shmsizeptr;

    return 0;


}


/*!
 * @function    process_nums
 * @brief       convert string to long
 *
 * @discussion  This function takes a number string and convert it
 *              to long integer value. It is a simple wrapper for the strtol function
 * @param       ops     The passed number as a string
 * @param       len     The size of the passed string
 * @result      The convert value in long otherwise it exit with failure 1.
*/
static long process_nums(const char *ops, size_t len) {

    long v;
    size_t l;
    int n = sscanf(ops, "%ld%ln", &v, &l);

    if (l != len) {

        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n", programmName, ops);
        exit(EXIT_FAILURE);

    } else if (n == EOF) {

        fprintf(stderr, "%sCannot pass the value <%s> to strtol \n"
                "sscanf:%s\n", programmName, ops, strerror(errno));
        
        exit(EXIT_FAILURE);
    }

    return v;

}


/*!
 * @function    shmseg_easy_init
 * @brief       initialize / create all the required
 *              resources needed - shared memory/ semaphores
 *
 * @discussion  This function takes a specided size of the shared memory
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
    shmsg->shmbff = (int *) -1;


    if (*shmsize < 1) {
        fprintf(stderr, "%s: specified shared memory size too small \n", programmName);
        return -1;
    }


    for (int i = 0; i < 2; ++i) {

        if ((shmsg->semid[i] = seminit(SEM_KEY + i, 0700, (i % 2 != 0 ? 0 : *shmsize))) == -1) {

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


    if ((shmsg->shmid = shmget(SHM_KEY, *shmsize, IPC_CREAT | SPERM)) == -1) {
        fprintf(stderr, "shmget: %s %s\n", programmName, strerror(errno));
        shmseg_easy_clean(shmsg);
        return -1;

    }


    if ((shmsg->shmbff = shmat(shmsg->shmid, NULL, mode)) == (int *) -1) {
        fprintf(stderr, "shmat: %s %s\n", programmName, "problem with shared memory attachment");
        shmseg_easy_clean(shmsg);
        return -1;
    }


    return 0;

}


/*!
 * @function    shmseg_easy_write
 * @brief       write the byte specified by c to the shared memory buffer.
 *
 * @discussion  This function takes int pointer interpreted as
 *              a char that is to be written to the specified shared
 *              memory segment's shared  buffer
 * @param       c    The char to be written.
 * @param       shmsgptr    The segment to put the char.
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
int shmseg_easy_write(int *c, shmseg *shmsgptr) {


}


/*!
 * @function    shmseg_easy_read
 * @brief       read a char / integer value from the shared memory buffer.
 *
 * @discussion  This function takes a pointer to the shared memory
 *              segment and read from it. It checks if it has read access P()
 *              if yes then it reads from the buffer and release the segment to
 *              another person. V() and returns the read value.
 * @param       shmsgptr    The shared memory segment to read from.
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


    if (V(shmsg->semid[0]) == -1) {
        fprintf(stderr, " V() : %s %s \n", programmName, strerror(errno));
        shmseg_easy_close(shmsg);
        return -1;
    }

    return data;


}


/*!
 * @function    shmseg_easy_clean
 * @brief       delete the semaphores and detach the shared memory
 *
 * @param       shmsg_ptr    A pointer to the shared memory segment
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
static int shmseg_easy_clean(shmseg *shmsg_ptr) {

    if (shmsg_ptr->semid[0] != -1) {
        for (int i = 0; i < 2; ++i) {
            if (semrm(shmsg_ptr->semid[i]) == -1) {
                fprintf(stderr, "semrm: %s %s\n", programmName, strerror(errno));
                return EXIT_FAILURE;
            }
        }

        shmsg_ptr->semid[0] = -1;
        shmsg_ptr->semid[1] = -1;
    }

    if (shmsg_ptr->shmbff != (int *) -1) {
        errno = 0;
        if (shmdt(shmsg_ptr->shmbff) == -1) {
            fprintf(stderr, "shmdt: %s %s \n", programmName, strerror(errno));
            return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;

}


/*!
 * @function    shmseg_easy_close
 * @brief       delete all the semaphores and (if detached by others) shared memory.
 *
 * @discussion  This function takes a pointer to the shared memory segment to
 *              delete. try to delete the semaphores and detach the shared memory if is
 *              not already done and then delete the shared memory buffer.
 * @param       shmsg_ptr    The shared memory segment to delete.
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
int shmseg_easy_close(shmseg *shmsg_ptr) {

    if (shmseg_easy_clean(shmsg_ptr) == EXIT_FAILURE) {
        fprintf(stderr, "shmseg_easy_clean: %s\n", "Problem cleaning up");
    }

    errno = 0;
    if (shmctl(shmsg_ptr->shmid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "shmctl: %s %s \n", programmName, strerror(errno));
        return EXIT_FAILURE;
    }

    shmsg_ptr->shmbff = (int *) -1;
    shmsg_ptr->shmid = -1;

    return EXIT_SUCCESS;

}



