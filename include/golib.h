/*
Copyright (c) 2015-2018, Ștefan Talpalaru <stefantalpalaru@yahoo.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _GOLIB_H
#define _GOLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define GCC_VERSION (__GNUC__ * 10000 \
		     + __GNUC_MINOR__ * 100 \
		     + __GNUC_PATCHLEVEL__)

// libgo symbols
typedef signed int int32 __attribute__((mode (SI)));
typedef signed int int64 __attribute__((mode (DI)));
typedef unsigned int uintptr __attribute__((mode (pointer)));
typedef unsigned int uint32  __attribute__((mode (SI)));
typedef unsigned int uint64  __attribute__((mode (DI)));
extern void* __go_go(void (*f)(void *), void *);
extern int32 runtime_gomaxprocsfunc(int32)
#if GCC_VERSION >= 70100 // 7.1.0
		__asm__("runtime.GOMAXPROCS")
#endif
;
extern void runtime_gosched();
#if GCC_VERSION >= 70100 // 7.1.0
extern int32 getproccount();
#endif

// helpers
#define SELECT_DIR_SEND 1
#define SELECT_DIR_RECV 2
#define SELECT_DIR_DEFAULT 3

typedef struct chan_select_case {
	uintptr dir;
	void *chan;
	void *send;
} chan_select_case;

typedef struct chan_select_result {
	int chosen;
	void *recv;
	_Bool recv_ok;
} chan_select_result;

typedef struct chan_recv2_result {
	void *recv;
	_Bool ok;
} chan_recv2_result;

extern void golib_main(int argc, char **argv);
extern void* go_malloc(uintptr size);
extern void* go_malloc0(uintptr size);
extern void go_run_finalizer(void (*f)(void *), void *obj);
extern void go_yield();
extern void go_gc();

// our golib.go symbols
extern void* chan_make(int) __asm__("main.Chan_make");
extern void chan_send(void *, void *) __asm__("main.Chan_send");
extern void* chan_recv(void *) __asm__("main.Chan_recv");
extern chan_recv2_result chan_recv2(void *) __asm__("main.Chan_recv2");
extern void chan_close(void *) __asm__("main.Chan_close");
extern chan_select_result chan_select(chan_select_case *, int) __asm__("main.Chan_select");
extern void go_sleep_ms(int64) __asm__("main.Sleep_ms");
extern void set_finalizer(void *, void (*f)(void *)) __asm__("main.Set_finalizer");
// keep this in sync with golib.go
typedef struct mem_stats {
	uint64 alloc; // bytes allocated and not yet freed
	uint64 total_alloc; // bytes allocated (even if freed)
	uint64 sys; // bytes obtained from system
	uint64 nlookup; // number of pointer lookups
	uint64 nmalloc; // number of mallocs
	uint64 nfree; // number of frees
	uint64 heap_objects; // total number of allocated objects
	uint64 pause_total_ns; // cumulative nanoseconds in GC stop-the-world pauses since the program started
	uint32 numgc; // number of completed GC cycles
} mem_stats;
extern mem_stats go_mem_stats();

#ifdef __cplusplus
}
#endif

#endif /* _GOLIB_H */

