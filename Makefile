all:
	gcc rwtest.c -D_GNU_SOURCE -o rwtest -O3
clean:
	rm -rf rwtest
install:
	mkdir -p /root/bin 2>/dev/null
	cp rwtest /root/bin/rwtest
uninstall:
	rm /root/bin/rwtest
