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
 * @     are required by other programs/methods
 * @todo ALL errors must be the GNU C EXIT_SUCCESS and FAILURE macros
 * @todo perror and sterror is used to display error informations, functions and error-types
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include "sharedlib.h"


/*! programName   global pointer to hold the program name */
static const char *programName = NULL;


/*!
 *@brief Receiver:
 *The receiver process waits for the availability of messages (i.e., characters) in the ring buffer.
 * It then reads these messages and prints them to stdout. The receiver acknowledges the reception of
 * the messages to the sender creating unoccupied memeory elements in the ring buffer again this way.
 * Once the sender process reads EOF from stdin, it signals this fact via the ring buffer to
 * the receiving process and afterwards terminates. - Once the receiving process obtains the EOF signalling
 * via the shared memory, this process terminates as well.
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


    if (EXIT_FAILURE == (args_parser(argc, argv, &size))) {
        return EXIT_FAILURE;
    }

    if (shmseg_easy_init(&size, SHM_RDONLY, &shm) == -1) {
        shmseg_easy_clean(&shm);
        return EXIT_FAILURE;
    }

    int c;

    do {
        if (ferror(stdin)) {
            fprintf(stderr, "%s -> fgetc: %s \n", programName, strerror(errno));
            shmseg_easy_clean(&shm);
            return EXIT_FAILURE;
        }

        c = shm.s_read();

        if (c != EOF) {
            if (fputc(c, stdout) == EOF) {
                fprintf(stderr, " fputc : %s %s \n", programName, strerror(errno));
                shmseg_easy_clean(&shm);
                return -1;
            }
        }

    } while (c != EOF);

    if (fflush(stdout) == EOF) {
        fprintf(stderr, " fflush : %s %s \n", programName, strerror(errno));
        shmseg_easy_clean(&shm);
        return -1;
    }


    return shmseg_easy_clean(&shm);


}
