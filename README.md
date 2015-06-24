# bsearch
A tool to search patterns, header and other data structure within a binary file system dump

# How to build
Just clone this and give a make

# Usage
 1. bsearch [OPTIONS] -p <pattern_hex> -f <file_name>
 2. bsearch [OPTIONS] -c <command> -f <file_name> <br>
<br>
   OPTIONS: <br>
     -a		search for all occurance <br>
     -t		text search, treats "pattern_hex" as text <br>
     -c		commands (see COMMAND section below) <br>
     -o		offset <br>
<br>
   COMMANDS: <br>
     dump	display contents of file with additional options <br>
