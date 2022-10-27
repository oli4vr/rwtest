# rwtest <br />
By Olivier Van Rompuy <br />
<br />
Build with : <br />
make     -> Build linux executables <br />
<br />
Sequential Read Write IO test<br />
<p>Initially created to test the impact of network share failover scenarios on active read processes.
This tool can be used to test the impact of failures and short service downtimes of network shares.
But it is also capable of benchmarking IOPS, Throughput, ...
</p>
<p>I will write -n blocks of -b blocksize to a new file (or overwrite an existing one) and then repeatedly read -n blocks of -b blocksize. In both read and write phases, times are measured in µs for fopen,fclose and each individual fwrite/fread. Statistics are processed and printed to stdout per -n set of blocks.
</p>
Syntax : <br />
rwtest [-h]<br />
or <br />
rwtest [-f Filename] [-w WriteIterations] [-r ReadIterations] [-b KB_blocksize] [-n #Blocks] <br />
<br />

