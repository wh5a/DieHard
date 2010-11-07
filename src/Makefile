DARWIN_COMMAND_STANDALONE := g++ -pipe -O3 -finline-limit=65000 -fkeep-inline-functions -finline-functions -ffast-math -fomit-frame-pointer -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -DNDEBUG  -I. -D_REENTRANT=1 -compatibility_version 1 -current_version 1 -dynamiclib libdiehard.cpp -o libdiehard.dylib;;

DARWIN_COMMAND_REPLICATED := g++ -pipe -O3 -finline-limit=65000 -fkeep-inline-functions -finline-functions -ffast-math -fomit-frame-pointer -DDIEHARD_REPLICATED=1 -DDIEHARD_MULTITHREADED=1 -DNDEBUG  -I. -D_REENTRANT=1 -compatibility_version 1 -current_version 1 -dynamiclib libdiehard.cpp -o libdiehard_r.dylib;;

LINUX_COMMAND_STANDALONE := g++ -finline-functions -static -malign-double -pipe -march=pentium4 -O3 -fomit-frame-pointer -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -Bsymbolic -o libdiehard.so -ldl -lpthread

LINUX_COMMAND_REPLICATED := g++ -static -malign-double -pipe -march=pentium4 -O3 -fno-rtti -finline-functions  -ffast-math -fomit-frame-pointer -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=1 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -Bsymbolic -o libdiehard_r.so -ldl -lpthread

SOLARIS_COMMAND_STANDALONE := CC -xO5 -fast -xildoff -xthreadvar=dynamic -L/usr/lib/lwp -R/usr/lib/lwp -DNDEBUG -I. -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=0 -G -PIC libdiehard.cpp sparc-interchange.il -o libdiehard.so -lthread -ldl -lCrun

SOLARIS_COMMAND_REPLICATED := CC -xO5 -fast -xildoff -xthreadvar=dynamic -L/usr/lib/lwp -R/usr/lib/lwp -DNDEBUG -I. -DDIEHARD_REPLICATED=1 -DDIEHARD_MULTITHREADED=0 -G -PIC libdiehard.cpp sparc-interchange.il -o libdiehard_r.so -lthread -ldl -lCrun

# After compiling:
# setenv LD_PRELOAD "$cwd/libdiehard.so"
# and whenever g++ supports thread-level storage on Solaris...
# g++ -fPIC -O3 -finline-limit=65000 -mcpu=ultrasparc -DNDEBUG  -I. -D_REENTRANT=1 -DDIEHARD_REPLICATED=0 -DDIEHARD_MULTITHREADED=1 -shared libdiehard.cpp -o libdiehard.so

DEPS = libdiehard.cpp bitmap.h \
	bumpalloc.h heapshield.cpp largeheap.h lockheap.h log2.h \
	marsaglia.h mmapalloc.h mmapwrapper.h platformspecific.h \
	randomheap.h diehardheap.h randomminiheap.h \
	randomnumbergenerator.h realrandomvalue.h sassert.h \
	sparc-interchange.il staticif.h staticlog.h  \
	usewinhard.cpp version.h wrapper.cpp

.PHONY: darwin linux solaris

linux:
	$(LINUX_COMMAND_STANDALONE)

solaris:
	$(SOLARIS_COMMAND_STANDALONE)
	$(SOLARIS_COMMAND_REPLICATED)

darwin:
	$(DARWIN_COMMAND_STANDALONE)
	$(DARWIN_COMMAND_REPLICATED)


clean:
	rm -f libdiehard.so libdiehard_r.so


