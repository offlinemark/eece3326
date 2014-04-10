/* Project 6b
 * Mark Mossberg, Yufeng Wang
 * 4/14/14
 *
 * Contains _______
 *
 * Compiled on Mac OS X 10.9 using g++
 */

// Project 6
//
// Assumes that directed edges in both directions (x,y) and (y,x) are present in the input file.
//

/* Planning
void prim(graph &g, graph &sf)
- declare: minCost = 100, marked, unmarked && set to high values
- unmark all nodes and edges
- mark start (0)
- for i = 0 to # edges (which is numNodes - 1)

    - for every node i in graph
        if isMarked(i)
            for every  node j in graph
                if !isMarked(j) && isEdge(i, j) && getEdgeWeight < minCost
                    minCost = edgeWeight
                    marked = i
                    unmarked = j;
    - mark edge(marked, unmarked)
    - mark(unmarked)
    - reset to high values
    
typedef struct edgepair {
    int i;
    int j;
    int cost;
} edgepair;

void kruskal(graph &g, graph &sf)
- declare priority queue: queue<edgepair> q

queue<edgepair> q = getEdges(graph &g)
- unmark all edges
- while q not empty
    - get top of queue
    - add edge
    - if isCyclic
        remove edge
    else
        mark edge

getEdges(graph g)
loop through all nodes i
    loop through all nodes again j
        check if at least one node is unmarked and there is an edge between the two
            get edgepair 
            add to queue
            mark i and j
return queue
*/

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"
#include <queue>
#include <vector>
#include <cstdlib>

using namespace std;

int const NONE = -1;  // Used to represent a node that does not exist
int const HIGH = 100;   // high edge weight

typedef struct edgepair {
    int i;
    int j;
    int cost;
} edgepair;

/* global::getNeighbors(graph, current node)
 * - loop through numNodes
 * - check isEdge
 * - push into vector
 * - return vector
 */
vector<int> getNeighbors(graph &g, int current)
{
    vector<int> neighbors;
    for (int i = 0; i < g.numNodes(); i++)
    {
        if (g.isEdge(current, i))
            neighbors.push_back(i);
    }
    return neighbors;
}

void dfs(graph &g, int current)
// generic depth first search traversal
{
    g.visit(current);
    vector<int> neighbors = getNeighbors(g, current);
    for (int i = 0; i < (int) neighbors.size(); i++)
    {
        if (!g.isVisited(neighbors[i]))
            dfs(g, neighbors[i]);
    }
}

/* global::dfsAddEdges(graph, current node, sf)
 * - visit current node
 * - getNeighbors(graph, current node): return vector
 * - for each neighbor
 *      - check isVisited
 *      - addEdge
 *      - call dfsAddEdges
 */
void dfsAddEdges(graph &g, int current, graph &sf)
{
    g.visit(current);
    vector<int> neighbors = getNeighbors(g, current);
    for (int i = 0; i < (int) neighbors.size(); i++)
    {
        if (!g.isVisited(neighbors[i]))
        {
            sf.addEdge(current, neighbors[i], g.getEdgeWeight(current, neighbors[i]));
            sf.addEdge(neighbors[i], current, g.getEdgeWeight(neighbors[i], current));
            dfsAddEdges(g, neighbors[i], sf);
        }
    }
}

/* global::dfsCyclic(graph, current, prev)
 * - visit current
 * - getNeighbors
 * - remove prev from neighbors
 * - for each neighbor
 *   - if isVisited -> return false, theres a cycle
 *      - reason: if is neighbor, we know theres an edge, and if isVisited, theres cycle
 *   - else -> call dfsCyclic(graph, neighbor[i], current)
 */
void dfsCyclic(graph &g, int current, int prev, bool &flag)
{
    g.visit(current);
    vector<int> neighbors = getNeighbors(g, current);

    // remove prev from neighbors
    // make sure neighbors is not empty so we dont erase from empty vector
    if (prev != NONE && !neighbors.empty())
    {
        int index = 0;
        for (int k = 0; k < (int) neighbors.size(); k++)
        {
            if (neighbors[k] == prev)
                index = k;
        }

        // at some index, it is the (index + 1)th element
        // so just have to do .begin() + index
        neighbors.erase(neighbors.begin() + index);
    }

    for (int i = 0; i < (int) neighbors.size(); i++)
    {
        if (g.isVisited(neighbors[i]))
            flag = true;
        else
            dfsCyclic(g, neighbors[i], current, flag);
    }
}

