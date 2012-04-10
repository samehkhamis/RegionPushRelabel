/////////////////////////////////////////////////////////////////////////////
// Filename: Example.h
// Author:   Sameh Khamis
//
// Description: Usage example
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;

#include "RegionPushRelabel.h"

typedef Array<                 // Each cell has 1 node
	Arc<0, 0, Offsets<1, 0> >,  // From node 0 in the cell to node 0 in the cell on the right
	Arc<0, 0, Offsets<-1, 0> >, // From node 0 in the cell to node 0 in the cell on the left
	Arc<0, 0, Offsets<0, 1> >,  // From node 0 in the cell to node 0 in the cell below
	Arc<0, 0, Offsets<0, -1> >  // From node 0 in the cell to node 0 in the cell above
> FourConnected;

typedef RegionPushRelabel<
	short, long,               // Capacity Type, Flow Type
	Layout<
		FourConnected,         // Layout is four-connected
		BlockDimensions<3, 3>  // Divide the graph into blocks of 3x3 cells each
	>,
	ThreadCount<2>             // Use two threads
> RegularGraph;

int main()
{
	// Even though the graph is 4x4, we allocate 6x6 nodes to have
	// a graph of dimensions that divide the block dimensions
	long d[] = {6, 6};
	RegularGraph *g = new RegularGraph(d);

	// I am calculating the node index dynamically since the node
	// index depends on the actual graph size (6x6), not the original
	// size (4x4)
	#define ID(i, j) (d[1] * (i) + (j))
	
	g->add_terminal_weights(ID(0, 0), 100, 0);    // Node (0, 0) is connected to source
	g->add_terminal_weights(ID(3, 3), 0, 100);    // and node (3, 3) to sink
	
	g->add_edge(ID(0, 0), ID(0, 1), 5, 0);        // Edge capacities from node 0 to 1 and vice versa
	g->add_edge(ID(0, 0), ID(1, 0), 5, 0);        // etc.
	g->add_edge(ID(0, 1), ID(0, 2), 1, 0);
	g->add_edge(ID(0, 1), ID(1, 1), 5, 0);
	g->add_edge(ID(0, 2), ID(0, 3), 5, 0);
	g->add_edge(ID(0, 2), ID(1, 2), 5, 0);
	g->add_edge(ID(0, 3), ID(1, 3), 5, 0);
	g->add_edge(ID(1, 0), ID(1, 1), 5, 0);
	g->add_edge(ID(1, 0), ID(2, 0), 5, 0);
	g->add_edge(ID(1, 1), ID(1, 2), 2, 0);
	g->add_edge(ID(1, 1), ID(2, 1), 5, 0);
	g->add_edge(ID(1, 2), ID(1, 3), 5, 0);
	g->add_edge(ID(1, 2), ID(2, 2), 5, 0);
	g->add_edge(ID(1, 3), ID(2, 3), 5, 0);
	g->add_edge(ID(2, 0), ID(3, 0), 5, 0);
	g->add_edge(ID(2, 1), ID(3, 1), 5, 0);
	g->add_edge(ID(2, 2), ID(2, 3), 5, 0);
	g->add_edge(ID(2, 2), ID(3, 2), 5, 0);
	g->add_edge(ID(2, 3), ID(3, 3), 5, 0);
	g->add_edge(ID(3, 0), ID(3, 1), 5, 0);
	g->add_edge(ID(3, 1), ID(3, 2), 3, 0);
	g->add_edge(ID(3, 2), ID(3, 3), 5, 0);
	
	g->compute_maxflow();
	cout << "Flow = " << g->get_flow() << endl;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			cout << "Segment of node (" << i << ", " << j << ") = " << g->get_segment(ID(i, j)) << endl;
	
	delete g;
	cin.ignore();
	return 0;
}
