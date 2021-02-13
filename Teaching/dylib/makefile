default: | build del run clean
good: | build run clean
del: liblib.dylib
	rm liblib.dylib	
run: runme
	./runme || true
clean: runme
	rm *.dylib
	rm runme
build: lib.cpp buildLib.cpp main.cpp
	g++ -shared -o liblib.dylib lib.cpp
	g++ -shared -L. -llib -o libbuild.dylib buildLib.cpp
	g++ -o runme main.cpp
