/*
 * Author: Aananth C N
 * Email: caananth@visteon.com
 * License: GPL v2
 *
 * Version: get the version in MY_VERSION macro below 
 * Date: Jun 20, 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/time.h>

#include "commands.h"
#include "utils.h"


#define MY_VERSION (1.41)


#define FNAME_SIZE	(128)
#define PAGE_SIZE	(4*1024)
#define PAGE_EXTRA	(128)

#define SWAP16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))
#define SWAP32(x) ((((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24) | \
		   (((x) & 0x0000FF00) << 8) | (((x) & 0x000000FF) << 24))

int validate_hex_pattern(char *p)
{
	int retval = 0;
	int i;

	for (i = 0; p[i]; i++) {
		if (toupper(p[i]) > 'F') {
			printf(" \'%s\' not a valid hex string\n", p);
			retval = -1;
			break;
		}
	}

	return retval;
}

int process_pattern_4_hex(char *pattern, int *ilen, unsigned int *pat_le,
		     unsigned int *pat_be)
{
	int len;
	int pat;

	if (validate_hex_pattern(pattern) < 0)
		return -1;

        len = (int) strlen(pattern);
        if ((strncmp(pattern, "0x", 2) == 0) ||
           (strncmp(pattern, "0X", 2) == 0)) {
                pat  = (int) strtol(pattern, NULL, 0);
                len -= 2;
        }
        else {
                pat  = (int) strtol(pattern, NULL, 16);
        }


	switch (len) {
	case 1:
	case 2:
		*ilen = 1; /* bytes */
		*pat_le = (unsigned int) pat;
		*pat_be = (unsigned int) pat;
		break;
	case 3:
	case 4:
		*ilen = 2; /* bytes */
		*pat_le = (unsigned int) pat;
		*pat_be = SWAP16((unsigned int) pat);
		break;
	default:
		*ilen = 4; /* bytes */
		*pat_le = (unsigned int) pat;
		*pat_be = SWAP32((unsigned int) pat);
		break;
	}

	printf(" Hex pattern: \'0x%x\' width \'%d\'\n", pat, len);

	return len;
}

int search_text_pattern(char *pattern, int all, const char *fname, int offset)
{
	int fd;
	int page_count, patt_found;
	int patt_found_history = 0, len, i;
	char page[PAGE_SIZE+PAGE_EXTRA];
	int address = -1;

	if (fname == NULL)
		return -1;

	if (pattern == NULL)
		return -1;

	fd = open(fname, O_RDONLY);
	if (fd < 0)
		return -1;

	len = strlen(pattern);
	if (len >= PAGE_EXTRA)
		return -1;

	patt_found = 0;
	page_count = offset / PAGE_SIZE;
	i = offset % PAGE_SIZE;
	lseek(fd, page_count * PAGE_SIZE, SEEK_SET); /* page boundary */

	printf(" Searching \"%s\" in %s...\n", pattern, fname);
	while (read(fd, page, PAGE_SIZE+PAGE_EXTRA) > 0) {
		for (; i < PAGE_SIZE; i++) {
			if (!strncmp((page+i), pattern, len)) {
				patt_found = 1;
				patt_found_history = 1;
			}

			if (patt_found == 1) {
				address = ((page_count * PAGE_SIZE) + i);
				printf(" Found \'%s\' at offset: \"0x%X\"\n",
				       pattern, address);
				if (!all) {
					goto exit_search;
				}
				else {
					patt_found = 0;
					i += len;
				}
			}
		}
		page_count++;
		i = 0;
		lseek(fd, page_count * PAGE_SIZE, SEEK_SET); /* page boundary */
	}
	if (!patt_found_history)
		printf(" Could not find the pattern \'%s\'!\n", pattern);

exit_search:
	close(fd);
	return address;
}

