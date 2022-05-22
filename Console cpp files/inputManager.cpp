#include "inputManager.h"



inputManager::inputManager() {
	//initilize the class variables
	setupGraph();
	setupImpressions();
	setupKeywordsList();
	setupClicks();
}


void inputManager::setupGraph() {

	//-------------------Reading the webgraphsFile
	fstream webGraphFile("webGraph.csv");

	if ((!webGraphFile.is_open())) {

		cout << "There was an error opening one of the input files! Please fix and try again!" << endl;
		system("pause");
		return;
	}

	//helper variables
	string line;
	string websiteOne = "", websiteTwo = "";
	bool flag = 0;
	vector<Edge> edgeList;
	Edge tempEdge;

	//read data line by line
	while (getline(webGraphFile, line)) {


		if (line == "") {
			break;
		}


		//Read the names of the websites line by line.
		for (int i = 0; i < line.size(); i++) {
			if (line.at(i) == ',') {
				flag = 1;
			}
			else if (!flag) {
				websiteOne += line.at(i);
			}
			else {
				websiteTwo += line.at(i);
			}
		}

		//intilize them in the map if they haven't been already.
		if (fromWebsitesToID.count(websiteOne) == 0) {
			fromIDtoWebsites.push_back(websiteOne);
			fromWebsitesToID[websiteOne] = fromWebsitesToID.size(); //does this make sense or will it cause trouble? it seems to be working.
		}
		if (fromWebsitesToID.count(websiteTwo) == 0) {
			fromIDtoWebsites.push_back(websiteTwo);
			fromWebsitesToID[websiteTwo] = fromWebsitesToID.size();
		}

		//add the edge to the vector of edges.
		tempEdge.beg = fromWebsitesToID[websiteOne];
		tempEdge.end = fromWebsitesToID[websiteTwo];
		edgeList.push_back(tempEdge);

		//reset
		flag = 0;
		websiteOne = "";
		websiteTwo = "";

	}


	Graph websitesGraph(edgeList, fromWebsitesToID.size());
	this->websitesGraph = websitesGraph;
	numSites = fromWebsitesToID.size();

	webGraphFile.close();
	
}


void inputManager::setupImpressions() {


	vector<int> tempImpressions;
	tempImpressions.resize(fromIDtoWebsites.size());
	fstream impressionsFile("numOfImpressions.csv");

	if ((!impressionsFile.is_open())) {
		cout << "There was an error opening one of the input files! Please fix and try again!" << endl;
		system("pause");
		return;
	}


	//helper variables
	string line;
	string website = "", tempImp = "";
	bool flag = 0;
	int tempX;

	while (getline(impressionsFile, line)) {

		if (line == "") {
			break;
		}

		//Read the names of the websites line by line.
		for (int i = 0; i < line.size(); i++) {
			if (line.at(i) == ',') {
				flag = 1;
			} else if (!flag) {
				website += line.at(i);
			} else {
				tempImp += line.at(i);
			}
		}

		stringstream ss(tempImp);
		ss >> tempX;
		if (fromWebsitesToID.find(website) != fromWebsitesToID.end()) {
			//if the website is found
			tempImpressions[fromWebsitesToID[website]] = tempX;
		} else {
			//if the website is not found then it wasn't in the graph csv file, so we add it to .
			numSites++;
			fromIDtoWebsites.push_back(website);
			fromWebsitesToID[website] = fromWebsitesToID.size();
			tempImpressions.resize(numSites);
			tempImpressions[numSites - 1] = tempX;
			websitesGraph.reSize(numSites); //Add it to the graph as a disconnected vertex
		}

		//reset
		flag = 0;
		website = "";
		tempImp = "";

	}


	//copy the contents from the temp impressionsvector to the impressions array.
	impressions = new int[numSites];
	for (int i = 0; i < numSites; i++) {
		impressions[i] = tempImpressions[i];
	}


	//When I make the file as an ouput stream, it deletes everything, so I will just have to write everything again.
	//This way I made sure, even the websites that weren't included are now included, this will make it easier to update
	ofstream outputFile("numOfImpressions.csv");
	for (int i = 0; i < numSites; i++) {
		if (i == (numSites - 1)) outputFile << fromIDtoWebsites[i] << "," << impressions[i];
		else outputFile << fromIDtoWebsites[i] << "," << impressions[i] << endl;
	}


	outputFile.close();
	impressionsFile.close();

}

