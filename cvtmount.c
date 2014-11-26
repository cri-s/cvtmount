/* cvtmount.c */
/* creates partial mount files for CSM84 from Protel pick&place file  */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#define getline mygetline

static int have_bottom;
static double X, Y, R, rot;
static double dummy1,dummy2,dummy3,dummy4;
static long I,J, Flag, Mnum,fdr_orient;
int cp,NumFeeders,head, fdrslot,height;
static char InFileName[256], FileName[256], CompFileName[256];
static char Designator[16],Footprint[16],PartType[16],side[16];
static char TopBot,a[256];
static char id[256];
static char line[4096];
struct FEEDER {
  int FdrSlot;               // feeder slot number [1-84]
  char PartName[33];         // name of part
  double orient;             // orientation in feeder
  int head;                  // head to mount with [1-3]
  double height;             // height of part
};
struct FEEDER feeders[999];
static unsigned int unit;

// static bool Done;
static FILE *InFile, *OutFile, *CompFile;

void outNum(RealNum, SigFrac)
double RealNum;
long SigFrac;
{
  double i;
  int j;
  char format[8];

  for (i = 1.0, j = 0; (i <= 3) || (i<= fabs(RealNum)); i *= 10.0, j++)
    {}
  //  printf("i j = %f %d, fabs(RealNum) = %f\n",i,j,fabs(RealNum));
  if (SigFrac > 0) 
    j += SigFrac+1;  /* J <= # int digits + 1 for decimal pt + #frac digits */
  if (RealNum < 0.0 )
    j += 1;          /* add a position for the minus sign */
  sprintf(format,"%%%d.%df",j,SigFrac);
  //  printf("format is : %s  Number is %f\n",format,RealNum);
    fprintf(OutFile, format, (RealNum));
}


void WriteLineNo(char*s)
{
  if(!strlen(s)) s="??";
  fprintf(OutFile,"M%d  %s  ",Mnum,s);
  Mnum += 1;
}

char*
getline(FILE*InFile) {
 int c;
  assert(InFile);
   redo:
	*line=0;
      fscanf(InFile,"%4095[^\n\r]",line);
     while((c=fgetc(InFile))&&c=='\n'||c=='\r'); ungetc(c,InFile);
        if(*line=='#') goto redo;
     return line;
}

#define MM	1
#define INCH	25.4*MM
#define MIL	INCH/1000.

void compfile(char *FileName) {
  NumFeeders = 1;
  CompFile = fopen(FileName,"r");
  assert(CompFile);
  while (!feof(CompFile) ) {
	getline(CompFile);
    sscanf(line,"%d %s %lg %d %lg\n",&feeders[NumFeeders].FdrSlot,feeders[NumFeeders].PartName,
	   &feeders[NumFeeders].orient,&feeders[NumFeeders].head
	,&feeders[NumFeeders].height);
    printf("feeder %d is %d %s %f %d\n",NumFeeders,feeders[NumFeeders].FdrSlot,&feeders[NumFeeders].PartName,
	   feeders[NumFeeders].orient,feeders[NumFeeders].head);
    NumFeeders ++;
  }
  fclose(CompFile);
  //  NumFeeders--;           // adjust count of components
}

