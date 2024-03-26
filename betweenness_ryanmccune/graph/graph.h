/*
	Graph data object header, graph.h
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	Reads in a graph from an adjacency list,
	stores graph in compressed sparse row format

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/

#ifndef GRAPH_H
#define GRAPH_H

#include<string>
using std::string;

class Graph{

	/* constructor, destructor, and public accessor functions */
	public:

		Graph(string);
		~Graph();
		int get_num_verts();
		int get_num_undir_edges();
		int get_num_dir_edges();
		int* get_csr1();
		int* get_csr2();
		int get_degree(int);
		string get_filename();


	/* private functions */
	private:

		void create_adjlist_from_file(string infile);
		void create_csr_from_adjlist();
		void remove_adjlist();
		int read_adjlist_rows(string infile);
		void read_row_neighbors(string infile);
		int parse_row_neighbors(string line);
		void read_adjlist(string infile);
		void parse_adjlist_row( string line, int* row );


	/* private variables */
	private:
		int num_verts;
		int num_edges;
		int* csr1;
		int* csr2;
		int* num_neighbors;
		int** adjlist;
		string filename;

};

#endif
