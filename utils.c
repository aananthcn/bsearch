#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


void dump_mem_tobuffer(char *obuf, char *ibuf, int sz, char *msg,
		       int addr, int bpline)
{
	int i, j, k;
	char *line, *s;

	s = obuf;

	/* Print header */
	for (i=0; i < 34; i++)
		s += sprintf(s, "*");
	s += sprintf(s, " %s ", msg);
	for (; i > 0; i--)
		s += sprintf(s, "*");
	s += sprintf(s, "\n");

	/* rows loop */
	for (i=0; i < sz; i += (j*4)) {
		/* print addr */
		s += sprintf(s, "%08X:", addr+i);

		/* print 'bpline' bytes per line */
		for (j=0; j < (bpline/4); j++) {
			s += sprintf(s, " %08X", *(((int*)(ibuf+i))+j));
		}
		s += sprintf(s, "  ");

		/* print characters */
		line = ibuf+i;
		for(k=0; k < bpline; k++) {
			if((line[k] >= 33) && (line[k] <= 126))
				s += sprintf(s, "%c",line[k]);
			else if(line[k] == -1)
				s += sprintf(s, " ");
			else
				s += sprintf(s, ".");
		}
		s += sprintf(s, "\n");
	}
}


void print_mem_onscreen(char *screen, int sz_y, int addr, int bpline)
{
	int i, j, k;
	char *line;

	/* rows loop */
	for (i=0; i < sz_y; i += (j*4)) {
		/* print addr */
		printw("%08X:", addr+i);

		/* print 'bpline' bytes per line */
		for (j=0; j < (bpline/4); j++) {
			printw(" %08X", *(((int*)(screen+i))+j));
		}
		printw("  ");

		/* print characters */
		line = screen+i;
		for(k=0; k < bpline; k++) {
			if((line[k] >= 33) && (line[k] <= 126))
				printw("%c",line[k]);
			else if(line[k] == -1)
				printw(" ");
			else
				printw("·");
		}
		printw("\n");
	}
}


void put_command_prompt(int c)
{
	int x, y;

	getyx(stdscr, y, x);
	move(y, 0);
	clrtoeol();
	move(y, x);
	move(y, 0);
	printw("%c", c);
	refresh();
}

void get_command_argument(char *input)
{
	int i, ch;

	echo();
	getstr(input);
	noecho();
}

int hexstring_to_int(const char *str)
{
	int value;

	if (str == NULL)
		return 0;

	/* convert string to int */
	if ((strncmp(str, "0x", 2) == 0) ||
	   (strncmp(str, "0X", 2) == 0)) {
		value = (int) strtol(str, NULL, 0);
	}
	else {
		value = (int) strtol(str, NULL, 16);
	}

	return value;
}

/*************************************************************************
 * Function: get_filename
 *
 * This function finds the name of the file from the path
 *
 * arg1: file path
 *
 * Returns null if any failure found else the file name
 */
const char* get_filename(const char *path)
{
        int i;

        if(path == NULL)
                return path;

        i = strlen(path);
        for(; i > 0; i--)
                if(path[i] == '/') {
                        i++;
                        break;
                }

        return (path+i);
}
