CC=gcc
CFLAGS=-fPIC
INCLUDE=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I.
OBJECTS=build/jvm_agent.o
LIBRARY=libLTTng_java.so
LDFLAGS=-ldl -llttng-ust -L/usr/lib/x86_64-linux-gnu

clean:
	rm -rf build
	rm -f libLTTng_java.so

all: $(LIBRARY)

build/%.o: %.c init
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIBRARY): $(OBJECTS)
	$(CC) -o $(LIBRARY) $(LDFLAGS) $(OBJECTS)

init:
	mkdir -p build
