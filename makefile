
.PHONY:com apc

all:com apc

com:
	@cd com && make

apc:
	@cd apc && make

setup:
	[ -d lib ] || mkdir lib
	[ -d bin ] || mkdir bin
	[ -d obj ] || mkdir obj

clean:
	@cd com && make clean
	@cd apc && make clean