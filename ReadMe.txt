/* KAI AUSTIN
* 16 DECEMBER 2013
* README.TXT
*/

HOW TO USE:
1. Run Map.c
2. Enter in Brandeis location to start, via the name or the location code
3. Enter in the location to end
4. Enter in if path desired for skate board
5. Enter in if path desired with the minimal time, as opposed to distance
6. Results will print to the console

HOW IT WORKS:
--Step 1
The under inputs data into the console, which is then parsed to determine the following:
	1. An index number of the location where to begin
	2. An index number of the location where to end
	3. A boolean (true=1, false=0) for a skate board path
	4. A boolean (true=1, false=0) for whether to favor minimal time over minimal distance

--Step 2
An adjacency map is created for all the nodes (location vertex) with edges pointing to all other 
nodes they are pointing to. Both the distance and the edge index (corresponding to MapDataEdges).
	// Example: -> Vertex (Edge index, Distance from Vertex)
	Vertex: 0
	Vertex: 1-> 5 (12, 1509)-> 4 (8, 7040)-> 3 (4, 4369)-> 2 (0, 5521)
	Vertex: 2-> 58 (14, 1225)-> 3 (11, 7040)-> 4 (6, 4369)-> 1 (1, 5521)
	Vertex: 3-> 40 (16, 1408)-> 2 (10, 7040)-> 1 (5, 4369)-> 4 (2, 5521)
	Vertex: 4-> 97 (18, 2446)-> 1 (9, 7040)-> 2 (7, 4369)-> 3 (3, 5521)
	Vertex: 5-> 6 (58, 287)-> 100 (56, 307)-> 1 (13, 1509)
	...
This graph is treated as a directed graph since both edges are accounted for in MapDataEdges,
therefore each edge is added only once. 

IF MINIMAL TIME, the distances from the vertex are replaced with the time (in seconds) instead.
IF SKATEBOARD, nothing particularly happens. The path printed out will just indicate you have to walk.

Vertex 0 is a black hole which nothing points to. It is a very lonely node.


--Step 3
A heap is then created, with every vertex in the graph being added. Their default weight/distance
is set to 2000000000. Normally this would be set to INFINITY, however this was causing
errors since INIFINITY is not a number and not mathmatically valid. Therefore, I chose 
a decently large number that still had an upper bound.

--Step 4
The source (index referring to the beginning location) of the heap is then set by making its 
weight = 0. The heap is adjusted to accomodate the new weight.

--Step 5
Dijkstra's algorithm begins. 

Pop off the node and check the distance from the node to all the other nodes it connects
to via the edges. If the first nodes distance from the source + (plus) the distance to the
next new node is < (less than) the current distance of the new node from the source,
then replace the new nodes weight with that new distance.
Adjust the heap to accomodate the new changes in weight.
Then move onto the next node.

 An array of the shortest path between the source and the end location (being recorded 
 simulaneously) is also updated.
Note, the calculated path is traced back in reverse, so start and end location are flipped.
However the results are the same since this is an undirected graph.

--Step 6
Repeat this process until the minimal distance (or time) from the source to all other nodes
has been calculated. The path from the source to the goal location will also have been calculated.

--Step 7
The index of each location's vertex is contained within an array, symbolizing the path. 
The indices are then extracted and traced to get their corresponding edge-indexes in the graph.

--Step 8
With the edge indexes known, the path and details are then printed out to the console as well
as Route.txt file for MatLab.


