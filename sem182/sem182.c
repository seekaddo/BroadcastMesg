/*******************************************************************************
*                                                                              *
*     Author:      G. Leber                                                    *
*                  H. Kantz                                                    *
*                  Technische Universitaet Wien                                *
*                  Institut fuer Technische Informatik E182/1                  *
*                  Treitlstrasse 3                                             *
*                  1040 Wien                                                   *
*                  Tel.: (0222) 58801 / 8176                                   *
*                  E-Mail: guenther@vmars.tuwien.ac.at                         *
*     Update:      R. Kirner (use of semctl with enum semun)                   *
*     File:        sem182.c						                               *
*     Version:     2.3							                               *
*     Date:        3/5/02						                               *
*                                                                              *
*******************************************************************************/

#ifndef lint
#define sem182_c_sccsID "@(#)sem182.c	2.2	6/27/94"
#endif /* lint */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#if __STDC__
#	include <stdlib.h>
#endif


#define SEM_NR 1


#if __STDC__
int	semrm(int semid)
#else
int	semrm(semid) 

int semid;

#endif

{
 return semctl(semid, SEM_NR, IPC_RMID, 0);
}


#if __STDC__
int	seminit(key_t key, int semperm, int initval)
#else
int	seminit(key, semperm, initval)

key_t key;
int semperm, initval;

#endif

{
#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
     union semun {
         int val;                    /* value for SETVAL */
         struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
         unsigned short int *array;  /* array for GETALL, SETALL */
     };
#endif

 int semid;
 union semun semarg;

 semarg.val = initval;
 if ( (semid = semget (key, SEM_NR, IPC_CREAT | IPC_EXCL | (semperm & 0666)))
									   < 0)
	return(-1);
	/* Initialize Semaphore */
 if (semctl (semid, 0, SETVAL, semarg) < 0) {
	int	semctlerror = errno;	/* save error condition */

	(void) semrm(semid);
	errno = semctlerror;	/* restore error condition for user */
	return(-1);
 }
 return(semid);
}


#if __STDC__
int	semgrab(key_t key)
#else
int	semgrab(key)

key_t key;

#endif

{
 return semget (key, SEM_NR, 0);
}



/* Dijkstra' s V(s) [signal] */

#if __STDC__
int	V(int semid)
#else
int	V(semid)

int semid;

#endif

{  
  struct sembuf semp[1];

  semp[0].sem_num = 0;
  semp[0].sem_op = 1;
  semp[0].sem_flg = 0;
  return(semop(semid, &semp[0], 1));
}


/* Dijkstra' s P(s) [wait] */

#if __STDC__
int	P(int semid)
#else
int	P(semid)

int semid;

#endif

{
  struct sembuf semp[1];

  semp[0].sem_num = 0;
  semp[0].sem_op  = -1;
  semp[0].sem_flg = 0;
  return(semop(semid, &semp[0], 1));
}

