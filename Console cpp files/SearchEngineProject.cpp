#include <iostream>
#include <fstream>
#include <unordered_map>	
#include <string> 
#include <vector>
//#include <utility> // std::pair
//#include <stdexcept> // std::runtime_error
#include <sstream> 
#include "Graph.h"
#include "inputManager.h"

using namespace std;


//Global variables I will need throught multiple functions, I am pretty sure this is a bad idea and bad practise

unordered_map<string, int> fromWebsitesToID;
vector<string>  fromIDtoWebsites;
vector<vector<string>> keywordsList;
Graph websitesGraph;
int* impressions;
int numSites;
int* clicks;
double* pageRank;
double* normPageRank;

//int* getFinalPageRank(double*); //this function will not  work because pagescores and pagerank values are not unique as far as I knowint
int* getOrderedpageScore();
void preformSearch();
void viewWebsite(vector<int> searchResults);
void viewResults(vector<int> searchResults);
void swap(int* xp, int* yp);
void heapify(int* arr, int n, int x, double*);
void build_heap(int* arr, int n, double*);
void heap_sort(int* arr, int n, double*);
void updateImpressionFile();
void updateClicksFile();

int main() {

	inputManager inputManagerObj;

	//Create an object of the class inputManager
	fromWebsitesToID = inputManagerObj.getFromWebsitesToID();
	fromIDtoWebsites = inputManagerObj.getFromIDtoWebsites();
	keywordsList = inputManagerObj.getKeywordsList();
	websitesGraph = inputManagerObj.getWebsiteGraph();
	impressions = inputManagerObj.getImpressions();
	numSites = inputManagerObj.getSize();
	clicks = inputManagerObj.getClicks();
	pageRank = websitesGraph.getRank();


	//--------Getting normPageRank
	double tempMin = pageRank[0];
	for (int i = 0; i < numSites; i++) {
		if (pageRank[i] < tempMin) {
			tempMin = pageRank[i];
		}
	}

	normPageRank = new double[numSites];
	for (int i = 0; i < numSites; i++) {
		normPageRank[i] = (pageRank[i] * 1.0) / (tempMin);
	}
	

	//--------Printing Graph
	/*websitesGraph.printGraph();*/

	//--------Printing keywords
	//This will contain the keywords for each website //the index/row will corrsopond to the id of the website while the strings of each row will corrspond to its keywords.
	/*cout << endl;
	for (int i = 0; i < keywordsList.size(); i++) {
		for (int j = 0; j < keywordsList[i].size(); j++) {
			cout << keywordsList[i][j] << " ";
		}
		cout << endl;
	}*/


	//--------Printing impressions
	//The number of impressions is the number of times a website has appeared in search results.
	//This files will be edited and updated throughout the use of the program.
	/*cout << endl;
	for (int i = 0; i < fromWebsitesToID.size(); i++) {
		cout << impressions[i] << " ";
	}
	cout << endl;*/


	//---------------Time to get normalized page Rank and final page rank.
	//for (int i = 0; i < numSites; i++) {
	//	cout << pageRank[i] << " ";
	//}

	/*cout << endl;
	for (int i = 0; i < numSites; i++) {
		cout << pageScore[i] << " ";
	}

	cout << endl;
	for (int i = 0; i < numSites; i++) {
		cout << ordPageScore[i] << " ";
	}*/


	int choice;

	cout  << "Welcome! " << endl << "What would you like to do?" << endl;
	cout << "1. New Search" << endl << "2. Exit" << endl;
	cout << endl << "Type in your choice: ";
	cin >> choice;

	//Should make sure that choice is equal to either 1 or 2, or if I am planning to make a UI then IT shouldn't matter.
	if (choice == 2) {
		//cout << endl << "terminating...";

		return 0;
	}else if (choice == 1) {
		preformSearch();
	}else {
		cout << "\nInvalid Input!";
		//fix this later.
	}


	
	return 0;
}




