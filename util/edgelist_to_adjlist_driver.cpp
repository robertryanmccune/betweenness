/*
	Edgelist to adjlist driver, edgelist_to_adjlist_driver.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	Convert edgelist to adjacency list

	adjacency list requirements:
		- undirected: edges appear twice, i.e. (2,5) and (5,2)
		- no redudant edges, i.e. (2,5) only listed once
		- no self loops
		- indexed at 0
		- sequential
		- each row formatted: 
				vertex_id neighbor_id_1 neighbor_id_2 ...
				i.e. for vertex 5:
				5 1 2 3 6 10				
		- tab delimmited
		- rows sorted by vertex_id
		- list of neighbors sorted by neighbor_id

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.

*/
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>
#include<set>
#include<map>
#include<algorithm>

#include "edgelist_to_adjlist_utils.cpp"

using namespace std;

int main(int argc, char* argv[]) {

	if(argc !=3) {
		cout << "usage:  ./edge_to_adj   edgelist   adjlist" << endl;
		return -1;
	}

	string filename = argv[1];
	string outfile = argv[2];

	vector<pair<int, int> > edges;
	map<int, vector<int> > adjlist1;
	vector<pair<int, vector<int> > > adjlist2;

	//read in and process an edgelist
	read_edgelist(filename, edges);
	edgelist_remove_duplicates(edges);
	edgelist_remove_self_loops(edges);

	//make undirected adjlist
	convert_edgelist_to_adjlist(edges, adjlist1);
	adjlist_remove_reverse_edges(adjlist1);
	convert_adjlist_to_edgelist(adjlist1, edges);
	make_edgelist_undirected(edges);
	convert_edgelist_to_adjlist(edges, adjlist1);

	//sort adjlist
	adjlist_sort_rows(adjlist1);

	convert_adjlist_map_to_vec(adjlist1, adjlist2);
	adjlist_relabel_verts_index_0(adjlist2);
	adjlist_write_to_file(adjlist2, outfile);

	return 0;
}