bool isCyclic(graph &g)
// Returns true if the graph g contains a cycle.  Otherwise, returns false.
/* clear all visits
 * use a flag for detecting cycles
 * call dfsCyclic
 * return the flag
 */
{
    g.clearVisit();
    bool flag = false;
    int start = 0;
    int prev = NONE;

    dfsCyclic(g, start, prev, flag);

    return flag;
}

void findSpanningForest(graph &g, graph &sf)
// Create a graph sf that contains a spanning forest on the graph g.  
/* clear all visits
 * loop through nodes and find all trees in forest
 * - call dfsAddEdges to create the edges
 */
{
    g.clearVisit();

    // if a node is not visited, call dfsAddEdges on it
    // to create a tree with the node as the start
    for (int i = 0; i < sf.numNodes(); i++)
    {
        if (!sf.isVisited(i))
            dfsAddEdges(g, i, sf);
    }
}

bool isConnected(graph &g)
// Returns true if the graph g is connected.  Otherwise returns false.
/* clear all visits
 * call generic dfs(graph, current node)
 * loop through nodes and check isVisited
 * - if one is not visited, parts of graph not connected, return false
 * - otherwise, true
 */
{
    g.clearVisit();
    int start = 0;
    dfs(g, start);
    for (int i = 0; i < g.numNodes(); i++)
    {
        if (!g.isVisited(i))
            return false;
    }
    return true;
}

/*
void prim(graph &g, graph &sf)
- declare: minCost = 100, marked, unmarked && set to high values
- unmark all nodes and edges
- mark start (0)
- for i = 0 to # edges (which is numNodes - 1)

    - for every node i in graph
        if isMarked(i)
            for every  node j in graph
                if !isMarked(j) && isEdge(i, j) && getEdgeWeight < minCost
                    minCost = edgeWeight
                    marked = i
                    unmarked = j;
    - mark edge(marked, unmarked)
    - mark(unmarked)
    - reset to high values
*/

void prim(graph &g, graph &sf)
{
    // declare: minCost = 100, marked, unmarked && set to high values
    int minCost = HIGH;
    int marked, unmarked;
    
    // unmark all nodes and edges
    g.clearMark();
    
    //mark start (0)
    g.mark(0);
    
    for (int e = 0; e < g.numNodes() - 1; e++)  // edges
    {
        // find the least edge
        for (int i = 0; i < g.numNodes() ; i++)
        {
            if (g.isMarked(i))
            {
                for (int j = 0; j < g.numNodes(); j++)
                {
                    if (!g.isMarked(j) && g.isEdge(i, j) && g.getEdgeWeight(i, j) < minCost) 
                    {
                        minCost = g.getEdgeWeight(i,j);
                        marked = i;
                        unmarked = j;
                    }
                }
            }
        }
        // mark edge(marked, unmarked)
        g.mark(marked, unmarked);
        sf.addEdge(marked, unmarked, g.getEdgeWeight(marked, unmarked));
        sf.addEdge(unmarked, marked, g.getEdgeWeight(unmarked, marked));
        
        // mark(unmarked)
        g.mark(unmarked);
        
        // reset to high values
        minCost = HIGH;
    }

}

/*
typedef struct edgepair {
    int i;
    int j;
    int cost;
} edgepair;

void kruskal(graph &g, graph &sf)
- declare priority queue: queue<edgepair> q

queue<edgepair> q = getEdges(graph &g)
- unmark all edges
- while q not empty
    - get top of queue
    - add edge
    - if isCyclic
        remove edge
    else
        mark edge

getEdges(graph g)
loop through all nodes i
    loop through all nodes again j
        check if at least one node is unmarked and there is an edge between the two
            get edgepair 
            add to queue
            mark i and j
return queue
*/

class compare {
    public:
    bool operator() (edgepair &p1, edgepair &p2) {
        if (p1.cost < p2.cost)
            return true;
        else 
            return false;
    }
};

