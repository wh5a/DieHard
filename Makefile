DEPS =  bitmap.h wrapper.cpp \
	bumpalloc.h heapshield.cpp largeheap.h lockheap.h log2.h \
	marsaglia.h mmapalloc.h mmapwrapper.h platformspecific.h \
	randomheap.h diehardheap.h randomminiheap.h \
	randomnumbergenerator.h realrandomvalue.h sassert.h \
	staticif.h staticlog.h  \
	libsamurai.cpp

libsamurai.a: libsamurai.o
	ar -cvr libsamurai.a libsamurai.o

libsamurai.o: $(DEPS)
	g++ -g -finline-functions -static -malign-double -pipe -march=pentium4 -O3 -fomit-frame-pointer -DNDEBUG  -I. -Isamurai -D_REENTRANT=1 -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -c libsamurai.cpp -Bsymbolic -o libsamurai.o -ldl -lpthread
