#ifndef I_msem182_h
#define I_msem182_h

/* SCCSId:	"@(#)	msem182.h	2.3	6/27/94	hp";
 *
 * Purpose: 	Convenience functions for Semaphores
 *
 * Motivation:	The semop function is hard to use and provides
 *              functionality which is seldom used or even annoying
 *		(like the fact that 0 isn't a nop). These functions
 *		should provide a cleaner interface.
 *
 * Modification history:
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

int mseminit( key_t key, int perm, int nsems, ...);


/*** function msemgrab
 *
 * Purpose:	grab existing semaphore field
 * In:		key	Key of SF.
 *		nsems	number of semaphores in the SF
 * Returns:	semid or -1 in case of error
 */
int msemgrab (key_t key, int nsems);


/*** function msemrm
 * 
 * Purpose: remove semaphore field
 * In:	    semid	id of SF to remove
 */
int msemrm(int semid);


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
int mV(int semid, int nsems, ...);


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
 *			a V operation should be performed.
 * Returns: -1 in case of error, else 0.
 */
int mP(int semid, int nsems, ...);

#endif /* I_msem182_h */
