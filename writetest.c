#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>

#define MAX_BLOCKS 2097152

int64_t timebuf[MAX_BLOCKS];
int64_t min_time,max_time;
unsigned char data[8388608];


int64_t getusecs() {
 int64_t tmp;
 struct timeval tv;
 gettimeofday(&tv,NULL);
 tmp=tv.tv_sec;
 tmp*=1000000;
 tmp+=tv.tv_usec;
 return tmp;
}


int main(int argc,char ** argv) {

 int64_t BS,PL;
 FILE * fp;

 int64_t *cbuf;

 int64_t stime;
 int64_t etime;


 int n,end;
 float sum_time=0;
 float avg_time=0,avg_bw=0,avg_iops=0;
 float fbuf;
 min_time=1000000000;max_time=0;

 printf("Sequential write stress test\n");
 printf("by Olivier Van Rompuy\n");

 if (argc<4) {
  printf("Syntax :\n");
  printf("%s file_path KB_blocksize GB_payload\n",argv[0]);
  return -1;
 }

 BS=atoi(argv[2]);

 if (BS<1 || BS>8192) {
  printf("Block size specified incorrect\n");
  return -2;
 }

 PL=atoi(argv[3]);

 if (PL<1 || PL>2048) {
  printf("Payload size specified incorrect\n");
  return -3;
 }

 for(n=0;n<8388608;n++) {
  data[n]=rand()&255;
 }


 cbuf=timebuf;

 BS<<=10;
 PL<<=30;

 end=PL/BS;

 fp=fopen(argv[1],"w");
 if (fp==NULL)
  return -4;

 /* Write a few blocks to eliminate startup latency/delays */
 
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);
 fwrite(data,BS,1,fp);

 for(n=0;n<end;n++) {

  stime=getusecs();
  fwrite(data,BS,1,fp);
  etime=getusecs();
  *cbuf=etime-stime;
  
  cbuf++;
 }
 fclose(fp);

 BS>>=10;
 for(n=0;n<end;n++) {
  sum_time+=timebuf[n];
  if (timebuf[n]<min_time) min_time=timebuf[n];
  if (timebuf[n]>max_time) max_time=timebuf[n];
 }
 avg_time=sum_time/end;

 fbuf=BS;
 avg_bw=(fbuf*1000000/avg_time)/1024;
 fbuf=end;
 avg_iops=fbuf*1000000/sum_time;

 PL>>=30;

 printf("Results :\n");
 fbuf=PL;
 printf("Total size       (GB) : %16.03f\n",fbuf);
 printf("Total IO time     (s) : %16.03f\n",sum_time/1000000);
 fbuf=end;
 printf("Total blocks      (#) : %16.03f\n",fbuf);
 fbuf=BS;
 printf("Block size       (KB) : %16.03f\n",fbuf);
 printf("Avg Write Time   (us) : %16.03f\n",avg_time);
 printf("Min Write Time   (us) : %12d\n",min_time);
 printf("Max Write Time   (us) : %12d\n",max_time);
 printf("Avg Throughput (MBps) : %16.03f\n",avg_bw);
 printf("Avg IOPS       (IOps) : %16.03f\n",avg_iops);

 return 0;
}

