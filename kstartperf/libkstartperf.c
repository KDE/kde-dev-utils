/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * libkstartperf.c: LD_PRELOAD library for startup time measurements.
 *
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *
 * Based heavily on kmapnotify.c:
 * 
 * (C) 2000 Rik Hemsley <rik@kde.org>
 * (C) 2000 Simon Hausmann <hausmann@kde.org>
 * (C) 2000 Bill Soudan <soudan@kde.org>
 */

#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <ltdl.h>


/* Prototypes */

int XMapWindow(Display *, Window);
int XMapRaised(Display *, Window);
void KDE_InterceptXMapRequest(Display *, Window);
void KDE_ShowPerformance();

/* Globals */

typedef Window (*KDE_XMapRequestSignature)(Display *, Window);
KDE_XMapRequestSignature KDE_RealXMapWindow = 0L;
KDE_XMapRequestSignature KDE_RealXMapRaised = 0L;


/* Functions */

int XMapWindow(Display * d, Window w)
{
    if (KDE_RealXMapWindow == 0L)
    {
	KDE_InterceptXMapRequest(d, w);
	KDE_ShowPerformance();
    }
    return KDE_RealXMapWindow(d, w);
}

int XMapRaised(Display * d, Window w)
{
    if (KDE_RealXMapRaised == 0L)
    {
	KDE_InterceptXMapRequest(d, w);
	KDE_ShowPerformance();
    }
    return KDE_RealXMapRaised(d, w);
}

void KDE_InterceptXMapRequest(Display * d, Window w)
{
    lt_dlhandle handle;

    handle = lt_dlopen("libX11.so");
    if (handle == 0L)
	handle = lt_dlopen("libX11.so.6");

    if (handle == 0L)
    {
	fprintf(stderr, "kstartperf: Could not dlopen libX11\n");
	exit(1);
    }

    KDE_RealXMapWindow = (KDE_XMapRequestSignature)lt_dlsym(handle, "XMapWindow");
    if (KDE_RealXMapWindow == 0L)
    {
	fprintf(stderr, "kstartperf: Could not find symbol XMapWindow in libX11\n");
	exit(1);
    }

    KDE_RealXMapRaised = (KDE_XMapRequestSignature)lt_dlsym(handle, "XMapRaised");
    if (KDE_RealXMapRaised == 0L)
    {
	fprintf(stderr, "kstartperf: Could not find symbol XMapRaised in libX11\n");
	exit(1);
    }
}

void KDE_ShowPerformance()
{
    char *env;
    long l1, l2;
    float dt;
    struct timeval tv;

    env = getenv("KSTARTPERF");
    if (env == 0L)
    {
	fprintf(stderr, "kstartperf: $KSTARTPERF not set!\n");
	exit(1);
    }
    if (sscanf(env, "%ld:%ld", &l1, &l2) != 2)
    {
	fprintf(stderr, "kstartperf: $KSTARTPERF illegal format\n");
	exit(1);
    }

    if (gettimeofday(&tv, 0L) != 0)
    {
	fprintf(stderr, "kstartperf: gettimeofday() failed.\n");
	exit(1);
    }
    
    dt = 1e3*(tv.tv_sec - l1) + 1e-3*(tv.tv_usec - l2);
    fprintf(stderr, "\nkstartperf: measured startup time at %7.2f ms\n\n", dt);
}

