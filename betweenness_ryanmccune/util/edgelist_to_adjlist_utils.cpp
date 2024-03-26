/*
	Utility functions for preprocessing graphs, 
	edgeslist_to_adjlist_utils.cpp.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>	

	Convert the common graph representation of edgelist
	to the adjacency format used by betweenness

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/


using namespace std;

/* read */
void read_edgelist(string, std::vector<pair<int, int> >&);
pair<int,int> parse_edge(string);
void edgelist_remove_duplicates(vector<pair<int, int> >&);
void edgelist_remove_self_loops(vector<pair<int, int> >&);
void convert_edgelist_to_adjlist(vector<pair<int, int> >&, map<int,vector<int> >&);
void adjlist_remove_reverse_edges(map<int, vector<int> >&);
void convert_adjlist_to_edgelist( map<int, vector<int> >&, vector<pair<int,int> >&);
void make_edgelist_undirected( vector<pair<int, int> >& edgelist);
void adjlist_sort_rows( map<int, vector<int> >& );
void convert_adjlist_map_to_vec( map<int, vector<int> >&, vector<pair<int, vector<int> > >& );
void adjlist_relabel_verts_index_0( vector<pair<int, vector<int> > >& );
void adjlist_write_to_file( vector<pair<int, vector<int> > >&, string);


/**
	Read in an edgelist file
	Store in a vector of edges
**/
void read_edgelist(string filename, std::vector<pair<int, int> >& edge_vec) {

	std::ifstream edgelist;
	edgelist.open(filename.c_str());
	string line;

	pair<int,int> edge;

	int count=0;
	while( getline(edgelist, line) ) {
		count++;
		if(line[0]=='#') { //ignore comments at top of graphs from SNAP
			//cout << "ignoring comment" << endl;
			continue;
		}
		edge=parse_edge(line);
		edge_vec.push_back(edge);
	}

	edgelist.close();
	cout << "read " << count << " lines" << endl;
	cout << edge_vec.size() << " edges" << endl;
	return;
}

/*
	Parse a row (edge) in an edgelist file
	return the edge as a pair on ints
*/
pair<int,int> parse_edge(string edge_str) {

	pair<int,int> edge;
	int v1,v2;
	int pos=edge_str.find("\t");
	v1=atoi( edge_str.substr(0,pos).c_str() );
	v2=atoi( edge_str.substr(pos+1,edge_str.length()).c_str() );
	//cout << edge_str << endl;
	//cout << v1 << " " << v2 << endl;
	//cout << endl;
	edge= make_pair(v1,v2);

	return edge;
}

/*
	Remove duplicate edges in an edge vector
	Use sets to detect duplicates
*/
void edgelist_remove_duplicates(vector<pair<int, int> >& edgelist) {
	set<pair<int, int> > edgelist_sans_duplicates;
	pair< set<pair<int, int> >::const_iterator, bool> result_pair;
	int num_duplicates=0;

	vector<pair<int, int> >::iterator edge_itr;
	
	for(edge_itr=edgelist.begin(); edge_itr!=edgelist.end(); ++edge_itr) {
		result_pair = edgelist_sans_duplicates.insert( *edge_itr );
		if(!result_pair.second) num_duplicates++;
	}	

	set<pair<int, int> >::iterator set_itr;
	edgelist.clear();
	for(set_itr=edgelist_sans_duplicates.begin(); set_itr!=edgelist_sans_duplicates.end(); ++set_itr)
		edgelist.push_back(*set_itr);

	cout << num_duplicates << " duplicate edges removed" << endl;
	return; 
}