int search_hex_pattern(char *pattern, int all, const char *fname, int offset)
{
	int fd;
	int ilen, i, page_count;
	unsigned int pat_le, pat_be, fdata;
	char page[PAGE_SIZE];
	char mesg[FNAME_SIZE];
	int patt_found, patlen, patt_found_history;
	int address = -1;

	if (fname == NULL)
		return -1;

	fd = open(fname, O_RDONLY);
	if (fd < 0)
		return -1;
	lseek(fd, offset, SEEK_SET); /* start from an offset */

	/* determine the size of pattern to search */
	patlen = process_pattern_4_hex(pattern, &ilen, &pat_le, &pat_be);
	if (patlen < 0) {
		printf(" Error: check your inputs!\n");
		return -1;
	}
	if (patlen > 8) {
		printf(" Error: this version supports < 8 char patterns only\n");
		return -1;
	}

	patt_found = 0;
	patt_found_history = 0;
	page_count = offset / PAGE_SIZE;
	i = offset % PAGE_SIZE;
	lseek(fd, page_count, SEEK_SET); /* start from an offset */

	/* read a page from file */
	while (read(fd, page, PAGE_SIZE) > 0) {
		for (; i <= PAGE_SIZE; i++) {
			/* pull the data from the page */
			switch (ilen) {
			case 4:
				fdata = (unsigned int)(*((int*)(page + i)));
				break;
			case 2:
				fdata = (unsigned int)(*((short*)(page + i)));
				break;
			default:
				fdata = (unsigned int)(*((char*)(page + i)));
				break;
			}

			/* compare the date with patterns */
			if (fdata == pat_le) {
				patt_found = 1;
				patt_found_history = 1;
				strcpy(mesg, "Little Endian format");
			}
			else if (fdata == pat_be) {
				patt_found = 1;
				patt_found_history = 1;
				strcpy(mesg, "Big Endian format");
			}

			/* print message and exist if comparision succeeded */
			if (patt_found == 1) {
				address = ((page_count * PAGE_SIZE) + i);
				printf(" Found \'%s\' in %s at offset: \"0x%X\"\n",
				       pattern, mesg, address);
				if (!all)
					goto exit_search;
				else
					patt_found = 0;
			}

		}
		page_count++;
		i = 0;
	}
	if (!patt_found_history)
		printf(" Could not find the pattern \'%s\'! \n", pattern);

exit_search:
	close(fd);
	return address;
}

void print_help(void)
{
	printf("\n usage: \n ------\n");
	printf(" 1. bsearch [OPTIONS] -p <pattern_hex> -f <file_name>\n");
	printf(" 2. bsearch [OPTIONS] -c <command> -f <file_name>\n");
	printf("\n   OPTIONS:\n");
	printf("     -a		search for all occurance\n");
	printf("     -t		text search, treats \"pattern_hex\" as text\n");
	printf("     -c		commands (see COMMAND section below)\n");
	printf("     -o		offset\n");
	printf("\n   COMMANDS:\n");
	printf("     dump	display contents of file with additional options\n");
	printf("     ext4	displays ext4 file system info\n");
	printf("\n\nv%.2f\n", MY_VERSION);
}

int main(int argc, char *argv[])
{
	int aflag, tflag, cflag, oflag;
	int patnum, patlen;
	struct timeval t1, t2;
	char *filename, *pattern, c;
	char *command, *offset;
	int ioffset;

	filename = pattern = command = offset = NULL;
	aflag = tflag = cflag = oflag = 0;

	while ((c = getopt(argc, argv, "atp:f:c:o:")) != -1) {
		switch (c)
		{
		case 'a':
			aflag = 1;
			break;
		case 't':
			tflag = 1;
			break;
		case 'p':
			pattern = optarg;
			break;
		case 'f':
			filename = optarg;
			break;
		case 'c':
			cflag = 1;
			command = optarg;
			break;
		case 'o':
			oflag = 1;
			offset = optarg;
			break;
		case '?':
			print_help();
			return 1;
		default:
			printf(" Invalid arguments provided\n");
			print_help();
			abort();
		}
	}

	if ((argc < 3) || (filename == NULL)) {
		print_help();
		return -1;
	}

	if (cflag) {
		execute_command(command, filename, offset);
	}
	else {
		if (pattern == NULL) {
			printf(" please enter the pattern value\n");
			print_help();
			return -1;
		}

		ioffset = hexstring_to_int(offset);
		gettimeofday(&t1, NULL);
		if (tflag) {
			search_text_pattern(pattern, aflag, filename, ioffset);
		}
		else {
			search_hex_pattern(pattern, aflag, filename, ioffset);
		}
		gettimeofday(&t2, NULL);

		printf(" Search time = %d sec\n", (int)(t2.tv_sec - t1.tv_sec));
	}

	return 0;
}
