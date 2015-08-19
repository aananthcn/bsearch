/*
 * Author: Aananth C N
 * Email: caananth@visteon.com
 * License: GPL v2
 *
 * Date: Jun 23, 2015
 */
#include "commands.h"
#include "bsearch.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

int rows, cols;


#define DUMP_BYTES_P_LINE (32)
#define DUMP_SEARCH_LEN	(128)

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

	if(str == NULL)
		return 0;

	/* convert string to int */
	if((strncmp(str, "0x", 2) == 0) ||
	   (strncmp(str, "0X", 2) == 0)) {
		value = (int) strtol(str, NULL, 0);
	}
	else {
		value = (int) strtol(str, NULL, 16);
	}

	return value;
}

int proc_dumpinput(int ch, int address, int screen_size, const char *file)
{
	char input[DUMP_SEARCH_LEN];
	static int last_search_key;
	static int last_address;
	static char last_input[DUMP_SEARCH_LEN];
	int offset, getinput;

	if(ch == ' ') {
		ch = last_search_key;
		getinput = 0;
		strcpy(input, last_input);
	}
	else {
		getinput = 1;
	}

	offset = last_address;

	switch(ch) {
		addch(':');
	case KEY_UP:
		address -= DUMP_BYTES_P_LINE;
		if(address < 0)
			address = 0;
		break;
	case KEY_DOWN:
		address += DUMP_BYTES_P_LINE;
		break;
	case KEY_NPAGE:
		address += screen_size;
		break;
	case KEY_PPAGE:
		address -= screen_size;
		if(address < 0)
			address = 0;
		break;
	case ':':
		put_command_prompt(':');
		get_command_argument(input);
		address = 0xFFFFFFF0 & hexstring_to_int(input);
		break;
	case '/':
		if(getinput) {
			put_command_prompt('/');
			get_command_argument(input);
		}
		address = search_text_pattern(input, 0, file, offset);
		last_search_key = '/';
		strcpy(last_input, input);
		break;
	case '?':
		if(getinput) {
			put_command_prompt('?');
			get_command_argument(input);
		}
		address = search_hex_pattern(input, 0, file, offset);
		last_search_key = '?';
		strcpy(last_input, input);
		break;
	default:
		break;
	}

	last_address = address + strlen(input);

	return address;
}


int cmd_dump(const char *file, const char *offset)
{
	int fd;
	int address;
	int ch, loop_on = 1;
	struct winsize w;
	char *screen, *line;
	int screen_size, i, j, k;

	/* convert string to int */
	if(offset != NULL) {
		address = 0xFFFFFFF0 & hexstring_to_int(offset);
	}
	else {
		address = 0;
	}

	/* get the screen size for display */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	rows = w.ws_row;
	cols = w.ws_col;
	screen_size = (rows-1) * DUMP_BYTES_P_LINE;
	screen = (char *) malloc(screen_size);
	if(screen == NULL) {
		printf(" Could not allocate memory for %d bytes\n",
		       screen_size);
		return -1;
	}

	fd = open(file, O_RDONLY);
	if(fd < 0) {
		printf(" Could not open %s\n", file);
		free(screen);
		return -1;
	}

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	ch = 0;

	do {
		address = proc_dumpinput(ch, address, screen_size, file);
		lseek(fd, address, SEEK_SET);
		read(fd, screen, screen_size);
		clear();

		for(i=0; i < screen_size; i += (j*4)) {
			printw("%08X:", address+i);
			for(j=0; j < (DUMP_BYTES_P_LINE/4); j++) {
				printw(" %08X", *(((int*)(screen+i))+j));
			}
			printw("  ");
			line = screen+i;
			for(k=0; k < DUMP_BYTES_P_LINE; k++) {
				if((line[k] >= 33) && (line[k] <= 126))
					printw("%c",line[k]);
				else if(line[k] == 0xFF)
					printw(" ");
				else
					printw(".");
			}
			printw("\n");
		}

		printw("Menu| \'q\' - exit, \':\' - goto offset, \'/\' - search text, \'?\' - search hex  ##  Keys| UP, DOWN, PAGE-UP, PAGE-DOWN, SPACE\n");
	}
	while((ch = getch()) != 'q');

	printw("\n\Exiting Now\n");
	endwin();



exit_dump:
	free(screen);
	close(fd);
	return 0;
}

int execute_command(const char *cmd, const char *file, const char *offset)
{
	if((cmd == NULL) || (file == NULL))	{
		printf(" Check your input for command \'%s\'\n", cmd);
		return -1;
	}

	if(0 == strcmp(cmd, "dump")) {
		if(cmd_dump(file, offset))
			return -1;
	}
	else
	{
		printf(" Command \"%s\" is not supported\n", cmd);
	}

	return 0;
}
