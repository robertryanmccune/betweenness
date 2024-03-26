GR = ./graph
DA = ./dynamic_array
PQ = ./priority_queue
UT = ./util

all: betweenness edgelist_to_adjlist

betweenness: betweenness.cpp
	g++ -g -O3 -I$(GR) -I$(DA) -I$(PQ) betweenness.cpp $(GR)/graph.cpp $(DA)/dynamic_array.cpp $(PQ)/priority_queue.cpp -fopenmp -o betweenness

edgelist_to_adjlist: util/edgelist_to_adjlist_driver.cpp
	g++ -g -O3 -I$(UT) $(UT)/edgelist_to_adjlist_driver.cpp -o $(UT)/edgelist_to_adjlist

clean: 
	rm betweenness
	rm util/edgelist_to_adjlist
