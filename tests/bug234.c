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
#include <stdio.h>
#include "golib.h"

void go_main() __asm__ ("main.main");
void go_main()
{
	void *c = chan_make(1);
	int i = 100;
	chan_send(c, &i);
	int x;
	chan_recv2_result res = chan_recv2(c);
	x = *(int*)res.recv;
	if(x != 100 || !res.ok) {
		printf("x=%d ok=%d want 100, 1\n", x, res.ok);
		exit(1);
	}
	chan_close(c);
	res = chan_recv2(c);
	if(res.recv != NULL || res.ok) {
		printf("recv=%p ok=%d want (nil), 0\n", res.recv, res.ok);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	golib_main(argc, argv);
	// not reached
}

