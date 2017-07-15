/*!
 * @file     sharedlib.h
 * @brief    Using Semaphores and Shared memory
 *           to simulate Server/Client message(sender/receiver)
 * @version     1.0 June 15 2017
 * @author      Dennis Addo ic16b026
 * @author      Robert Niedermaier ic16b089
 * @see         sharedlib.h#
 * @see        Project Web site https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel3.html
*/

#ifndef BROADCASTMESG_SRSHMEM_H
#define BROADCASTMESG_SRSHMEM_H




/*!\include
 * -------------------------------------------------------------- includes --
 */

#include <stdio.h>


/*!
 * --------------------------------------------------------------- defines --
 */


/*!
 * @typedef     shmseg
*/
typedef struct __attribute__ ((__packed__)) shmseg{
    int  shmid;                         /**< the shared memory ID */
    int  semid[2];                      /**< semaphores for read[1] and write[0] */
    int  (*s_read)(void);               /**< a function pointer to read from the sh segmt buffer */
    int  (*s_write)(int *c);            /**< a function pointer to write to the sh segmt buffer */
    void (*detach)(struct shmseg *shm); /**< a function pointer to detach the shared memory from the global shmbff */
}shmseg;




extern int args_parser(int argc, char *argv[], size_t *shmsizeptr);

extern int shmseg_easy_init(const size_t *shmsize, const int mode, shmseg *shmsg);

extern int shmseg_easy_clean(shmseg *shm);





#endif //BROADCASTMESG_SRSHMEM_H
