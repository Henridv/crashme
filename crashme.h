#ifndef __CRASHME_H__
#define __CRASHME_H__


#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

typedef void (*BADBOY)();

extern int kill(pid_t, int);
void old_main(), copyright_note(), vfork_main(), badboy_loop();

void record_note(void);
void open_record(void);
void note(long);
unsigned char *bad_malloc(long);
void again_handler(int);
void my_signal(int, void (*func)());    /* GLO: 2nd arg det'd by experiment */
void set_up_signals(void);
void compute_badboy_1(long);

void proto_badboy(void);
BADBOY castaway(unsigned char *);
void compute_badboy(void);
void try_one_crash(void);
void copyright_note(long);
void old_main(int, char **);
int record_status(long);
void summarize_status(void);
void monitor_fcn(int sig);
void vfork_main(long, long, char *, char *, long, char *);

#endif
