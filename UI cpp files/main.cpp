#include <iostream>
#include <fstream>
#include <unordered_map>	
#include <string> 
#include <vector>
#include <sstream> 
#include <SFML/Graphics.hpp>
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
double* CTR;
double* pageScore;


//Textures and sprites
sf::Texture logoTexture;//1270x840
sf::Texture searchBoxTexture;//1270x840
sf::Texture exitTexture;//1270x840
sf::Texture arrowTexture;//1270x840
sf::Texture cardTexture;//1270x840
sf::Texture nextTexture;//1270x840
sf::Sprite logoSprite;
sf::Sprite searchBoxSprite;
sf::Sprite exitSprite;
sf::Sprite arrowSprite;
sf::Sprite backSprite;
sf::Sprite cardSprite1;
sf::Sprite cardSprite2;
sf::Sprite cardSprite3;
sf::Sprite nextSprite;
sf::Sprite prevSprite;
sf::Font searchFont;
sf::Font siteTitleFont;
sf::Font infoFont;
sf::Text searchText;
sf::Text limitText;
sf::Text madeText;
sf::Text siteTexts[3], infoTexts[3];
sf::Text viewText, mainSiteText;


//---------Search engine functions
vector<int> preformSearch(string);  //this function is responsible for getting the search results from search query and returning them ordered according page score.
int* getOrderedpageScore(); //this returns the websites ordered according to the pagescore values.
void updateImpressionFile();   //this updates the impressions file with the new impressions values.
void updateClicksFile();    //this updates the clicks file with the new clicks values.

//-----------Sort functions
//these are modified so that the array is ordered according to the values of another array called ref, this is to sort the websites according to page ranks.
void swap(int* xp, int* yp);
void heapify(int* arr, int n, int x, double*);  
void build_heap(int* arr, int n, double*);
void heap_sort(int* arr, int n, double*);

//-----------UI functions
void createSprites();   //creates the sprites and textures and so on.
void viewSiteFunc();    //resets the positions of the cards
void viewNextResultsFunc(int&, vector<int>);    //this implements the behaviour when pressing the next arrow to show the next page of results.
void viewPrevResultsFunc(int&, vector<int>);    //this implements the behaviour when pressing the prev arrow to show the prev page of results.
void checkInteractive(sf::Vector2i);    //this implements some interactive UI, like how the buttons highlight when you hover them with the mouse.

