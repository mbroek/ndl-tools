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

#include "../ndl-lib/ndllib.h"
#include "../config.h"
#include "ndl-check.h"


int	Zones = 0;
int	Regions = 0;
int	Hosts = 0;
int	Hubs = 0;
int	Nodes = 0;
int	Holds = 0;
int	Downs = 0;
int	Privates = 0;
int	Points = 0;


/*
 * Read nodelist byte by byte to check the primary formatting.
 */
int check_ascii(char *filename)
{
    int			rc = 0, llen = 0, lineno = 1, is_cmt = FALSE;
    FILE		*fp;
    unsigned char	c, lc = '\0';

    if ((fp = fopen(filename, "r")) == NULL) {
	fprintf(stderr, "check_ascii: can't open '%s': ", filename);
	perror("");
	return -1;
    }

    while (fread(&c, sizeof(c), 1, fp)) {
	switch (c) {
	    case '\r':	if (lc == '\r') {
			    printf("check_ascii: line %d cr-cr sequence detected.\n", lineno);
			    rc = 4;
			}
			if (lc == '\n') {
			    printf("check_ascii: line %d lf-cr (Mac) sequence detected.\n", lineno);
			    rc = 4;
			}
			break;
	    case '\n':	if (lc == '\n') {
			    printf("check_ascii: line %d lf-lf sequence detected.\n", lineno);
			    rc = 4;
			}
			if (lc == '\r') {
			    if (llen == 0) {
				printf("check_ascii: line %d is empty.\n", lineno);
				rc = 3;
			    }
			    llen = 0;
			} else {
			    printf("check_ascii: line %d not cr-lf terminated.\n", lineno);
			    rc = 1;
			}
			lineno++;
			is_cmt = FALSE;
			break;
	    case '\32': break;
	    default:	llen++;
			/*
			 * If first character is ; then it's a comment line
			 */
			if (lc == '\n' && c == ';' && llen == 1) {
			    is_cmt = TRUE;
			}
			/*
			 * Check illegal characters in data lines
			 */
			if (! is_cmt && (c < 32)) {
			    printf("check_ascii: line %d illegal control-%c.\n", lineno, c + 64);
			}
			break;
	}
	lc = c;
    }
    if (c != 26) {
	printf("check_ascii: no EOF character at end of file.\n");
	rc = 2;
    }

    fclose(fp);
    if (rc == 0) {
    	printf("check_ascii: file %s has %d lines, ascii formatting ok.\n", filename, lineno);
    } else {
	printf("check_ascii: file %s is bad formatted.\n", filename);
    }
    return rc;
}



int check_type(char *filename)
{
    FILE	*fp;
    char	*buf;
    int		rc = 0, lineno = 0;

    fp = fopen(filename, "r");
    buf = calloc(1024, sizeof(char));

    while (Fgets(buf, 1023, fp)) {
	if (lineno == 0) {
	    if (buf[0] != ';' || buf[1] != 'A' || buf[2] != ' ') {
		printf("check_type : file %s is not a nodelist or nodediff.\n", filename);
		rc = 2;
		break;
	    }
	}
	if ((buf[0] == 'A' || buf[0] == 'C' || buf[0] == 'D') && isdigit(buf[1])) {
	    printf("check_type : file %s looks like a nodediff.\n", filename);
	    rc = 1;
	    break;
	}
	lineno++;
    }

    free(buf);
    fclose(fp);
    if (rc == 0)
	printf("check_type : file %s looks like a nodelist.\n", filename);

    return rc;
}



int check_crc(char *filename)
{
    int		rc;

    rc = ndl_check_crc(filename);
    printf("check_crc  : file %s crc is %s.\n", filename, rc ? "wrong":"ok");
    return rc;
}



