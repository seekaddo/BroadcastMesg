//
// Created by Addo Dennis on 15/05/2017.
//

/*!
 * @file      sharedlib.h
 * @brief    All shared memory functions.
 *           Using Semaphores and Shared memory
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
 *              The shared memory segment
 *shmid      :shared memory id
 *semid      :array of semaphores 0-> write 1->read
*/
typedef struct __attribute__ ((__packed__)) shmseg{
    int  shmid;
    int  semid[2];
    int  (*s_read)(void);
    int  (*s_write)(int *c);
    void (*detach)(void);
}shmseg;







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
extern int args_parser(int argc, char *argv[], size_t *shmsizeptr);


/*!
 * @brief       initialize / create all the required
 *              resources needed - shared memory/ semaphores
 * @param       shmsize  The weight of the chicken.
 * @param       mode     The sauce for cooking. Could be a
 *                        NULL pointer.
 *@param        shmsg    The shared memory segment
 *
 * @result      An int ( 0 on success and -1 on failure).
*/
extern int shmseg_easy_init(const size_t *shmsize, const int mode, shmseg *shmsg);


/*!
 * @brief       delete all the semaphores and (if detached by others) shared memory.
 * @param       shmsg_ptr    The shared memory segment to delete.
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
extern int shmseg_easy_clean(shmseg *shmsgptr);





#endif //BROADCASTMESG_SRSHMEM_H
