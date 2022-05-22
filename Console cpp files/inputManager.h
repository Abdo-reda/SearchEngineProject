#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <sstream> 
#include <unordered_map>
#include "Graph.h"

using namespace std;

class inputManager {

private:
	unordered_map<string, int> fromWebsitesToID;
	vector<string>  fromIDtoWebsites;
	vector<vector<string>> keywordsList;
	Graph websitesGraph;
	int* impressions;
	int numSites;
	int* clicks;

public:

	inputManager(); //empty constructor

	//helper functions used in the constructor
	void setupGraph();	//read websiteGraph.csv and intilizes it.
	void setupImpressions();
	void setupKeywordsList();	//read keywords.csv and create a vectors of vectors that will contain them.
	void setupClicks();

	//getter methods
	Graph getWebsiteGraph();
	int* getImpressions();	
	vector<vector<string>> getKeywordsList();
	int getSize();
	unordered_map<string, int> getFromWebsitesToID();
	vector<string>  getFromIDtoWebsites();
	int* getClicks();

	

};

#endif