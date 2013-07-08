char *crashme_version = "2.45 13-JUN-2006";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>

#include "crashme.h"

#ifdef VMS
#include <processes.h>
#endif

#ifdef pyr
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/vmmac.h>
#define strchr index
#endif

/* GLO    Global variables - CAUTION   */

BADBOY badboy;

long nbytes, nseed, ntrys;
long incptr = 0;
long offset = 0;
long next_offset = 0;
long malloc_flag = 0;
unsigned char *the_data;
char *note_buffer;
char *notes;

long verbose_level = 5;

FILE *logfile;

char *subprocess_ind = "subprocess";   /* move to global variable section */

void record_note()
{
    char *logfilename;

    if (!(logfilename = getenv("CRASHLOG")))
        return;

    if (!(logfile = fopen(logfilename,
                          (strncmp(note_buffer, "Subprocess", 10) == 0)
                          ? "a" : "w")))
    {
        perror(logfilename);
        return;
    }

    if (note_buffer[strlen(note_buffer) - 1] != '\n')
        strcat(note_buffer, "\n");

    fputs(note_buffer, logfile);
    fclose(logfile);
    logfile = NULL;
    return ;
}              /* end of record_note()  */

void open_record()
{
    char *logfilename;

    if (!(logfilename = getenv("CRASHLOG")))
        return;

    if (!(logfile = fopen(logfilename, "a")))
    {
        perror(logfilename);
        return;
    }

}              /* end of open_record() */

void close_record()
{
    if (logfile)
    {
        fclose(logfile);
        logfile = NULL;
    }
}

void note(long level)
{
    if (level > verbose_level)
        return;

    strcat(note_buffer, "\n");
    fputs(note_buffer, stdout);
    fflush(stdout) ;                    /* flush the output */

    if (logfile)
    {
        fputs(note_buffer, logfile);
        fflush(logfile) ;               /* flush the buffer to the file */
    }
}

jmp_buf again_buff;

unsigned char *bad_malloc(long n)            /* GLO - this is function bad_malloc */
{
    unsigned char *data;
    data = (unsigned char *) malloc(n);

#ifdef pyr

    if (mprotect(((int)data / PAGSIZ)*PAGSIZ, (n / PAGSIZ + 1)*PAGSIZ,
                 PROT_READ | PROT_WRITE | PROT_EXEC))
        perror("mprotect");

#endif

    return (data);
}

void again_handler(int sig)
{
    char *ss;
    switch (sig)
    {
    case SIGILL:  ss =   " illegal instruction"; break;

#ifdef SIGTRAP
    case SIGTRAP: ss =   " trace trap"; break;
#endif

    case SIGFPE:  ss =   " arithmetic exception"; break;

#ifdef SIGBUS
    case SIGBUS:  ss =  " bus error"; break;
#endif

    case SIGSEGV: ss =  " segmentation violation"; break;

#ifdef SIGIOT
    case SIGIOT:  ss = " IOT instruction"; break;
#endif

#ifdef SIGEMT
    case SIGEMT:  ss = " EMT instruction"; break;
#endif

#ifdef SIGALRM
    case SIGALRM: ss = " alarm clock"; break;
#endif

    case SIGINT:  ss = " interrupt"; break;

    default:      ss = "";  break;

    }    /* end of switch(sig) */


    sprintf(notes, "Got signal %d%s", sig, ss);
    note(5);
    longjmp(again_buff, 3);

}     /* end of again_handler(sig) */

#ifdef MASK_EXAMPLE

sigset_t        olwpset;

status = sigprocmask(0, NULL, &olwpset);
if (status < 0)
{
    ErrorMessagePtr = strerror (errno) ;    // get the error no. string
    cerr << "ERROR: number = " << errno << " which translates as: "
         << endl << "  "  << ErrorMessagePtr << endl ;
    cerr << "  Call to sigprocmask (0, NULL, &olwpset = 0x"
         << hex << &olwpset << "  failed.  " << dec
         << endl ;
    return (EXIT_MASK_FAIL) ;               // return failure status

}

sigset_t         target_set, old_set ;
status = sigprocmask(SIG_UNBLOCK, &target_set, &old_set);
act.sa_mask = target_set ;

#endif    /* end of #ifdef MASK_EXAMPLE */



