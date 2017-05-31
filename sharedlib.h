//
// Created by Addo Dennis on 15/05/2017.
//

/*!
 * @header      sharedlib.h
 * @abstract    All shared memory functions.
 * @discussion  Using Semaphores and Shared memory
 *              to simulate Server/Client message(sender/receiver)
 * @version     1.0 June 15 2017
 * @author      Dennis Addo ic16b026
 * @author      Robert Niedermaier ic16b089
 * @see         sharedlib.h#
 * @link        Project Web site https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel3.html
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
 * @field    shmid     shared memory id
 * @field    semid  array of semaphores 0-> write 1->read
 * @field    shmbff   shared memory ringbuffer
*/
typedef struct shmmg{
    int shmid;
    int semid[2];
    int *shmbff;
}shmseg;







/*!\fn
 * ------------------------------------------------------------- functions --
 * */

/*!
 * @function    args_parser
 * @abstract    process the command options arguments.
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
extern int args_parser(int argc, char *argv[], size_t *shmsize);


/*!
 * @function    shmseg_easy_init
 * @abstract    initialize / create all the required
 *              resources needed - shared memory/ semaphores
 * @param       shmsize    The weight of the chicken.
 * @param       mode     The sauce for cooking. Could be a
 *                        NULL pointer.
 *@param        shmsg
 *
 * @result      An int ( 0 on success and -1 on failure).
*/
extern int shmseg_easy_init(const size_t *shmsize, const int mode, shmseg *shmsg);


/*!
 * @function    shmseg_easy_write
 * @abstract    write the byte specified by c to the shared memory buffer.
 * @param       c    The char to be written.
 * @param       shmsgptr    The segment to put the char.
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
extern int shmseg_easy_write(int *c , shmseg *shmsgptr);


/*!
 * @function    shmseg_easy_read
 * @abstract    read a char / integer value from the shared memory buffer.

 * @param       shmsgptr    The shared memory segment to read from.
 *
 * @result      An int ( the read value on success and -1 on failure).
*/
extern int shmseg_easy_read(shmseg *shmsgptr);
//extern int shmseg_easy_clean(shmseg *shmsgptr);

/*!
 * @function    shmseg_easy_close
 * @abstract    delete all the semaphores and (if detached by others) shared memory.
 * @param       shmsg_ptr    The shared memory segment to delete.
 *
 * @result      An int ( 0 on success and 1 on failure).
*/
extern int shmseg_easy_close(shmseg *shmsgptr);




#endif //BROADCASTMESG_SRSHMEM_H