int main() {

    //-----------Helper variables
    double w = 1280, h = 720;
    double mouseX, mouseY;
    sf::Vector2i localPosition;
    sf::Vector2f tempPos;
    sf::FloatRect searchTextRect;
    sf::FloatRect tempRectThing;
    string searchString = "";
    bool viewFlag = 0;
    bool startFlag = 0;
    bool flagViewTitle = 0;
    bool easterFlag = 0;
    bool flippedAnim = 0;
    int tempAlpha = 0;
    vector<int> searchResults;
    int viewItr = 0;
    string tempString;



    //----------Creaing renderWindow
    sf::RenderWindow myWindow(sf::VideoMode(w, h), "My Search Engine", sf::Style::Titlebar);
    myWindow.setVerticalSyncEnabled(true);


    //------------Setting up textures
    createSprites();


    //--------Reading input and intilizing program
    inputManager inputManagerObj;
    fromWebsitesToID = inputManagerObj.getFromWebsitesToID();
    fromIDtoWebsites = inputManagerObj.getFromIDtoWebsites();
    keywordsList = inputManagerObj.getKeywordsList();
    websitesGraph = inputManagerObj.getWebsiteGraph();
    impressions = inputManagerObj.getImpressions();
    numSites = inputManagerObj.getSize();
    clicks = inputManagerObj.getClicks();
    pageRank = websitesGraph.getRank();
    CTR = new double[numSites];
    pageScore = new double[numSites];

    //--------Getting normPageRank
    double tempMin=pageRank[0];
    for (int i = 0; i < numSites; i++) {
        if (pageRank[i] < tempMin) {
            tempMin = pageRank[i];
        }
    }
    normPageRank = new double[numSites];
    for (int i = 0; i < numSites; i++) {
        normPageRank[i] = (pageRank[i]*1.0) / (tempMin);
    }



    //------------main/game loop
    while (myWindow.isOpen()) {

        sf::Event event;

        while (myWindow.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                myWindow.close();
            }

            //----- if a mouse button has been pressed
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    mouseX = event.mouseButton.x;
                    mouseY = event.mouseButton.y;
                    sf::Vector2f myMousePos(mouseX, mouseY);

                    if (exitSprite.getGlobalBounds().contains(myMousePos)) {
                        myWindow.close();
                    }

                    if (arrowSprite.getGlobalBounds().contains(myMousePos)) {
                        viewFlag = 0;
                        startFlag = 1;
                        viewItr = 0;
                        //reset positions to do animation again.
                        viewSiteFunc();
                        searchResults.clear();
                        searchResults = preformSearch(searchString);
                        viewNextResultsFunc(viewItr, searchResults);
                    }

                    if (backSprite.getGlobalBounds().contains(myMousePos)) {
                        viewFlag = 0;
                        viewItr -= 3;
                        viewNextResultsFunc(viewItr, searchResults);
                    }

                    if (nextSprite.getGlobalBounds().contains(myMousePos)) {
                        viewNextResultsFunc(viewItr, searchResults);
                    }

                    if (prevSprite.getGlobalBounds().contains(myMousePos)) {
                        viewPrevResultsFunc(viewItr, searchResults);
                    }


                    for (int i = 0; i < 3;i++) {
                        if (siteTexts[i].getGlobalBounds().contains(myMousePos)) {
                            
                            viewSiteFunc();
                            viewFlag = 1;
                            tempString = (siteTexts[i].getString());
                            for (int i = 0; i < tempString.size(); i++) {
                                if (tempString.at(i) == '.') {
                                    tempString.erase(0,i+2);
                                    break;
                                }
                            }
                            clicks[fromWebsitesToID[tempString]]++;
                            updateClicksFile();
                            mainSiteText.setString(tempString);
                            tempRectThing = mainSiteText.getGlobalBounds();
                            mainSiteText.setPosition(sf::Vector2f((w / 2.0) - tempRectThing.width / 2.0, (h / 3.0) + 60));;
                        }
                    }

                    //---easter egg
                    if (logoSprite.getGlobalBounds().contains(myMousePos)) {
                        tempAlpha = 0;
                        flippedAnim = 0;
                        easterFlag = 1;
                        madeText.setFillColor(sf::Color(70, 70, 70, 0));
                       
                    }
                    

                }
            }



            //------handling keyboard/text input
            if (event.type == sf::Event::TextEntered) {

                searchTextRect = searchText.getGlobalBounds();
                limitText.setFillColor(sf::Color(255, 255, 255, 0));

                if ((event.text.unicode == 8)) {
                    if ((searchString.size() > 0))  searchString.erase(searchString.size() - 1);
                } else if ((event.text.unicode == 13)) {
                    //-----Pressing enter is the same as clicking on the arrow and preforming search.
                    viewFlag = 0;
                    startFlag = 1;
                    viewItr = 0;
                    viewSiteFunc();
                    searchResults.clear();
                    searchResults = preformSearch(searchString);
                    viewNextResultsFunc(viewItr, searchResults);

                } else if (searchTextRect.width < 550) {
                    //hide text max limit has been reached
                    if (event.text.unicode < 128) {
                        searchString += static_cast<char>(event.text.unicode);
                    }

                } else {
                    //max limit has been reached
                    limitText.setFillColor(sf::Color(123, 123, 123, 255));
                }

                searchText.setString(searchString);
                myWindow.draw(searchText);
            }


        }




        //-------Clearing
        myWindow.clear(sf::Color(34, 34, 34, 255));


        //-------Basic Animation:
        if (startFlag) {

            tempPos = logoSprite.getPosition();

            if (tempPos.x > 130) {
                logoSprite.scale(0.985f, 0.985f);
                logoSprite.move(-10.5f, -3.3f);
            }

            sf::Vector2f tempPosTwo = searchBoxSprite.getPosition();

            if (tempPosTwo.y > tempPos.y + 10) {
                searchBoxSprite.move(-0.f, -5.5f);
                searchText.move(-0.f, -5.5f);
                arrowSprite.move(-0.f, -5.5f);
                limitText.move(-0.f, -7.5f);
            }

            sf::Vector2f tempPosThree = cardSprite1.getPosition();

            if ((tempPosThree.y > tempPosTwo.y + 160) && !viewFlag) {

                cardSprite1.move(-0.f, -15.0f);
                cardSprite2.move(-0.f, -15.0f);
                cardSprite3.move(-0.f, -15.0f);
                for (int i = 0; i < 3; i++) {
                    siteTexts[i].move(-0.f, -15.0f);
                    infoTexts[i].move(-0.f, -15.0f);
                }
                nextSprite.move(-0.f, -15.0f);
                prevSprite.move(-0.f, -15.0f);
            }

        }

        if (easterFlag) {
            
            if (!flippedAnim) {
                tempAlpha = (tempAlpha + 3)%256;
                madeText.setFillColor(sf::Color(70, 70, 70, tempAlpha));
            } else {
                tempAlpha = (tempAlpha - 3) % 256;
                madeText.setFillColor(sf::Color(70, 70, 70, tempAlpha));
            }

            if (tempAlpha >= 251) {
                flippedAnim = 1;
            } else if (tempAlpha < 0) {
                flippedAnim = 0;
                tempAlpha = 0;
                easterFlag = 0;
                madeText.setFillColor(sf::Color(70, 70, 70, tempAlpha));
            } 
             
        }





        //--------adding interactivity
        localPosition = sf::Mouse::getPosition(myWindow);
        checkInteractive(localPosition);

        


        //-----Drawing
        myWindow.draw(logoSprite);
        myWindow.draw(searchBoxSprite);
        myWindow.draw(searchText);
        myWindow.draw(limitText);
        myWindow.draw(arrowSprite);
        myWindow.draw(exitSprite);
        myWindow.draw(madeText);
        myWindow.draw(cardSprite1);
        myWindow.draw(cardSprite2);
        myWindow.draw(cardSprite3);
        for (int i = 0; i < 3; i++) {
            myWindow.draw(siteTexts[i]);
            myWindow.draw(infoTexts[i]);
        }
        if (viewFlag) { //depending on the value of viewFlag some sprites won't be drawn.
            myWindow.draw(viewText);
            myWindow.draw(mainSiteText);
            myWindow.draw(backSprite);
        }
        myWindow.draw(nextSprite);
        myWindow.draw(prevSprite);

        myWindow.display();

    }

    return 0;
}





