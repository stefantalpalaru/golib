##description

**golib** is a library exposing Go's channels and goroutines to plain C and to any
other language able to use C libraries.

There are two catches:
- your *main()* function needs to call *golib\_main()* which will not return control
  to it. You should put the actual main code in another function with the
  assembler name of *'main.main'* (see examples).
- we use gccgo (from upstream GCC) instead of the main Go toolchain, for
  technical reasons, so the channel/goroutine performance is lower and the
  memory usage is higher. Apparently because gccgo doesn't do escape analysis.

##requirements

- GCC with Go support (tested with gcc-4.9.2)

##inspiration

- [this Stack Overflow response][8]
- [uWSGI's gccgo plugin][4]
- [GCC's libgo][5]

##discussion

The main problem with *libgo* is that it exposes extremely little in terms of a
public API. Just the [function name mangling scheme][6] and [a couple of
types][7] (I had no luck with *\_\_go\_slice*, BTW). But there is a huge public
API in the form of the Go language specifications and its included packages.

So the strategy is to use as little as possible directly from libgo and write
Go code for the rest. Convincing autoconf and libtool to create a library from
Go and C code was not easy, because they mostly lack builtin gccgo support, but
it was done.

When building this library and programs using it it's important to leave the
debug info unstripped because the Go runtime needs it. So use '-g' in your
CFLAGS and make sure you don't have '-s' in LDFLAGS (already done in **golib**'s
build system) and if you're a distribution packager make sure that the
installed binaries are excepted from stripping.

Don't be alarmed by the use of Pthreads. *libgo* only uses it for mutexes, the
coroutines are proper user-space M:N coroutines just like in Go.

##build

```sh
./autogen.sh
./configure
make
```

##run tests

```sh
make check
```

##benchmarks

```sh
/usr/bin/time -v ./benchmarks/cw-c
/usr/bin/time -v ./benchmarks/cw-go
```

With the [chinese whispers benchmark][1] I see on my system (gcc-4.9.2, go-1.4.2,
AMD FX-8320E, Linux 4.0.0 x86\_64) that the **golib** version is 2.4 times slower
and uses 5.7 times more memory than the go version. This should be a worse case
scenario since the benchmark creates 500000 goroutines and then passes integers
from one to the other, incrementing them with each pass. So it's basically
testing the concurrency and message passing overhead. An interesting aspect is
that enabling multiple core usage speeds up slightly the go version while
slowing down the gccgo one. But look at the bright side: when gccgo improves,
**golib** will reap the benefits ;-)

##API docs

See the [benchmarks][2] and [tests][3]. We'll have proper documentation once the API is
stable. Right now it's more of a technical preview.

It might be worth noting that we pass pointers through the channels and they should point to heap allocated memory. Freeing that memory is the receiver's responsibility.

##license

BSD-2

##credits

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

