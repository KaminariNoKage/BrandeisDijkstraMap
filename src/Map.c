


/*Brandeis Map Program*/
/* KAI AUSTIN - FALL 2013 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>

/***************************************************************************************/
/*Global parameters.                                                                   */
/***************************************************************************************/

/*Path to the map folder (that is prepended to all file names).*/
char PATH[100] = "";

/*File names*/
#define FileVertices    "MapDataVertices.txt" /*Map vertex data.*/
#define FileEdges       "MapDataEdges.txt"    /*Map edge data.*/
#define FilePath        "Route.txt"           /*Computed route points - feet.*/
#define FilePathCropped "RouteCropped.txt"    /*Computed route points - cropped pixels.*/

/*Limits*/
#define MaxString 100       /*Maximum length of any input string.*/
#define MaxLabel 5          /*Maximum length of a location label (includes ending \0).*/
#define MaxVertex 175       /*Maximum number of vertices.*/
#define MaxEdge   600       /*Maximum number of edges.*/
#define UndefinedIndex -1   /*All array insides start at 0; -1 is like a nil pointer.*/
#define InfiniteCost  10000 /*Cost of an edge that does not exist.*/

/*Speeds (based on 3.1 mph average human walking speed according to Wikipedia).*/
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
/*MAP DATA                                                                             */
/***************************************************************************************/

/*Vertex data.*/
int  nV=0;                        /*Number of vertices.*/
int  Vindex[MaxVertex];           /*Vertex index.*/
char Vlabel[MaxVertex][MaxLabel]; /*Vertex label - each at most 3 chars plus the \0.*/
int  VX[MaxVertex];               /*Vertex X-coord. in feet from upper left corner.*/
int  VY[MaxVertex];               /*Vertex Y-coord. in feet from upper left corner.*/
char *Vname[MaxVertex];           /*Vertex name - pointer to an allocated string.*/

/*Edge data.*/
int  nE=0;              /*Number of edges.*/
int  Eindex[MaxEdge];   /*Edge index.*/
int  Estart[MaxEdge];   /*Edge start (a vertex number).*/
int  Eend[MaxEdge];     /*Edge end (a vertex number).*/
int  Elength[MaxEdge];  /*Edge length in feet.*/
int  Eangle[MaxEdge];   /*Edge angle in degrees clockwise from north.*/
char Edir[MaxEdge][6];  /*Edge direction - one of North, NE, East, ..., NW*/
char Ecode[MaxEdge];    /*Edge code - f/F, u/U, d/D, s/t (steps up/down), b (bridge)*/
char *Ename[MaxEdge];   /*Edge name - pointer to an allocated string.*/

/*User input for current request.*/
int Begin, Finish; /*Vertex indices of begin and finish locations.*/
int BoardFlag;     /*1 if have skateboard*/
int TimeFlag;      /*1 to minimize time instead of distance*/



/***************************************************************************************/
/*INPUT / OUTPUT FUNCTIONS                                                             */
/***************************************************************************************/
#include "MapInputData.h" /*Used by GetVertices, GetEdges (called by main).*/
#include "MapInputUser.h" /*Used by GetRequest (called by main).*/
#include "MapOutput.h"    /*PrintLeg and PrintSummary functions used by Dijkstra.*/
#include "MapTime.h"


/***************************************************************************************/
/*GRAPH ADJACENCY LIST DATA STRUCTURE                                                  */
/***************************************************************************************/

/*Each node in the adjacency list*/
struct AdjListNode{
	int dest;	/*Current node value*/
	int lenToNext;	/*Length to the next node*/
	int skateboard;	/*Can cross the edge with skateboard*/
	int edgeIndex;	/*Corresponds to MapDataVertices*/
	struct AdjListNode* next; /*Points to next node*/
};

/*Represents adjacency list*/
struct AdjList{
	struct AdjListNode* head; /*Points to head node*/
};

/*Graph for the adjacency lists*/
struct AdjGraph{
	int S;	/*Size of the graph*/
	struct AdjList* array;	/*An array (list) a adjacency nodes)*/
};

