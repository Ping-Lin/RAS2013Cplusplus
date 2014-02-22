#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <climits>
#include "Constants.h"
#include "Block.h"
#include "Track.h"
using namespace std;
using namespace Constants;

enum EventTime{tr, th, tc, tp1, tp2, to};   //tr=event time at receivingArea...h=hump, c=classification, p1=pullback1, p2=pullback2, o=outboundArea
vector<double> eventTime[6];   //time for event time(like the )
double latestTime = 0.0f;   //save the events latest time
Track *receivingTracks, *classificationTracks, *outboundTracks;   //feature same as the name
void readInboundCar(vector< vector<Block> > &);   //read the csv file to save the railcar information

//every event function
double receivingTrain(vector <vector<Block> > &);   //receive the train, argument is the blocklist
double humpTrain();   //hump the train
double classifyTrain();   //classify the train
double pullBackTrain(int);   //pull back the train, argument is pullback No. 
double goOutTrain();   //go out train

//receivingArea function
int findLongestTrainAtBlockList(vector< vector<Block> >, double);   //find the longest train at blocklist that is inside the limits, return the id of that train

int main(){
    ios::sync_with_stdio(false);   //stop the integration of stdio and printf, scanf
    vector< vector<Block> > blockList;
    //read the file for blocklist
    readInboundCar(blockList);

    //allocate memory for tracks
    receivingTracks = new Track[RECEIVING_TRACKS_NUMBER];
    classificationTracks = new Track[CLASSIFICATION_TRACKS_NUMBER];
    outboundTracks = new Track[DEPARTURE_TRACKS_NUMBER];
    
    //time array to save every event last time
    for(int i=0 ; i<6 ; i++){
	eventTime[i].push_back(0.0f);   //initial
    }
    latestTime = blockList[0][0].timeAtReceivingArea;   //set the first time
    
    double tmpTime;
    int eventIdSequence[6];
    //do the events
    while(1){
	//select the smallest time, using insertion sort to save all the id sort, because if the smallest time event can't do, 
	//than do the second small event, etc., save the id sequence into eventIdSequence
	eventIdSequence[0] = 0;
	for(int i=1 ; i<6 ; i++){
	    eventIdSequence[i] = i;
	    tmpTime = eventTime[eventIdSequence[i]].back();
	    int j;
	    for(j=i-1 ; j>=0 ; --j){
		if(eventTime[eventIdSequence[j]].back()>tmpTime)
		    eventIdSequence[j+1] = eventIdSequence[j];
		else
		    break;
	    } 
	    eventIdSequence[j+1] = i;
	}
	
	//select which event to do by the eventIdSequence
	bool ifBreak = false;
	for(int i=0 ; i<6 ; i++){
	    switch(eventIdSequence[i]){
		case tr:   //receivingArea
		    tmpTime = receivingTrain(blockList);
		    break;
		case th:   //hump
		    tmpTime = humpTrain();
		    break;
		case tc:   //classification
		    tmpTime = classifyTrain();
		    break;
		case tp1:   //pullback 1
		    tmpTime = pullBackTrain(1);
		    break;
		case tp2:   //pullback 2
		    tmpTime = pullBackTrain(2);
		    break;
		case to:   //outboundArea
		    tmpTime = goOutTrain();
		    break;
	    }
	    //cout << tmpTime << endl;
	    if(eventTime[eventIdSequence[i]].back()!= tmpTime){
		eventTime[eventIdSequence[i]].push_back(tmpTime);
		cout << "addTime: " << eventTime[eventIdSequence[i]].back() << endl;
		ifBreak = true;
	    }
	    if(ifBreak == true){
		break;
	    }
	    
	}
	if(tmpTime > latestTime)
	    latestTime = tmpTime;

	//if ifBreak==false that means there is no event-->break
	if(ifBreak==false)
	    break;
    }
    /*for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++)
	cout << receivingTracks[i].train[0].trainId << endl;*/
}

