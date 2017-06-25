/*!
 * @file sender.c
 *
 * Beispiel 3
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "sharedlib.h"


/*! programName   a readonly global pointer to hold the program name */
static const char *programName = NULL;


/*!
 * @brief      Using the Shared memory s_write to store the read
 *             stdin characters in the shared memory buffer with
 *             the help of additional interface to initialise the shared
 *             memory segment (interface shmseg_easy_init and args_parser)
 *
 *             On success with end of file the resources are not cleaned up
 *             only the shared memory address is detached for receiver to clean up
 *             when done.
 *             On Failure resources are cleaned up and exit failure
 *
 *@param       argc    The number of arguments.
 *@param       argv     Array of char pointer to the passed
 *                       arguments (command line).
 *@result EXIT_SUCCESS on success and EXIT_FAILURE on failure
 * */
int main(int argc, char *argv[]) {


    shmseg shm;

    size_t shmsize;
    programName = argv[0];


    if ((args_parser(argc, argv, &shmsize)) != 0) {
        return EXIT_FAILURE;
    }

    if (shmseg_easy_init(&shmsize, 0, &shm) == -1) {
        (void) shmseg_easy_clean(&shm);
        return EXIT_FAILURE;
    }


    int data;

    do {
        data = fgetc(stdin);

        if (ferror(stdin) == -1) {
            fprintf(stderr, "%s -> fgetc: %s \n", programName, strerror(errno));
            (void) shmseg_easy_clean(&shm);
            return EXIT_FAILURE;
        }

        if (shm.s_write(&data) == -1) {
            return EXIT_FAILURE;
        }

    } while (data != EOF);


    shm.detach(&shm);

    return 0;

}
