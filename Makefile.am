AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libgolib.la
# https://autotools.io/libtool/version.html
libgolib_la_LDFLAGS = -version-info 0:0:0
libgolib_la_SOURCES = src/golib.go \
					  src/utils.c
golib_lo_SOURCES = src/golib.go
pkginclude_HEADERS = include/golib.h

check_PROGRAMS = tests/select \
				 tests/bug234
tests_select_SOURCES = tests/select.c
tests_select_LDADD = libgolib.la -lgo
tests_select_LDFLAGS = -no-install
tests_bug234_SOURCES = tests/bug234.c
tests_bug234_LDADD = libgolib.la -lgo
tests_bug234_LDFLAGS = -no-install

TESTS = $(check_PROGRAMS)

# gccgo benchmarks
noinst_DATA = benchmarks/cw-go-gccgo
benchmarks/%-go-gccgo: benchmarks/%-go.go
	$(GOC) $(CFLAGS) $(LDFLAGS) $< -o $@

# Go benchmarks
if HAVE_GO
noinst_DATA += benchmarks/cw-go
benchmarks/%-go: benchmarks/%-go.go
	go build -o $@ $<
endif

MOSTLYCLEANFILES := $(noinst_DATA)

# C benchmarks
noinst_PROGRAMS = benchmarks/cw-c
benchmarks_cw_c_SOURCES = benchmarks/cw-c.c
benchmarks_cw_c_LDADD = libgolib.la -lgo
benchmarks_cw_c_LDFLAGS = -no-install

EXTRA_DIST := benchmarks/*-go.go \
	tests/*.go \
	.ycm_extra_conf.py \
	autogen.sh \
	LICENSE \
	README.md

LTCOMPILE_GO = $(LIBTOOL) $(AM_V_lt) --tag=GO $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=compile $(GOC) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS) $(CFLAGS)
.go.lo:
	$(LTCOMPILE_GO) -c $< -o $@