/*
	Remove self loops
	Look for edges where vert1 == vert2
*/
void edgelist_remove_self_loops(vector<pair<int, int> >& edgelist) { 
	int num_self_loops=0;
	vector<pair<int, int> >::iterator edge_itr;
	vector<pair<int, int> > edgelist_no_loops;

	for(edge_itr= edgelist.begin(); edge_itr!= edgelist.end(); ++edge_itr) {
		if ( (*edge_itr).first == (*edge_itr).second )
			num_self_loops++;
		else
			edgelist_no_loops.push_back( (*edge_itr) );
	}
	
	edgelist=edgelist_no_loops;
	cout << num_self_loops << " self loops removed" << endl;
}

/*
	Convert an edgelist to an adjacency list.
	Edgelist is vector of vert pairs,
	Adjacency list is a map of vectors.
*/
void convert_edgelist_to_adjlist(vector<pair<int, int> >& edgelist, map<int,vector<int> >& adjlist) {

	vector<pair<int, int> >::iterator edge_itr;
	int v1,v2;
	int count=0;
	for(edge_itr=edgelist.begin(); edge_itr!=edgelist.end(); ++edge_itr) {
		v1=(*edge_itr).first;
		v2=(*edge_itr).second;
		if( adjlist.find(v1) == adjlist.end() ) {
			vector<int> row;
			row.push_back(v2);
			adjlist.insert( make_pair(v1, row) );
		} else {
			vector<int> row=(*(adjlist.find(v1))).second;
			row.push_back(v2);
			adjlist.erase(v1);
			adjlist.insert( make_pair(v1,row) );
		}
		count ++;
		//if(count%1000==0) cout << count << endl;
	}
	edgelist.clear();

	cout << count << " edges converted to adjlist" << endl;

	return;
}


/*
	for each edge, find if a reverse exists
	if so, remove the reverse, and keep the "first" edge found
*/
void adjlist_remove_reverse_edges(map<int, vector<int> >& adjlist) { 

	int num_reverse_edges_removed=0;
	map<int,vector<int> >::iterator adj_itr;
	for(adj_itr=adjlist.begin(); adj_itr!=adjlist.end(); ++adj_itr) {
		int v=(*adj_itr).first;
		vector<int> v_adj= adjlist[v];
		vector<int>::iterator v_row_itr;
		for(v_row_itr=v_adj.begin(); v_row_itr!=v_adj.end(); ++v_row_itr) {
			int u=*v_row_itr;
			if( adjlist.find(u) == adjlist.end() ) continue;
			else {
				vector<int> u_adj = adjlist[u];
				vector<int>::iterator u_row_itr;
				for(u_row_itr=u_adj.begin(); u_row_itr!=u_adj.end(); ++u_row_itr) {
					if(*u_row_itr == v) {
						num_reverse_edges_removed++;
						vector<int> row_with_v = adjlist[u];
						row_with_v.erase( find(row_with_v.begin(), row_with_v.end(), v) );
						vector<int> row_without_v = row_with_v;
						adjlist.erase(u);
						adjlist.insert( make_pair( u, row_without_v) );
					}
				}
			}
		}
	}

	cout << num_reverse_edges_removed << " reverse edges removed" << endl;
	return;
}

/*
	Convert an adjacency list to an edgelist
*/
void convert_adjlist_to_edgelist( map<int, vector<int> >& adjlist, vector<pair<int, int> >& edgelist) {
	
	edgelist.clear();
	int edges_processed=0;

	map<int, vector<int> >::iterator adj_itr;
	vector<int>::iterator row_itr;

	for(adj_itr=adjlist.begin(); adj_itr!=adjlist.end(); ++adj_itr) {
		for(row_itr=((*adj_itr).second).begin(); row_itr!=((*adj_itr).second).end(); ++row_itr) {
			edgelist.push_back( make_pair( (*adj_itr).first, *row_itr) );
			edges_processed++;
		}
	}

	adjlist.clear();
	cout << edges_processed << " edges coverted to edgelist" << endl;
	return;
}

