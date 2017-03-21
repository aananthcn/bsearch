#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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

