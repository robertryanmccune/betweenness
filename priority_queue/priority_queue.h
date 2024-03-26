/*
	Priority Queue header, priority_queue.h
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>	

	Custom priority queue to work with 
	(node, distance) pairs

	Elements in the queue are keyed by node id, 
	with the value being the distance to source node

	A PQ_elem is an element in the priority queue,
	with a node and dist value.
	
	Members:
	
		queue - the priority queue, as an array, storing PQ_elems

		ref - an array that stores node's position, or index, in the queue.
		the index to ref is a node's id, the value is its index in queue

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
		
*/

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

struct PQ_elem{
	int node;
	int dist;
};

class PriorityQueue {

	public:
		PriorityQueue(int);
		~PriorityQueue();

		void init(int);
		void swap(int, int);

		PQ_elem top();
		PQ_elem pop_min();
		void heapify(int);
		void update( int, int );

		int left(int);
		int right(int);
		int parent(int);
		int get_size();
		bool is_empty();
		bool is_queued(int);
		int get_dist(int);

		void print();

		PQ_elem operator[](int) const; // righthand operator

	private:
		PQ_elem* queue;
		int* ref;
		int size;
		/* 
			when an element is popped,
			it's swapped to the back of the queue,
			and the size is reduced by 1.
			we keep size_orig to keep track of old elements
		*/
		int size_orig;

};

#endif
