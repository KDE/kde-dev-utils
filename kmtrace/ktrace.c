/* More debugging hooks for `malloc'.
   Copyright (C) 1991,92,93,94,96,97,98,99,2000 Free Software Foundation, Inc.
		 Written April 2, 1991 by John Gilmore of Cygnus Support.
		 Based on mcheck.c by Mike Haertel.
		 Hacked by AK
		 Cleanup and performance improvements by
		 Chris Schlaeger <cs@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   The author may be reached (Email) at the address mike@ai.mit.edu,
   or (US mail) as Mike Haertel c/o Free Software Foundation.
*/

#define _LIBC
#define MALLOC_HOOKS
#define _GNU_SOURCE

#ifndef	_MALLOC_INTERNAL
#define	_MALLOC_INTERNAL
#include <malloc.h>
#include <bits/libc-lock.h>
#endif

#include <dlfcn.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef USE_IN_LIBIO
# include <libio/iolibio.h>
# define setvbuf(s, b, f, l) _IO_setvbuf (s, b, f, l)
#endif

/* This is the most important parameter. It should be set to
 * two times the maximum number of mallocs the application
 * uses at a time. IIRC prime numbers are very good candidates
 * for this value.
 * I added a list of some prime numbers for conveniance.
 * 10007, 20011, 30013, 40031, 50033, 60037, 70039, 80051, 90053, 100057,
 * 110059, 120067, 130069, 140071, 150077, 160079, 170081, 180097, 190121,
 * 200131, 210139, 220141, 230143, 240151, 250153, 260171, 270191, 280199,
 * 290201, 300221, 310223, 320237, 330241, 340261, 350281, 360287, 370373,
 * 380377, 390389
 */
#define TR_CACHE_SIZE 80051

/* The DELTA value is also a value for the maximum
 * number of iterations during a positive free/realloc
 * search. It must NOT divide TR_CACHE_SIZE without
 * remainder! */
#define DELTA 23

/* The high and low mark control the flushing algorithm.
 * Whenever the hashtable reaches the high mark every
 * DELTAth entry is written to disk until the low
 * filling mark is reached. */
#define TR_HIGH_MARK ((int) (TR_CACHE_SIZE * 0.5))
#define TR_LOW_MARK ((int) ((TR_CACHE_SIZE * 0.5) - (TR_CACHE_SIZE / DELTA)))

/* Maximum call stack depth. No checking for overflows
 * is done. Adjust this value with care! */
#define TR_BT_SIZE		60

#define PROFILE 1

/* The hash function. Since the smallest allocated block is probably
 * not smaller than 8 bytes we ignore the last 3 LSBs. */
#define HASHFUNC(a) ((((unsigned long) a) >> 3) % TR_CACHE_SIZE)

#define TR_HASHTABLE_SIZE	9973

#define TR_NONE		0
#define TR_MALLOC	1
#define TR_REALLOC	2
#define TR_FREE		3

#define TRACE_BUFFER_SIZE 512

void kuntrace(void);

static void tr_freehook __P ((__ptr_t, const __ptr_t));
static __ptr_t tr_reallochook __P ((__ptr_t, __malloc_size_t,
									const __ptr_t));
static __ptr_t tr_mallochook __P ((__malloc_size_t, const __ptr_t));
/* Old hook values.  */
static void (*tr_old_free_hook) __P ((__ptr_t ptr, const __ptr_t));
static __ptr_t (*tr_old_malloc_hook) __P ((__malloc_size_t size,
										   const __ptr_t));
static __ptr_t (*tr_old_realloc_hook) __P ((__ptr_t ptr,
											__malloc_size_t size,
											const __ptr_t));

static FILE* mallstream;
static const char mallenv[]= "MALLOC_TRACE";
static char malloc_trace_buffer[TRACE_BUFFER_SIZE];


/* Address to breakpoint on accesses to... */
__ptr_t mallwatch;

__libc_lock_define_initialized (static, lock);


typedef struct
{
	__ptr_t ptr;
	__malloc_size_t size;
	int bt_size;
	void** bt;
} tr_entry;

static int bt_size;
static void *bt[TR_BT_SIZE + 1];
static char tr_offsetbuf[20];
static tr_entry tr_cache[TR_CACHE_SIZE];
static int tr_cache_level;
static int tr_cache_pos;
static void *tr_hashtable[TR_HASHTABLE_SIZE];
#ifdef PROFILE
static unsigned long tr_mallocs = 0;
static unsigned long tr_logged_mallocs = 0;
static unsigned long tr_frees = 0;
static unsigned long tr_logged_frees = 0;
static unsigned long tr_current_mallocs = 0;
static unsigned long tr_max_mallocs = 0;
static unsigned long tr_flashes = 0;
static unsigned long tr_failed_free_lookups = 0;
static unsigned long tr_malloc_collisions = 0;
#endif

