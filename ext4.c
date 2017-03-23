#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ncurses.h>

#include "utils.h"
#include "ext4.h"


#define EXT4_SUPER_BLOCK0_OFFSET		1024
#define MAX_SCREENS			8

static int MaxRows, MaxCols;
static int BlockSize;


void dump_super_block(char *screen, struct ext4_super_block *p)
{
	struct tm *ti;
	time_t t;
	int i;
	char *s;

	s = screen;

	s += sprintf(s, "+==================================================================+\n");
	s += sprintf(s, "|                          EXT4 SUPER BLOCK                        |\n");
	s += sprintf(s, "+==================================================================+\n");
	s += sprintf(s, "Inodes count                    = %d\n", p->s_inodes_count);
	s += sprintf(s, "Blocks count                    = %d\n", p->s_blocks_count_lo);
	s += sprintf(s, "Reserved blocks count           = %d\n", p->s_r_blocks_count_lo);
	s += sprintf(s, "Free blocks count               = %d\n", p->s_free_blocks_count_lo);
	s += sprintf(s, "Free inodes count               = %d\n", p->s_free_inodes_count);
	s += sprintf(s, "First data block                = %d\n", p->s_first_data_block);
	s += sprintf(s, "Block size: 2^(10+this_number)  = %d\n", p->s_log_block_size);
	s += sprintf(s, "Cluster size: 2^(this_number)   = %d\n", p->s_log_cluster_size);
	s += sprintf(s, "Blocks per group                = %d\n", p->s_blocks_per_group);
	s += sprintf(s, "Inodes per group                = %d\n", p->s_inodes_per_group);
	t = p->s_mtime;
	s += sprintf(s, "Mount time                      = %s", asctime(ti = localtime(&t)));
	t = p->s_wtime;
	s += sprintf(s, "Write time                      = %s", asctime(ti = localtime(&t)));
	s += sprintf(s, "Mount count                     = %d\n", p->s_mnt_count);
	s += sprintf(s, "Maximal mount count             = %d\n", p->s_max_mnt_count);
	s += sprintf(s, "Magic signature                 = 0x%X\n", p->s_magic);
	s += sprintf(s, "File system state (1 = clean)   = %d\n", p->s_state);
	s += sprintf(s, "Behaviour when detecting errors = %d\n", p->s_errors);
	s += sprintf(s, "Minor revision level            = %d\n", p->s_minor_rev_level);
	t = p->s_lastcheck;
	s += sprintf(s, "Time of last check              = %s", asctime(ti = localtime(&t)));
	s += sprintf(s, "Max time between checks         = %d\n", p->s_checkinterval);
	s += sprintf(s, "Creator OS (0 = Linux)          = %d\n", p->s_creator_os);
	s += sprintf(s, "Revision level                  = %d\n", p->s_rev_level);
	s += sprintf(s, "Default uid for reserved blocks = %d\n", p->s_def_resuid);
	s += sprintf(s, "Default gid for reserved blocks = %d\n", p->s_def_resgid);
	s += sprintf(s, "First non-reserved inode        = %d\n", p->s_first_ino);
	s += sprintf(s, "Size of inode structure         = %d\n", p->s_inode_size);
	s += sprintf(s, "Block group no of this superblk = %d\n", p->s_block_group_nr);
	s += sprintf(s, "Compatible feature set          = %d\n", p->s_feature_compat);
	s += sprintf(s, "Incompatible feature set        = %d\n", p->s_feature_incompat);
	s += sprintf(s, "Readonly compatible feature set = %d\n", p->s_feature_ro_compat);
	s += sprintf(s, "128-bit uuid for volume         = ");
	for (i=0; i < 16; i++)
		s += sprintf(s, "%x", p->s_uuid[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Volume name                     = %s\n", p->s_volume_name);
	s += sprintf(s, "Last mounted directory	        = %s\n", p->s_last_mounted);
	s += sprintf(s, "Compression algorithm           = %d\n", p->s_algorithm_usage_bitmap);
	s += sprintf(s, "Pre allocate blocks             = %d\n", p->s_prealloc_blocks);
	s += sprintf(s, "Pre allocate directory blocks   = %d\n", p->s_prealloc_dir_blocks);
	s += sprintf(s, "Reserved gdt blocks             = %d\n", p->s_reserved_gdt_blocks);
	s += sprintf(s, "uuid of journal superblock      = ");
	for (i=0; i < 16; i++)
		s += sprintf(s, "%x", p->s_journal_uuid[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Inode number of journal file    = %d\n", p->s_journal_inum);
	s += sprintf(s, "Device number of journal file   = %d\n", p->s_journal_dev);
	s += sprintf(s, "Start of inode-list to delete   = %d\n", p->s_last_orphan);
	s += sprintf(s, "HTREE hash seed                 = ");
	for (i=0; i < 4; i++)
		s += sprintf(s, "%x", p->s_hash_seed[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Default hash version to use     = %d\n", p->s_def_hash_version);
	s += sprintf(s, "Journal backup type             = %d\n", p->s_jnl_backup_type);
	s += sprintf(s, "Size of the group descriptor    = %d\n", p->s_desc_size);
	s += sprintf(s, "Default mount options           = %d\n", p->s_default_mount_opts);
	s += sprintf(s, "First metablock block group     = %d\n", p->s_first_meta_bg);
	t = p->s_mkfs_time;
	s += sprintf(s, "Filesystem created time         = %s", asctime(ti = localtime(&t)));
	s += sprintf(s, "Block count                     = %d\n", p->s_blocks_count_hi);
	s += sprintf(s, "Reserved block count            = %d\n", p->s_r_blocks_count_hi);
	s += sprintf(s, "Free block count                = %d\n", p->s_free_blocks_count_hi);
	s += sprintf(s, "Min inode size (extra)          = %d\n",
		     p->s_min_extra_isize);
	s += sprintf(s, "New inode should reserve bytes  = %d\n",
		     p->s_want_extra_isize);
	s += sprintf(s, "Miscelleneous flags             = %d\n", p->s_flags);
	s += sprintf(s, "RAID stride                     = %d\n",
		     p->s_raid_stride);
	s += sprintf(s, "Multi Mount Protection - wait   = %d\n",
		     p->s_mmp_update_interval);
	s += sprintf(s, "Multi Mount Protection - blocks = %llu\n",
		     p->s_mmp_block);
	s += sprintf(s, "Blocks on all disk (N*stride)   = %d\n",
		     p->s_raid_stripe_width);
	s += sprintf(s, "Metadata checksum algorithm     = %d\n",
		     p->s_checksum_type);
	s += sprintf(s, "Versioning level for encryption = %d\n",
		     p->s_encryption_level);
	s += sprintf(s, "Kilobytes written (lifetime)    = %llu\n",
		     p->s_kbytes_written);
	s += sprintf(s, "Filesystem Error count          = %d\n",
		     p->s_error_count);
	t = p->s_first_error_time;
	s += sprintf(s, "First error time                = %s",
		     asctime(ti = localtime(&t)));
	s += sprintf(s, "First error inode               = %d\n",
		     p->s_first_error_ino);
	s += sprintf(s, "First error block               = %llu\n",
		     p->s_first_error_block);
	s += sprintf(s, "First error function            = %s\n",
		     p->s_first_error_func);
	s += sprintf(s, "First error line number         = %d\n",
		     p->s_first_error_line);
	t = p->s_last_error_time;
	s += sprintf(s, "Last error time                 = %s",
		     asctime(ti = localtime(&t)));
	s += sprintf(s, "Last error inode                = %d\n",
		     p->s_last_error_ino);
	s += sprintf(s, "Last error block                = %llu\n",
		     p->s_last_error_block);
	s += sprintf(s, "Last error function             = %s",
		     p->s_last_error_func);
	s += sprintf(s, "Last error line number          = %d\n",
		     p->s_last_error_line);
	s += sprintf(s, "Mount options                   = ");
	for (i=0; i < 64; i++)
		s += sprintf(s, "%x", p->s_mount_opts[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Inode for tracking user quota   = %d\n",
		     p->s_usr_quota_inum);
	s += sprintf(s, "Inode for tracking group quota  = %d\n",
		     p->s_grp_quota_inum);
	s += sprintf(s, "Overhead blocks / clusters      = %d\n",
		     p->s_overhead_clusters);
	s += sprintf(s, "Encryption algorithms in use    = ");
	for (i=0; i < 4; i++)
		s += sprintf(s, "%x", p->s_encrypt_algos[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Salt used for str2key algorithm = ");
	for (i=0; i < 16; i++)
		s += sprintf(s, "%x", p->s_encrypt_pw_salt[i]);
	s += sprintf(s, "\n");
	s += sprintf(s, "Location of lost+found inode    = %d\n", p->s_lpf_ino);
	s += sprintf(s, "Checksum crc32 (superblock)     = %d\n", p->s_checksum);
}


void dump_group_desc(char *screen, struct ext4_group_desc *p)
{
	struct tm *ti;
	time_t t;
	int i, bracketor;
	char *s;
	u64 ul;

	s = screen;
	bracketor = 0;

	s += sprintf(s, "+==================================================================+\n");
	s += sprintf(s, "|                       BLOCK GROUP DESCRIPTOR                     |\n");
	s += sprintf(s, "+==================================================================+\n");
	ul = p->bg_block_bitmap_lo | ((u64)p->bg_block_bitmap_hi << 32);
	s += sprintf(s, "Block bitmap location           = %llu\n", ul);
	ul = p->bg_inode_bitmap_lo | ((u64)p->bg_inode_bitmap_hi << 32);
	s += sprintf(s, "Inode bitmap location           = %llu\n", ul);
	ul = p->bg_inode_table_lo | ((u64)p->bg_inode_table_hi << 32);
	s += sprintf(s, "Inode table location            = %llu\n", ul);
	ul = p->bg_free_blocks_count_lo | ((u64)p->bg_free_blocks_count_hi << 16);
	s += sprintf(s, "Free blocks count               = %llu\n", ul);
	ul = p->bg_free_inodes_count_lo | ((u64)p->bg_free_inodes_count_hi << 16);
	s += sprintf(s, "Free inode count                = %llu\n", ul);
	ul = p->bg_used_dirs_count_lo | ((u64)p->bg_used_dirs_count_hi << 16);
	s += sprintf(s, "Directories count               = %llu\n", ul);
	s += sprintf(s, "EXT4_BG_flags                   = 0x%x ", p->bg_flags);
	if (p->bg_flags & 0x04) {
		s += sprintf(s, "[INODE ZEROED");
		bracketor = 1;
	}
	if (p->bg_flags & 0x02) {
		if (bracketor)
			s += sprintf(s, "|");
		else
			s += sprintf(s, "[");
		s += sprintf(s, "BLOCK UNINIT");
		bracketor = 1;
	}
	if (p->bg_flags & 0x01) {
		if (bracketor)
			s += sprintf(s, "|");
		else
			s += sprintf(s, "[");
		s += sprintf(s, "INODE UNINIT");
		bracketor = 1;
	}
	if (bracketor) {
		s += sprintf(s, "]");
		bracketor = 0;
	}
	s += sprintf(s, "\n");
	ul = p->bg_exclude_bitmap_lo | ((u64)p->bg_exclude_bitmap_hi << 32);
	s += sprintf(s, "Snapshot exclusion bitmap loc.  = %llu\n", ul);
	ul = p->bg_block_bitmap_csum_lo | ((u64)p->bg_block_bitmap_csum_hi << 16);
	s += sprintf(s, "Block bitmap checksum           = %llu\n", ul);
	ul = p->bg_inode_bitmap_csum_lo | ((u64)p->bg_inode_bitmap_csum_hi << 16);
	s += sprintf(s, "Inode bitmap checksum           = %llu\n", ul);
	ul = p->bg_itable_unused_lo | ((u64)p->bg_itable_unused_hi << 16);
	s += sprintf(s, "Unused inode count              = %llu\n", ul);
}


int search_screen(char *screen, char *str, int offset)
{
	int line_no = 0;
	char line[4096];
	int i, j;
	int len;

	/* convert the search string to lower */
	len = strlen(str);
	for (i = 0; i < len; i++) {
		str[i] = tolower(str[i]);
	}

	/* search the string in screen */
	len = strlen(screen);
	for (i = j = 0; (i < len) && (j < sizeof(line)-1); i++, j++) {
		/* start copying a line from offset */
		if (line_no >= offset) {
			line[j] = tolower(screen[i]);
		}

		/* check for end of current line */
		if (screen[i] == '\n') {
			line[i+1] = '\0';
			line_no++;
			j = 0;
		}

		/* search string in current line */
		if ((j == 0) && (line_no)) {
			if (strstr(line, str) != NULL) {
				/* found the string */
				line_no--;
				goto found;
			}
		}

	}
	line_no = 0; //scroll back

found:

	return line_no;
}

void print_help(char *screen)
{
	char *s;

	s = screen;
	clear();
	s += sprintf(s, "+==================================================================+\n");
	s += sprintf(s, "|                       EXT4 ANALYZER HELP                         |\n");
	s += sprintf(s, "+==================================================================+\n");
	s += sprintf(s, " \'s\' - print \"Super Block\"\n");
	s += sprintf(s, " \'b\' - print \"Block Group Description\"\n");
	s += sprintf(s, " \'\' - \n");
	s += sprintf(s, " \'\' - \n");
	s += sprintf(s, " \'/\' - search text in screen\n");
	s += sprintf(s, " \'n\' - search next\n");
}

static void print_menu(void)
{
	move(MaxRows, 0);
	printw("Menu| \'q\' - exit; \'?\' - help; other commands: \'s\' \'b\' \'n\'");
}


int print_screen(char *screen, int offset, int maxlen)
{
	int i, j, c;
	int len;

	len = strlen(screen);

	clear();
	for (i = j = 0; i < len && j < maxlen+offset; i++) {
		c = screen[i];

		if (j >= offset) {
			addch(c);
		}

		if (c == '\n') {
			j++;
		}

	}

	print_menu();
	return (i >= len);
}

int cmd_ext4(const char *file)
{
	int fd, ch, end;
	int address = EXT4_SUPER_BLOCK0_OFFSET;
	struct ext4_super_block super_block;
	struct ext4_group_desc	group_desc;

	char * sb = (char *)&super_block;
	char *bg = (char *)&group_desc;

	char *screen, *line;
	int screen_size;
	struct winsize w;
	int offset = 0;
	char str[1024];

	/* screen is where all display content will be dumped */
	screen = line = NULL;

	/* get the screen size for display */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	MaxRows = w.ws_row-1;
	MaxCols = w.ws_col;
	screen_size = MaxRows * MaxCols;
	screen = (char *) malloc(MAX_SCREENS * screen_size);
	if (screen == NULL) {
		printf(" Could not allocate memory for %d bytes\n",
		       screen_size);
		return -1;
	}

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf(" Could not open %s\n", file);
		return -1;
	}
	printf("%s::%s()\n", __FILE__, __func__);

	/* initialize screen for printing */
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	scrollok(stdscr, TRUE);
	ch = 0;

	/* fetch super block to get block size */
	lseek(fd, address, SEEK_SET);
	read(fd, sb, sizeof(super_block));
	BlockSize = 1 << (10+super_block.s_log_block_size);


	/* print help as default screen */
	print_help(screen);
	print_screen(screen, offset, MaxRows);

	/* wait for user command and display contents as per menu */
	while ((ch = getch()) != 'q') {
		switch (ch) {
		case KEY_UP:
			offset--;
			if (offset < 0)
				offset = 0;
			break;
		case KEY_PPAGE:
			offset -= 10;
			if (offset < 0)
				offset = 0;
			break;
		case KEY_DOWN:
			if (!end)
				offset++;
			if (offset > (MAX_SCREENS-1) * MaxRows)
				offset = (MAX_SCREENS-1) * MaxRows;
			break;
		case KEY_NPAGE:
			if (!end)
				offset += 10;
			if (offset > (MAX_SCREENS-1) * MaxRows)
				offset = (MAX_SCREENS-1) * MaxRows;
			break;
		case '/':
			put_command_prompt('/');
			get_command_argument(str);
			offset = search_screen(screen, str, 0);
			break;
		case '?':
			print_help(screen);
			break;
		case 'n':
			offset = search_screen(screen, str, offset+1);
			break;
		case 's':
			/* fetch and print super block */
			address = EXT4_SUPER_BLOCK0_OFFSET;
			lseek(fd, address, SEEK_SET);
			read(fd, sb, sizeof(super_block));
			dump_super_block(screen, &super_block);
			offset = 0;
			break;
		case 'b':
			/* fetch and print block group descriptor */
			address = BlockSize;
			lseek(fd, address, SEEK_SET);
			read(fd, bg, sizeof(group_desc));
			dump_group_desc(screen, &group_desc);
			offset = 0;
			break;
		default:
			break;
		}

		end = print_screen(screen, offset, MaxRows);
		/* menu for further action */
		print_menu();

	}

	printw("\n\Exiting Now\n");
	endwin();

	if (screen != NULL) {
		free(screen);
	}
	close(fd);
	return 0;
}