/*For adding new nodes*/
struct AdjListNode* newAdjListNode(int dest, int lenToNext, int edgeIn){
	struct AdjListNode* nuNode = malloc(sizeof(struct AdjListNode));
	nuNode->dest = dest;
	nuNode->lenToNext = lenToNext;
	nuNode->edgeIndex = edgeIn;
	nuNode->next = NULL;

	return nuNode;
}

/*Main Graph making struct*/
struct AdjGraph* createGraph(int V){
	struct AdjGraph* graph = malloc(sizeof(struct AdjGraph));
	graph->S = V;

	/*Make array of adjLists*/
	graph->array = (struct AdjList*) malloc(V* sizeof(struct AdjList));

	/*Initialize each head of array (make null/empty)*/
	int m;
	for (m = 0; m < V; m++){
			graph->array[m].head = NULL;
	}

	return graph;
}

/*Undirected graph edge, goes both ways*/
void addEdge(struct AdjGraph* graph, int src, int dest, int leng, int edgeIn){

	/*Edge from src->dest*/
	struct AdjListNode* nuNode = newAdjListNode(dest, leng, edgeIn);
	nuNode->next = graph->array[src].head;
	graph->array[src].head = nuNode;

	/*Edge from dest->src, if undirected however edges accounted for in MatDataVerticies*/
	/*nuNode = newAdjListNode(src, leng, edgeIn);*
	* nuNode->next = graph->array[dest].head;*
	* graph->array[dest].head = nuNode;*/
}

/*Print out the graph, for debugging purposes*/
void printAdjGraph(struct AdjGraph* graph){
	int v;
	for (v=0; v < graph->S; v++){
		struct AdjListNode* toPrint = graph->array[v].head;
		printf("Vertex: %d", v);
		while (toPrint){
			printf("-> %d (%d, %d)", toPrint->dest, toPrint->edgeIndex, toPrint->lenToNext);
			toPrint = toPrint->next;
		}
		printf("\n");
	}
}


/***************************************************************************************/
/*HEAP DATA STRUCTURE                                                                  */
/***************************************************************************************/
/*Node of a Heap*/
struct HeapNode {
	int v;	/*Value of the Node*/
	int dist;	/*Distance from parent*/
};
/*The Heap itself*/
struct Heap{
	int S;	/*Size of the heap*/
	int maxS;	/*Maximum size of the heap*/
	int curPos;	/*The current position*/
	struct HeapNode* array;	/*Array of the nodes*/
};

/*Make a Heap Node*/
struct HeapNode makeHeapNode(int v, int dist){
	struct HeapNode* nuNode = malloc((sizeof(struct HeapNode)));
	nuNode->v = v;
	nuNode->dist = dist;
	return *nuNode;
}

/*Makes a Heap*/
struct Heap* makeHeap(int maxS){
	struct Heap* nuHeap = malloc((sizeof(struct Heap)));
	nuHeap->S = 0;
	nuHeap->maxS = maxS;
	nuHeap->array = malloc(maxS * sizeof(struct HeapNode*));
	return nuHeap;
}

void incHeapSize(struct Heap* heap){
	/*Doubles the max-size of the heap*/
	heap->maxS = 2 * heap->maxS;
}

/*Getting i in Heaps*/
int PARENT(int h){
	return floor(h/2);
}
int LEFT(int li){
	if (li == 0){
		return 1;
	}else{
		return 2 * li;
	}
}
int RIGHT(int ri){
	if (ri == 0){
		return 2;
	}else {
		return (2 * ri) + 1;
	}
}

/*Perculate up the heap*/
void percup(struct Heap* heap, int g){
	struct HeapNode thisNode = heap->array[g];
	struct HeapNode parNode = heap->array[PARENT(g)];

	int parentDist = parNode.dist;
	int curNodeDist = thisNode.dist;

	while (g > 0 &&  parentDist > curNodeDist ){
		/*Temporary variables*/
		struct HeapNode curTemp = heap->array[g];
		struct HeapNode parTemp = heap->array[PARENT(g)];

		/*Switch child with its parent*/
		heap->array[g] = parTemp;
		heap->array[PARENT(g)] = curTemp;

		g = PARENT(g);
		thisNode = heap->array[g];
	}
}