/*
** read the csv file to save the railcar information
*/
void readInboundCar(vector< vector<Block> > &bl){
    fstream inFile;
    inFile.open("./data_readme/Input_data_set_1.csv");
    if(inFile.is_open()){
	string tmp;
	getline(inFile, tmp);
	Block b;
	vector<Block> tmpBl;
	int count=1;
	while(inFile >> b.arrivalDay >> b.trainId >> b.timeAtReceivingArea >> b.blockName >> b.carNo){
	    if((b.carNo==1) && (count!=1)){
		bl.push_back(tmpBl);
		tmpBl.clear();
		vector<Block>(tmpBl).swap(tmpBl);
	    }
	    tmpBl.push_back(b);
	    count++;
	}
	tmpBl.push_back(b);   //final time
	bl.push_back(tmpBl);
	
	inFile.close();
    }
    else
	cout << "File open error.";
}

/*
* Receive the train.
* need the function: findLongestTrainAtBlockList(vector< vector<Block> >, double),
* if success than return new time, else return old time at receivingArea time(rt)
*/
double receivingTrain(vector< vector<Block> > &bl){
    int id = findLongestTrainAtBlockList(bl, latestTime);   //to find the id that has the longest size train by time limit
    cout << "[blockList]id: " << id << endl;
    if(id==-1)   //no empty track
	return eventTime[tr].back();
    else if(id==-2){   //there are some empty tracks, but time limit so can't go in
	int maxLength = 0;
	id=0;
	for(int i=0 ; ; i++){
	    if((bl[i][0].timeAtReceivingArea <= latestTime)){
		if(bl[i].size() > maxLength){
		    maxLength = bl[i].size();
		    id = i;
		}
	    }
	    else
		break;
	}
    }
    //do the event to save the train
    for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++){
	if(receivingTracks[i].ifEmpty == true){
	    if(bl[id][0].timeAtReceivingArea < receivingTracks[i].humpTime){
		//update time
		for(vector<Block>::iterator it=bl[id].begin() ; it != bl[id].end() ; ++it){
		    (*it).timeAtReceivingArea = receivingTracks[i].humpTime;
		}
	    }
	    receivingTracks[i].ifEmpty = false;
	    receivingTracks[i].train = bl[id];
	    bl.erase(bl.begin()+id);

	    cout << "[ReceivingArea Time]: " << receivingTracks[i].train[0].timeAtReceivingArea << ", Train id: " << receivingTracks[i].train[0].trainId << endl;
	    return receivingTracks[i].train[0].timeAtReceivingArea;
	}
    } 
    return eventTime[tr].back();
}

double humpTrain(){
    return 0.0f;
}

double classifyTrain(){
    return 0.0f;
}

double pullBackTrain(int no){
    return 0.0f;
}

double goOutTrain(){
    return 0.0f;
}

/*
* find the limit times that the train can give, and has the longest size
* return the id, if id==-1 than that means there is no train can go in now,
* if id==-2 than that means there are some empty tracks but can't go in because of time limit
*/
int findLongestTrainAtBlockList(vector< vector<Block> >bl, double t){
    int maxLength = 0;
    int id = -1;
    int minHumpTime = INT_MAX;
    for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++){   //empty tracks min hump time
	if((receivingTracks[i].ifEmpty == true) && (receivingTracks[i].humpTime < minHumpTime)){
	    minHumpTime = receivingTracks[i].humpTime;
	}
    }
    if(minHumpTime == INT_MAX)   //no empty train
	return id;

    for(int i=0 ; ; i++){
	if((bl[i][0].timeAtReceivingArea <= t) && (bl[i][0].timeAtReceivingArea >= minHumpTime)){
	    if(bl[i].size() > maxLength){
		maxLength = bl[i].size();
		id = i;
	    }
	}
	else{
	    if(id==-1){   //there are some empty tracks but can't go in because of time limit
		return -2;
	    }
	    break;
	}
    }
    return id;
}
