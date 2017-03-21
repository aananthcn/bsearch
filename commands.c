/*
 * Author: Aananth C N
 * Email: caananth@visteon.com
 * License: GPL v2
 *
 * Date: Jun 23, 2015
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "bsearch.h"
#include "dump.h"
#include "utils.h"
#include "ext4.h"



int execute_command(const char *cmd, const char *file, const char *offset)
{
	if ((cmd == NULL) || (file == NULL))	{
		printf(" Check your input for command \'%s\'\n", cmd);
		return -1;
	}

	if (0 == strcmp(cmd, "dump")) {
		if (cmd_dump(file, offset))
			return -1;
	}
	else if (0 == strcmp(cmd, "ext4")) {
		if (cmd_ext4(file))
			return -1;
	}
	else
	{
		printf(" Command \"%s\" is not supported\n", cmd);
	}

	return 0;
}
