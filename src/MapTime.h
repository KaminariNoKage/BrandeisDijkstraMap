/***************************************************************************************/
/*Speeds (based on 3.1 mph average human walking speed according to Wikipedia).        */
/***************************************************************************************/
#define WalkSpeed 272    /*ft/min = (3.1 miles/hr) * (5280 ft/mile) / (60 mins/hr)*/
#define WalkFactorU 0.9  /*Multiply walk speed by this for walk up.*/
#define WalkFactorD 1.1  /*Multiply walk speed by this for walk down.*/
#define SkateFactorU 1.1 /*Multiply walk speed by this for skateboard up.*/
#define SkateFactorF 2.0 /*Multiply walk speed by this for skateboard flat.*/
#define SkateFactorD 5.0 /*Multiply walk speed by this for skateboard down.*/
#define StepFactorU 0.5  /*Multiply walk speed by this for walk up steps.*/
#define StepFactorD 0.9  /*Multiply walk speed by this for walk down steps.*/
#define BridgeFactor 1.0 /*Multiply walk speed by this for walking on a bridge.*/

/***************************************************************************************/
/*TIME (IN SECONDS) TO TRAVERSE AN EDGE i                                              */
/*   Note: This gives a reference for how roundoff is done when computing the seconds  */
/*   to traverse and edge, and because seconds are stored as an integer,               */
/*   results will be repeatable from one implementation to another of Dijkstra code.   */
/***************************************************************************************/
int Time(i) {
int t = (int)( (60 * ( (double)Elength[i] / (double)WalkSpeed) ) + 0.5 );
switch (Ecode[i]) {
   case 'f':                                                break;
   case 'F': t /= (BoardFlag) ? SkateFactorF : 1;           break;
   case 'u': t /= WalkFactorU;                              break;
   case 'U': t /= (BoardFlag) ? SkateFactorU : WalkFactorU; break;
   case 'd': t /= WalkFactorD;                              break;
   case 'D': t /= (BoardFlag) ? SkateFactorD : WalkFactorD; break;
   case 's': t /= StepFactorU;                              break;
   case 't': t /= StepFactorD;                              break;
   case 'b': t /= BridgeFactor;                             break;
   }
return(t);
}