int* getOrderedpageScore() {

	
	double* CTR = new double[numSites];
	double* pageScore = new double[numSites];

	//CTR is updated!
	for (int i = 0; i < numSites;i++) {
		if (impressions[i] != 0) {
			CTR[i] = (clicks[i] * 1.0) / (impressions[i]);
		}else {
			CTR[i] = 0;
		}
	
	}

	for (int i = 0; i < numSites; i++) {
		double tempOne = ((0.1 * impressions[i]) / (1 + (0.1 * impressions[i])));
		pageScore[i] = 0.4 * normPageRank[i] + ((1 - tempOne) * normPageRank[i] + tempOne * CTR[i]) * 0.6;
	}

	int* ordPageRank = new int[numSites];
	for (int i = 0; i < numSites; i++) {
		ordPageRank[i] = i;
	}
	
	heap_sort(ordPageRank, numSites, pageScore);
	
	delete[]CTR;
	delete[]pageScore;

	return ordPageRank;
}

void preformSearch() {

	//----------------------------------Setting up the SearchQuery 	 "This part needs working"
	//Do things with this search query
	//look for "" and ANDS and ORS and so on.
	string searchQuery;
	vector<string> orKeywords;
	vector<string> andKeywords;
	bool afterQ = 0;
	bool firstword = 0;
	string tempOne = "", tempTwo = "";
	cout << "Input you search query: ";
	getline(cin, searchQuery);
	getline(cin, searchQuery);
	stringstream ss(searchQuery);
	for (int i = 0; i < searchQuery.size(); i++) {
		if (searchQuery.at(i) == '"') {
			/*if (afterQ == 1) {
				tempTwo = tempOne;
				tempOne = "";
			}*/
			if (i == searchQuery.size() - 1) {
				//what should I do if the last words are AND or OR ?!
				//For simplicity I will just ignore them and do nothing
				if ((!firstword) && (tempTwo != "")) {
					firstword = 1;
					if (tempOne == "AND") {
						andKeywords.push_back(tempTwo);
					} else {
						orKeywords.push_back(tempTwo);
					}

				}

				if (tempOne == "AND") {
					//nothing
				} else if (tempOne == "OR") {
					//nothing
				} else if (tempTwo == "AND") {
					andKeywords.push_back(tempOne);
				} else {
					orKeywords.push_back(tempOne);
				}
				tempTwo = tempOne;
				tempOne = "";
			}
			afterQ = !afterQ;
		}else if (afterQ) {
			tempOne += searchQuery.at(i);
		}else if (searchQuery.at(i) != ' ') {
			tempOne += searchQuery.at(i);
			if (i == searchQuery.size() - 1) {
				//what should I do if the last words are AND or OR ?!
				//For simplicity I will just ignore them and do nothing
				if ((!firstword) && (tempTwo != "")) {
					firstword = 1;
					if (tempOne == "AND") {
						andKeywords.push_back(tempTwo);
					}
					else {
						orKeywords.push_back(tempTwo);
					}

				}

				if (tempOne == "AND") {
					//nothing
				}
				else if (tempOne == "OR") {
					//nothing
				}
				else if (tempTwo == "AND") {
					andKeywords.push_back(tempOne);
				}
				else {
					orKeywords.push_back(tempOne);
				}
				tempTwo = tempOne;
				tempOne = "";
			}

		}
		else if (searchQuery.at(i) == ' ') {

			if ((!firstword) && (tempTwo != "")) {
				firstword = 1;
				if (tempOne == "AND") {
					andKeywords.push_back(tempTwo);
				}
				else {
					orKeywords.push_back(tempTwo);
				}

			}

			if (tempTwo == "AND") {
				andKeywords.push_back(tempOne);
			}
			else if (tempTwo == "OR") {
				orKeywords.push_back(tempOne);
			}
			else if ((tempTwo != "") && (tempOne != "AND") && (tempOne != "OR")) {
				orKeywords.push_back(tempOne);
			}


			tempTwo = tempOne;
			tempOne = "";
		}
	}

	//cout << endl << "The or keywords: ";
	//for (int i = 0; i < orKeywords.size(); i++) {
	//	cout << orKeywords[i] << ", ";
	//}
	//cout << endl << "The and keywords: ";
	//for (int i = 0; i < andKeywords.size(); i++) {
	//	cout << andKeywords[i] << ", ";
	//}
	//cout << endl;

	//For simplicity and stuff I will assume a lot of things, just because I want to die and I don't care
	//1. Quotes can only appear at the beginning and at the end of the search query and maybe after/before AND and OR, neither one could be missing.
	//2. AND take precedance over ORs, this will not work. Let's decide we will go from left to right, first come first served. the word is decided by what is left to it first!!
	// what we can do is to get two strings, two strings? is that possible? maybe using string stream.
	// what should I do when there is only a single quote, well by program doesn't react LOL



	//-----Search your list of keywords and return the ids of the websites we will show, we will save them in a vector called results
	//---- OR keywords:
	vector<int> results;
	for (int j = 0; j < keywordsList.size(); j++) {
		for (int i = 0; i < orKeywords.size(); i++) {

			if (find(keywordsList[j].begin(), keywordsList[j].end(), orKeywords[i]) != keywordsList[j].end()) {
				results.push_back(j); //maybe use sets to prevent duplicates?
				break;
			}
		}
	}

	//---- AND keywords:
	bool foundALL = 1;
	for (int j = 0; j < keywordsList.size(); j++) {
		for (int i = 0; i < andKeywords.size(); i++) {
			if (find(keywordsList[j].begin(), keywordsList[j].end(), andKeywords[i]) == keywordsList[j].end()) {
				//if one of them is not found then 
				foundALL = 0;
			}
		}

		if (foundALL && (andKeywords.size()!=0)) {
			results.push_back(j); //maybe use sets to prevent duplicates?
		}

		foundALL = 1;
	}


	

	//---------I need to update/recalculate and refresh pageScore array.
	//This is necessary because CTR and impressions can change.
	int* ordPageScore = getOrderedpageScore();


	//----------Sort the websites we will show depending on their page score.
	//basically we go and check if the maximum pagescored website is in the results show it, otherwise see the 2nd max pagescore and so on.
	vector<int> searchResults; //I am pretty sure I will need this later.
	cout << "\nSearch results: " << endl;
	

	if (results.size() == 0) {
		cout << "EMPTY !" << endl;
	}else {
		int tmpItr = 0;
		for (int i = numSites - 1; i >= 0; i--) {
			if (find(results.begin(), results.end(), ordPageScore[i]) != results.end()) {
				searchResults.push_back(ordPageScore[i]);
				cout << tmpItr + 1 << ". " << fromIDtoWebsites[ordPageScore[i]] << endl;
				impressions[ordPageScore[i]]++; //it appeared in the search result. but I will have to write/update them in a file later.
				tmpItr++;
			}
		}
	}

	//----------------Need to edit/update the file impressions file here.
	updateImpressionFile();


	int choice;
	cout << endl << "Would you like to:" << endl;
	cout << "1. Choose a webpage to open" << endl << "2. New search" << endl << "3. Exit" << endl;
	cout << endl << "Type in your choice: ";
	cin >> choice;

	if (choice == 3) {
		//cout << endl << "terminating...";
		return ;
	}
	else if (choice == 2) {
		preformSearch();
	}else if (choice == 1) {
		viewWebsite(searchResults);
	}else {
		cout << "\nInvalid input!";
		//fix this later. maybe I could return back to search results or do something.
	}

}


