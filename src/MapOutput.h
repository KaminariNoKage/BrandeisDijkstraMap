/*************************************************************************************/
/*PATH OUTPUT PARAMETERS AND VARIABLES                                               */
/*************************************************************************************/
#define MapWidthFeet    5521 /*Width in feet of map.*/
#define MapHeightFeet   4369 /*Height in feet of map.*/
#define MapWidthPixels  2528 /*Width in pixels of map.*/
#define MapHeightPixels 2000 /*Height in pixels of map.*/
#define CropLeft         150 /*Pixels cropped from left of map.*/
#define CropDown         125 /*Pixels cropped from top of map.*/
int TotalLegs;     /*Total legs travelled on current path.*/
int TotalDistance; /*Total distance travelled on current path.*/
int TotalTime;     /*Total time travelled on current path.*/
FILE *pfile;  /*Path output file pointer (coordinates in feet).*/
FILE *pcfile; /*Path Cropped output file pointer (coordinates in pixels).*/


/*************************************************************************************/
/*OUTPUT VERTEX (feet coordinates) AND CROPPED VERTEX (pixel coordinates)            */
/*************************************************************************************/
void OutputPathVertex(int x, int y) {
fprintf(pfile,"%d %d\n",x,y);
fprintf(pcfile,"%d %d\n",
   (int)( ( x * ((double)MapHeightPixels / (double)MapHeightFeet) ) - CropLeft),
   (int)( ( y * ((double)MapWidthPixels  / (double)MapWidthFeet ) ) - CropDown)
   );
}


/*************************************************************************************/
/*INITIALIZE ROUTE OUTPUT                                                            */
/*************************************************************************************/
void RouteOpen() {
TotalLegs = TotalDistance = TotalTime = 0;
pfile  = FileOpen(FilePath,"w");
pcfile = FileOpen(FilePathCropped,"w");
OutputPathVertex(VX[Begin],VY[Begin]);
}


/*************************************************************************************/
/*PRINT ONE LEG OF A ROUTE                                                           */
/*************************************************************************************/
/*   Arg 1: Index of the edge to be traversed.*/
/*   Arg 2: Seconds to travel along this edge.*/
void PrintLeg(int edge, int seconds) {

/*Print the FROM line.*/
printf("\nFROM: (%s) %s\n",Vlabel[Estart[edge]],Vname[Estart[edge]]);

/*Print the ON line if edge name if defined.*/
if (strlen(Ename[edge])>2) printf("ON: %s\n",Ename[edge]);

/*Print the method of travel along this edge.*/
switch (Ecode[edge]) {
   case 'f': printf("%s","Walk"); break;
   case 'F': if (BoardFlag) printf("%s","Glide"); else printf("%s","walk"); break;
   case 'u': printf("%s","Walk up"); break;
   case 'U': if (BoardFlag) printf("%s","Board up"); else printf("%s","walk up"); break;
   case 'd': printf("%s","Walk down"); break;
   case 'D': if (BoardFlag) printf("%s","Coast down"); else printf("%s","walk down"); break;
   case 's': printf("%s","Climb up"); break;
   case 't': printf("%s","Go down"); break;
   case 'b': printf("%s","Walk"); break;
   }

/*Complete travel info line with distance, degrees, direction.*/
printf(" %d feet in direction %d degrees %s.\n",Elength[edge],Eangle[edge],Edir[edge]);

/*Print the TO line.*/
printf("TO: (%s) %s\n",Vlabel[Eend[edge]],Vname[Eend[edge]]);

/*Print the time line.*/
printf("(");
if ( (BoardFlag) && (strchr("fdustb",Ecode[edge])!=NULL) ) printf("no skateboards allowed, ");
if (seconds<60) printf("%d seconds)\n",seconds);
else printf("%.1f minutes)\n",((double)seconds)/60);

/*Output path.*/
OutputPathVertex(VX[Eend[edge]],VY[Eend[edge]]);

/*Update Summary Data*/
TotalLegs++;
TotalDistance += Elength[edge];
TotalTime += seconds;

}


/*************************************************************************************/
/*FINISH ROUTE OUTPUT                                                                */
/*************************************************************************************/
void RouteClose() {
if (TotalLegs==0) printf("\n*** Sorry, could not find a route!\n");
else printf("\nTotal distance = %d feet, total time = %.1f minutes.\n",TotalDistance,((double)TotalTime)/60);
fclose(pfile);
fclose(pcfile);
}
