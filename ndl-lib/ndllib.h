/****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************
 *
 * This file is part of ndl-tools
 *
 * This package is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * Ndl-tools is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MBSE BBS; see the file COPYING.  If not, write to the Free
 * Software Foundation, 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *****************************************************************************/

#ifndef _NLDLIB_H
#define	_NDLLIB_H


#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../config.h"


#define	LEVEL_ERROR	0
#define	LEVEL_TOP	1
#define	LEVEL_ZONE	2
#define	LEVEL_REGION	3
#define	LEVEL_HOST	4
#define	LEVEL_HUB	5
#define	LEVEL_NODE	6
#define	LEVEL_HOLD	7
#define	LEVEL_DOWN	8
#define	LEVEL_PRIVATE	9
#define	LEVEL_POINT	10


char *Fgets(char *, int, FILE *);
char *xmalloc(size_t);
char *xstrcpy(char *);
char *xstrcat(char *, char *);
int  ndl_check_crc(char *);

#endif
