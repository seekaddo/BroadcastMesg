
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
 *      are required by other programs/methods
 *
 *
 */

#define _GNU_SOURCE
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




/*! SPERM   This the permission for the shared memory 0700*/
#define SPERM (S_IRUSR | S_IWUSR | S_IXUSR )

/*! SHM_KEY   This the permission for the shared memory */
#define SHM_KEY ((getuid() * 100) + 0)

/*! SEM_KEY   This the key for the semaphores */
#define SEM_KEY ((getuid() * 200) + 1)

static const char *programmName = NULL; /*! programmName   global pointer to hold the program name */
static shmseg *shmsgpr = NULL;          /*! SPERM   This the shared memory segment*/
static int *shmbff = (void *) -1;       /*! SPERM   This the shared memory buffer*/
static size_t shmssize_g = 0;           /*! shmssize_g   global value for the shared memory buffer size*/

static int shmseg_easy_write(int *c);
static int shmseg_easy_read(void);
static void detach(shmseg *shm);
static size_t process_nums(const char *ops);


/*!
 * @brief       process the command options arguments.
 *              This function take a argc and argv from
 *              the main method and process the passed
 *              (command line) arguments
 * @param       argc    The number of arguments.
 * @param       argv     Array of char pointer to the passed
 *                       arguments (command line).
 *@param        shmsizeptr  pointer to the size of the shared memory
 *                       the passed requested size to be copied here
 *
 * @result      An int ( 0 on success and 1 on failure).
 */
int args_parser(int argc, char *argv[], size_t *shmsizeptr) {


    int opt;
    size_t s = 0;
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


    /*This will work on a 32bit linux machine
     * 4096 page --> 1024 * (4 -> 32bit system)
     * or (buf.shmall * sizeof(int)) mostly is 4096/1024 --> 4
     * it is ok for this project. if platform is 64 then the
     * logic for calculating the shm max in (kbytes) will be different
     * ipcs -lm for more details on a 64bit platform
     * */

#if defined(__unix__) && defined(__i386__)

    struct shminfo buf;
    if(shmctl(0, IPC_INFO, (struct shmid_ds *) &buf) < 0){
        fprintf(stderr,"shmctl: %s %s \n",programmName, strerror(errno));
        exit(EXIT_FAILURE);
    }


    if ( s >= (buf.shmall * 4) ) {
        fprintf(stderr, "%s: specified shared memory size too big\n"
                "Usage: %s [-m] <ringbuffer size> \n",programmName,programmName);

        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }

#else

    /*(SIZE_MAX / sizeof(int) +1) == (SIZE_MAX / 4(page size / 1024 (1 kbyte)))
     * the shmall and the ipc calling process shared memory limits is not the same (page size segment and
     * the other in kbytes) (SIZE_MAX / sizeof(int)) see ipcs -lm for more details.
     * on a 64 bit system will this be different. Since are not doing 64bit compatible
     * shared memory is enough for this on "annuminas"
     *
     * on BSD see man ipcs or ipcs -l.
     * on a 32 bit system is SIZE_MAX = 4294967295 like UINT_MAX which after
     * converting to kbytes witht he corresponding page size is less than 1bit than
     * the ipc calling process shm limits ipcs -lm 1073741823 != 1073741824
     * */
    if ( s >= (SIZE_MAX / sizeof(int)) +1) {
        fprintf(stderr, "%s: specified shared memory size too big\n"
                "Usage: %s [-m] <ringbuffer size> \n", programmName, programmName);

        *shmsizeptr = 0;
        return EXIT_FAILURE;
    }
#endif

    *shmsizeptr = s;
    shmssize_g = *shmsizeptr;

    return 0;


}


/*!
 * @brief       convert string to long
 *              This function takes a number string and convert it
 *              to long integer value. It is a simple wrapper for the strtol function
 * @param       ops     The passed number as a string
 * @result      The converted value in size_t otherwise it exit with failure 1.
 */