/*Perculate down the heap*/
void perdown(struct Heap* heap){
	int size = heap->S;
	int pos = 0;
	int newPos;	/*The node with a greater value to check*/
	while(1){
		int leftC = LEFT(pos);
		int rightC = RIGHT(pos);

		/*Check if i has a left leaf*/
		if(leftC < size){
			/*If left leaf, check if right leaf*/
			if(rightC < size){
				/*Determine which of the 3 has the smallest distance value*/
				int curVal = heap->array[pos].dist;
				int leftVal = heap->array[leftC].dist;
				int rightVal = heap->array[rightC].dist;

				if (leftVal <= rightVal && leftVal < curVal){ newPos = LEFT(pos); }
				else if (rightVal <= leftVal && rightVal < curVal){ newPos = RIGHT(pos); }
				else { break; }

			}else {
				newPos = leftC;
			}
		}else { break; }

		/*Updating the array by switching the lower value with the higher one*/
		struct HeapNode temp = heap->array[pos];
		heap->array[pos] = heap->array[newPos];
		heap->array[newPos] = temp;
		pos = newPos;
	}
}

/*Insert node into the heap*/
void insertNode(struct Heap* heap, int v, int dist){
	/*Check size of heap*/
	int maxSize = heap->maxS;
	int curSize = heap->S;

	if (maxSize == curSize){
		incHeapSize(heap);
	}

	/*Make and add the nuNode to the heap*/
	heap->array[curSize] = makeHeapNode(v, dist);
	percup(heap, curSize);
	perdown(heap);
	heap->S++;
}

int deleteMin(struct Heap* heap){
	/*Make sure the heap is not empty*/
	if (heap->S > 0){
		/*Get the top node*/
		int v = heap->array[0].v;

		/*Set the root to the last node*/
		heap->array[0] = heap->array[heap->S-1];
		heap->S--;

		/*Adjust rest of the heap to accomodate change*/
		perdown(heap);
		percup(heap, heap->S-1);
		return v;
	}
	return 0;
}

void printHeap(struct Heap* heap){
	printf("[");
	int f;
	for(f=0; f < heap->S; f++){
		printf("%d (%d) , ", heap->array[f].v, heap->array[f].dist);
	}
}


/***************************************************************************************/
/*Utility Functions for Paths and Edges					                               */
/***************************************************************************************/
int getPathLength(int s, int t, int prec[]){
	int pathLeng = 0;
	int current = t;

	/*Get the length of the path*/
	while (current != s){
		if (current <= 0){ break; }
		pathLeng++;
		current = prec[current];
	}
	pathLeng++;
	return pathLeng;
}

void getPath(int path[], int s, int t, int prec[]){
	int e = 0;
	int current = t;

	/*Gets the whole of the shortest path*/
	while (current != s){
		if (current <= 0){ break; }
		path[e] = current;
		current = prec[current];
		e++;
	}
	path[e] = current;
}
/*Unpack everything from the files into a graph*/
void unpackEdges(struct AdjGraph* graph, int wantSkateboard, int minTime){
	int d;
	for (d = 0; d < nE; d++){
		/*Note, this ignores "x"*/

		int timeVDist = Elength[d];
		if (minTime == 1){
			/*If minimum time is desired (as opposed to minimum distance)*/

			int timeT = d;
			int timeTT = d+1;
			if (d % 2){
				//Check if d is odd, if it is, adjust T and TT accordingly
				timeTT = d;
				timeT = d-1;
			}

			/* Check which of the two times is smaller */
			if (Time(timeT) > Time(timeTT)){
				/*Switch the distance with the time*/
				timeVDist = Time(timeTT);
			}
			else {
				timeVDist = Time(timeT);
			}
		}
		addEdge(graph, Estart[d], Eend[d], timeVDist, d);
	}
}


