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

#include "../config.h"
#include "ndllib.h"

char *xmalloc(size_t size)
{
    char *tmp;

    tmp = malloc(size);
    if (!tmp) 
	abort();
			            
    return tmp;
}


char *xstrcpy(char *src)
{
    char    *tmp;

    if (src == NULL) 
	return(NULL);
    tmp = xmalloc(strlen(src)+1);
    strcpy(tmp, src);
    return tmp;
}



char *xstrcat(char *src, char *add)
{
    char    *tmp;
    size_t  size = 0;

    if ((add == NULL) || (strlen(add) == 0))
	return src;
    if (src)
	size = strlen(src);
    size += strlen(add);
    tmp = xmalloc(size + 1);
    *tmp = '\0';
    if (src) {
	strcpy(tmp, src);
	free(src);
    }
    strcat(tmp, add);
    return tmp;
}

