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

// we are not freeing allocated memory but we make up for it by also disabling the GC in the Go version

#include <stdlib.h>
#include <stdio.h>
#include "golib.h"

typedef struct channels {
    void* left;
    void* right;
} channels;

void whisper(void* args) {
    long *r = (long *)malloc(sizeof(long));
    channels *chans = (channels *)args;

    *r = *(long *)chan_recv(chans->right);
    /*__go_free(chans->right);*/
    *r += 1;
    chan_send(chans->left, r);
    /*free(args);*/
}

void first_whisper(void* chan) {
    long *v = (long *)malloc(sizeof(long));

    *v = 1;
    chan_send(chan, v);
}

void go_main() __asm__ ("main.main");
void go_main() {
    // a slowdown in this scenario for gccgo-4.9.2 but not for go-1.4.2
    /*runtime_gomaxprocsfunc(runtime_ncpu);*/

    const long n = 500000;
    void *leftmost = chan_make(0);
    void *right = leftmost;
    void *left = leftmost;
    long i;
    long res;
    channels *chans;
    // used to keep track of goroutines so we can free them
    void **goroutines = (void **)malloc((n + 1) * sizeof(void*));

    for(i = 0; i < n; i++) {
        right = chan_make(0);
        chans = (channels *)malloc(sizeof(channels));
        chans->left = left;
        chans->right = right;
        goroutines[i] = __go_go(whisper, chans);
        left = right;
    }
    goroutines[i] = __go_go(first_whisper, right);
    res = *(long *)chan_recv(leftmost);
    /*__go_free(leftmost);*/
    /*for(i = 0; i <= n; i++) {*/
        /*__go_free(goroutines[i]);*/
    /*}*/
    /*free(goroutines);*/
    printf("%ld\n", res);
}

int main(int argc, char **argv) {
    golib_main(argc, argv);
    // not reached
}

