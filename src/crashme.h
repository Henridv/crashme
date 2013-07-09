#ifndef __CRASHME_H__
#define __CRASHME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef void (*BADBOY)(void);

extern int kill(pid_t, int);
void badboy_loop(void);

void open_record(void);
void close_record(void);
void record_note(void);
void note(long level);

unsigned char *bad_malloc(long);
void again_handler(int);
void my_signal(int, void (*func)(int));    /* GLO: 2nd arg det'd by experiment */
void set_up_signals(void);
void compute_badboy_1(long);

void proto_badboy(void);
BADBOY castaway(unsigned char *);
void compute_badboy(void);

int	 record_status(long);
void try_one_crash(void);
void copyright_note(long);
void old_main(int, char **);
void summarize_status(void);
void monitor_fcn(int sig);
void vfork_main(long, long, char *, char *, long, char *);

#ifdef __cplusplus
}
#endif

#endif /* __CRASHME_H__ */
