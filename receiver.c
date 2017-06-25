/*!
 * @file receiver.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include "sharedlib.h"


/*! programName   global pointer to hold the program name */
// ### FB BP: It is used in exactly one function - main() - below. No reason to make it
//            a global variable.
static const char *programName = NULL;


/*!
 *@brief        Using the Shared memory s_read from the shared memory buffer with
 *             the help of additional interface to initialise the shared
 *             memory segment (interface shmseg_easy_init and args_parser)
 *
 *
 *@param       argc    The number of arguments.
 *@param       argv     Array of char pointer to the passed
 *                       arguments.
 *@result EXIT_SUCCESS on success and EXIT_FAILURE on failure
 * */
int main(int argc, char *argv[]) {


    shmseg shm;
    size_t size;
    programName = argv[0];


    if ((args_parser(argc, argv, &size)) != 0) {
        return EXIT_FAILURE;
    }

    if (shmseg_easy_init(&size, SHM_RDONLY, &shm) == -1) {
        (void) shmseg_easy_clean(&shm);
        return EXIT_FAILURE;
    }

    int c;

    do {

        c = shm.s_read();

        if (c != EOF) {
            if (fputc(c, stdout) == EOF) {
                fprintf(stderr, " fputc : %s %s \n", programName, strerror(errno));
                (void) shmseg_easy_clean(&shm);
                return EXIT_FAILURE;
            }
        }

    } while (c != EOF);

    if (fflush(stdout) == EOF) {
        fprintf(stderr, " fflush : %s %s \n", programName, strerror(errno));
        (void) shmseg_easy_clean(&shm);
        return EXIT_FAILURE;
    }


    return shmseg_easy_clean(&shm);

}