/*
	Make an edgelist undirected.
	Input edgelist contains no duplicates,
	so iterate through the input vector,
	add the reverse to a new vector,
	then iterate through the new vector
	and add to the original vec
*/
void make_edgelist_undirected( vector<pair<int, int> >& edgelist) {


	vector<pair<int, int> >::iterator edge_itr;
	vector<pair<int, int> > reverse_edges;
	for( edge_itr=edgelist.begin(); edge_itr != edgelist.end(); ++edge_itr) {
		reverse_edges.push_back( make_pair( (*edge_itr).second, (*edge_itr).first) );
	}
	for( edge_itr=reverse_edges.begin(); edge_itr != reverse_edges.end(); ++edge_itr) {
		edgelist.push_back( *edge_itr );
	}

	cout << reverse_edges.size() << " edges added, " << edgelist.size() << " total edges" << endl;
	return;
}

/*
	Sort each row of an adjacency list
*/
void adjlist_sort_rows( map<int, vector<int> >& adjlist) {
	int num_rows_sorted=0;
	for( map<int, vector<int> >::iterator adj_itr = adjlist.begin(); adj_itr!= adjlist.end(); ++adj_itr) {
		sort( ((*adj_itr).second).begin(), ((*adj_itr).second).end() );
		num_rows_sorted++;
	}
	cout << num_rows_sorted << " rows of adjlist sorted" << endl;
	return;
}

/*
	Convert an adjacency list from a map representation to 
	vector of vectors representation
*/
void convert_adjlist_map_to_vec( map<int, vector<int> >& adjlist, vector<pair<int, vector<int> > >& adjlist_vec) {
	for( map<int, vector<int> >::iterator adj_itr=adjlist.begin(); adj_itr!= adjlist.end(); ++adj_itr) {
		adjlist_vec.push_back( make_pair( (*adj_itr).first, (*adj_itr).second) );
	}
	adjlist.clear();
	cout << adjlist_vec.size() << " verts in new adjlist" << endl;
	return;
}

/*
	Relabel all vertices of an adjacency list so the 
	graph is indexed at 0
*/
void adjlist_relabel_verts_index_0( vector<pair<int, vector<int> > >& adjlist) {

	map<int, int> vert_relabel;
	int v;
	int v_relabel = 0;
	int relabel_count=0;

	// create map of new labels
	vector<pair<int, vector<int> > >::iterator adj_itr;
	for( adj_itr = adjlist.begin(); adj_itr != adjlist.end(); ++adj_itr) {
		v = (*adj_itr).first;
		vert_relabel.insert( make_pair(v, v_relabel) );
		v_relabel++;
	}

	// relabel all verts
	vector<int> row, row_relabel;
	for(int i=0; i<adjlist.size(); i++) {
		v = adjlist[i].first;	 
		adjlist[i].first = vert_relabel[v];
		relabel_count++;

		for(int j=0; j<(adjlist[i].second).size(); j++) {
			(adjlist[i].second)[j] = vert_relabel[ (adjlist[i].second)[j] ];
			relabel_count++;
		}
	}

	cout << relabel_count << " vert_ids relabelled" << endl;
	return;
}

/*
	Write an adjacency list to file
*/
void adjlist_write_to_file( vector<pair<int, vector<int> > >& adjvec, string filename) {

	int count=0;
	ofstream outfile;
	outfile.open( filename.c_str() );
	for( vector<pair<int, vector<int> > >::iterator adjv_itr = adjvec.begin(); adjv_itr!=adjvec.end(); ++adjv_itr) {
		outfile << (*adjv_itr).first << " ";
		count++;
		for( vector<int>::iterator row_itr = ((*adjv_itr).second).begin(); row_itr != ((*adjv_itr).second).end(); ++row_itr) {
			outfile << *row_itr;
			count++;
			if( (row_itr+1)!= ((*adjv_itr).second).end() ) outfile << " ";
		}
		outfile << endl;
	}
	outfile.close();

	cout << count << " vert ids written to file" << endl;
	return;
}








