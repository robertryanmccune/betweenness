/*
	Min Priority Queue Implementation, priority_queue.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	Stores (key,distance) pairs, with smallest value at front of queue
	keys are nodes, values are distances from src

	Created to work with node ids and distances

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/

#include <limits.h>
#include <stdio.h>
#include <cstdlib>

#include "priority_queue.h"

PriorityQueue::PriorityQueue(int sz) {
	queue = new PQ_elem[sz];
	ref = new int[sz];
	size = sz;
	size_orig = sz;
}

PriorityQueue::~PriorityQueue() {
	delete [] queue;
	delete [] ref;
}


void PriorityQueue::init(int src) {

	for(int i=0; i<size; i++) {
		queue[i].node = i;
		queue[i].dist = INT_MAX;
		ref[i]=i;
	}

	/* make src the head of the queue */
	queue[src].dist = 0;
	swap(src, 0);
	
	
	return;
}

/* given queue indices, swap two elements in the queue */
void PriorityQueue::swap(int i, int j) {

	/* swap queue positions */	
	int node1 = queue[i].node;
	int dist1 = queue[i].dist;

	queue[i].node = queue[j].node;
	queue[i].dist = queue[j].dist;

	queue[j].node = node1;
	queue[j].dist = dist1;

	/* update position in refs */
	ref[node1] = j;
	ref[ queue[i].node ] = i;
	
}

PQ_elem PriorityQueue::top(){
	return queue[0];
}

PQ_elem PriorityQueue::pop_min() {
	PQ_elem min = queue[0];

	swap(0, size-1);
	size--;
	heapify(0);

	return min;
}

/*
	help maintain the heap property.
	for index i, assume left and right child's trees
	are proper heaps, but that i may not be in place.
	let i float down to proper place in heap
*/
void PriorityQueue::heapify(int i) {
	int l = left(i);
	int r = right(i);
	int smallest;
	if( l <= size-1 && queue[l].dist < queue[i].dist ) {
		smallest = l;
	} else {
		smallest = i;
	}
	if( r <= size-1 && queue[r].dist < queue[smallest].dist ) {
		smallest = r;
	}
	if( smallest != i ) {
		swap(i, smallest);
		heapify(smallest);
	}
}

/* update a node's distance, then update heap */
void PriorityQueue::update(int node, int dist) {
	int i = ref[node];
	queue[i].dist = dist;
	int p = parent(i);
	while ( i > 0 && queue[p].dist > queue[i].dist ) {
		swap(i, p);
		i = p;
		p = parent(i);
	}
}

int PriorityQueue::left(int i) {
	return 2*i+1;
}

int PriorityQueue::right(int i) {
	return 2*i + 2;
}

int PriorityQueue::parent(int i) {
	if ( i%2 == 0 ) {
		return i/2-1;
	} else {
		return (i-1)/2;
	}
}

int PriorityQueue::get_size() {
	return size;
}

bool PriorityQueue::is_empty() {
	if(size == 0) return true;
	else return false;
}

bool PriorityQueue::is_queued(int v) {
	if( ref[v] <= size-1 ) return true;
	else return false;
}

int PriorityQueue::get_dist(int v) {
	int i = ref[v];
	return queue[i].dist;
}

void PriorityQueue::print() {
	printf("queue\n");
	for(int i=0; i<size; i++) {
		printf("%d:%d ", queue[i].node, queue[i].dist);		
		//cout << queue[i].node << ":" << queue[i].dist << " ";
	} printf("\n");
	printf("refs\n");
	for(int i=0; i<size_orig; i++) {
		if( ref[i] <= size-1 ) {		
			printf("%d:%d ", i, ref[i]);
		}
	} printf("\n\n");
}


PQ_elem PriorityQueue::operator[](int i) const {

	if( i >=0 && i < size_orig ) {
		return queue[i];
	} else {
		printf("error: index out of range");
		exit(1);
	}

}