/* This function is called when the block being alloc'd, realloc'd, or
 * freed has an address matching the variable "mallwatch".  In a
 * debugger, set "mallwatch" to the address of interest, then put a
 * breakpoint on tr_break.  */
void tr_break __P ((void));
void
tr_break()
{
}

static void __inline__
tr_backtrace(void **bt, int size)
{
	int i;
	Dl_info info;
	for (i = 0; i < size; i++)
	{
		long hash = (((unsigned long)bt[i]) / 4) % TR_HASHTABLE_SIZE;
		if ((tr_hashtable[hash]!= bt[i]) && dladdr(bt[i], &info) &&
			info.dli_fname  && *info.dli_fname)
		{
			if (bt[i] >= (void *) info.dli_saddr)
				sprintf(tr_offsetbuf, "+%#lx", (unsigned long)
						(bt[i] - info.dli_saddr));
			else
				sprintf(tr_offsetbuf, "-%#lx", (unsigned long)
						(info.dli_saddr - bt[i]));
			fprintf(mallstream, "%s%s%s%s%s[%p]\n",
					info.dli_fname ?: "",
					info.dli_sname ? "(" : "",
					info.dli_sname ?: "",
					info.dli_sname ? tr_offsetbuf : "",
					info.dli_sname ? ")" : "",
					bt[i]);
			tr_hashtable[hash] = bt[i];
		}
		else
		{
			fprintf(mallstream, "[%p]\n", bt[i]);
		}
	} 
}

static void __inline__
tr_log(const __ptr_t caller, __ptr_t ptr, __ptr_t old,
	   __malloc_size_t size, int op)
{
	int i, end;

	switch (op)
	{
	case TR_FREE:
		i = end = HASHFUNC(ptr);
		do
		{
			if (tr_cache[i].ptr == ptr)
			{
				tr_cache[i].ptr = NULL;
				free(tr_cache[i].bt);
				tr_cache_level--;
				return;
			}
			if (++i >= TR_CACHE_SIZE)
				i = 0;
#ifdef PROFILE
			tr_failed_free_lookups++;
#endif
		} while (i != end);

		/* We don't know this allocation, so it has been flushed to disk
		 * already. So flush free as well. */
		fprintf(mallstream, "@\n");
		bt_size = backtrace(bt, TR_BT_SIZE);
		tr_backtrace(&(bt[1]), bt_size - 2);
		fprintf(mallstream, "- %p\n", ptr);
#ifdef PROFILE
		tr_logged_frees++;
#endif
		return;

	case TR_REALLOC:
		/* If old is 0 it's actually a malloc. */
		if (old)
		{
			i = end = HASHFUNC(old);
			do
			{
				if (tr_cache[i].ptr == old)
				{
					int j = HASHFUNC(ptr);
					/* We move the entry otherwise the free will be
					 * fairly expensive due to the wrong place in the
					 * hash table. */
					tr_cache[i].ptr = NULL;
					for ( ; ; )
					{
						if (tr_cache[j].ptr == NULL)
							break;

						if (++j >= TR_CACHE_SIZE)
							i = 0;
					}
					tr_cache[j].ptr = ptr;
					if (ptr)
					{
						tr_cache[j].size = tr_cache[i].size;
						tr_cache[j].bt_size = tr_cache[i].bt_size;
						tr_cache[j].bt = tr_cache[i].bt;
					}
					else
						tr_cache_level--;
					tr_cache[i].size = size;
					return;
				}
				if (++i >= TR_CACHE_SIZE)
					i = 0;
			} while (i != end);
			fprintf(mallstream, "@\n");
			bt_size = backtrace(bt, TR_BT_SIZE);
			tr_backtrace(&(bt[1]), bt_size - 2);
			fprintf(mallstream, "< %p\n", old);
			fprintf(mallstream, "> %p %#lx\n", ptr,
					(unsigned long) size);
			return;
		}

	case TR_MALLOC:
		if (tr_cache_level >= TR_HIGH_MARK)
		{
			/* The hash table becomes ineffective when the high mark has
			 * been reached. We still need some more experience with
			 * the low mark. It's unclear what reasonable values are. */
			int pos = tr_cache_pos;
#ifdef PROFILE
			tr_flashes++;
#endif
			do
			{
				if (tr_cache[pos].ptr)
				{
#ifdef PROFILE
					tr_logged_mallocs++;
#endif
					fprintf(mallstream, "@\n");
					tr_backtrace(&(tr_cache[pos].bt[1]),
								 tr_cache[pos].bt_size - 2);
					fprintf(mallstream, "+ %p %#lx\n",
							tr_cache[pos].ptr, 
							(unsigned long int)
							tr_cache[pos].size);
					tr_cache[pos].ptr = NULL;
					tr_cache_level--;
				}
				if ((pos += DELTA) >= TR_CACHE_SIZE)
					pos %= TR_CACHE_SIZE;
			} while (tr_cache_level > TR_LOW_MARK);
		}

		i = HASHFUNC(ptr);
		for ( ; ; )
		{
			if (tr_cache[i].ptr == NULL)
				break;

			if (++i >= TR_CACHE_SIZE)
				i = 0;
#ifdef PROFILE
			tr_malloc_collisions++;
#endif
		}

		tr_cache[i].ptr = ptr; 
		tr_cache[i].size = size;
		tr_cache[i].bt = (void**) malloc(TR_BT_SIZE * sizeof(void*));
		tr_cache[i].bt_size = backtrace(
			tr_cache[i].bt, TR_BT_SIZE);
		realloc(tr_cache[i].bt, tr_cache[i].bt_size * sizeof(void*));
		tr_cache_level++;

		return;

	case TR_NONE:
		if (tr_cache[tr_cache_pos].ptr)
		{
#ifdef PROFILE
			tr_logged_mallocs++;
#endif
			fprintf(mallstream, "@\n");
			tr_backtrace(&(tr_cache[tr_cache_pos].bt[1]),
						 tr_cache[tr_cache_pos].bt_size - 2);
			fprintf(mallstream, "+ %p %#lx\n", 
					tr_cache[tr_cache_pos].ptr, 
					(unsigned long int)
					tr_cache[tr_cache_pos].size);
			tr_cache[tr_cache_pos].ptr = NULL;
			free(tr_cache[tr_cache_pos].bt);
			tr_cache_level--;
		}

		if (++tr_cache_pos >= TR_CACHE_SIZE)
			tr_cache_pos = 0;
		break;
	}
}

