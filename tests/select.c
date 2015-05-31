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

#include <stdlib.h>
#include <stdio.h>
#include "golib.h"

unsigned int counter = 0;
unsigned int shift = 0;

unsigned int GetValue(){
    counter++;
    return 1 << shift;
}

int Send(void *a, void *b){
    int i = 0;
    int num_cases = 3;
    chan_select_case *cases = go_malloc(sizeof(chan_select_case) * num_cases);
    unsigned int *val1;
    unsigned int *val2;
    chan_select_result result;

    for (;;) {
        // we're allocating memory that won't end up pointed at by a channel value,
        // but we care more about simulating Go's select statement behavior for this test
        val1 = go_malloc(sizeof(int));
        val2 = go_malloc(sizeof(int));
        *val1 = GetValue();
        *val2 = GetValue();
        cases[0] = (chan_select_case){.dir = SELECT_DIR_SEND, .chan = a, .send = val1 };
        cases[1] = (chan_select_case){.dir = SELECT_DIR_SEND, .chan = b, .send = val2 };
        cases[2] = (chan_select_case){.dir = SELECT_DIR_DEFAULT, .chan = NULL, .send = NULL };

        result = chan_select(cases, num_cases);
        switch(result.chosen){
            case 0:
                i++;
                a = NULL;
                break;
            case 1:
                i++;
                b = NULL;
                break;
            case 2:
                goto LOOP_END;
        }
        shift++;
    }
LOOP_END:
    return i;
}


void go_main() __asm__ ("main.main");
void go_main() {
    void *a = chan_make(1);
    void *b = chan_make(1);

    int v = Send(a, b);
    if(v != 2){
        printf("Send returned %d != 2\n", v);
        exit(1);
    }

    unsigned int av = *(unsigned int *)chan_recv(a);
    unsigned int bv = *(unsigned int *)chan_recv(b);
    if((av|bv) != 3){
        printf("bad values %d %d\n", av, bv);
        exit(1);
    }

    v = Send(a, NULL);
    if(v != 1){
        printf("Send returned %d != 1\n", v);
        exit(1);
    }

    if(counter != 10){
        printf("counter is %d != 10\n", counter);
        exit(1);
    }
}

int main(int argc, char **argv) {
    golib_main(argc, argv);
    // not reached
}

