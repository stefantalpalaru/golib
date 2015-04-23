##description

**golib** is a library exposing Go's channels and goroutines to plain C and to any
other language able to use C libraries.

There are two catches:
- your main() function needs to call golib\_main() which will not return control
  to it. You should put the actual main code in another function with the
  assembler name of 'main.main' (see examples).
- we use gccgo (from upstream GCC) instead of the main Go toolchain, for
  technical reasons, so the channel/goroutine performance is lower and the
  memory usage is higher. Apparently because gccgo doesn't do escape analysis.

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
AMD FX-8320E, Linux 4.0.0 x86\_64) that the golib version is 2.4 times slower
and uses 5.7 times more memory than the go version. This should be a worse case
scenario since the benchmark creates 500000 goroutines and then passes integers
from one to the other, incrementing them with each pass. So it's basically
testing the concurrency and message passing overhead. An interesting aspect is
that enabling multiple core usage speeds up slightly the go version while
slowing down the gccgo one. But look at the bright side: when gccgo improves,
golib will reap the benefits ;-)

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

