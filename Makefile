# fa3549856c35a4bfde761453eeed8c5e
CFLAGS=-O$(O) -std=c++11 -Xlinker -rpath -Xlinker ./tbb/lib/ia32/gcc4.4/ -L ./tbb/lib/intel64/gcc4.4/
O=2
LFLAGS=-ltbb -L ./tbb/lib/intel64/gcc4.4/ -I ./tbb/include/
LFLAGS_DEBUG = -ltbb_debug -L ./tbb/lib/intel64/gcc4.4/ -I ./tbb/include/
OBJS=objs/main.o objs/chunkinfo.o


.PHONY: all
all: objs tbb-wc

./tbb-wc: $(OBJS)
	@ echo "    LINK ./tbb-wc"
	@ $(CXX) $(OBJS) -o "./tbb-wc" $(LFLAGS)

objs/main.o: main.cpp chunkinfo.h
	@ echo "    CXX  ./main.cpp"
	@ $(CXX) $(CFLAGS) -c "./main.cpp" -o $@
objs/chunkinfo.o: chunkinfo.cpp chunkinfo.h
	@ echo "    CXX  ./chunkinfo.cpp"
	@ $(CXX) $(CFLAGS) -c "./chunkinfo.cpp" -o $@

objs:
	@ mkdir "objs"
.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./tbb-wc"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././tbb-wc

.PHONY: d debug
d: debug
debug: LFLAGS = ${LFLAGS_DEBUG}
debug: CFLAGS += -DDEBUG -DTBB_USE_DEBUG -g3 -Wall -Wextra
debug: O=0

debug: all

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S $(CHK_SOURCES)
