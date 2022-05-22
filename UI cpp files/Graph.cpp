#include "Graph.h"


Graph::Graph() {}

Graph::Graph(vector<Edge> const& edgesList, int N) {
	size = N;
	adjList.resize(N);
	reverseAdjList.resize(N);

	for (auto& x : edgesList) {
		adjList[x.beg].push_back(x.end);
		reverseAdjList[x.end].push_back(x.beg);	//This will be used in the pageRank algorithm
	}
}

void Graph::printGraph() {
	for (int i = 0; i < size; i++) {
		cout << i << " --> ";

		for (int v : adjList[i]) {
			cout << v << " ";
		}
		cout << endl;

	}
}

double* Graph::getRank() {

	otherTerm = (1.0 - dampingFactor) / size;
	double* prevRnk = new double[size];	//ranks of the nodes from previous iteration
	double* currRnk = new double[size]; //ranks of the nodes in this iteration
	double* difference = new double[size];
	double tempSum = 0;	//will hold the sum rnk(y)/out(y)
	bool diffFlag = 0;
	int tempNode;
	vector<int> sinkNodes; //this will contain which nodes are sink nodes which can be calculated in O(n).
	double sinkNodesRnk = 0; //this will contain the rank of the sink nodes;

	//base value
	for (int i = 0; i < size; i++) {
		prevRnk[i] = 1.0 / size;
		difference[i] = 1.0 / size;
		if (adjList[i].size() == 0) {
			sinkNodes.push_back(i);
		}
	}







	//Total complexity is O(N^2) worst case

	for (int i = 0; i < 100; i++) {

		//I will have to update the sum of sinkNodes ranks here.
		sinkNodesRnk = 0;
		for (int j = 0; j < sinkNodes.size(); j++) {
			sinkNodesRnk += prevRnk[sinkNodes[j]];
		}

		//we will update j node.
		for (int j = 0; j < size; j++) {



			for (int k = 0; k < reverseAdjList[j].size(); k++) {
				//get all  the nodes that point to node j
				tempNode = reverseAdjList[j][k];
				tempSum += (prevRnk[tempNode]) / (adjList[tempNode].size());

			}


			

			//It is working el 7md el allah
			tempSum = (otherTerm)+(dampingFactor * tempSum) + (dampingFactor * (sinkNodesRnk / size));

			difference[j] = abs(prevRnk[j] - tempSum);
			currRnk[j] = tempSum;

			tempSum = 0;

		}

		//They are pointers! so we need to copy element by element.
		for (int i = 0; i < size; i++) {
			prevRnk[i] = currRnk[i];
		}

		diffFlag = 0;
		//after each iteration check if the difference is less than 0.01
		for (int j = 0; j < size; j++) {
			if (difference[j] > marginError) {
				diffFlag = 1;
			}
		}

		//printing everything to check what is happening:
		/*for (int i = 0; i < size; i++) {
			cout << currRnk[i] << " ";
		}
		cout << endl;*/
		/*for (int i = 0; i < size; i++) {
			cout << difference[i] << " ";
		}
		cout << endl << endl;*/

		//We can print their sum and their sum should be one
		/*double tempSUM=0;
		for (int i = 0; i < size; i++) {
			tempSUM += currRnk[i] ;
		}
		cout << "\nThe sum of page Ranks: " << tempSUM << endl;*/


		//I checked this is working
		if (!diffFlag) {
			break;
		}

	}

	return currRnk;

}

void Graph::reSize(int N) {
	size = N;
	adjList.resize(N);
	reverseAdjList.resize(N);
}