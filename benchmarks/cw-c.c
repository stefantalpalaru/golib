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


#include <stdlib.h>
#include <stdio.h>
#include "golib.h"

typedef struct channels {
	void* left;
	void* right;
} channels;

void whisper(void* args)
{
	long *r = (long *)go_malloc(sizeof(long));
	/*channels *chans = (channels *)args;*/
	channels *chans;
	writebarrierptr((void**)&chans, args);

	*r = *(long *)chan_recv(chans->right);
	*r += 1;
	chan_send(chans->left, r);
}

void first_whisper(void* chan)
{
	long *v = (long *)go_malloc(sizeof(long));

	*v = 1;
	chan_send(chan, v);
}

void go_main() __asm__ ("main.main");
void go_main()
{
	// a slowdown in this scenario for gccgo-6.3.0 but not for gcc-7.1.0 or Go
	runtime_gomaxprocsfunc(getproccount());

	const long n = 500000;
	/*void *leftmost = chan_make(0);*/
	void *leftmost;
	writebarrierptr(&leftmost, chan_make(0));
	/*void *right = leftmost;*/
	void *right;
	writebarrierptr(&right, leftmost);
	/*void *left = leftmost;*/
	void *left;
	writebarrierptr(&left, leftmost);
	long i;
	long res;
	channels *chans;

	for(i = 0; i < n; i++) {
		/*right = chan_make(0);*/
		writebarrierptr(&right, chan_make(0));
		/*chans = (channels *)go_malloc(sizeof(channels));*/
		writebarrierptr((void**)&chans, go_malloc(sizeof(channels)));
		/*chans->left = left;*/
		writebarrierptr(&(chans->left), left);
		/*chans->right = right;*/
		writebarrierptr(&(chans->right), right);
		__go_go(whisper, chans);
		/*left = right;*/
		writebarrierptr(&left, right);
	}
	__go_go(first_whisper, right);
	res = *(long *)chan_recv(leftmost);
	printf("%ld\n", res);
}

int main(int argc, char **argv)
{
	golib_main(argc, argv);
	// not reached
}

