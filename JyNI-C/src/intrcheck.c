/* This File is based on intrcheck.c from CPython 2.7.6 release.
 * It has been modified to suit JyNI needs.
 *
 * Copyright of the original file:
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 * 2011, 2012, 2013, 2014, 2015 Python Software Foundation.  All rights reserved.
 *
 * Copyright of JyNI:
 * Copyright (c) 2013, 2014, 2015 Stefan Richthofer.  All rights reserved.
 *
 *
 * This file is part of JyNI.
 *
 * JyNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JyNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with JyNI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */


/* Check for interrupts */

#include "Python_JyNI.h"
#include "pythread_JyNI.h"

#ifdef QUICKWIN

#include <io.h>

void
PyOS_InitInterrupts(void)
{
}

void
PyOS_FiniInterrupts(void)
{
}

int
PyOS_InterruptOccurred(void)
{
    _wyield();
}

#define OK

#endif /* QUICKWIN */

#if defined(_M_IX86) && !defined(__QNX__)
#include <io.h>
#endif

#if defined(MSDOS) && !defined(QUICKWIN)

#ifdef __GNUC__

/* This is for DJGPP's GO32 extender.  I don't know how to trap
 * control-C  (There's no API for ctrl-C, and I don't want to mess with
 * the interrupt vectors.)  However, this DOES catch control-break.
 * --Amrit
 */

#include <go32.h>

void
PyOS_InitInterrupts(void)
{
    _go32_want_ctrl_break(1 /* TRUE */);
}

void
PyOS_FiniInterrupts(void)
{
}

int
PyOS_InterruptOccurred(void)
{
    return _go32_was_ctrl_break_hit();
}

#else /* !__GNUC__ */

/* This might work for MS-DOS (untested though): */

void
PyOS_InitInterrupts(void)
{
}

void
PyOS_FiniInterrupts(void)
{
}

int
PyOS_InterruptOccurred(void)
{
    int interrupted = 0;
    while (kbhit()) {
        if (getch() == '\003')
            interrupted = 1;
    }
    return interrupted;
}

#endif /* __GNUC__ */

#define OK

#endif /* MSDOS && !QUICKWIN */


#ifndef OK

/* Default version -- for real operating systems and for Standard C */

#include <stdio.h>
#include <string.h>
#include <signal.h>

static int interrupted;

void
PyErr_SetInterrupt(void)
{
    interrupted = 1;
}

extern int PyErr_CheckSignals(void);

static int
checksignals_witharg(void * arg)
{
    return PyErr_CheckSignals();
}

static void
intcatcher(int sig)
{
    extern void Py_Exit(int);
    static char message[] =
"python: to interrupt a truly hanging Python program, interrupt once more.\n";
    switch (interrupted++) {
    case 0:
        break;
    case 1:
#ifdef RISCOS
        fprintf(stderr, message);
#else
        write(2, message, strlen(message));
#endif
        break;
    case 2:
        interrupted = 0;
        Py_Exit(1);
        break;
    }
    PyOS_setsig(SIGINT, intcatcher);
    Py_AddPendingCall(checksignals_witharg, NULL);
}

static void (*old_siginthandler)(int) = SIG_DFL;

void
PyOS_InitInterrupts(void)
{
    if ((old_siginthandler = PyOS_setsig(SIGINT, SIG_IGN)) != SIG_IGN)
        PyOS_setsig(SIGINT, intcatcher);
}

void
PyOS_FiniInterrupts(void)
{
    PyOS_setsig(SIGINT, old_siginthandler);
}

int
PyOS_InterruptOccurred(void)
{
    if (!interrupted)
        return 0;
    interrupted = 0;
    return 1;
}

#endif /* !OK */

void
PyOS_AfterFork(void)
{
#ifdef WITH_THREAD
    PyEval_ReInitThreads();
    PyThread_ReInitTLS();
#endif
}
