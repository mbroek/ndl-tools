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


#include "ndllib.h"
#include "../config.h"


/*
 * Fgets() is like fgets() but never returns the line terminator
 * at end of line and handles that line terminators:
 *
 * DOS/WINDOWS  ->  CR/LF
 * UNIX         ->  LF only
 * MAC          ->  CR only
 */
char *Fgets(char *l, int size, FILE *f) {
    char *cp = l;
    int  cr, eol = FALSE;
    
    if (feof(f)) 
	return NULL;

    cr = FALSE;
    while ((size > 1) && !feof(f)) {
	int	c = fgetc(f);

	if (c == EOF) {
	    if (ferror(f)) 
		return NULL;
	    break;
	}
	if (cr && c != '\n') {
	    /* CR end-of-line (MAC) */
	    ungetc(c,f);
	    eol = TRUE;
	    break;
	} else {
	    cr = (c == '\r');
	}
	if ( cr )
	    continue;
	--size;
	if (c == '\n') { 
	    eol = TRUE; 
	    break; 
	}
	*(cp++) = c;
    }
    *cp = '\0';

    cr = FALSE;
    while (!eol && !feof(f)) {
	int	c = fgetc(f);
	if (c == EOF)
	    break;
	if (cr && c != '\n') {
	    /* CR end-of-line (MAC) */
	    ungetc(c,f);
	    break;
	} else
	    cr = (c=='\r');
	if ( cr )
	    continue;
	if (c=='\n') 
	    break;
    }
    return l;
}