static void
tr_freehook (ptr, caller)
     __ptr_t ptr;
     const __ptr_t caller;
{
	if (ptr == NULL)
		return;

#ifdef PROFILE
	tr_frees++;
	tr_current_mallocs--;
#endif

	if (ptr == mallwatch)
		tr_break ();
	__libc_lock_lock (lock);
	__free_hook = tr_old_free_hook;

	if (tr_old_free_hook != NULL)
		(*tr_old_free_hook) (ptr, caller);
	else
		free(ptr);
	tr_log(caller, ptr, 0, 0, TR_FREE);

	__free_hook = tr_freehook;
	__libc_lock_unlock (lock);
}

static __ptr_t
tr_mallochook (size, caller)
     __malloc_size_t size;
     const __ptr_t caller;
{
	__ptr_t hdr;

	__libc_lock_lock (lock);

	__malloc_hook = tr_old_malloc_hook;
	__realloc_hook = tr_old_realloc_hook;

	if (tr_old_malloc_hook != NULL)
		hdr = (__ptr_t) (*tr_old_malloc_hook) (size, caller);
	else
		hdr = (__ptr_t) malloc(size);
	tr_log(caller, hdr, 0, size, TR_MALLOC);

	__malloc_hook = tr_mallochook;
	__realloc_hook = tr_reallochook;

#ifdef PROFILE
	tr_mallocs++;
	tr_current_mallocs++;
	if (tr_current_mallocs > tr_max_mallocs)
		tr_max_mallocs = tr_current_mallocs;
#endif
	__libc_lock_unlock (lock);

	if (hdr == mallwatch)
		tr_break ();

	return hdr;
}

static __ptr_t
tr_reallochook (ptr, size, caller)
     __ptr_t ptr;
     __malloc_size_t size;
     const __ptr_t caller;
{
	__ptr_t hdr;

	if (ptr == mallwatch)
		tr_break ();

	__libc_lock_lock (lock);

	__free_hook = tr_old_free_hook;
	__malloc_hook = tr_old_malloc_hook;
	__realloc_hook = tr_old_realloc_hook;

	if (tr_old_realloc_hook != NULL)
		hdr = (__ptr_t) (*tr_old_realloc_hook) (ptr, size, caller);
	else
		hdr = (__ptr_t) realloc (ptr, size);

	tr_log(caller, hdr, ptr, size, TR_REALLOC);

	__free_hook = tr_freehook;
	__malloc_hook = tr_mallochook;
	__realloc_hook = tr_reallochook;

#ifdef PROFILE
	/* If ptr is 0 there was no previos malloc of this location */
	if (ptr == NULL)
	{
		tr_mallocs++;
		tr_current_mallocs++;
		if (tr_current_mallocs > tr_max_mallocs)
			tr_max_mallocs = tr_current_mallocs;
	}
#endif

	__libc_lock_unlock (lock);

	if (hdr == mallwatch)
		tr_break ();

	return hdr;
}


