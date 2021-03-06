CC=gcc
CFLAGS=-fPIC -fno-strict-aliasing -fno-omit-frame-pointer -Wall
INCLUDE=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I.
OBJECTS=build/jvm_agent.o
LIBRARY=libLTTng_java.so
LDFLAGS=-ldl -llttng-ust --shared
# -L/usr/lib/x86_64-linux-gnu

clean:
	rm -rf build
	rm -f libLTTng_java.so

all: $(LIBRARY)

build/%.o: %.c init
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIBRARY): $(OBJECTS)
	$(CC) -o $(LIBRARY)  $(OBJECTS) $(LDFLAGS)

init:
	mkdir -p build