void my_signal(sig, func)
int sig;
void func(int) ;
{
#ifndef SA_ONESHOT
    signal(sig, func);
#else

    struct sigaction act;
    sigset_t         target_set, old_set ;
    int status;                    /* local status variable */


    status = sigprocmask(SIG_UNBLOCK, &target_set, &old_set) ;
    act.sa_mask = target_set ;
    act.sa_handler = func;

#ifdef OLD_MASK
    act.sa_mask = SA_NOMASK ;      /* Linux data type mismatch */
#endif

#ifdef __linux__
    act.sa_restorer = 0;
#endif /* linux */

    act.sa_flags = SA_NOMASK;

#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;
#endif

    sigaction(sig, &act, 0);

#endif /* SA_ONESHOT */
}

void set_up_signals(void)
{
    my_signal(SIGILL, again_handler);

#ifdef SIGTRAP
    my_signal(SIGTRAP, again_handler);
#endif

    my_signal(SIGFPE, again_handler);

#ifdef SIGBUS
    my_signal(SIGBUS, again_handler);
#endif

    my_signal(SIGSEGV, again_handler);

#ifdef SIGIOT
    my_signal(SIGIOT, again_handler);
#endif

#ifdef SIGEMT
    my_signal(SIGEMT, again_handler);
#endif

#ifdef SIGALRM
    my_signal(SIGALRM, again_handler);
#endif

    my_signal(SIGINT, again_handler);
}

void compute_badboy_1(n)
long n;
{
    long j;
    if (malloc_flag == 1)
        the_data = bad_malloc(n);

    for (j = 0; j < n; ++j)
        the_data[j] = (rand() >> 7) & 0xFF;

    if (nbytes < 0)
    {
        sprintf(notes, "Dump of %ld bytes of data", n);
        note(1);
        for (j = 0; j < n; ++j)
        {
            fprintf(stdout, "%3d", the_data[j]);
            if ((j % 20) == 19)
                putc('\n', stdout);

            else
                putc(' ', stdout);
        }
        putc('\n', stdout);
        fflush (stdout) ;
    }

}     /* end of compute_badboy_1(n)    */

void proto_badboy()
{
    printf("Hello world.\n");
}

#if defined(__ALPHA) && defined(VMS) && !defined(NOCASTAWAY)
#include <pdscdef.h>
#endif

BADBOY castaway(unsigned char *dat)
{

#if defined(VAX) && !defined(NOCASTAWAY)
    /* register save mask avoids bashing our callers locals */

    ((unsigned short *)dat)[0] = 0x0FFC;

#endif

#if defined(__ALPHA) && defined(VMS) && !defined(NOCASTAWAY)

    struct pdscdef *p, *b;
    p = (struct pdscdef *) proto_badboy;
    b = (struct pdscdef *) dat;
    memcpy(b, p, sizeof(struct pdscdef));
    b->pdsc$q_entry[1] = 0;
    b->pdsc$q_entry[0] = (int)&dat[sizeof(struct pdscdef)];

#endif

#if (defined(_IBMR2) || defined(_POWER)) && !defined(NOCASTAWAY)

    struct fdesc
    {
        void *fcn_addr;
        void *toc;
        void *linkage;
    } *p, *b;
    p = (struct fdesc *) proto_badboy;
    b = (struct fdesc *) dat;
    memcpy(b, p, sizeof(struct fdesc));
    b->fcn_addr = (void *) &dat[sizeof(struct fdesc)];
#endif

#if defined(__hpux) && defined(__hppa) && !defined(NOCASTAWAY)

    struct fdesc
    {
        void *fcn_addr;
        void *toc;
    } *p, *b;
    p = (struct fdesc *) proto_badboy;
    b = (struct fdesc *) dat;
    memcpy(b, p, sizeof(struct fdesc));
    b->fcn_addr = (void *) &dat[sizeof(struct fdesc)];

#endif
    return ((BADBOY)dat);
}

/*
 * If nbytes is less than zero, n = nbytes * -1; else n = nbytes.
 */

void compute_badboy()
{
    long n;

    n = (nbytes < 0) ? - nbytes : nbytes;
    if (incptr == 0)
    {
        compute_badboy_1(n);
        badboy = castaway(the_data);
    }
    else if ((next_offset == 0) || (next_offset > ((n * 90) / 100)))
    {
        compute_badboy_1(n);
        offset = 0;
        next_offset = offset + incptr;
        badboy = castaway(the_data);
    }
    else
    {
        offset = next_offset;
        next_offset = offset + incptr;
        badboy = castaway(&the_data[offset]);
    }

}     /* end of void compute_badboy()    */