void tracePath(struct AdjGraph* graph, int edgePath[], int calPath[], int pLen){
	/*Traces the calculated path and makes an array of the edge index-es*/

	int b;
	int edgeIndex;
	struct AdjListNode* temp;
	for (b=0; b < pLen; b++){
		/*Get the index of the calculated path from the graph*/
		int c = calPath[b];
		temp = graph->array[c].head;
		while (temp != NULL){
			/*Trace the graph until next in calc == node->v*/
			if (temp->dest == calPath[b+1]){
				edgeIndex = temp->edgeIndex;
				edgePath[b] = edgeIndex;
				break;
			}
			temp = temp->next;
		}
	}
}

/***************************************************************************************/
/*Dijkstra's Single-Source Shortest Path Algorithm                                     */
/***************************************************************************************/

/*Do Dijkstra algorithm, reverse back edges, print path with PrintLeg.*/
void Dijkstra(struct AdjGraph* graph, int src, int end, int wantBoard, int minTime) {
	int rmNodeIndex, a;
	struct AdjListNode* rmNode;

	int distArray[graph->S];	/*For storing all the distances*/
	int previous[graph->S];	/*The previous node of the vertex*/
	int visited[graph->S];

	/*Making the AdjGraph*/
	unpackEdges(graph, wantBoard, minTime);

	printAdjGraph(graph);

	struct Heap* heap = makeHeap(graph->S);	/*initializing Heap*/

	/*Filling arrays with starting data*/
	for (a=0; a < graph->S; a++){
		if (a != src){
			distArray[a] = 1000000000;
			heap->array[a] = makeHeapNode(a, distArray[a]);
			previous[a] = a;
			visited[a] = 0;
		}
		insertNode(heap, a, distArray[a]);
	}

	heap->array[src] = makeHeapNode(src, 0);
	distArray[src] = 0;	/*Distance from the source is 0*/
	previous[src] = src;
	visited[src] = 1;


	/*Decreasing Edge Weight*/
	/*Getting index*/
	a = previous[src];

	/*Update the node in the heap*/
	heap->array[a].dist = distArray[0];

	while(a && heap->array[a].dist < heap->array[PARENT(a)].dist){
		previous[heap->array[a].v] = PARENT(a);
		previous[heap->array[PARENT(a)].v] = a;
		a = PARENT(a);
	}
	percup(heap, a);
	perdown(heap);

	while (heap->S > 0){
		rmNodeIndex = deleteMin(heap);	/*Remove and get the lowest node*/

		rmNode = graph->array[rmNodeIndex].head;

		/*Now check each of the edges*/
		while (rmNode != NULL){
			int curVal = distArray[rmNode->dest];
			int updateVal = distArray[rmNodeIndex] + rmNode->lenToNext;

			if (curVal > updateVal && visited[rmNode->dest] == 0){
				/*Update the array with the new value*/
				distArray[rmNode->dest] = updateVal;
				previous[rmNode->dest] = rmNodeIndex;
				visited[rmNode->dest] = 1;

				/*Update the heap*/
				heap->array[rmNode->dest].dist = updateVal;

				int k = previous[rmNode->dest];

				/*Update the node in the heap*/
				heap->array[k].dist = distArray[0];
				percup(heap, rmNode->dest);
				perdown(heap);
			}

			rmNode = rmNode->next;
		}
	}

	/*Get the calculated path*/
	int pLen = getPathLength(src, end, previous);
	int path[pLen];
	int edges[pLen-1];
	getPath(path, src, end, previous);

	/*Trace it back*/
	tracePath(graph, edges, path, pLen);

	/*Now print everything out to the console*/
	int z;
	for (z=0; z < pLen-1; z++){
		int q = (int)edges[z];
		PrintLeg(q, Time(q));
	}
}

/***************************************************************************************/
/*START OF PROGRAM                                                                     */
/***************************************************************************************/
int main() {
GetVertices();
GetEdges();

while (GetRequest() != 0) {
	fflush(stdout);
	RouteOpen();
	/*Make the graph*/
	struct AdjGraph* nuGraph = createGraph(nV);
	Dijkstra(nuGraph, Finish, Begin, BoardFlag, TimeFlag);
	RouteClose();
}
return(0);
}