int check_struc(char *filename)
{
    int		rc = 0, i, errors = 0, lineno = 0, level, lastlevel = LEVEL_TOP, composite = FALSE, firstline = TRUE;
    FILE	*fp;
    char	*buf;
    static char	*p, *q;

    fp = fopen(filename, "r");
    buf = calloc(1024, sizeof(char));

    while (Fgets(buf, 1023, fp)) {
	lineno++;
	if ((buf[0] == ';') && (buf[1] == 'S')) {
	    composite = TRUE;
	}
	if ((buf[0] != ';') && (buf[0] != '\032')) {
//	    printf("%s\n", buf);

	    p = buf;
	    if ((q = strchr(p,',')))
		*q++ = '\0';

	    if (strlen(p)) {
		if (strcasecmp(p, "Zone") == 0)
		    level = LEVEL_ZONE;
		else if (strcasecmp(p, "Region") == 0)
		    level = LEVEL_REGION;
		else if (strcasecmp(p, "Host") == 0)
		    level = LEVEL_HOST;
		else if (strcasecmp(p, "Hub") == 0)
		    level = LEVEL_HUB;
		else if (strcasecmp(p, "Hold") == 0)
		    level = LEVEL_HOLD;
		else if (strcasecmp(p, "Down") == 0)
		    level = LEVEL_DOWN;
		else if (strcasecmp(p, "Pvt") == 0)
		    level = LEVEL_PRIVATE;
		else if (strcasecmp(p, "Point") == 0)
		    level = LEVEL_POINT;
		else
		    level = LEVEL_ERROR;
	    } else {
		level = LEVEL_NODE;
	    }

	    if (firstline) {
		firstline = FALSE;
		switch (level) {
		    case LEVEL_ZONE:	if (composite)
					    printf("check_struc: file %s is a composite nodelist.\n", filename);
					else
					    printf("check_struc: file %s is a Zone segment.\n", filename);
					break;
		    case LEVEL_REGION:	printf("check_struc: file %s is a Region segment.\n", filename);
					break;
		    case LEVEL_HOST:	printf("check_struc: file %s is a Host segment.\n", filename);
					break;
		    case LEVEL_HUB:	printf("check_struc: file %s is a Hub segment.\n", filename);
					break;
		    case LEVEL_NODE:	printf("check_struc: file %s is a Node segment.\n", filename);
		}
	    }

	    /*
	     * Statistics count
	     */
	    switch (level) {
		case LEVEL_ZONE:	Zones++;
					break;
		case LEVEL_REGION:	Regions++;
					break;
		case LEVEL_HOST:	Hosts++;
					break;
		case LEVEL_HUB:		Hubs++;
					break;
		case LEVEL_NODE:	
		case LEVEL_PRIVATE:	Nodes++;
					break;
		case LEVEL_HOLD:	Holds++;
					break;
		case LEVEL_DOWN:	Downs++;
					break;
		case LEVEL_POINT:	Points++;
					break;
	    }

	    /*
	     * Get 2nd field, node number
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 2nd field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    for (i = 0; i < strlen(p); i++)
		if (! isdigit(p[i])) {
		    errors++;
		    printf("check_struc: file %s line %d invalid nodenumber (%s) in 2nd field.\n", filename, lineno, p);
		    break;
		}
	    i = atoi(p);
	    if ((i < 0) || (i > 32767)) {
		errors++;
		printf("check_struc: file %s line %d invalid nodenumber (%s) in 2nd field.\n", filename, lineno, p);
	    }

	    /*
	     * Get 3rd field, Node name
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 3rd field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    for (i = 0; i < strlen(p); i++) {
		if (((p[i] < 32) || (p[i] > 126) || (p[i] == ' ') || p[i] == ',')) {
		    printf("check_struc: file %s line %d invalid character(s) (%s) in 3rd field.\n", filename, lineno, p);
		    errors++;
		    break;
		}
	    }

	    /*
	     * Get 4th field, Location
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 4th field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    for (i = 0; i < strlen(p); i++) {
		if (((p[i] < 32) || (p[i] > 126) || (p[i] == ' ') || p[i] == ',')) {
		    printf("check_struc: file %s line %d invalid character(s) (%s) in 4th field.\n", filename, lineno, p);
		    errors++;
		    break;
		}
	    }

	    /*
	     * Get 5th field, Sysop name
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 5th field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    for (i = 0; i < strlen(p); i++) {
		if (((p[i] < 32) || (p[i] > 126) || (p[i] == ' ') || p[i] == ',')) {
		    printf("check_struc: file %s line %d invalid character(s) (%s) in 5th field.\n", filename, lineno, p);
		    errors++;
		    break;
		}
	    }

	    /*
	     * Get 6th field, Phone number
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 6th field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    if (strcasecmp(p, "-Unpublished-")) {
		for (i = 0; i < strlen(p); i++) {
		    if (! isdigit(p[i]) && (p[i] != '-')) {
			printf("check_struc: file %s line %d invalid character(s) (%s) in 6th field.\n", filename, lineno, p);
			errors++;
			break;
		    }
		}
	    }

	    /*
	     * Get 7th field, Baudrate
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 7th field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';
	    i = atoi(p);
	    if ((i != 300) && (i != 1200) && (i != 2400) && (i != 9600) && (i != 9600) &&
		(i != 14400) && (i != 16800) && (i != 19200) && (i != 28800) && (i != 33600) && (i != 38400)) {
		errors++;
		printf("check_struc: file %s line %d invalid baudrate (%s).\n", filename, lineno, p);
	    }

	    /*
	     * Get 8th field and up, flags.
	     */
	    p = q;
	    if (p == NULL) {
		errors++;
		printf("check_struc: file %s line %d no 8th field.\n", filename, lineno);
		continue;
	    }
	    if ((q=strchr(p,',')))
		*q++ = '\0';

	    /*
	     * From now on, p may be empty, flags are optional.
	     */
//	    if (strlen(p) == 0) 
//	    printf("'%s' '%s'\n", p, q);

	    lastlevel = level;
//	    break;
	}
    }

    fclose(fp);
    free(buf);

    printf("             Zones   [%5d]  Regions  [%5d]  Hosts    [%5d]  Hubs     [%5d]\n", Zones, Regions, Hosts, Hubs);
    printf("             Nodes   [%5d]  Hold     [%5d]  Down     [%5d]  Points   [%5d]\n", Nodes, Holds, Downs, Points);
    if (errors)
	printf("             Errors  [%5d]\n", errors);
    return rc;
}



int main(int argc, char **argv)
{
    char	*cmd;
    int		rc = 0;

    fprintf(stdout, "NDL-CHECK %s nodelist checker\n", VERSION);
    fprintf(stdout, "          %s\n\n", COPYRIGHT);

    if (argc < 2) {
	fprintf(stderr, "Usage:   ndl-check [nodelist]\n");
	return 1;
    }

    cmd = xstrcpy(argv[1]);
    rc = check_ascii(cmd);
    if (rc)
	return rc;

    rc = check_type(cmd);
    if (rc)
	return 0;

    rc = check_crc(cmd);
    rc = check_struc(cmd);

    return rc;
}

