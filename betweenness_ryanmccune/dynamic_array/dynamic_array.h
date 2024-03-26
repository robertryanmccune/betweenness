#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
/*
	Dynamic Array, dynamic_array.h
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	A dynamic array -like a vector but minimal for the use case,
	can dynamically allocate memory to expand, or "push_back"

	Has an array of a certain capacity,
	and tracks the number of elements in the array with size.
	when size reaches capacity, the array capacity is doubledr

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/
/*
	
*/
class DynamicArray {

	public:
		DynamicArray();
		~DynamicArray();
		void push_back(int);
		void expand();
		void print_capacity();
		void print_size();
		int get_size();
		
		void set(int,int);
		int at(int);
		//int operator[](int) const; // righthand operator

	private:
		int* array;
		int size;
		int capacity;


};

#endif
