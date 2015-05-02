/*
Copyright (c) 2015, Ștefan Talpalaru <stefantalpalaru@yahoo.com>
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

// libgo symbols
/*extern void runtime_gosched();*/
/*extern void runtime_netpollinit();*/
/*extern void runtime_lockOSThread();*/
extern void* __go_go(void (*f)(void *), void *);
typedef signed int int32 __attribute__ ((mode (SI)));
typedef signed int int64 __attribute__ ((mode (DI)));
extern int32 runtime_gomaxprocsfunc(int32 n);
extern int32 runtime_ncpu;
extern void __go_free(void *);
extern void runtime_stoptheworld();
extern void runtime_starttheworld();

// helpers
#define SELECT_DIR_SEND 1
#define SELECT_DIR_RECV 2
#define SELECT_DIR_DEFAULT 3
typedef struct chan_select_case {
    unsigned int dir;
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

// our golib.go symbols
extern void* chan_make(int) __asm__ ("main.Chan_make");
extern void chan_send(void *, void *) __asm__ ("main.Chan_send");
extern void* chan_recv(void *) __asm__ ("main.Chan_recv");
extern chan_recv2_result chan_recv2(void *) __asm__ ("main.Chan_recv2");
extern void chan_close(void *) __asm__ ("main.Chan_close");
extern chan_select_result chan_select(chan_select_case *, int) __asm__ ("main.Chan_select");
extern void go_sleep_ms(int64) __asm__ ("main.Sleep_ms");

#ifdef __cplusplus
}
#endif

#endif /* _GOLIB_H */