#ifdef _LIBC
extern void __libc_freeres (void);

/* This function gets called to make sure all memory the library
 * allocates get freed and so does not irritate the user when studying
 * the mtrace output.  */
static void
release_libc_mem (void)
{
	/* Only call the free function if we still are running in mtrace
	 * mode. */
	if (mallstream != NULL)
		__libc_freeres ();

	kuntrace();
}
#endif

/* We enable tracing if either the environment variable MALLOC_TRACE
 * is set, or if the variable mallwatch has been patched to an address
 * that the debugging user wants us to stop on.  When patching
 * mallwatch, don't forget to set a breakpoint on tr_break! */
void
ktrace()
{
#ifdef _LIBC
	static int added_atexit_handler;
#endif
	char* mallfile;

	/* Don't panic if we're called more than once.  */
	if (mallstream != NULL)
		return;

#ifdef _LIBC
	/* When compiling the GNU libc we use the secure getenv function
	 * which prevents the misuse in case of SUID or SGID enabled
     * programs.  */
	mallfile = __secure_getenv (mallenv);
#else
	mallfile = getenv (mallenv);
#endif
	if (mallfile != NULL || mallwatch != NULL)
    {
		mallstream = fopen (mallfile != NULL ? mallfile : "/dev/null", "w");
		if (mallstream != NULL)
		{
			char buf[512];
            
			/* Be sure it doesn't malloc its buffer!  */
			setvbuf (mallstream, malloc_trace_buffer, _IOFBF,
					 TRACE_BUFFER_SIZE);
			fprintf (mallstream, "= Start\n");
			memset(buf, 0, sizeof(buf));
			readlink("/proc/self/exe", buf, sizeof(buf));
			if(*buf)
				fprintf (mallstream, "#%s\n", buf);

			/* Save old hooks and hook in our own functions for all
			 * malloc, realloc and free calls */
			tr_old_free_hook = __free_hook;
			__free_hook = tr_freehook;
			tr_old_malloc_hook = __malloc_hook;
			__malloc_hook = tr_mallochook;
			tr_old_realloc_hook = __realloc_hook;
			__realloc_hook = tr_reallochook;

			tr_cache_pos = TR_CACHE_SIZE;
			do
			{
				tr_cache[--tr_cache_pos].ptr = NULL;
			} while (tr_cache_pos);
			tr_cache_level = 0;

			memset(tr_hashtable, 0, sizeof(void*) * TR_HASHTABLE_SIZE);
#ifdef _LIBC
			if (!added_atexit_handler)
			{
				added_atexit_handler = 1;
				atexit (release_libc_mem);
			}
#endif
		}
	}
}

void
kuntrace()
{
	if (mallstream == NULL)
		return;

	/* restore hooks to original values */
	__free_hook = tr_old_free_hook;
	__malloc_hook = tr_old_malloc_hook;
	__realloc_hook = tr_old_realloc_hook;

	/* Flush cache. */
	while (tr_cache_level)
		tr_log(NULL, 0, 0, 0, TR_NONE);

	fprintf (mallstream, "= End\n");
#ifdef PROFILE
	fprintf(mallstream, "\nMax Mallocs:    %8ld   Cache Size:   %8ld"
			"   Flashes:      %8ld\n"
			"Mallocs:        %8ld   Frees:        %8ld   Leaks:        %8ld\n"
			"Logged Mallocs: %8ld   Logged Frees: %8ld   Logged Leaks: %8ld\n"
			"Avg. Free lookups: %ld  Malloc collisions: %ld\n",
			tr_max_mallocs, TR_CACHE_SIZE, tr_flashes,
			tr_mallocs, tr_frees, tr_current_mallocs,
			tr_logged_mallocs, tr_logged_frees,
			tr_logged_mallocs - tr_logged_frees,
			tr_failed_free_lookups / tr_frees,
			tr_malloc_collisions);
#endif
	fclose (mallstream);
	mallstream = NULL;
}

int fork()
{
  int result;
  if (mallstream)
     fflush(mallstream);
  result = __fork();
  if (result == 0)
  {
    if (mallstream)
    {
      fclose(mallstream);
      __free_hook = tr_old_free_hook;
      __malloc_hook = tr_old_malloc_hook;
      __realloc_hook = tr_old_realloc_hook;
    }
  }
  return result;
}
