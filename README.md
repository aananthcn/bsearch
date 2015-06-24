# bsearch
A tool to search patterns, header and other data structure within a binary file system dump

# How to build
Just clone this and give a make

# Usage
 1. bsearch [OPTIONS] -p <pattern_hex> -f <file_name>
 2. bsearch [OPTIONS] -c <command> -f <file_name>

   OPTIONS:
     -a		search for all occurance
     -t		text search, treats "pattern_hex" as text
     -c		commands (see COMMAND section below)
     -o		offset

   COMMANDS:
     dump	display contents of file with additional options