vector<int> preformSearch(string searchQuery) {

    

    vector<string> orKeywords;
    vector<string> andKeywords;
    bool afterQ = 0;
    bool firstword = 0;
    string tempOne = "", tempTwo = "";

    for (int i = 0; i < searchQuery.size(); i++) {
        if (searchQuery.at(i) == '"') {

            if (i == searchQuery.size() - 1) {
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
        } else if (afterQ) {
            tempOne += searchQuery.at(i);
        } else if (searchQuery.at(i) != ' ') {
            tempOne += searchQuery.at(i);
            if (i == searchQuery.size() - 1) {

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

        } else if (searchQuery.at(i) == ' ') {

            if ((!firstword) && (tempTwo != "")) {
                firstword = 1;
                if (tempOne == "AND") {
                    andKeywords.push_back(tempTwo);
                } else {
                    orKeywords.push_back(tempTwo);
                }

            }

            if (tempTwo == "AND") {
                andKeywords.push_back(tempOne);
            } else if (tempTwo == "OR") {
                orKeywords.push_back(tempOne);
            } else if ((tempTwo != "") && (tempOne != "AND") && (tempOne != "OR")) {
                orKeywords.push_back(tempOne);
            }


            tempTwo = tempOne;
            tempOne = "";
        }
    }

  
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

        if (foundALL && (andKeywords.size() != 0)) {
            results.push_back(j); //maybe use sets to prevent duplicates?
        }

        foundALL = 1;
    }

    //---------I need to update/recalculate and refresh pageScore array.
    int* ordPageScore = getOrderedpageScore();


    //----------Sort the websites we will show depending on their page score.
    vector<int> searchResults; 
    int tmpItr = 0;
    for (int i = numSites - 1; i >= 0; i--) {
        if (find(results.begin(), results.end(), ordPageScore[i]) !=results.end()) {
            searchResults.push_back(ordPageScore[i]);
            impressions[ordPageScore[i]]++; 
            tmpItr++;
        }
    }
    

    //----------------Need to edit/update the file impressions file here.
    updateImpressionFile();

    return searchResults;
}

/*
1. Update clicks file and stuff
2. Update the functionality of the backtoresults button
3.
*/


int* getOrderedpageScore() {

    //CTR is updated!
    for (int i = 0; i < numSites; i++) {
        if (impressions[i] != 0) {
            CTR[i] = (clicks[i] * 1.0) / (impressions[i]);
        }
        else {
            CTR[i] = 0;
        }
    }

    //get pageScores from formula
    for (int i = 0; i < numSites; i++) {
        double tempOne = ((0.1 * impressions[i]) / (1 + (0.1 * impressions[i])));
        pageScore[i] = 0.4 * normPageRank[i] + ((1 - tempOne) * normPageRank[i] + tempOne * CTR[i]) * 0.6;
    }

    //createordPageRank and sort it.
    int* ordPageRank = new int[numSites];
    for (int i = 0; i < numSites; i++) {
        ordPageRank[i] = i;
    }

    heap_sort(ordPageRank, numSites, pageScore);

    return ordPageRank;
}

void createSprites() {


    //-----------Helper variables
    double w = 1280, h = 720;


    //--------Creating logo texture and sprite.
    if (!logoTexture.loadFromFile("SearchEngineLogoDropShadow.png")) {
        //error
        cout << "There was an error";
    }
    logoSprite.setTexture(logoTexture);
    logoSprite.setOrigin(sf::Vector2f(635.f, 420.f));
    logoSprite.setScale(sf::Vector2f(0.5f, 0.5f));
    logoSprite.setPosition(sf::Vector2f(w / 2.0, h / 3.0));


    //--------Creating search box texture and sprite. 
    if (!searchBoxTexture.loadFromFile("searchBox.png")) {
        //error
        cout << "There was an error";
    }
    searchBoxSprite.setTexture(searchBoxTexture);
    searchBoxSprite.setOrigin(sf::Vector2f(635.f, 420.f));
    searchBoxSprite.setScale(sf::Vector2f(0.5f, 0.5f));
    searchBoxSprite.setPosition(sf::Vector2f(w / 2.0, h / 2.0));



    //--------Creating exit texture and sprite
    if (!exitTexture.loadFromFile("exitSmall.png")) {
        //error
        cout << "There was an error";
    }
    exitSprite.setTexture(exitTexture);
    exitSprite.setOrigin(sf::Vector2f(63.5f, 42.f));
    //exitSprite.setScale(sf::Vector2f(0.1f, 0.1f));
    exitSprite.setPosition(sf::Vector2f(w - 60.f, 88.f));
    exitTexture.setSmooth(true);


    //--------Creating arrow and backArrow texture and sprite.
    if (!arrowTexture.loadFromFile("arrow.png")) {
        //error
        cout << "There was an error";
    }
    arrowSprite.setTexture(arrowTexture);
    arrowSprite.setOrigin(sf::Vector2f(63.5f, 42.f));
    //exitSprite.setScale(sf::Vector2f(0.1f, 0.1f));
    arrowSprite.setPosition(sf::Vector2f((w * 3.0 / 4.0) + 15, (h / 2.0) + 5));
    arrowTexture.setSmooth(true);
    backSprite.setTexture(arrowTexture);
    backSprite.setOrigin(sf::Vector2f(63.5f, 42.f));
    backSprite.setRotation(180.f);
    backSprite.setPosition(sf::Vector2f(120.f, 200.f));


    //--------Creating search text   
    if (!searchFont.loadFromFile("Oswald-Regular.ttf")) {
        //error
        cout << "There was an error loading font!\n";
    }
    searchText.setFont(searchFont);
    searchText.setCharacterSize(24);
    searchText.setFillColor(sf::Color(34, 34, 34, 255));
    searchText.setPosition(sf::Vector2f((w / 4.0) + 40, (h / 2.0) - 10));
    //searchText.setString(searchQuery);


     //--------Creating max limit text
    limitText.setFont(searchFont);
    limitText.setCharacterSize(18);
    limitText.setPosition(sf::Vector2f((w / 4.0) + 40, (h / 2.0) + 45));
    limitText.setFillColor(sf::Color(255, 255, 255, 0));
    limitText.setString("*max character limit has been reached!");


    //--------Creating creator/Made text  
    madeText.setFont(searchFont);
    madeText.setCharacterSize(22);
    madeText.setFillColor(sf::Color(70, 70, 70, 0));
    madeText.setPosition(sf::Vector2f((40.f), (h - 40.f)));;
    madeText.setString("Made by Abdelrahman Abdelmonem 2021.");


    //--------Creating card texture and sprite.
    if (!cardTexture.loadFromFile("card.png")) {
        //error
        cout << "There was an error";
    }
    cardSprite1.setTexture(cardTexture);
    cardSprite1.setOrigin(sf::Vector2f(635.f, 420.f));
    cardSprite1.setScale(sf::Vector2f(0.5f, 0.4f));
    cardSprite1.setPosition(sf::Vector2f(w / 2.0, h + 220));
    cardSprite2.setTexture(cardTexture);
    cardSprite2.setOrigin(sf::Vector2f(635.f, 420.f));
    cardSprite2.setScale(sf::Vector2f(0.5f, 0.4f));
    cardSprite2.setPosition(sf::Vector2f(w / 2.0, h + 400));
    cardSprite3.setTexture(cardTexture);
    cardSprite3.setOrigin(sf::Vector2f(635.f, 420.f));
    cardSprite3.setScale(sf::Vector2f(0.5f, 0.4f));
    cardSprite3.setPosition(sf::Vector2f(w / 2.0, h + 580));


    //--------Creating Results text 
    if (!siteTitleFont.loadFromFile("RobotoMono-Regular.ttf")) {
        cout << "There was an error loading font!\n";
    }
    if (!infoFont.loadFromFile("RobotoMono-LightItalic.ttf")) {
        cout << "There was an error loading font!\n";
    }
    for (int i = 0; i < 3;i++) {
        siteTexts[i].setFont(siteTitleFont);
        siteTexts[i].setCharacterSize(16);
        siteTexts[i].setFillColor(sf::Color(34, 34, 34, 255));
        siteTexts[i].setPosition(sf::Vector2f((w / 4.0) + 40, h + (145) + 180 * i));
        infoTexts[i].setFont(infoFont);
        infoTexts[i].setCharacterSize(15);
        infoTexts[i].setFillColor(sf::Color(123, 123, 123, 255));
        infoTexts[i].setPosition(sf::Vector2f((w / 4.0) + 40, h + 170 + 180 * i));
    }
   



    //--------Creating viewing text
    viewText.setFont(siteTitleFont); mainSiteText.setFont(siteTitleFont);
    viewText.setCharacterSize(52); mainSiteText.setCharacterSize(52);
    viewText.setFillColor(sf::Color(177, 28, 28, 255)); mainSiteText.setFillColor(sf::Color(177, 28, 28, 255));
    viewText.setString("You are viewing");   
    sf::FloatRect tempRectThing = viewText.getGlobalBounds();
    viewText.setPosition(sf::Vector2f((w / 2.0) - tempRectThing.width / 2.0, (h / 3.0)));;
   



    //--------Creating next texture and sprite.
    if (!nextTexture.loadFromFile("next.png")) {
        //error
        cout << "There was an error";
    }
    nextSprite.setTexture(nextTexture);
    nextSprite.setOrigin(sf::Vector2f(63.5 / 2.0f, 21.f));
    nextSprite.setPosition(sf::Vector2f((w / 2.0) + 30, (h + 680)));
    prevSprite.setTexture(nextTexture);
    prevSprite.setOrigin(sf::Vector2f(63.5 / 2.0f, 21.f));
    prevSprite.setRotation(180.f);
    prevSprite.setPosition(sf::Vector2f((w / 2.0) - 30, (h + 680)));
    //nextTexture.setSmooth(true);
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

void heapify(int* arr, int n, int x, double* ref) {

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

void viewSiteFunc() {
    
    double w = 1280, h = 720;
    cardSprite1.setPosition(sf::Vector2f(w / 2.0, h + 220));
    cardSprite2.setPosition(sf::Vector2f(w / 2.0, h + 400));
    cardSprite3.setPosition(sf::Vector2f(w / 2.0, h + 580));
    for (int i = 0; i < 3; i++) {
        siteTexts[i].setPosition(sf::Vector2f((w / 4.0) + 40, h + 145 + 180*i));
        infoTexts[i].setPosition(sf::Vector2f((w / 4.0) + 40, h + 170 + 180 * i));
    }
    nextSprite.setPosition(sf::Vector2f((w / 2.0) + 30, (h + 680)));
    prevSprite.setPosition(sf::Vector2f((w / 2.0) - 30, (h + 680)));
}

void viewNextResultsFunc(int& viewItr, vector<int> searchResults) {

    string tempInfo="";
    int currentSite;

    if (searchResults.size()==0) {
        //There are no search results.
        //maybe show an image, temporality just do nothing
    }

    if (viewItr > searchResults.size()) {
        return;
    }

   

    for (int i = 0; i < 3; i++) {

       

        if (viewItr >= searchResults.size()) {
            siteTexts[i].setString( to_string(viewItr+1) + ". N/A");
            infoTexts[i].setString("Empty");
          
        } else {

            currentSite = searchResults[viewItr];

            tempInfo = "Keywords: ";
            for (int i = 0; i < keywordsList[currentSite].size(); i++) {
                tempInfo += keywordsList[currentSite][i] + ", ";
            }

            tempInfo += "\nPageScore: ";
            tempInfo += to_string(pageScore[currentSite]);

            tempInfo += "\nPageRank: ";
            tempInfo += to_string(normPageRank[currentSite]);

            tempInfo += "\nCTR: ";
            tempInfo += to_string(CTR[currentSite]);  

            tempInfo += "\nClicks: ";
            tempInfo += to_string(clicks[currentSite]);

            tempInfo += "\nImpressions: ";
            tempInfo += to_string(impressions[currentSite]-1); //because we want to show the unpdated 

            infoTexts[i].setString(tempInfo);
            siteTexts[i].setString(to_string(viewItr + 1) + ". " + fromIDtoWebsites[currentSite]);
        }
        viewItr++;
    }
}

void viewPrevResultsFunc(int& viewItr, vector<int> searchResults) {


    string tempInfo = "";
    int currentSite;

    if (viewItr == 3) {
        return;
    }

    viewItr -= 6;

    for (int i = 0; i < 3; i++) {
        if (viewItr >= searchResults.size()) {
            siteTexts[i].setString(to_string(viewItr + 1) + ". N/A");
            infoTexts[i].setString("Empty");
        } else {
            currentSite = searchResults[viewItr];

            tempInfo = "Keywords: ";
            for (int i = 0; i < keywordsList[currentSite].size(); i++) {
                tempInfo += keywordsList[currentSite][i] + ", ";
            }

            tempInfo += "\nPageScore: ";
            tempInfo += to_string(pageScore[currentSite]);

            tempInfo += "\nPageRank: ";
            tempInfo += to_string(normPageRank[currentSite]);

            tempInfo += "\nCTR: ";
            tempInfo += to_string(CTR[currentSite]);

            tempInfo += "\nClicks: ";
            tempInfo += to_string(clicks[currentSite]);

            tempInfo += "\nImpressions: ";
            tempInfo += to_string(impressions[currentSite] - 1); //because we want to show the unpdated 

            infoTexts[i].setString(tempInfo);
            siteTexts[i].setString(to_string(viewItr + 1) + ". " + fromIDtoWebsites[currentSite]);
        }
        viewItr++;
    }

}

void checkInteractive(sf::Vector2i localPosition) {


    sf::Vector2f tempPos;

    tempPos.x = localPosition.x;;
    tempPos.y = localPosition.y;;

    if (exitSprite.getGlobalBounds().contains(tempPos)) {
        exitSprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        exitSprite.setColor(sf::Color(205, 205, 205, 255));
    }

    if (arrowSprite.getGlobalBounds().contains(tempPos)) {
        arrowSprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        arrowSprite.setColor(sf::Color(205, 205, 205, 255));
    }

    if (backSprite.getGlobalBounds().contains(tempPos)) {
        backSprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        backSprite.setColor(sf::Color(205, 205, 205, 255));
    }


    for (int i = 0; i < 3; i++) {
        if (siteTexts[i].getGlobalBounds().contains(tempPos)) {
            siteTexts[i].setStyle(sf::Text::Underlined);
        } else {
            siteTexts[i].setStyle(sf::Text::Regular);
        }
    }


    if (nextSprite.getGlobalBounds().contains(tempPos)) {
        nextSprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        nextSprite.setColor(sf::Color(205, 205, 205, 255));
    }

    if (prevSprite.getGlobalBounds().contains(tempPos)) {
        prevSprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        prevSprite.setColor(sf::Color(205, 205, 205, 255));
    }
}