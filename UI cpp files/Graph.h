#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>    // std::find
using namespace std;

struct Edge {
	int beg, end;
};


class Graph {
	//adjacency list
	vector<vector<int>> adjList;
	vector<vector<int>> reverseAdjList;
	int size;
	double marginError = 0.01;
	double dampingFactor = 0.85;
	double otherTerm;

public:

	//The constructors
	Graph();
	Graph(vector<Edge> const& edgesList, int N);

	void printGraph(); //prints the graph
	double* getRank(); //gets the array of normalized page ranks
	void reSize(int);

};

#endif

