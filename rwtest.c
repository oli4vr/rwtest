#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>
#include <time.h>

#define MAX_BLOCKS 2097152
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
 int64_t tstime;
 int64_t tetime;
 time_t now;
 char *fname;
 char deff[]="rwtest.data";
 int n,m,o,end,fails,rc,ir,iw,ii;
 int linenr=0;
 float sum_time=0;
 float avg_time=0,avg_bw=0,avg_iops=0;
 float hival,loval;
 float fbuf;
 float maxt,mint;
 float otime,ctime,ttime;
 int more,less;
 struct tm *local;

 printf("Sequential IO Read Write Test\n");
 printf("by Olivier Van Rompuy\n");

 fname=deff;
 end=16384;
 BS=128;
 iw=1;
 ir=16;

 if (argc > 1) {
 argc--;
 argv++;

 if (strcmp(argv[0],"-h")==0) {
  printf("\nSyntax :\n");
  printf("  rwtest -h\n    ||\n");
  printf("  rwtest [-f Filename] [-w WriteIterations] [-r ReadIterations] [-b KB_blocksize] [-n #Blocks]\n");
  printf("\nDefaults :\n");
  printf("  WriteIterations   = 1\n");
  printf("  ReadIterations    = 16\n");
  printf("  KB_blocksize      = 128\n");
  printf("  #Blocks           = 16384\n");
  printf("  Filename          = rwtest.data\n\n");
  return -1;
 }

 for(;argc>0;argc--) {
  if (argv[0][0]=='-') {
   if (argc<2) { printf("Error : Missing parameter value for %s\n",argv[0]); exit(-1);}
   switch (argv[0][1]) {
    case 'w':
	    iw=atoi(argv[1]);
	    if (iw<=0) { printf("Error : Invalid # write iterations\n"); exit(-1);}
            argc--;
            argv++;
     break;
    case 'r':
	    ir=atoi(argv[1]);
	    if (ir<=0) { printf("Error : Invalid # read iterations\n"); exit(-1);}
            argc--;
            argv++;
     break;
    case 'b':
	    BS=atoi(argv[1]);
	    if (BS!=4 && BS!=8 && BS!=16 && BS!=32 && BS!=64 && BS!=128 && BS!=256 && BS!=512 && BS!=1024) {
		    printf("Error : Invalid Block Size\n");
		    exit(-1);
	    }
            argc--;
            argv++;
     break;
    case 'n':
	    end=atoi(argv[1]);
	    if (end<=0) { printf("Error : Invalid # blocks per iteration\n"); exit(-1);}
            argc--;
            argv++;
     break;
    case 'f':
     	    fname=argv[1];
            argc--;
            argv++;
     break;
   }
   argv++;
  }
 }
 }

 // Fill the buffer with random bytes
 BS<<=10;
 for(n=0;n<BS;n++) {
  data[n]=rand()&255;
 }

 // Write Iterations
 printf("\n\nSequential Write Phase:\n");
 for (ii=0;ii<iw;ii++)
 {
  if (linenr==0)
    printf("\nW TIME        Tmin      Tavg        Tmax          <      #           >      #  Fails     Topen    Tclose   TotTimeMS      IOPS      MBps\n");

  cbuf=timebuf;

  PL<<=30;

  fails=0;
  tstime=getusecs();
  stime=getusecs();
  fp=fopen(fname,"w+b");
  etime=getusecs();
  otime=etime-stime;
  if (fp==NULL)
          exit(4);
  for(n=0;n<end;n++) {

   stime=getusecs();
   rc=fwrite(data,BS,1,fp);
   fails+=(rc<1);
   etime=getusecs();
   *cbuf=etime-stime;
   cbuf++;
  }
  stime=getusecs();
  fclose(fp);
  etime=getusecs();
  ctime=etime-stime;
  tetime=getusecs();
  ttime=tetime-tstime;
 
  time(&now);
  local=localtime(&now);

  min_time=1000000000;max_time=0;sum_time=0;
  for(m=0;m<end;m++) {
   sum_time+=timebuf[m];
   if (timebuf[m]<min_time && timebuf[m]!=0) min_time=timebuf[m];
   if (timebuf[m]>max_time) max_time=timebuf[m];
  }
  avg_time=sum_time/end;

  maxt=max_time;mint=min_time;
  hival=(maxt+(avg_time*7))/8;
  loval=(mint+(avg_time*7))/8;
  more=0;less=0;
  for(m=0;m<end;m++) {
   if (timebuf[m]<loval) less++;
   if (timebuf[m]>hival) more++;
  }
  fbuf=(BS>>10);
  avg_bw=(fbuf*1000000/avg_time)/1024;
  fbuf=end;
  printf("%02d:%02d:%02d %9.0f %9.0f %11.0f  %9.0f %6d %11.0f %6d %6d %9.0f %9.0f %11.0f %9.0f %9.0f\n",local->tm_hour,local->tm_min,local->tm_sec,mint,avg_time,maxt,loval,less,hival,more,fails,otime,ctime,ttime/1000,fbuf*1000000/ttime,avg_bw);
  linenr=(linenr+1)&15;
 }


 linenr=0;
 // Read Iterations
 printf("\n\nSequential Read Phase:\n");
 for (ii=0;ii<ir;ii++) {
  if (linenr==0)
	  printf("\nR TIME        Tmin      Tavg        Tmax          <      #           >      #  Fails     Topen    Tclose   TotTimeMS      IOPS      MBps\n");
  cbuf=timebuf;
  fails=0;
  tstime=getusecs();
  stime=getusecs();
  fp=fopen(fname,"r+b");
  etime=getusecs();
  otime=etime-stime;
  if (fp==NULL)
	  exit(4);
  for(n=0;n<end;n++) {

   stime=getusecs();
   rc=fread(data,BS,1,fp);
   fails+=(rc<1);
   etime=getusecs();
   *cbuf=etime-stime;
   cbuf++;
  }
  stime=getusecs();
  fclose(fp);
  etime=getusecs();
  ctime=etime-stime;
  tetime=getusecs();
  ttime=tetime-tstime;
 
  time(&now);
  local=localtime(&now);

  min_time=1000000000;max_time=0;sum_time=0;
  for(m=0;m<end;m++) {
   sum_time+=timebuf[m];
   if (timebuf[m]<min_time && timebuf[m]!=0) min_time=timebuf[m];
   if (timebuf[m]>max_time) max_time=timebuf[m];
  }
  avg_time=sum_time/end;
  maxt=max_time;mint=min_time;
  hival=(maxt+(avg_time*7))/8;
  loval=(mint+(avg_time*7))/8;
  more=0;less=0;
  for(m=0;m<end;m++) {
   if (timebuf[m]<loval) less++;
   if (timebuf[m]>hival) more++;
  }
  fbuf=(BS>>10);
  avg_bw=(fbuf*1000000/avg_time)/1024;
  fbuf=end;
  printf("%02d:%02d:%02d %9.0f %9.0f %11.0f  %9.0f %6d %11.0f %6d %6d %9.0f %9.0f %11.0f %9.0f %9.0f\n",local->tm_hour,local->tm_min,local->tm_sec,mint,avg_time,maxt,loval,less,hival,more,fails,otime,ctime,ttime/1000,fbuf*1000000/ttime,avg_bw);
  linenr=(linenr+1)&15;
 }

 return 0;
}

