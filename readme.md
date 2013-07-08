CRASHME
=======

See the documentation in crashme.txt or read this document for a
description of command line arguments to this program.

USAGE
-----
crashme: Create a string of random bytes and then jump to it.

    crashme [+]<nbytes>[.inc] <srand> <ntrys> [nsub] [verboseness]

Suggested test: At least let the thing run the length of your lunch break,
in this case 1 hour, 10 minutes, and 30 seconds.

    crashme +2000 666 100 1:10:30 2

Also, it may spend more time trapping and less time computing random bytes
by using these arguments:

    crashme +2000.80 666 100 1:10:30 2

CRASH REPORTS
-------------

<pre>
Date,         Machine                    Crashme        Reported
Crashme Ver   Make    Model OS Version   Arguments      by:

10-JUL-90 1.0 SUN     4/110 4.1          1000 20 200    GJC@paradigm.com
10-JUL-90 1.0 SUN     4/280 4.0.3        1000 20 200    GJC@paradigm.com
31-JUL-90 1.0 DIGITAL DECstation 3100    100 10 10000   GAVRON@ARIZONA.EDU
31-JUL-90 1.0 IBM     RT                 100 10 10000   GAVRON@ARIZONA.EDU
 1-AUG-90 1.0 DIGITAL DECstation 5000    10000 230 1000 hudgens@scri.fsu.edu
 3-AUG-90 1.0 Alliant FX/2800                           SJA@SIRIUS.HUT.FI
27-JUN-91 1.2 SUN     4/110 4.1.1        10 1000 10     LPH@PARADIGM.COM
27-JUN-91 1.2 SUN     4/110 4.1.1        1000 20 200 10 LPH@PARADIGM.COM
29-JUN-91 1.2 SUN     4/40C 4.1.1        9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/60 4.1.1         9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/100 4.1.1        9 29748 5877 4 jon@uk.ac.oxford.robots
29-JUN-91 1.2 SUN     4/65 4.1.1         9 29748 5877 4 jon@uk.ac.oxford.robots
18-JUL-91 1.2 SGI     Iris4d Unix 3.3.2  1000 $$ 1000 4 tsacas@ilog.ilog.fr
29-JUL-91 1.1 IBM     RS/6000 AIX 1.3    script         brandis@inf.ethz.ch
 5-SEP-91 1.6 IBM     RS/6000-320 AIX 3.1.5 +2000 666 50 40:00:00 LPH
26-SEP-91 1.8 Nixdorf Targon/35 TOS3.3   script         petri@ibr.cs.tu-bs.de
 9-SEP-93 2.0 Microsoft WNT Build 511 i486 +1000 24131 50 gjc@mitech.com
 3-FEB-94 1.8 HP710/HP-UX 9.00 +2000 666 100 2:00:00 5 UFOP@fpsp.fapesp.br
 5-MAY-94 2.0 HP807/HPUX 9.00  4000 666 100 00:30:00 2 UFOP@fpsp.fapesp.br
</pre>

Notes
-----
Crashme V1.0 {1000 20 200} used to down the SUN 4/110. V1.2 does *not*
crash SUNOS 4.1.1 on the same arguments. Although using the extra argument
for subprocesses it will crash, with the console reporting:
"Bad Trap, Bad Kernel Read Fault, Bus error. Reboot"

Script means invoking file with many calls to crashme such as this:

    #/bin/csh
    crashme 1020 234 500 &
    crashme 394 38484 5723 &
    crashme 3784 474 474 &
    crashme 437 4747 38 &
    crashme 47848 4745 123 &
    crashme 4747 4747 3463 &
    crashme 474 46464 262 &
    crashme 37 3644 3723 &
    crashme 374 46464 22 &
    crashme 3747 464 363 &
    crashme 347 4747 44 &
    crashme 37374 374 66 &
    crashme 3737 474 4444 &

The 4-argument case of crashme could likely do as well as executing
a script.

<pre>
25-SEP-2005 GLO - Add a #define for Solaris
28-SEP-2005 GLO - Minor edits to improve readability for me & no one else
03-OCT-2005 GLO - Reformat the code for additional white space.  This is
                    for my readability and no one else's.
14-JUN-2006 GLO - Modifications to comply with 'gcc' (Linux/Posix) signaling
                    standards
29-JUN-2007 GLO - For Solaris X64, add a "#define solaris"
                  For -Xc add extern int kill(,) even though signal.h
                    defines it
</pre>
