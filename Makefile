CC=gcc
CFLAGS=
INCLUDE=-I$(JAVA_HOME)/include -I.
OBJECTS=build/tp.o build/jvm_agent.o
LIBRARY=libLTTng_java.so
LDFLAGS=-ldl -llttng-ust

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