main(int argc, char *argv[])
{
 int c;
  I = 2;
  Flag = 0;
  Mnum = 0;
  OutFile = NULL;
  InFile = NULL;
  CompFile = NULL;
  if (argc < 4) {
    printf("Usage : component_file,  pick&place file, top_mount_file, bot_mount_file\n");
    exit(-1);
  }
  sscanf(argv[1],"%s", &CompFileName);
  sscanf(argv[2],"%s", &InFileName);
  sscanf(argv[3],"%s", &FileName);
  compfile(CompFileName);             // read the component file
  //    open the data file
  InFile = fopen(InFileName,"r");
  getline(InFile);  // throw away first two lines
  getline(InFile);  // throw away first two lines
      c=sscanf(line,"Unit = %s",Designator);
  if(c==1) 
	if (!strcmp(Designator,"inch")) unit=INCH; else
	if (!strcmp(Designator,"mil")) unit=MIL; else
	if (!strcmp(Designator,"mm")) unit=MM; else
        printf("Syntax error inside Unit definition\n");

  OutFile = fopen(FileName,"w");
  if (OutFile != NULL)
    rewind(OutFile);
  else
    OutFile = tmpfile();
  if (OutFile == NULL)
    exit(-1);
  while (feof(InFile) == 0) {
      printf("reading line %d\n",I);
      getline(InFile);
      c=sscanf(line,"%s %s %lgmm %lgmm %lgmm %lgmm %lgmm %lgmm %s %lg %s\n",Designator,Footprint,&X,&Y,&dummy1,&dummy2,
	     &dummy3,&dummy4,side,&R,PartType),
		X*=MM,Y*=MM,dummy1*=MM,dummy2*=MM,
	     dummy3*=MM,dummy4*=MM,
	     TopBot=toupper(*side)=='T'?'T':'B';
  if(c<10)
      c=sscanf(line,"%s %s %lgmil %lgmil %lgmil %lgmil %lgmil %lgmil %s %lg %s\n",Designator,Footprint,&X,&Y,&dummy1,&dummy2,
	     &dummy3,&dummy4,side,&R,PartType),
		X*=MIL,Y*=MIL,dummy1*=MIL,dummy2*=MIL,
	     dummy3*=MIL,dummy4*=MIL,
	     TopBot=toupper(*side)=='T'?'T':'B';
//		X/=1./.0254,Y/=1./.0254,dummy1/=1./.0254,dummy2/=1./.0254,
  if(c<10)
      c=sscanf(line,"%s %s %lginch %lginch %lginch %lginch %lginch %lginch %s %lg %s\n",Designator,Footprint,&X,&Y,&dummy1,&dummy2,
	     &dummy3,&dummy4,side,&R,PartType),
		X*=INCH,Y*=INCH,dummy1*=INCH,dummy2*=INCH,
	     dummy3*=INCH,dummy4*=INCH,
	     TopBot=toupper(*side)=='T'?'T':'B';
//		X/=1./.0254,Y/=1./.0254,dummy1/=1./.0254,dummy2/=1./.0254,
//	     dummy3/=1./.0254,dummy4/=1./.0254;
 if(c<10) {
      c=sscanf(line,"%s %s %lg %lg %lg %lg %lg %lg %s %lg %s\n",Designator,Footprint,&X,&Y,&dummy1,&dummy2,
	     &dummy3,&dummy4,side,&R,PartType);
	if(c>=10)
		if(!unit) printf("error: unit definition is missing\n"),exit(-1);
		X*=unit,Y*=unit,dummy1*=unit,dummy2*=unit,
	     dummy3*=unit,dummy4*=unit,
	     TopBot=toupper(*side)=='T'?'T':'B';
}
  if(c<10) 
printf("error: found only %d items\n",c);
   /*   fscanf(InFile,"%s %s %lgmm %lgmm %lgmm %lgmm %lgmm %lgmm %c %lg %s\n",Designator,Footprint,&X,&Y,&dummy1,&dummy2,
	     &dummy3,&dummy4,&TopBot,&R,PartType);
**/
      printf("%s %s %lg %lg %lg %lg %lg %lg %c %lg '%s'\n",&Designator,&Footprint,X,Y,dummy1,dummy2,
	     dummy3,dummy4,TopBot,R,&PartType);
      if (TopBot == 'B' && Flag == 0) {
	Flag = 1;               // set flag to say we are doing bottom side now
	Mnum = 0;               // reset mount line number
	fclose(OutFile);
	sscanf(argv[4],"%s", &FileName);
	OutFile = fopen(FileName,"w");
	if (OutFile != NULL)
	  rewind(OutFile);
	else
	  OutFile = tmpfile();
	if (OutFile == NULL)
	  exit(-1);
      }
      head = 0;                    // 0 means no component match
	if(PartType[0]=='\'')  strncpy(PartType,PartType+1,sizeof(PartType)-1),PartType[sizeof(PartType)-1]=0;
	if(PartType[0]=='\"')  strncpy(PartType,PartType+1,sizeof(PartType)-1),PartType[sizeof(PartType)-1]=0;
	c=strlen(PartType); if(c) c--;
	if(PartType[c]=='\'') PartType[c]=0;
	if(PartType[c]=='\"') PartType[c]=0;
	if(strlen(PartType)==0) strcpy(id,Footprint); else
	if(strlen(Footprint)==0) strcpy(id,PartType); else
	c=sprintf(id,"%s-%s",Footprint,PartType);
//	  printf("matched part %s \n",&id);
      for (J=1;J<=NumFeeders;J++) {
//	  printf("test part %s \n",feeders[J].PartName);
	if (strcmp(id,feeders[J].PartName) == 0) {
	  fdr_orient = feeders[J].orient;
	  height = feeders[J].height*10; if(height>=1000) height=0;
	  head = feeders[J].head;
	  fdrslot = feeders[J].FdrSlot;
	  printf("matched part %s to component %d\n",&id,J);
	  break; 
	}
      }
      if (head == 0) {
	printf("Failed to match part '%s' to any component.\n",&id);
	//	exit(-1);
      }
      else {
	WriteLineNo(Designator);
	if (Flag == 1)  X = -X;  // mirror X axis for back side of board
	outNum(X,2);
	fprintf(OutFile,"  ");
	outNum(Y,2);
	fprintf(OutFile,"  ");
	rot = R-feeders[J].orient;  // correct rotation on parts that are not at nominal rotation in feeder
	while (rot > 128.00) rot -= 360.0;
	while (rot <= -128.00) rot += 360.0;
	outNum(rot,2);
	fprintf(OutFile,"  ");
	fprintf(OutFile, "%d%3.3d%2.2d\n",head,height,fdrslot);
      }
      I++;              // increment input file line counter
  }
  fclose(OutFile);
}



/* End. */