/* maybe add this code before invoking badboy. But it didn't seem
   to be needed when using +1000.
   FlushInstructionCache(GetCurrentProcess(),
                         the_data,(nbytes < 0) ? - nbytes : nbytes);
*/

void try_one_crash()
{
    if (nbytes > 0)
        (*badboy)();

    else if (nbytes == 0)
        while (1);

    else
        return;
}



int main(int argc, char **argv)
/*
 * int argc; char **argv;
 */
{
    long nsubs, hrs, mns, scs, tflag, j, m;
    note_buffer = (char *) malloc(512);
    notes = note_buffer;
    if ((argc == 7) &&
            (strcmp(argv[6], subprocess_ind) == 0))
    {
        sprintf(note_buffer, "Subprocess %s: ", argv[4]);
        notes = note_buffer + strlen(note_buffer);
        verbose_level = atol(argv[5]);
        sprintf(notes, "starting");
        note(3);
        old_main(4, argv);
    }
    else if (argc == 4)
        old_main(4, argv);

    else if ((argc == 6) && ((strlen(argv[4]) == 0) ||
                             (strcmp(argv[4], ".") == 0)))
    {
        verbose_level = atol(argv[5]);
        old_main(4, argv);
    }
    else if ((argc == 5) || (argc == 6))
    {
        if (argc == 6)
            verbose_level = atol(argv[5]);

        copyright_note(1);
        if (argc < 7)
            m = argc;
        else
            m = 6;

        strcpy(notes, "crashme");
        for (j = 1; j < m; ++j)
        {
            strcat(notes, " ");
            strcat(notes, argv[j]);
        }

        note(1);
        record_note();
        if (strchr(argv[4], ':'))
        {
            sscanf(argv[4], "%ld:%ld:%ld", &hrs, &mns, &scs);
            tflag = 1;
            nsubs = (((hrs * 60) + mns) * 60) + scs;
            sprintf(notes, "Subprocess run for %ld seconds (%ld %02ld:%02ld:%02ld)",
                    nsubs, hrs / 24, hrs % 24, mns, scs);
        }
        else
        {
            tflag = 0;
            nsubs = atol(argv[4]);
            sprintf(notes, "Creating %ld crashme subprocesses", nsubs);
        }

        note(1);
        vfork_main(tflag, nsubs, argv[0], argv[1], atol(argv[2]), argv[3]);
    }
    else
    {
        sprintf(notes,
                "crashme [+]<nbytes>[.inc] <srand> <ntrys> [nsub] [verbose]");
        note(0);
    }

    return (EXIT_SUCCESS);
}

void copyright_note(long n)
{
    sprintf(notes, "Crashme: (c) Copyright 1990-1994 George J. Carrette");
    note(n);
    sprintf(notes, "Version: %s", crashme_version);
    note(n);
}

void old_main(int argc, char **argv)
{
    char *ptr;
    copyright_note(3);
    nbytes = atol(argv[1]);
    if ( (ptr = strchr(argv[1], '.')) )
        incptr = atol(&ptr[1]);

    if (argv[1][0] == '+')
        malloc_flag = 1;

    nseed = atol(argv[2]);
    ntrys = atol(argv[3]);
    sprintf(notes, "crashme %s%ld.%ld %ld %ld",
            (malloc_flag == 0) ? "" : "+", nbytes, incptr, nseed, ntrys);
    note(3);
    record_note();
    if (malloc_flag == 0)
    {
        the_data = bad_malloc((nbytes < 0) ? -nbytes : nbytes);
        badboy = castaway(the_data);
        sprintf(notes, "Badboy at %d. 0x%X", (int)badboy, (int)badboy);
        note(3);
    }
    srand(nseed);

    badboy_loop();
}                        /* end of old_main()  */

void badboy_loop(void)
{
    int i;
    for (i = 0; i < ntrys; ++i)
    {
        compute_badboy();
        if (offset)
            sprintf(notes, "try %d, offset %ld", i, offset);
        else if (malloc_flag == 1)
            sprintf(notes, "try %d, Badboy at %d. 0x%X", i, (int)badboy, (int)badboy);
        else
            sprintf(notes, "try %d", i);

        note(5);

        if (setjmp(again_buff) == 3)
        {
            sprintf(notes, "Barfed");
            note(5);
        }
        else
        {
            set_up_signals();
            alarm(10);
            try_one_crash();
            sprintf(notes, "didn't barf!");
            note(5);
        }
    }         /* end of for(i = 0; i < ntrys; ++i)  */

}            /* end of badboy() */

