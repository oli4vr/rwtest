#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <time.h>

#define MAX_BLOCKS 2097152
#define GROUP_READ 16384
#define MAX_BUFF 8388608
int64_t timebuf[MAX_BLOCKS];
int64_t min_time,max_time;
unsigned char data[MAX_BUFF];


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

 time_t now;


 int n,m,o,end;
 float sum_time=0;
 float avg_time=0,avg_bw=0,avg_iops=0;
 float hival,loval;
 float fbuf;
 float maxt,mint;
 int more,less;

 struct tm *local;

 printf("IO Read Latency Test\n");
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

 for(n=0;n<end;n++) {
  fwrite(data,BS,1,fp);
 }
 fclose(fp);


 min_time=1000000000;max_time=0;
 while (1) {
  cbuf=timebuf;
  fp=fopen(argv[1],"r");
  if (fp==NULL)
	  exit(4);
  for(n=0;n<GROUP_READ;n++) {

   stime=getusecs();
   fread(data,BS,1,fp);
   etime=getusecs();
   *cbuf=etime-stime;
   cbuf++;
  }
  fclose(fp);
 
  time(&now);
  local=localtime(&now);

  min_time=1000000000;max_time=0;sum_time=0;
  for(m=0;m<GROUP_READ;m++) {
   sum_time+=timebuf[m];
   if (timebuf[m]<min_time && timebuf[m]!=0) min_time=timebuf[m];
   if (timebuf[m]>max_time) max_time=timebuf[m];
  }
  avg_time=sum_time/GROUP_READ;
  maxt=max_time;mint=min_time;
  hival=(maxt+(avg_time*7))/8;
  loval=(mint+(avg_time*7))/8;
  more=0;less=0;
  for(m=0;m<GROUP_READ;m++) {
   if (timebuf[m]<loval) less++;
   if (timebuf[m]>hival) more++;
  }
  printf("%02d:%02d:%02d tMIN=%9.01f us     tAVG=%9.01f us     tMAX=%9.01f us           #<%9.01f us= %06d    #>%9.01f us= %06d\n",local->tm_hour,local->tm_min,local->tm_sec,mint,avg_time,maxt,loval,less,hival,more);
 }

 return 0;
}

