
all: optionaltest
.PHONY : all optionaltest

clean:
	rm -rf optionaltest build

build:
	mkdir -p build
	cd build; cmake ../

optionaltest: build
	cd build; $(MAKE) optionaltest; mv optionaltest ../
