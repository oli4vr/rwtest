all:
	gcc rwtest.c -o rwtest -O3
clean:
	rm -rf rwtest
install:
	mkdir -p ~/bin 2>/dev/null
	cp rwtest ~/bin/rwtest
uninstall:
	rm ~/bin/rwtest
	
	
