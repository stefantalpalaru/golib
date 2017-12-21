/*
Copyright (c) 2015-2017, Ștefan Talpalaru <stefantalpalaru@yahoo.com>
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

#include <stdlib.h>
#include <unistd.h>
#include "golib.h"

#define ROUND(x, n) (((x) + (n) - 1) & ~(uintptr)((n) - 1))

typedef _Bool bool;
enum {
	false	= 0,
	true	= 1,
};

enum
{
	// flags to malloc
	FlagNoScan	= 1<<0,	// GC doesn't have to scan object
	FlagNoProfiling	= 1<<1,	// must not profile
	FlagNoGC	= 1<<2,	// must not free or scan for pointers
	FlagNoZero	= 1<<3, // don't zero memory
	FlagNoInvokeGC	= 1<<4, // don't invoke GC
};

// libgo symbols that we don't need to export in golib.h
#if GCC_VERSION >= 70100 // 7.1.0
extern bool runtime_isarchive;
extern bool runtime_isstarted;
extern void runtime_cpuinit();
extern void setncpu(int32) __asm__("runtime.setncpu");
extern int32 getproccount();
extern void setpagesize(uintptr) __asm__("runtime.setpagesize");
extern void* runtime_sched;
extern void* runtime_getsched() __asm__("runtime.getsched");
#endif
extern void runtime_check()
#if GCC_VERSION >= 70100 // 7.1.0
	__asm__("runtime.check")
#endif
;
extern void runtime_args(int32, char **)
#if GCC_VERSION >= 70100 // 7.1.0
	__asm__("runtime.args")
#endif
;
extern void runtime_osinit();
extern void runtime_schedinit();
extern void runtime_main();
extern void runtime_mstart(void *);
extern void* runtime_m() __attribute__((noinline, no_split_stack));
extern void* runtime_mallocgc(uintptr size, uintptr typ, uint32 flag);
// extern void runtime_netpollinit(void); // not in gccgo-7.1.0
extern void runtime_pollServerInit() __asm__("net.runtime_pollServerInit");

// have the GC scan the BSS
extern char edata, end;
struct root_list {
	struct root_list *next;
	struct root {
		void *decl;
		size_t size;
	} roots[];
};
extern void __go_register_gc_roots (struct root_list* r);
static struct root_list bss_roots = {
	NULL,
	{
		{ NULL, 0 },
		{ NULL, 0 },
	},
};

void golib_main(int argc, char **argv)
{
#if GCC_VERSION >= 70100 // 7.1.0
	runtime_isarchive = false;
	runtime_isstarted = true;

	runtime_cpuinit();
#endif
	runtime_check();
	/*printf("edata=%p, end=%p, end-edata=%d\n", &edata, &end, &end - &edata);*/
	bss_roots.roots[0].decl = &edata;
	bss_roots.roots[0].size = &end - &edata;
	__go_register_gc_roots(&bss_roots);
	runtime_args(argc, argv);
#if GCC_VERSION >= 80000 // 8.0.0
	setncpu(getproccount());
	setpagesize(getpagesize());
#else
	runtime_osinit();
#endif
#if GCC_VERSION >= 70100 // 7.1.0
	runtime_sched = runtime_getsched();
#endif
	runtime_schedinit();
	runtime_pollServerInit();
	__go_go((void (*)(void *))runtime_main, NULL);
	runtime_mstart(runtime_m());
	abort();
}

void* go_malloc(uintptr size)
{
	return runtime_mallocgc(ROUND(size, sizeof(void*)), 0, FlagNoZero);
}

void* go_malloc0(uintptr size)
{
	return runtime_mallocgc(ROUND(size, sizeof(void*)), 0, 0);
}

void go_run_finalizer(void (*f)(void *), void *obj)
{
	f(obj);
}
