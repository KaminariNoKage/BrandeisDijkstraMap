/*************************************************************************************/
/*MAP DATA INPUT FUNCTIONS THAT ARE CALLED FROM MAIN                                 */
/*************************************************************************************/

#define DefaultEdgeCode 'F'

/*File pointers.*/
FILE *vfile; /*Vertex file.*/
FILE *efile; /*Edges file.*/

/*Open a file (use fclose to close a file).*/
/*   Arg 1: file name (PATH will be prepended to this name)*/
/*   Arg 2: "r" for read or "w" for write.*/
/*   Returns a file pointer.*/
FILE *FileOpen(char *filename, char *rw) {
FILE *filepointer;
char s[MaxString];
strcpy(s,PATH);
strcat(s,filename);
if ((filepointer = fopen(s,rw)) == NULL) {
     printf("Unable to open file: %s\n", s);
     exit(1);
     }
return(filepointer);
}

/*Remove last character of a a string s if it matches the character c.*/
void RemoveLastChar(char *s, char c) {
int lastchar = strlen(s)-1;
if (s[lastchar]==c) s[lastchar] = '\0';
}

/*Get vertex list.*/
void GetVertices() {
char s[MaxString];
char *ptr;
vfile = FileOpen(FileVertices,"r");
while (fgets(s,MaxString,vfile) != NULL) if ( (s[0]!='\n') && (s[0]!='/') ) {
   if (nV>=MaxVertex) {printf("ERROR: Max number of vertices exceeded.\n"); exit(1);}
   sscanf(s,"%d %s %d %d",&Vindex[nV],Vlabel[nV],&VX[nV],&VY[nV]);
   if ((Vname[nV] = malloc(strlen(s)+1)) == NULL) {printf("\nMemory allocation failed.\n"); exit(1);}
   ptr = strchr(s,'"');
   strcpy(Vname[nV],++ptr); /*start one character past leading "*/
   RemoveLastChar(Vname[nV],'\n');
   RemoveLastChar(Vname[nV],'"');
   nV++;
   }
fclose(vfile);
}

/*Get edge list (edge labels are skipped)*/
void GetEdges() {
char s[MaxString];
char CodeString[MaxString];
char *ptr;
efile = FileOpen(FileEdges,"r");
while (fgets(s,MaxString,efile) != NULL)  if ( (s[0]!='\n') && (s[0]!='/') ) {
   if (nE>=MaxEdge) {printf("ERROR: Max number of edges exceeded.\n"); exit(1);}
   sscanf(s,"%d %*s %*s %d %d %d %d %s %s",
      &Eindex[nE],&Estart[nE],&Eend[nE],&Elength[nE],&Eangle[nE],Edir[nE],CodeString);
   Ecode[nE] = CodeString[1];
   if (Ecode[nE]=='x') Ecode[nE] = DefaultEdgeCode;
   if ((Ename[nE] = malloc(strlen(s)+1)) == NULL) {printf("\nMemory allocation failed.\n"); exit(1);}
   ptr = strchr(s,'"');
   strcpy(Ename[nE],++ptr); /*start one character past leading "*/
   RemoveLastChar(Ename[nE],'\n');
   RemoveLastChar(Ename[nE],'"');
   nE++;
   }
fclose(efile);
}

/*Print the vertices - useful for debugging.*/
void PrintVertices() {
int i;
for (i=0; i<nV; i++) printf("%d: %s (%d,%d) %s\n",Vindex[i],Vlabel[i],VX[i],VY[i],Vname[i]);
}

/*Print the edges - useful for debugging.*/
void PrintEdges() {
int i;
for (i=0; i<nE; i++) printf("%d: %d->%d length=%d angle=%d %s code=%c\n",
   Eindex[i],Estart[i],Eend[i],Elength[i],Eangle[i],Edir[i],Ecode[i]);
}