void viewWebsite(vector<int> searchResults) {

	if (searchResults.size() ==0) {
		cout << "There are no websites to view! returning back to search results!\n" << endl;
		viewResults(searchResults);
	}

	int x;
	cout << "input the number of the website you want to visit according to the search results list: ";
	cin >> x;


	if ( (x>searchResults.size()) || (x<=0) ) {
		cout << "Incorrect input, the number you put is not on the list, please try again!\n";
		viewWebsite(searchResults);
	}else {
		cout << "\nYou are now viewing " << fromIDtoWebsites[searchResults[x-1]] << endl;


		//-------Need to update the clicks array and file here.
		clicks[searchResults[x - 1]]++;
		updateClicksFile();

		int choice;

		cout << endl << "Would you like to:" << endl;
		cout << "1. Back to search results" << endl << "2. New search" << endl << "3. Exit" << endl;
		cout << endl << "Type in your choice: ";
		cin >> choice;

		if (choice == 3) {
			//cout << endl << "terminating...";
			return;
		}else if (choice == 2) {
			preformSearch();
		}else if (choice == 1) {
			viewResults(searchResults);
		}else {
			cout << "\nInvalid input!";
			//fix this later. maybe I could return back to search results or do something.
		}

	}
}

void viewResults(vector<int> searchResults) {

	cout << "\nSearch results: " << endl;

	if (searchResults.size() == 0) {
		cout << "EMPTY !" << endl;
	}else {
		for (int i = 0; i < searchResults.size(); i++) {
			cout << i + 1 << ". " << fromIDtoWebsites[searchResults[i]] << endl;
		}
	}

	int choice;

	cout << endl << "Would you like to:" << endl;
	cout << "1. Choose a webpage to open" << endl << "2. New search" << endl << "3. Exit" << endl;
	cout << endl << "Type in your choice: ";
	cin >> choice;

	if (choice == 3) {
		//cout << endl << "terminating...";
		return;
	}
	else if (choice == 2) {
		preformSearch();
	}
	else if (choice == 1) {
		viewWebsite(searchResults);
	}else {
		cout << "\nInvalid input!";
		//fix this later. maybe I could return back to search results or do something.
	}

}


