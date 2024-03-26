/*
	Graph data object implementation, graph.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	primarily developed for loading a graph from file into CSR format

	reads adjacency list graphfile into adjacency list data structure representation, 
	then converts the adjacency list to CSR (Compressed Sparse Row) format

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/

#include<fstream>
using std::ifstream;

#include<cstdlib>
using std::atoi;

#include<iostream>
using std::cout;
using std::endl;

#include "graph.h"

/* Constructor */
Graph::Graph(string infile) {

	filename = infile;
	create_adjlist_from_file(filename);
	create_csr_from_adjlist();
	remove_adjlist();

}

/* Destructor */
Graph::~Graph() {
	delete [] csr1;
	delete [] csr2;
}

/* num verts accessor */
int Graph::get_num_verts() {
	return num_verts;
}

/* num undirected edges accessor */
int Graph::get_num_undir_edges() {
	return (num_edges/2);
}

/* num directed edges accessor */
int Graph::get_num_dir_edges() {
	return num_edges;
}

int* Graph::get_csr1() {
	return csr1;
}

int* Graph::get_csr2() {
	return csr2;
}

int Graph::get_degree(int v) {
	return csr1[v+1] - csr1[v];
}

string Graph::get_filename() {
	return filename;
}

/***** Private functions *****/

/*
	Create an internal representation of
	an adjacency list from an
	adjacency list graph file
*/
void Graph::create_adjlist_from_file(string infile) {

	num_verts = read_adjlist_rows(infile);
	num_neighbors= new int[num_verts];
	adjlist=new int*[num_verts];
	read_row_neighbors(infile);
	int row_size;
	num_edges=0;
	for(int i=0; i<num_verts; i++) {
		row_size = num_neighbors[i];
		adjlist[i]=new int[row_size];
		num_edges+=row_size;
	}
	read_adjlist(infile);
	return;
}

/*
	Create a graph in the compressed sparse row format
	from the adjacency list format
*/
void Graph::create_csr_from_adjlist() {
	csr1=new int[num_verts+1];
	csr2=new int[num_edges];
	int row_size=0;
	int edge_count=0;
	csr1[0]=0;
	for(int i=0; i<num_verts; i++) {
		row_size=num_neighbors[i];
		for(int j=0; j<row_size; j++) {
			csr2[edge_count] = adjlist[i][j];
			edge_count++;
		}
		csr1[i+1]=edge_count;
	}
	return;

}

/*
	delete and free the memory of 
	an adjacency list
*/
void Graph::remove_adjlist() {
	for(int i=0; i<num_verts; i++)
		delete [] adjlist[i];
	delete [] adjlist;
	delete [] num_neighbors;
	return;
}

/*
	Compute the number of rows (and thus vertices)
	in an adjacency list file
*/
int Graph::read_adjlist_rows(string infile) {
	ifstream instream;
	instream.open(infile.c_str());
	string line;
	int num_rows=0;
	while( getline( instream, line) )
		num_rows++;
	instream.close();
	return num_rows;
}

/*
	Compute the number of neighbors for
	each vertice in the adjacency list file
*/
void Graph::read_row_neighbors(string infile) {
	ifstream instream;
	instream.open( infile.c_str() );
	string line;
	int row=0;
	int neighbor_count=0;
	while( getline( instream, line) ) {
		neighbor_count=parse_row_neighbors( line );	
		num_neighbors[row]=neighbor_count;
		row++;
	}
	instream.close();
	return;
}

/*
	Parse a row of an adjacency list
	Return the number of neighbors
*/
int Graph::parse_row_neighbors( string line ) {
	
	int neighbor_count=0;
	bool parsed_v=false;
	for(int i=0; i<line.size(); i++) {
		if( line[i] == ' ' ) {
			if(parsed_v==false) {
				parsed_v=true;
			} else {
				neighbor_count++;
			}
		}		
	}
	neighbor_count++;

	return neighbor_count;
}

/*
	Read in an adjacency list and
	parse each row
*/
void Graph::read_adjlist(string infile) {

	ifstream instream;
	instream.open( infile.c_str() );
	string line;
	int row_count=0;
	while( getline( instream, line ) ) {
		parse_adjlist_row( line, adjlist[row_count] );
		row_count++;
	}

	return;
}

/*
	parse each row of an adjacency list.
	Populate each row with the neighors
*/
void Graph::parse_adjlist_row( string line, int* row ) {

	string vert_str;
	int start, length;
	bool parsed_v=false;
	start=0;
	int vert;
	int vert_count=0;
	for(int i=0; i<line.size(); i++) {
		if( line[i] == ' ' ) {
			length=i-start;
			vert_str = line.substr( start , length );
			if(parsed_v==false) {
				parsed_v=true;
			} else {
				vert = atoi(vert_str.c_str() );
				row[vert_count] = vert;
				vert_count++;
			}
			start=i+1;
		}		
	}
	length=line.size()-start;
	vert_str = line.substr( start , length );
	vert = atoi(vert_str.c_str() );
	row[vert_count] = vert;

	return;
}
