/*
	Parallel Betweenness Centrality, centrality.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	Compute betweenness centrality for all nodes in a graph,
	with the Brandes algorithm, in parallel

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/

#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <iomanip>
#include <omp.h>

using namespace std;

#include "priority_queue.h"
#include "graph.h"
#include "dynamic_array.h"

typedef DynamicArray Darray;
typedef unsigned long long uint64;

int brandes(Graph&, float*);
void write_outfile(string, float*, int);
uint64 getTimeMs64();

/*
	Main function loads graph, runs centrality, and writes results to file
	with timing info, thread info, and status print-outs
*/
int main(int argc, char* argv[]) {

	// usage and input
  if(argc != 3) {  
		printf("usage: ./betweenness  [ingraph]  [outfile]\n");
		return -1;
  }
	string graph_str = argv[1];
	string outfile_str = argv[2];	

	// timing variables and start
	uint64 total_start, total_end;
	uint64 btwn_start, btwn_end;
	printf("Betweenness Centrality begin\n");
	total_start = getTimeMs64();

	// load graph from inputfile string
	Graph g(graph_str);
	printf("Loaded graph with %d vertices and %d edges\n", g.get_num_verts(), g.get_num_undir_edges() );

	// init centrality
	float* centrality;
	centrality = (float*)calloc(g.get_num_verts(), sizeof(float));

	

	// threading info
	if( omp_get_max_threads() > 1 ) {
		printf("Running in parallel...   YES\n");
		printf("Threads available: %d\n", omp_get_max_threads() );
	} else {
		printf("Running in parallel...   NO\n");
	}

	// run centrality
	printf("Beginning betweenness centrality computation...\n");	
	btwn_start = getTimeMs64();
	brandes(g, centrality);
	btwn_end = getTimeMs64();
	printf("Computing complete\n");

	// write results to file
	printf("Writing output file\n");
	write_outfile( outfile_str, centrality, g.get_num_verts() );
	printf("Output complete\n");

	// end
	total_end = getTimeMs64();
	printf("Program complete\n");
	printf("Betweenness Runtime: %d ms\n", btwn_end-btwn_start);
	printf("Total runtime: %d ms\n", total_end-total_start);
	
	free(centrality);

	return 0;

}

/*
	Compute betweenness centrality using Brandes method
*/
int brandes(Graph &g, float* centrality) {

	int* csr1 = g.get_csr1();
	int* csr2 = g.get_csr2();
	int num_verts = g.get_num_verts();

	/* 
		Compute centrality for each node i.
		Can be done in parallel
	*/
	#pragma omp parallel for shared(centrality)
	for(int i=0; i<num_verts; i++) {

		// declarations 
		PriorityQueue Q(num_verts);
		Darray** parents;
		int* num_paths;
		float* delta;

		// initializations
		Q.init(i);		
		parents = new Darray*[num_verts];
		delta = new float[num_verts];
		num_paths = new int[num_verts];
		for(int j=0; j< num_verts; j++) {
			parents[j]=new Darray();
			delta[j] = 0.0;
			num_paths[j]=0;
		}		
		num_paths[i] = 1;	

		/* 
			Dijkstra's Single-Source Shortest Path 
		*/
		while ( !Q.is_empty() ) {
						
			int current_node = Q.top().node;
			int min_dist = Q.top().dist;
			Q.pop_min();

			// get neighbors of current node
			int offset = csr1[current_node];
			int degree = csr1[current_node+1] - offset;
			int neighbor;
			
			// default edge weight of 1
			int weight = 1;	

			/*
				For each neighbor of the current_node,
				see if a new shortest path is found.
				If so, update the distance, parent, and position in Q
			*/
			for(int j=0; j<degree; j++) {
				neighbor = csr2[j+offset];
				if( Q.is_queued(neighbor) ) {
					int cur_dist = Q.get_dist(neighbor);					
					if( min_dist + weight < cur_dist ) {
						Q.update( neighbor, min_dist+weight );
						(*(parents[neighbor])).push_back(current_node);
						num_paths[neighbor] = num_paths[current_node];
					} else if( min_dist + weight == cur_dist ) {
						(*parents[neighbor]).push_back(current_node);
						num_paths[neighbor] += num_paths[current_node];
					}
				}
			}
		}

		/* 
			Propogate centrality from earliest discovered nodes to i.
			The priority_queue Q preserves the order of discovery,
			then use parents and num_paths to calculate the dependency delta.
			Delta is added to the centrality.
		*/		
		for(int j=0; j<num_verts; j++) {		
			int childNode = Q[j].node;
			Darray* parent_nodes = parents[childNode];
			for(int k=0; k<(*parent_nodes).get_size(); k++) {
				int parent_node = (*parent_nodes).at(k);
				if( num_paths[childNode] > 0 ) {					
					delta[parent_node] += num_paths[parent_node] / float(num_paths[childNode]) * (1 + delta[childNode]);
				} /*else {
					printf("error: num paths is 0\n");
				}*/
			}
			if (childNode != i ) {
				#pragma omp critical
				{
					centrality[childNode] += delta[childNode]/2.0;
				}
			}
		}

		// deallocate
		delete [] delta;
		delete [] num_paths;
		for(int j=0; j<num_verts; j++) {
			delete parents[j];
		}
		delete [] parents;
	}

	return 0;
}

/*
	Write one node_id and centrality value per line, tab-delimited, to a file
*/
void write_outfile( string str, float* cent, int num_verts ) {

	ofstream outstream;
	outstream.open( str.c_str() );
	
	outstream << "node\tbetweenness\n";

	for(int i=0; i<num_verts; i++) {
		outstream << i << "\t" << setprecision(7) << fixed << cent[i]  << endl;
	}

	outstream.close();
	return;
}

/* 
	Return the system time in milliseconds
*/
uint64 getTimeMs64()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64 ret = tv.tv_usec;
  // convert micro secs (10^-6) to millisecs (10^-3)
  ret /= 1000;
  
  // add seconds (10^0) after converting to millisecs (10^-3)
  ret += (tv.tv_sec*1000);
  return ret;
}	


