# rwtest <br />
By Olivier Van Rompuy <br />
<br />
Build with : <br />
make all    -> Build linux executables <br />
<br />
or <br />
make win    -> Build windows exe files on linux with x86_64-w64-mingw32-gcc <br />
<br />
IO Latency testing syntax : <br />
readtest file_path KB_blocksize GB_payload <br />
writetest file_path KB_blocksize GB_payload <br />
<br />
<br />
The readtest generates a file of size X, reads the first 16k blocks of a file over and over again and provides stats per sequential read of these 16384 blocks of block size Y. <br />

