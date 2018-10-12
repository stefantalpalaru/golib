## description

**golib** is a library exposing Go's channels and goroutines to plain C and to any
other language able to use C libraries.

There are three catches:
- your *main()* function needs to call *golib\_main()* which will not return control
  to it. You should put the actual main code in another function with the
  assembler name of *'main.main'* (see examples).
- you need to use write barriers when assigning pointers
- we use gccgo (from upstream GCC) instead of the main Go toolchain, for
  technical reasons, so the channel/goroutine performance is lower and the
  memory usage is higher.

## requirements

- GCC with Go support (tested with GCC 7.3.0, 8.2.0).

## inspiration

- [this Stack Overflow response][8]
- [uWSGI's gccgo plugin][4]
- [GCC's libgo][5]

## discussion

The main problem with *libgo* is that it exposes extremely little in terms of a
public API. Just the [function name mangling scheme][6] and [a couple of
types][7] (I had no luck with *\_\_go\_slice*, BTW). But there is a huge public
API in the form of the Go language specifications and its included packages.

So the strategy is to use as little as possible directly from libgo and write
Go code for the rest. Convincing autoconf and libtool to create a library from
Go and C code was not easy, because they mostly lack builtin gccgo support, but
it was done.

When building this library and programs using it, it's important to leave the
debug info unstripped because the Go runtime needs it. So use '-g' in your
CFLAGS and make sure you don't have '-s' in LDFLAGS (already done in **golib**'s
build system) and if you're a distribution packager make sure that the
installed binaries are excepted from stripping, like in the [Gentoo ebuild][9].

Don't be alarmed by the use of Pthreads. *libgo* only uses it for mutexes, the
coroutines are proper user-space M:N coroutines just like in Go.

## build

```sh
./autogen.sh
./configure
make
```

## run tests

```sh
make check
```

## benchmarks

```sh
/usr/bin/time -v ./benchmarks/cw-c
/usr/bin/time -v ./benchmarks/cw-go
```

With the [chinese whispers benchmark][1] I see on my system (gcc-8.2.0, go-1.11,
AMD FX-8320E, Linux 4.18.7 x86\_64) that the **golib** version is 4.3 times slower
and uses 4.1 times more memory than the Go version. This should be a worst case
scenario since the benchmark creates 500000 goroutines and then passes integers
from one to the other, incrementing them with each pass. So it's basically
testing the concurrency and message passing overhead. But look at the bright
side: when gccgo improves, **golib** will reap the benefits ;-)

Instructions for building with non-default compilers:
```sh
./configure CC=gcc-7.1.0 GOC=gccgo-7.1.0
make clean
make
```

## API docs

See the [benchmarks][2] and [tests][3]. We'll have proper documentation once the API is
stable. Right now it's more of a technical preview.

It might be worth noting that we pass pointers through the channels and they
should point to heap allocated memory. Since I didn't figure out how to make
the standard library's malloc() play nice with Go's garbage collector, you should use the
latter exclusively by replacing malloc() with go\_malloc().

With recent GCC versions it's necessary to replace all assignments having a heap
pointer in the right side with calls to writebarrierptr(). Even when the
destination is on stack, due to how the new Go garbage collector is implemented.

## TODO

Move as much code as possible from C (where we're chasing the moving target
that is GCC libgo's private API) to Go that has a much more stable API in the
language specifications.

## license

BSD-2

## credits

- author: Ștefan Talpalaru <stefantalpalaru@yahoo.com>

- homepage: https://github.com/stefantalpalaru/golib

[1]: benchmarks/cw-c.c
[2]: benchmarks/
[3]: tests/
[4]: https://github.com/unbit/uwsgi/tree/master/plugins/gccgo
[5]: https://github.com/gcc-mirror/gcc/tree/master/libgo
[6]: https://gcc.gnu.org/onlinedocs/gccgo/Function-Names.html#Function-Names
[7]: https://gcc.gnu.org/onlinedocs/gccgo/C-Type-Interoperability.html#C-Type-Interoperability
[8]: http://stackoverflow.com/questions/6125683/call-go-functions-from-c/15760986#15760986
[9]: https://github.com/stefantalpalaru/gentoo-overlay/blob/master/dev-libs/golib/golib-9999.ebuild

