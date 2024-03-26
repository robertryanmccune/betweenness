/*
	Dynamic Array, dynamic_array.cpp
	Copyright 2023, Ryan McCune	<robertryanmccune@gmail.com>

	Implementation file for a dynamic array,
	a minimal vector

	This software is distributed under 
	the modified Berkeley Software Distribution (BSD) License.
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "dynamic_array.h"

/*
	Create a Dynamic Array object,
	with an array of capacity 1,
	and a size of 0, since no elements are added
*/
DynamicArray::DynamicArray() {
	size=0;
	capacity=2;
	array = (int*)calloc( capacity, sizeof(int) );
	for(int i=0; i<2; i++) {
		array[i]=0;
	}
	if(array == NULL) {
		printf("bad initialization of DynamicArray\n");
		exit(1);
	}
}

DynamicArray::~DynamicArray() {

	free(array);

}

/*
	Add an element to the array
*/
void DynamicArray::push_back(int v) {

	if(size == capacity) {
		expand();
	}
	array[size]=v;
	size++;

	return;
}

void DynamicArray::expand() {

	capacity*=2;
	array = (int*)realloc(array, capacity*sizeof(int) );
	if(array == NULL) {
		printf("bad DynamicArray realloc of capacity: %d\n", capacity);
		exit(1);
	}

	return;
}

int DynamicArray::get_size() {
	return size;
}	

void DynamicArray::print_capacity() {

	printf("capacity: %d\n",capacity);
	return;
}

void DynamicArray::print_size() {

	printf("size: %d\n",size);
	return;
}

int DynamicArray::at(int i) {
	return array[i];
}

void DynamicArray::set(int i, int v) {
	array[i]=v;
}

/*
int DynamicArray::operator[](int i) const {

	if( i <0 && i >= size ) {
		printf("error: index out of range");
		exit(1);
	}
	return array[i];
	
}
*/