static size_t process_nums(const char *ops) {


    char *end;
    long val = strtol(ops, &end, 10);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)) {

        fprintf(stderr,"%s: -> %s\n",programmName,strerror(errno));
        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops, programmName);
        exit(EXIT_FAILURE);
    }

    if (val <= 0) {

        fprintf(stderr, "%s: Invalid specified shared memory size : ringbuffer size <= %zu \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, SIZE_MAX / sizeof(int), programmName);

        exit(EXIT_FAILURE);
    } else if (*end != '\0') {
        fprintf(stderr, "%s: Cannot pass the value <%s> to strtol \n"
                "Usage: %s [-m] <ringbuffer elements>\n", programmName, ops, programmName);
        exit(EXIT_FAILURE);
    }

    return (size_t) val;
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
 * @result      0 on success and -1 on failure.
 */
int shmseg_easy_init(const size_t *shmsize, const int mode, shmseg *shmsg) {

    shmsg->semid[0] = -1; //write access semaphore
    shmsg->semid[1] = -1; //read access semaphore
    shmsg->shmid = -1;
    shmbff = (void *) -1;


    shmsg->s_read = shmseg_easy_read;
    shmsg->s_write = shmseg_easy_write;
    shmsg->detach = detach;


    for (int i = 0; i < 2; ++i) {

        if ((shmsg->semid[i] = seminit(SEM_KEY + i, 0700, (i % 2 == 0 ? *shmsize : 0))) == -1) {

            if (errno == EEXIST) {
                if ((shmsg->semid[i] = semgrab(SEM_KEY + i)) == -1) {
                    fprintf(stderr, "semgrab: %s %s \n", programmName, strerror(errno));
                    return -1;
                }
            } else {
                fprintf(stderr, "seminit: %s %s \n", programmName, strerror(errno));
                return -1;
            }

        }
    }

    if ( (shmsg->shmid = shmget(SHM_KEY, *shmsize * sizeof(int), IPC_CREAT | SPERM) ) == -1) {
        fprintf(stderr, "shmget: %s %s\n", programmName, strerror(errno));
        /*shmseg_easy_clean(shmsg);*/
        return -1;

    }


    if ( (shmbff = shmat(shmsg->shmid, NULL, mode)) == (void *) -1) {
        fprintf(stderr, "shmat: %s %s\n", programmName, strerror(errno));
        return -1;
    }

    shmsgpr = shmsg;

    return 0;
}


/*!
 * @brief       write the byte specified by c to the shared memory buffer(shmbff).
 *              This function takes int pointer interpreted as
 *              a char that is to be written to the specified shared
 *              memory segment's(shmsgpr) shared  buffer. Using P() and V()
 *              for checking for write acccess and releasing to another process
 *              when done.
 * @param       c    The value to be written.
 *
 * @result      0 on success and -1 on failure.
 */
static int shmseg_easy_write(int *c) {

    if (shmbff == (void *) -1) {
        fprintf(stderr, "Oops: %s ->: Please initialize the shared memory"
                " segment before you can write to buffer\n", programmName);
        return -1;
    }

    static size_t i = 0;
    int l;


    do {
        l = P(shmsgpr->semid[0]);
    } while (l == -1 && errno == EINTR);

    if (l == -1) {
        fprintf(stderr, " P() : %s %s \n", programmName, strerror(errno));
        (void) shmseg_easy_clean(shmsgpr);
        return -1;
    }

    shmbff[(i++) % shmssize_g] = *c;


    do {

        l = V(shmsgpr->semid[1]);
    } while (l == -1 && errno == EINTR);


    if (l == -1) {
        fprintf(stderr, " V() : %s %s \n", programmName, strerror(errno));
        (void) shmseg_easy_clean(shmsgpr);
        return -1;
    }


    return 0;

}


/*!
 * @brief       read a char/integer value from the shared memory buffer (shmbff).
 *              This function takes a pointer to the shared memory
 *              segment and read from it. It checks if it has read access P()
 *              if yes then it decrease the value and reads from the buffer
 *              and release the segment to another process waiting with V().
 *              Then returns the read value.
 *
 * @result      An int ( the read value on success and -1 on failure(or exit(EXIT_FAILURE))).
 */
static int shmseg_easy_read(void) {

    if (shmbff == (void *) -1) {
        fprintf(stderr, "Oops : %s ->: Please initialize the shared memory"
                " segment before you can read from buffer\n", programmName);
        exit(EXIT_FAILURE);
    }

    int data, l;
    static size_t indx = 0;


    do {
        errno = 0;
        l = P(shmsgpr->semid[1]);

    } while (l == -1 && errno == EINTR);


    if (l == -1) {
        fprintf(stderr, " P() : %s %s \n", programmName, strerror(errno));
        (void) shmseg_easy_clean(shmsgpr);
        return -1;
    }


    data = shmbff[(indx++) % shmssize_g];


    do {
        l = V(shmsgpr->semid[0]);

    } while (l == -1 && errno == EINTR);

    if (l == -1) {

        fprintf(stderr, " V() : %s %s \n", programmName, strerror(errno));
        (void) shmseg_easy_clean(shmsgpr);
        return -1;
    }

    return data;


}


/*!
 * @brief       delete all the semaphores and (if detached by others) shared memory.
 *              This function takes a pointer to the shared memory segment to
 *              delete. try to delete the semaphores and detach the shared memory if is
 *              not already done and then delete the shared memory buffer.
 * @param       shm    The shared memory segment to delete.
 *
 * @result      An int ( 0 on success and 1 on failure).
 */
int shmseg_easy_clean(shmseg *shm) {

    if (shm->semid[0] != -1 && shm->semid[1] != -1) {
        for (int i = 0; i < 2; ++i) {
            if (semrm(shm->semid[i]) == -1) {
                fprintf(stderr, "semrm: %s %s\n", programmName, strerror(errno));
                return EXIT_FAILURE;
            }
        }

    }

    shm->detach(shm);


    if (shmbff != (void *) -1) {
        if (shmctl(shm->shmid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "%s ->  shmctl: %s \n", programmName, strerror(errno));
            return EXIT_FAILURE;
        }
    }

    shmbff = (void *) -1;
    shm->shmid = -1;
    shm->semid[0] = -1;
    shm->semid[1] = -1;

    shm->detach = NULL;
    shm->s_read = NULL;
    shm->s_read = NULL;

    return EXIT_SUCCESS;

}

/**
 * @brief       detaches the shared memory segment
 *              allocated at the shmbff's address
 *@param    void
 *@result   nothing.
 */
static void detach(shmseg *shm) {
    if (shmbff != (void *) -1) {
        //errno = 0;
        if (shmdt(shmbff) == -1) {
            fprintf(stderr, "shmdt: %s %s \n", programmName, strerror(errno));
            shmseg_easy_clean(shm);
            exit(EXIT_FAILURE);
        }
    }
}

