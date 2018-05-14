// Copyright (c) 2015-2017, Ștefan Talpalaru <stefantalpalaru@yahoo.com>
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package main

import (
	// "fmt"
	"reflect"
	"runtime"
	"time"
	"unsafe"
)

func Chan_make(size int) chan *int {
	c := make(chan *int, size)
	return c
}

func Chan_send(c chan *int, v *int) {
	c <- v
}

func Chan_recv(c chan *int) (v *int) {
	v = <-c
	return
}

func Chan_recv2(c chan *int) (v *int, ok bool) {
	v, ok = <-c
	return
}

func Chan_close(c chan *int) {
	close(c)
}

type Chan_select_case struct {
	Dir  int
	Chan chan *int
	Send *int
}

func Chan_select(cases *Chan_select_case, num_cases int) (chosen int, recv *int, recv_ok bool) {
	select_cases := make([]reflect.SelectCase, num_cases)
	cases2 := (*[1 << 30]Chan_select_case)(unsafe.Pointer(cases))
	for i := 0; i < num_cases; i++ {
		dir := reflect.SelectDir(cases2[i].Dir)
		// somehow the Value of a typed nil is not the zero Value
		var c reflect.Value
		if cases2[i].Chan == (chan *int)(nil) {
			c = reflect.ValueOf(nil)
		} else {
			c = reflect.ValueOf(cases2[i].Chan)
		}
		var send reflect.Value
		if cases2[i].Send == (*int)(nil) {
			send = reflect.ValueOf(nil)
		} else {
			send = reflect.ValueOf(cases2[i].Send)
		}
		select_cases[i] = reflect.SelectCase{Dir: dir, Chan: c, Send: send}
	}
	var recv_val reflect.Value
	chosen, recv_val, recv_ok = reflect.Select(select_cases)
	if recv_val.IsValid() {
		recv = recv_val.Interface().(*int)
	} else {
		recv = (*int)(nil)
	}
	return
}

func Sleep_ms(n int64) {
	time.Sleep((time.Duration)(n) * time.Millisecond)
}

type bogus struct {
	s string
}

//extern go_run_finalizer
func go_run_finalizer(f unsafe.Pointer, obj *bogus)

func Set_finalizer(obj *bogus, f unsafe.Pointer) {
	runtime.SetFinalizer(obj, func(x *bogus) { go_run_finalizer(f, x) })
}

// keep this in sync with golib.h
type mem_stats struct {
	alloc          uint64
	total_alloc    uint64
	sys            uint64
	nlookup        uint64
	nmalloc        uint64
	nfree          uint64
	heap_objects   uint64
	pause_total_ns uint64
	numgc          uint32
}

func Get_mem_stats() mem_stats {
	var ms runtime.MemStats
	runtime.ReadMemStats(&ms)
	return mem_stats{
		ms.Alloc,
		ms.TotalAlloc,
		ms.Sys,
		ms.Lookups,
		ms.Mallocs,
		ms.Frees,
		ms.HeapObjects,
		ms.PauseTotalNs,
		ms.NumGC,
	}
}
