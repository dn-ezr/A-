
.PHONY:com apc test

all:com apc

com:
	@cd com && make

apc:
	@cd apc && make

test:
	@cd test && make

setup:
	[ -d lib ] || mkdir lib
	[ -d bin ] || mkdir bin
	[ -d obj ] || mkdir obj
	[ -d test/bin ] || mkdir test/bin

clean:
	@cd com && make clean
	@cd apc && make clean
	@cd test && make clean