struct status_list
{
    long status;
    long count;
    struct status_list *next;
};

struct status_list *slist = NULL;

int record_status(long n)
/*
 * long n;
 */
{
    struct status_list *l;
    for (l = slist; l != NULL; l = l->next)

        if (n == l->status)
            return (++l->count);

    l = (struct status_list *) malloc(sizeof(struct status_list));
    l->count = 1;
    l->status = n;
    l->next = slist;
    slist = l;
    return (1);
}

void summarize_status(void)
{
    struct status_list *l;
    sprintf(notes, "exit status ... number of cases");
    note(2);
    for (l = slist; l != NULL; l = l->next)
    {
        sprintf(notes, "exit status ... number of cases");
        sprintf(notes, "%11ld ... %5ld", l->status, l->count);
        note(2);
    }
}

long monitor_pid = 0;
long monitor_period = 5;
long monitor_limit =  6; /* 30 second limit on a subprocess */
long monitor_count = 0;
long monitor_active = 0;

void monitor_fcn(int sig)
{
    long status;
    my_signal(SIGALRM, monitor_fcn);
    alarm(monitor_period);
    if (monitor_active)
    {
        ++monitor_count;
        if (monitor_count >= monitor_limit)
        {
            sprintf(notes, "time limit reached on pid %ld 0x%lX. using kill.",
                    monitor_pid, monitor_pid);
            note(3);
            status = kill(monitor_pid, SIGKILL);
            if (status < 0)
            {
                sprintf(notes, "failed to kill process");
                note(3);
            }
            monitor_active = 0;
        }
    }
}

void vfork_main(long tflag, long nsubs, char *cmd,  char *nb, long sr, char *nt)
{
    long j, pid, n, seq, total_time, dys, hrs, mns, scs;
    int status;
    char arg2[20], arg4[20], arg5[20];
    time_t before_time, after_time;
    if (tflag == 1)
    {
        seq = 1;
        n = 100000000;
    }
    else if (nsubs < 0)
    {
        n = -nsubs;
        seq = 1;
    }
    else
    {
        n = nsubs;
        seq = 0;
    }

    if (seq == 1)
    {
        my_signal(SIGALRM, monitor_fcn);
        alarm(monitor_period);
    }

    time(&before_time);
    sprintf(arg5, "%ld", verbose_level);
    for (j = 0; j < n; ++j)
    {
        sprintf(arg2, "%ld", sr + j);
        sprintf(arg4, "%ld", j + 1);

#ifdef VMS
        status = vfork();
#else
        status = fork();
#endif

        if (status == 0)
        {
            status = execl(cmd, cmd, nb, arg2, nt, arg4, arg5, subprocess_ind, (char *)0);
            if (status == -1)
            {
                perror(cmd);
                exit(1);
            }
        }
        else if (status < 0)
        {
            perror(cmd);
        }
        else
        {
            sprintf(notes, "pid = %d 0x%X (subprocess %ld)", status, status, j + 1);
            note(3);
            if (seq == 1)
            {
                monitor_pid = status;
                monitor_count = 0;
                monitor_active = 1;
                while ((pid = wait(&status)) > 0)
                {
                    monitor_active = 0;
                    sprintf(notes, "pid %ld 0x%lX exited with status %d",
                            pid, pid, status);
                    note(3);
                    record_status(status);
                }

                /* GLO - should monitoring be re-enabled here? */

            }

            if (tflag == 1)
            {
                time(&after_time);
                total_time = after_time - before_time;
                if (total_time >= nsubs)
                {
                    sprintf(notes, "Time limit reached after run %ld", j + 1);
                    note(2);
                    break;
                }
            }
        }                /* end of if (status == 0) else   */
    }                   /* end of for(j = 0; j < n; ++j)  */

    if (seq == 0)
        while ((pid = wait(&status)) > 0)
        {
            sprintf(notes, "pid %ld 0x%lX exited with status %d", pid, pid, status);
            note(3);
            record_status(status);
        }

    time(&after_time);
    total_time = after_time - before_time;
    scs = total_time;
    mns = scs / 60;
    hrs = mns / 60;
    dys = hrs / 24;
    scs = scs % 60;
    mns = mns % 60;
    hrs = hrs % 24;
    open_record();
    sprintf(notes,
            "Test complete, total real time: %ld seconds (%ld %02ld:%02ld:%02ld)",
            total_time, dys, hrs, mns, scs);
    note(1);
    summarize_status();
    close_record();
}

