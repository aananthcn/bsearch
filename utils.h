#ifndef UTILS_H
#define UTILS_H


#define MAX(a, b)	((a) > (b)) ?  (a) : (b)
#define MIN(a, b)	((a) < (b)) ?  (a) : (b)


void put_command_prompt(int c);
void get_command_argument(char *input);
int hexstring_to_int(const char *str);
void print_mem_onscreen(char *screen, int sz_y, int mem, int bpline);
void dump_mem_tobuffer(char *obuf, char *ibuf, int sz, char *msg,
		       int addr, int bpline);

#endif
