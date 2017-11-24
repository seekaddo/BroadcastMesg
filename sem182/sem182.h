/*******************************************************************************
*                                                                              *
*     Author:      G. Leber                                                    *
*                  Technische Universitaet Wien                                *
*                  Institut fuer Technische Informatik E182/1                  *
*                  Treitlstrasse 3                                             *
*                  1040 Wien                                                   *
*                  Tel.: (0222) 58801 / 8176                                   *
*                  E-Mail: guenther@vmars.tuwien.ac.at                         *
*     File:        sem182.h						       *
*     Version:     2.2							       *
*     Date:        6/27/94						       *
*                                                                              *
*******************************************************************************/

#ifndef SEM182_H
#define SEM182_H

#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif

#include <sys/types.h>

/* sem182.c */
int semrm P_((int semid));
int seminit P_((key_t key, int semperm, int initval));
int semgrab P_((key_t key));
int V P_((int semid));
int P P_((int semid));

#undef P_

#endif /* SEM182_H */
