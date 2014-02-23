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
#include "BlockToTrack.h"
#include "BlockValues.h"
using namespace std;
using namespace Constants;

enum EventTime{tr, th, tp1, tp2, to};   //tr=event time at receivingArea...h=hump, c=classification, p1=pullback1, p2=pullback2, o=outboundArea
vector<double> eventTime[6];   //time for event time(like the )
double latestTime = 0.0f;   //save the events latest time
Track *receivingTracks, *classificationTracks, *outboundTracks;   //feature same as the name

//for print out
vector< vector<Block> >humpBlockList;   //print inbound_train_info
void readInboundCar(vector< vector<Block> > &);   //read the csv file to save the railcar information

//every event function
double receivingTrain(vector <vector<Block> > &);   //receive the train, argument is the blocklist
double humpTrain();   //hump the train
double pullBackTrain(int);   //pull back the train, argument is pullback No. 
double goOutTrain();   //go out train

//receivingArea function
int findLongestTrainAtBlockList(vector< vector<Block> >, double);   //find the longest train at blocklist that is inside the limits, return the id of that train

//hump function
vector<int> findLongestAtReceivingArea();   //find the longest train at receiving area and return the train id sequence from long to short
bool putIntoClassificationArea(vector<Block>);

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
    for(int i=0 ; i<5 ; i++){
	eventTime[i].push_back(0.0f);   //initial
    }
    latestTime = blockList[0][0].timeAtReceivingArea;   //set the first time
    
    double tmpTime;
    int eventIdSequence[5];
    //do the events
    while(1){
	//select the smallest time, using insertion sort to save all the id sort, because if the smallest time event can't do, 
	//than do the second small event, etc., save the id sequence into eventIdSequence
	eventIdSequence[0] = 0;
	for(int i=1 ; i<5 ; i++){
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
	for(int i=0 ; i<5 ; i++){
	    switch(eventIdSequence[i]){
		case tr:   //receivingArea
		    tmpTime = receivingTrain(blockList);
		    break;
		case th:   //hump
		    tmpTime = humpTrain();
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
	for(int i=0 ; bl.size() ; i++){
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
    double minHumpT = INT_MAX;
    int minId;
    bool ifSuccess=false;
    for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++){
	if(receivingTracks[i].ifEmpty == true){
	    if(minHumpT > receivingTracks[i].humpTime){
		minHumpT = receivingTracks[i].humpTime;
		minId = i;
	    }
	    if(bl[id][0].timeAtReceivingArea >= receivingTracks[i].humpTime){
		receivingTracks[i].ifEmpty = false;
		receivingTracks[i].train = bl[id];
		bl.erase(bl.begin()+id);
		ifSuccess = true;
		minId = i;
		break;
	    }
	}
    }
    if(ifSuccess==false){
	if(bl[id][0].timeAtReceivingArea < receivingTracks[minId].humpTime){
	    //update time
	    for(vector<Block>::iterator it=bl[id].begin() ; it != bl[id].end() ; ++it){
		(*it).timeAtReceivingArea = receivingTracks[minId].humpTime;
	    }
	}
	receivingTracks[minId].ifEmpty = false;
	receivingTracks[minId].train = bl[id];
	bl.erase(bl.begin()+id);
    }

    cout << "[ReceivingArea Time]: " << receivingTracks[minId].train[0].timeAtReceivingArea << ", Train id: " << receivingTracks[minId].train[0].trainId << ", Track: R" << minId+1 << endl;
    return receivingTracks[minId].train[0].timeAtReceivingArea;
}
/*
* hump the train from receivingArea to classification area
* need the function: vector<int> findLongestAtReceivingArea(), bool putIntoClassificationArea(vector<Block>);
*/
double humpTrain(){
    vector<int> id = findLongestAtReceivingArea();
    double time;
    vector<Block> tmpList;
    if(id[0]==-1)
	return eventTime[th].back();
    else{
	for(int i=0 ; i<id.size() ; i++){   //from long to short
	    time = receivingTracks[id[i]].train[0].timeAtReceivingArea + TECHNICAL_INSPECTION_TIME;   //add technical time
	    if(eventTime[th].back()+HUMP_INTERVAL <= time)   //count the time
		time = time + HUMP_INTERVAL;
	    else
		time = eventTime[th].back() + HUMP_INTERVAL;
	    for(vector<Block>::iterator it=receivingTracks[id[i]].train.begin() ; it!=receivingTracks[id[i]].train.end() ; ++it){
		(*it).timeStartHump = time;
		(*it).timeEndHump = time + HUMP_RATE*receivingTracks[id[i]].train.size();
	    }
	    if(putIntoClassificationArea(receivingTracks[id[i]].train) == false){   //receive error
		continue;
	    }
	    else{   //receive success
		for(vector<Block>::iterator b=receivingTracks[id[i]].train.begin() ; b!=receivingTracks[id[i]].train.end() ; ++b){
		    tmpList.push_back((*b));
		}
		//eventTime[th].push_back(time);
		cout << "[Hump]Start: " << time << endl;
		receivingTracks[id[i]].humpTime = time;
		time+=HUMP_RATE*receivingTracks[id[i]].train.size();
		receivingTracks[id[i]].train.clear();
		vector<Block>(receivingTracks[id[i]].train).swap(receivingTracks[id[i]].train);
		receivingTracks[id[i]].ifEmpty = true;
		
		humpBlockList.push_back(tmpList);
		
		return time;
	    }
	}
	cout << "[Hump]classification tracks are full." << endl;
    }
    return eventTime[th].back();
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
    if(minHumpTime == INT_MAX)   //no empty track
	return id;

    for(int i=0 ; i<bl.size() ; i++){
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
    return id;   //no train coming
}

/**
* find the longest train at receiving at receiving area, because maybe hump can't move the longest train,
* so give the track id sequence of size, from long to short
* return -1 if no train can hump
*/
vector<int> findLongestAtReceivingArea(){
    int maxLength = 0;
    vector<int> id;   //track id sequence for size, long to short
    vector<int>::iterator it = id.begin();
    for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++){
	int count = id.size();
	for(int j=id.size()-1 ; j>=0 ; j--){
	    if((receivingTracks[i].ifEmpty==false)){
		if(receivingTracks[i].train.size() > receivingTracks[id[j]].train.size())
		    count = j;
	    }
	}
	if(receivingTracks[i].ifEmpty == false){
	    it = id.begin();
	    id.insert(it+count, i);
	}
    }
     
    if(id.empty()){
	id.push_back(-1);
    }
    return id;
}
/*
* classify the train into classification area, if success return true, else return false
* argument: the train
*/
bool putIntoClassificationArea(vector<Block> bl){
    bool ifAdd = false;
    vector<int> addId;
    for(vector<Block>::iterator b=bl.begin() ; b!=bl.end() ; ++b){
	ifAdd = false;
	for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
	    if((classificationTracks[i].ifEmpty == false) && (classificationTracks[i].train.size()+1 <= CLASSIFICATION_TRACKS_CAPACITY)){
		if(classificationTracks[i].train[0].blockName.compare((*b).blockName)==0){   //if the same
		    classificationTracks[i].train.push_back((*b));
		    ifAdd = true;
		    addId.push_back(i);
		    classificationTracks[i].btt.back().endingTime = (*b).timeEndHump;
		    break;
		}
	    }
	}
	if(ifAdd==false){
	    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
		if(classificationTracks[i].ifEmpty==true){
		    classificationTracks[i].train.push_back((*b));
		    cout << "[Hump]: new track: " << classificationTracks[i].train[0].blockName << endl;
		    classificationTracks[i].ifEmpty = false;   //a new track to save
		    
		    if(classificationTracks[i].btt.size()>0)
			classificationTracks[i].btt.back().endingTime = (*b).timeStartHump-HUMP_INTERVAL;

		    BlockToTrack tmp;
		    tmp.trackNo = i;
		    tmp.blockName = (*b).blockName;
		    tmp.dayNo = ((int)((*b).timeStartHump-1))/24+1;
		    tmp.startingTime = (*b).timeStartHump-HUMP_INTERVAL;
		    classificationTracks[i].btt.push_back(tmp);

		    ifAdd = true;
		    addId.push_back(i);
		    break;
		}
	    }
	}

	if(ifAdd == false){
	    for(vector<int>::iterator it=addId.begin() ; it!=addId.end() ; ++it){
		classificationTracks[(*it)].train.pop_back();
	    }
	    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
		if(classificationTracks[i].train.empty())
		    classificationTracks[i].ifEmpty = true;
	    }
	    return false;
	}
    }
    return true;
}