typedef priority_queue<edgepair, vector<edgepair>, compare> pqueue;

pqueue getEdges(graph &g)
{
    pqueue edges;
    for (int i = 0; i < g.numNodes(); i++)
    {
        for (int j = 0; j < g.numNodes(); j++)
        {
            if ((g.isMarked(i) || g.isMarked(j) ) && g.isEdge(i, j))
            {
                edgepair pair = {i, j, g.getEdgeWeight(i, j)};
                edges.push(pair);
                g.mark(i);
                g.mark(j);
            }
        }
    }
    return edges;
}


void kruskal(graph &g, graph &sf)
{
    pqueue edges = getEdges(g);
    g.clearMark();
    while (!edges.empty())
    {
        edgepair pair = edges.top();
        edges.pop();
        
        sf.addEdge(pair.i, pair.j, pair.cost);
        sf.addEdge(pair.j, pair.i, pair.cost);
        if (isCyclic(sf))
        {
            sf.removeEdge(pair.i, pair.j);
            sf.removeEdge(pair.j, pair.i);
        }
    }
}


int main()
{
   //char x;
   ifstream fin;
   stack <int> moves;
   string fileName;
   
   // Read the name of the graph from the keyboard or
   // hard code it here for testing.
   
   // fileName = "graph1.txt";

   cout << "Enter full filename: ";
   cin >> fileName;
   
   fin.open(fileName.c_str());
   if (!fin)
   {
      cerr << "Cannot open " << fileName << endl;
      exit(1);
   }

   try
   {
      cout << "Reading graph" << endl;
      graph g(fin);

      cout << g;

      bool connected;
      bool cyclic;

      connected = isConnected(g);
      cyclic = isCyclic(g);

      if (connected)
          cout << "Graph is connected" << endl;
      else
          cout << "Graph is not connected" << endl;

      if (cyclic)
          cout << "Graph contains a cycle" << endl;
      else
          cout << "Graph does not contain a cycle" << endl;

      cout << endl;
     
      cout << "Finding spanning forest" << endl;

      // Initialize an empty graph to contain the spanning forest
      graph sf(g.numNodes());
      findSpanningForest(g,sf);

      cout << endl;

      cout << sf;

      cout << "Spanning forest weight: " << sf.getTotalEdgeWeight()/2 << endl;

      connected = isConnected(sf);
      cyclic = isCyclic(sf);

      if (connected)
          cout << "Spanning forest is connected" << endl;
      else
          cout << "Spanning forest is not connected" << endl;

      if (cyclic)
          cout << "Spanning forest contains a cycle" << endl;
      else
          cout << "Spanning forest does not contain a cycle" << endl;
          
    
        graph sf1(g.numNodes());
        prim(g, sf1);
        cout << endl << "Prim" << endl;
        cout << sf1;
        cout << "sf1 weight: " << sf1.getTotalEdgeWeight()/2 << endl;
        
      connected = isConnected(sf1);
      cyclic = isCyclic(sf1);

      if (connected)
          cout << "sf1 prim is connected" << endl;
      else
          cout << "sf1 prim is not connected" << endl;

      if (cyclic)
          cout << "sf1 prim contains a cycle" << endl;
      else
          cout << "sf1 prim does not contain a cycle" << endl;

        graph sf2(g.numNodes());
        prim(g, sf2);
        cout << endl << "Kruskal" << endl;
        cout << sf2;
        cout << "sf2 weight: " << sf2.getTotalEdgeWeight()/2 << endl;
        
      connected = isConnected(sf2);
      cyclic = isCyclic(sf2);

      if (connected)
          cout << "sf2 kruskal is connected" << endl;
      else
          cout << "sf2 kruskal is not connected" << endl;

      if (cyclic)
          cout << "sf2 kruskal contains a cycle" << endl;
      else
          cout << "sf2 kruskal does not contain a cycle" << endl;
      cout << endl;
   }    
   catch (indexRangeError &ex) 
   { 
      cout << ex.what() << endl; exit(1);
   }
   catch (rangeError &ex)
   {
      cout << ex.what() << endl; exit(1);
   }
}

