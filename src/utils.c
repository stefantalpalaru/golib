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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "golib.h"

#define ROUND(x, n) (((x) + (n) - 1) & ~(uintptr)((n) - 1))

typedef _Bool bool;
enum {
	false	= 0,
	true	= 1,
};

// libgo symbols that we don't need to export in golib.h
extern bool runtime_isarchive;
extern bool runtime_isstarted;
extern bool runtime_iscgo;
extern void runtime_cpuinit();
extern void setncpu(int32) __asm__("runtime.setncpu");
extern void setpagesize(uintptr) __asm__("runtime.setpagesize");
extern void* runtime_sched;
extern void* runtime_getsched() __asm__("runtime.getsched");
extern void runtime_check() __asm__("runtime.check");
extern void runtime_args(int32, char **) __asm__("runtime.args");
extern void runtime_osinit();
extern void runtime_schedinit()
#if GCC_VERSION >= 80100 // 8.1.0
	__asm__("runtime.schedinit")
#endif
;
extern void runtime_main()
#if GCC_VERSION >= 80100 // 8.1.0
	__asm__("runtime.main")
#endif
;
extern void runtime_mstart(void *);
extern void* runtime_m() __attribute__((noinline, no_split_stack));
extern void runtime_gosched()
#if GCC_VERSION >= 80100 // 8.1.0
	__asm__("runtime.Gosched")
#endif
;
#if GCC_VERSION >= 80100 // 8.1.0
extern void runtime_gc() __asm__("runtime.GC");
#else
extern void runtime_gc(int32);
#endif
extern void runtime_pollServerInit()
#if GCC_VERSION >= 80100 // 8.1.0
	__asm__("internal_poll.runtime_pollServerInit")
#else
	__asm__("net.runtime_pollServerInit")
#endif
;
extern void runtime_setIsCgo() __asm__("runtime.setIsCgo");
// partial struct head from gcc/libgo/go/reflect/type.go
typedef struct rtype {
	uintptr size;
	uintptr ptrdata;	// size of memory prefix holding all pointers
	uint32  hash;		// hash of type; avoids computation in hash tables
	uint8   kind;		// enumeration for C
	int8	align;		// alignment of variable with this type
	uint8	fieldAlign;	// alignment of struct field with this type
	uint8	_;		// unused/padding

	uintptr hashfn;		// hash function
	uintptr equalfn;	// equality function

	uint8	*gcdata ;	// garbage collection data
} rtype;
extern void runtime_typedmemmove(rtype *typ, void *dest, void *src) __asm__("runtime.typedmemmove");
extern void* allocate_array_of_pointers(uintptr num) __asm__("main.Allocate_array_of_pointers");

// golib.go symbols that we don't need to export in golib.h
extern mem_stats get_mem_stats() __asm__("main.Get_mem_stats");

// have the GC scan the BSS
extern char edata, end;
extern char **environ;
extern uint8 _end[];
uintptr __go_end;
struct root_list {
	struct root_list *next;
#if GCC_VERSION >= 80100 // 8.1.0
	int count;
#endif
	struct root {
		void *decl;
		size_t size;
#if GCC_VERSION >= 80100 // 8.1.0
		size_t ptrdata;
		unsigned int *gcdata;
#endif
	} roots[2];
};
extern void __go_register_gc_roots (struct root_list* r)
#if GCC_VERSION >= 80100 // 8.1.0
	__asm__("runtime.registerGCRoots")
#endif
;
static struct root_list bss_roots = {
	NULL,
#if GCC_VERSION >= 80100 // 8.1.0
	1,
#endif
	{
		{
			NULL,
			0,
#if GCC_VERSION >= 80100 // 8.1.0
			0,
			NULL,
#endif
		},
		{
			NULL,
			0,
#if GCC_VERSION >= 80100 // 8.1.0
			0,
			NULL,
#endif
		},
	},
};

// from GCC's libgo/runtime/go-main.c
int golib_main(int argc, char **argv)
{
	runtime_isarchive = false;
	if (runtime_isstarted)
		return 0;
	runtime_isstarted = true;

	if (runtime_iscgo)
		runtime_setIsCgo();

	__go_end = (uintptr)_end;
	/*printf("_end=%p\n", _end);*/
	runtime_cpuinit();
	runtime_check();
	/*printf("edata=%p, end=%p, end-edata=%ld\n", &edata, &end, &end - &edata);*/
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
	runtime_sched = runtime_getsched();
	runtime_schedinit();
	runtime_pollServerInit();
	__go_go((void (*)(void *))runtime_main, NULL);
	runtime_mstart(runtime_m());
	abort();
}

void* go_malloc(uintptr size)
{
	return allocate_array_of_pointers(ROUND(size, sizeof(void*)) / sizeof(void*));
}

void go_run_finalizer(void (*f)(void *), void *obj)
{
	f(obj);
}

// wrappers to handle GCC's constant naming changes from Nim code

// yield execution to another goroutine
void go_yield()
{
	runtime_gosched();
}

// run the garbage collector
void go_gc()
{
#if GCC_VERSION >= 80100 // 8.1.0
	runtime_gc();
#else
	runtime_gc(2);
#endif
}

// memory statistics
mem_stats go_mem_stats()
{
	return get_mem_stats();
}

void typedmemmove(void *dest, void *src, uintptr size)
{
	// round it down to pointer size
	uintptr new_size = size & ~(sizeof(void*) - 1);

	if(new_size) {
		// check the pointer alignment of dest and src
		if ((((uintptr)dest|(uintptr)src) & (sizeof(void*) - 1)) == 0) {
			rtype typ;
			typ.kind = 0;
			typ.size = new_size;
			runtime_typedmemmove(&typ, dest, src);
		} else {
			// the pointers are missaligned, so let memmove() do all the copying
			new_size = 0;
		}
	}
	if(size > new_size) {
		memmove(dest + new_size, src + new_size, size - new_size);
	}
}