void updateImpressionFile() {

	//loops through the file and changes the impressions of each website by impression[i]
	ofstream outputFile("numOfImpressions.csv");
	for (int i = 0; i < numSites; i++) {
		if (i == (numSites - 1)) outputFile << fromIDtoWebsites[i] << "," << impressions[i];
		else outputFile << fromIDtoWebsites[i] << "," << impressions[i] << endl;

	}
	outputFile.close();

}

void updateClicksFile() {


	//loops through the file and changes the clicks of each website by clicks[i]
	ofstream outputFile("numOfClicks.csv");

	for (int i = 0; i < numSites; i++) {
		if (i == (numSites - 1)) outputFile << fromIDtoWebsites[i] << "," << clicks[i];
		else outputFile << fromIDtoWebsites[i] << "," << clicks[i] << endl;
	}

	outputFile.close();
}


void swap(int* xp, int* yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void heapify(int* arr, int n, int x, double*ref) {

	//get the index of the children nodes and initialize largest.
	int largest = x;
	int l = (2 * x) + 1;
	int r = (2 * x) + 2;

	if (l < n && ref[l] > ref[largest]) {
		largest = l;
	}

	if (r < n && ref[r] > ref[largest]) {
		largest = r;
	}

	if (largest != x) {
		swap(arr[x], arr[largest]);	//it seems that swap is already defined in c++ STL so we don't need to define it.
		swap(ref[x], ref[largest]);	//it seems that swap is already defined in c++ STL so we don't need to define it.
		heapify(arr, n, largest, ref);
	}

}

void build_heap(int* arr, int n, double* ref) {

	//we call heapify on each the elements in the array who are not leafs.
	for (int i = (n / 2 - 1); i >= 0; i--) {
		heapify(arr, n, i, ref);
	}
}

void heap_sort(int* arr, int n, double* ref) {

	build_heap(arr, n, ref); //transform the array into a heap

	//We swap and then call heapify and we swap and call heapify and so on....
	for (int i = n - 1; i >= 0; i--) {
		swap(arr[0], arr[i]);//put the first (largest) element in its correct position.
		swap(ref[0], ref[i]);//put the first (largest) element in its correct position.
		heapify(arr, i, 0, ref);
	}
}


//
//int* getFinalPageRank(double* newItrRank) {
//
//
//	//after getting the itrRank, I should get the final page rank.
//	//To get the final page Rank, I will have to sort them but also remember their positions because their position corrospond to the websites ID.
//	//maybe put them in a map, and they will get sorted automatically. The idea worked.
//
//	int size = fromIDtoWebsites.size();
//
//	int* finalRank = new int[size];
//	map<double, int> pageRanksMap;
//	for (int i = 0; i < size; i++) {
//		pageRanksMap[newItrRank[i]] = i;
//	}
//
//	int itr = 0;
//	for (auto it : pageRanksMap) {
//		finalRank[it.second] = itr;
//		itr++;
//	}
//
//	return finalRank;
//
//}
//
