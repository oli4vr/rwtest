all:
	gcc writetest.c -D_GNU_SOURCE -o writetest -O3
	gcc readtest.c -D_GNU_SOURCE -o readtest -O3
win:
	x86_64-w64-mingw32-gcc readtest.c -DWINDOWS -o readtest.exe -O3
	x86_64-w64-mingw32-gcc writetest.c -DWINDOWS -o writetest.exe -O3
clean:
	rm -rf writetest readtest *.exe
install:
	mkdir -p /root/bin 2>/dev/null
	cp writetest /root/bin/writetest
	cp readtest /root/bin/readtest
uninstall:
	rm /root/bin/writetest /root/bin/writetest
