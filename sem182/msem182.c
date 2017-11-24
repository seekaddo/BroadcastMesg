char msem182_c_sccs_id [] = 
	"@(#)	msem182.c	2.5	2002/03/05	rk";
/*
 * Purpose: 	Convenience functions for Semaphores
 *
 * Motivation:	The semop function is hard to use and provides
 *              functionality which is seldom used or even annoying
 *		(like the fact that 0 isn't a nop). These functions
 *		should provide a cleaner interface.
 *
 * Modification history:
 *  2.5 Use of semctl with enum semun
 *	2.2	Changed order of parameters in mseminit.
 *	2.1	Changed the semantics of mP and mV. The arguments now
 *		specify the index of the semaphore to operate on, 
 *		not the operation.
 *	1.1	got the version from the solution to Ex. 2.11 
 *		(which was derived from an out-of-date version of 
 *		sem182.c which had been heavily hacked by gusti) and
 *		adapted it to the STBS and the newest version of
 *		sem182.c.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <msem182.h>

/*** function mseminit
 * 
 * Purpose: Create and initialize a semaphore field.
 *	    The SF must not exist yet.
 * In:      key Key of semaphore field
 *          nsems   Number of semaphores the field shall contain
 *          perm    permission bits for ugo
 *	    ...	    nsems initial values for semaphores
 * Returns: Semaphore descriptor or -1 in case of error.
 */

int mseminit( key_t key, int perm, int nsems, ...)
{
#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
     union semun {
         int val;                    /* value for SETVAL */
         struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
         unsigned short int *array;  /* array for GETALL, SETALL */
     };
#endif

    int     semid;
    union semun semarg;
    va_list ap;
    int     i;

    if (
        (semid = semget (key, nsems, IPC_CREAT | IPC_EXCL | (perm & 0666)))

        < 0
       )
    {
        return -1;
    }

    /* Initialize Semaphore */
    va_start (ap, nsems);
    for (i = 0; i < nsems; i ++) 
    {
        semarg.val = va_arg (ap, int);
        if (semctl (semid, i, SETVAL, semarg) < 0) 
        {
           int save_errno = errno;
           (void) msemrm(semid);
           errno = save_errno;
           return -1;
        }
    }
    va_end (ap);
    
    return semid;
}


/*** function msemgrab
 *
 * Purpose:	grab existing semaphore field
 * In:		key	Key of SF.
 *		nsems	number of semaphores in the SF
 * Returns:	semid or -1 in case of error
 */
int msemgrab (key_t key, int nsems)
{
	return semget (key, nsems, 0);
}


/*** function msemrm
 * 
 * Purpose: remove semaphore field
 * In:	    semid	id of SF to remove
 */
int msemrm(int semid) 
{
    return semctl (semid, 0, IPC_RMID, 0);
}


/*** function mV
 *
 * Purpose: Dijkstra's V for a whole semaphore field.
 * In:	    semid	Id of SF
 *	    nsems	Number of Semaphores in the field.
 *			(I am not sure if I should check this. 
 *			It may help catch some errors, but too many
 *			are caught by the OS, and a smaller number 
 *			might be handy if you want only a work on a
 *			subset).
 *	    ...		nsems values indicating the semaphores on which
 *			a V operation should be performed.
 * Returns: -1 in case of error, else 0.
 */
int mV(int semid, int nsems, ...)
{  
    struct sembuf   *semp;
    int             i;
    va_list         ap;

    if ((semp = calloc (nsems, sizeof (struct sembuf))) == NULL)
    {
        return -1;
    }

    va_start (ap, nsems);
    for (i = 0; i < nsems; i ++) 
    {
        semp[i].sem_num = va_arg (ap, int);
        semp[i].sem_op = 1;
    }
    va_end (ap);

    if (semop(semid, semp, nsems) < 0)
    {
        free (semp);
        return -1;
    }
    free (semp);
    return 0;
}


/*** function mP
 *
 * Purpose: Dijkstra's P for a whole semaphore field.
 * In:	    semid	Id of SF
 *	    nsems	Number of Semaphores in the field.
 *			(I am not sure if I should check this. 
 *			It may help catch some errors, but too many
 *			are caught by the OS, and a smaller number 
 *			might be handy if you want only a work on a
 *			subset).
 *	    ...		nsems values indicating the semaphores on which
 *			a P operation should be performed.
 * Returns: -1 in case of error, else 0.
 */
int mP(int semid, int nsems, ...)
{  
    struct sembuf   *semp;
    int             i;
    va_list         ap;

    if ((semp = calloc (nsems, sizeof (struct sembuf))) == NULL)
    {
        return -1;
    }

    va_start (ap, nsems);
    for (i = 0; i < nsems; i ++) 
    {
        semp[i].sem_num = va_arg (ap, int);
        semp[i].sem_op = -1;
    }
    va_end (ap);

    if (semop(semid, semp, nsems) < 0)
    {
        free (semp);
        return -1;
    }
    free (semp);
    return 0;
}
