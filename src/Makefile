LINUX_COMMAND_STANDALONE := g++ -finline-functions -static -malign-double -pipe -march=pentium4 -O3 -fomit-frame-pointer -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -Bsymbolic -o libdiehard.so -ldl -lpthread

LINUX_COMMAND_REPLICATED := g++ -static -malign-double -pipe -march=pentium4 -O3 -fno-rtti -finline-functions  -ffast-math -fomit-frame-pointer -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=1 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -Bsymbolic -o libdiehard_r.so -ldl -lpthread

# After compiling:
# setenv LD_PRELOAD "$cwd/libdiehard.so"

DEPS = libdiehard.cpp bitmap.h \
	bumpalloc.h heapshield.cpp largeheap.h lockheap.h log2.h \
	marsaglia.h mmapalloc.h mmapwrapper.h platformspecific.h \
	randomheap.h diehardheap.h randomminiheap.h \
	randomnumbergenerator.h realrandomvalue.h sassert.h \
	sparc-interchange.il staticif.h staticlog.h  \
	version.h wrapper.cpp

.PHONY: darwin linux solaris

libdiehard.so: $(DEPS)
	make debug

linux:
	$(LINUX_COMMAND_STANDALONE)

debug:
	g++ -g -finline-functions -static -malign-double -pipe -march=pentium4 -O3 -fomit-frame-pointer -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -Bsymbolic -o libdiehard.so -ldl -lpthread