void inputManager::setupKeywordsList() {

	
	keywordsList.resize(fromWebsitesToID.size());

	fstream keywordsFile("keywords.csv");

	if ((!keywordsFile.is_open())) {

		cout << "There was an error opening one of the input files! Please fix and try again!" << endl;
		system("pause");
		return;
	}


	//helper variables
	string line;
	string website = "", tempKeyword = "";
	bool flag = 0;

	while (getline(keywordsFile, line)) {

		if (line == "") {
			break;
		}

		//Read the names of the websites line by line.
		for (int i = 0; i < line.size(); i++) {
			if ((line.at(i) == ',') && (!flag)) {
				flag = 1;
			}
			else if ((line.at(i) == ',') && (flag)) {
				keywordsList[fromWebsitesToID[website]].push_back(tempKeyword);
				tempKeyword = "";
			}
			else if (!flag) {
				website += line.at(i);
			}
			else {
				tempKeyword += line.at(i);
			}
		}

		//reset
		keywordsList[fromWebsitesToID[website]].push_back(tempKeyword); //add the last word
		flag = 0;
		website = "";
		tempKeyword = "";

	}

	keywordsFile.close();
}


void inputManager::setupClicks() {

	//It will look for a clicks file, if it doesn't exist then it will create one and initilize the value of each website by 0.
	//If it does exist then setup the clicks file correctly.
	//The clicks file will have the same format as the impressions file.

	clicks = new int[numSites];
	for (int i = 0; i < numSites; i++) {
		clicks[i] = 0;
	}

	fstream clicksFile("numOfClicks.csv");
	if ((clicksFile.is_open())) {

		//helper variables
		string line;
		string website = "", tempImp = "";
		bool flag = 0;
		int tempX;

		while (getline(clicksFile, line)) {

			//Read the names of the websites line by line.
			for (int i = 0; i < line.size(); i++) {
				if (line.at(i) == ',') {
					flag = 1;
				} else if (!flag) {
					website += line.at(i);
				} else {
					tempImp += line.at(i);
				}
			}

			stringstream ss(tempImp);
			ss >> tempX;
			if (fromWebsitesToID.find(website) != fromWebsitesToID.end()) {
				//if the website is found
				clicks[fromWebsitesToID[website]] = tempX;
			}


			//reset
			flag = 0;
			website = "";
			tempImp = "";

		}


	}


	//If the clicksFile doesn't exist, then create it and let everythign be 0.
	ofstream outputFile("numOfClicks.csv");

	for (int i = 0; i < numSites; i++) {
		if (i == (numSites - 1)) outputFile << fromIDtoWebsites[i] << "," << clicks[i];
		else outputFile << fromIDtoWebsites[i] << "," << clicks[i] << endl;
	}

	outputFile.close();
	clicksFile.close();


	

}



Graph inputManager::getWebsiteGraph() {
	return websitesGraph;
}

int* inputManager::getImpressions() {
	return impressions;
}

vector<vector<string>> inputManager::getKeywordsList() {
	return keywordsList;
}

int inputManager::getSize() {
	return numSites;
}

unordered_map<string, int> inputManager::getFromWebsitesToID() {
	return fromWebsitesToID;
}

vector<string> inputManager::getFromIDtoWebsites() {
	return fromIDtoWebsites;
}

int* inputManager::getClicks() {
	return clicks;
}
