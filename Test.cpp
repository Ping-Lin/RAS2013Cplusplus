#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <sstream>
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

//read file
void readInboundCar(vector< vector<Block> > &);   //read the csv file to save the railcar information

//outputfile
void outPutInboundTrainInfo();   //inbound train information
vector< vector<string> > countBlocks(vector<Block>);   //count [blockName] details
string changeToFormatTime(double);   //change double time to format time
string intToString(int &);   //int to string, return string value
void outPutBlockToTrack(); //solution block to track output
void outPutOutboundTrainInfo();   //output outbound train information
void outPutRailCarItinerary();   //output outbound train detail
void outPutClassificationRemains();   //output remains detail of classificationList

//for print out
vector< vector<Block> >humpBlockList;   //print inbound_train_info
vector< vector<Block> >outboundBlockList;   //print outboundtrain info

//every big event function
double receivingTrain(vector <vector<Block> > &);   //receive the train, argument is the blocklist
double humpTrain();   //hump the train
double pullBackTrain(int);   //pull back the train, argument is pullback No. 
double goOutTrain();   //go out train

//receivingArea function
int findLongestTrainAtBlockList(vector< vector<Block> >, double);   //find the longest train at blocklist that is inside the limits, return the id of that train

//hump function
vector<int> findLongestAtReceivingArea();   //find the longest train at receiving area and return the train id sequence from long to short
bool putIntoClassificationArea(vector<Block>);   //move blocks into classification area

//pullBack function
vector<int> findTrainCombine();   //find classification area combination
double findBlockLatestTime(vector<Block>);   //find the block latest time from classification area, to check for the pullback time
double smallestPullBackTimeOfEmptyTrack(double pt);   //find the smallest time of empty track at outbound area, if argument is bigger than the time, than return argument
bool receivePullBackBlock(vector<Block>);   //classification area train to outbound area

//outboundArea function
int countGoOut=1;   //count for go out train
int firstGoOutTrainId();   //find the first go out train track id


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
    outPutInboundTrainInfo();
    outPutBlockToTrack();
    outPutOutboundTrainInfo();
    outPutRailCarItinerary();
    outPutClassificationRemains();
}

/*
** read the csv file to save the railcar information
*/
void readInboundCar(vector< vector<Block> > &bl){
    fstream inFile;
    inFile.open("./data_readme/Input_data_set_5.csv");
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
	    b.timeAtReceivingArea = b.timeAtReceivingArea + (b.arrivalDay-1)*24;
	    tmpBl.push_back(b);
	    count++;
	}
	//final time
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
    if(id==-1){   //no empty track, or no train
	cout << "[ReceivingArea]No train." << endl;
	return eventTime[tr].back();
    }
    else if(id==-2){   //there are some empty tracks, but time limit so can't go in
	int maxLength = 0;
	id=0;
	for(int i=0 ; i<bl.size() ; i++){
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
    else if(id==-3){
	cout << "[ReceivingArea]No empty track." << endl;
	return eventTime[to].back();
    }
    //do the event to save the train
    double minHumpT = INT_MAX;
    int minId;   //min hump time track id
    bool ifSuccess=false;
    for(int i=0 ; i<RECEIVING_TRACKS_NUMBER ; i++){
	if(receivingTracks[i].ifEmpty == true){   //find the min hump time track id
	    if(minHumpT > receivingTracks[i].humpTime){
		minHumpT = receivingTracks[i].humpTime;
		minId = i;
	    }
	    if(bl[id][0].timeAtReceivingArea >= receivingTracks[i].humpTime){
		receivingTracks[i].ifEmpty = false;
		for(vector<Block>::iterator it=bl[id].begin() ; it != bl[id].end() ; ++it)
		    (*it).receivingTrackNo = i;
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
		(*it).receivingTrackNo = minId;
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
	    
	    if(eventTime[th].back()+HUMP_INTERVAL >= time)   //count the time
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
/*
* find the combination train from classification area, and satisfy the rules of longest first
* need the function: vector<int> findTrainCombine(), findBlockLatestTime(vector<Block>);double smallestPullBackTimeOfEmptyTrack(double);
* bool receivePullBackBlock(vector<Block>);
*/
double pullBackTrain(int no){
    int pN;
    if(no == 1)
	pN = tp1;
    else
	pN = tp2;
    vector<int> tmpIdCombine;
    vector<Block> train;   //for output to outbound area
    double time;   //time set up
    tmpIdCombine = findTrainCombine();
    if((tmpIdCombine.empty() == false) &&(tmpIdCombine[0] == -1)){   //classification can't go out
	cout << "[pullBack]Train in classification track are too small to go out." << endl;
	if(no == 1)
	    return eventTime[pN].back();
    }
    else if((tmpIdCombine.empty() == false) && (tmpIdCombine[0] != -1)){   //classification can pull
	for(int i=0 ; i<tmpIdCombine.size() ; i++)
	    train.insert(train.end(), classificationTracks[tmpIdCombine[i]].train.begin(), classificationTracks[tmpIdCombine[i]].train.end());   //same as java addAll
	time = findBlockLatestTime(train);
	if(eventTime[pN].back() > time)   //set up time
	    time = eventTime[pN].back();;
	time = smallestPullBackTimeOfEmptyTrack(time);
	cout << "[PullBack" << no << "] start: " << time << endl;\
	for(vector<Block>::iterator b=train.begin() ; b!=train.end() ; ++b){
	    (*b).timeStartPullBack = time;
	    (*b).timeEndPullBack = time + PULL_BACK_INTERVAL + PULL_BACK_MULTI_EACH_ADDITIONAL*1.0f*(tmpIdCombine.size()-1);
	    (*b).pullBackEngineNo = no;
	}
	if(receivePullBackBlock(train)==false){   //if receive error
	    cout << "[PullBack" << no << "]Outbound area is full." << endl;
	}
	else{   //receive correct
	    for(int i=0 ; i<tmpIdCombine.size() ; i++){
		classificationTracks[tmpIdCombine[i]].train.clear();
		vector<Block>(classificationTracks[tmpIdCombine[i]].train).swap(classificationTracks[tmpIdCombine[i]].train);
		classificationTracks[tmpIdCombine[i]].ifEmpty = true;
	    }
	    cout << "[PullBack" << no << "] count: " << train.size() << endl << "Block Name: ";
	    for(vector<Block>::iterator b=train.begin() ; b!=train.end() ; ++b)
		cout << (*b).blockName << ",";
	    cout << endl;
	    return time + PULL_BACK_INTERVAL + PULL_BACK_MULTI_EACH_ADDITIONAL*1.0f*(tmpIdCombine.size()-1);
	}
    }
    else{
	cout << "[PullBack" << no << "]There is no train at classification area." << endl;
    }

    return eventTime[pN].back();
}
/*
* go out train from outbound area.
* need the function: int firstGoOutTrainId();
*/
double goOutTrain(){
    double time;
    int id = firstGoOutTrainId();
    if(id!=-1){
	time = outboundTracks[id].train[0].timeEndPullBack + TECHNICAL_INSPECTION_TIME;   //go out train technical time
	if(countGoOut!=1){
	    if(eventTime[to].back() + OUTBOUND_TRAIN_INTERVAL >= time)
		time = eventTime[to].back() + OUTBOUND_TRAIN_INTERVAL;
	}
	cout << "[Outbound]" << time << endl;
	
	//set the block information
	for(vector<Block>::iterator b=outboundTracks[id].train.begin() ; b!=outboundTracks[id].train.end() ; ++b){
	    (*b).outboundTrainNo = countGoOut;
	    (*b).departureTrackNo = id;
	    (*b).timeDepartureAtDepartureArea = time;
	    (*b).departureDay = (int)(time/24)+1;
	}
	outboundTracks[id].pullBackTime = time;
	vector<Block> tmpBlockList;
	tmpBlockList.insert(tmpBlockList.end(), outboundTracks[id].train.begin(), outboundTracks[id].train.end());
	outboundBlockList.push_back(tmpBlockList);
	
	//clear the track
	outboundTracks[id].train.clear();
	vector<Block>(outboundTracks[id].train).swap(outboundTracks[id].train);
	outboundTracks[id].ifEmpty = true;
	countGoOut++;
	return time;
    }
    return eventTime[to].back();
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
    if(minHumpTime == INT_MAX){   //no empty track
	id = -3;
	return id;
    }

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
    double time;
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
    
    for(int i=0, count=0 ; count<id.size() ; i++, count++){   //longest first, but need FIFO
	time = receivingTracks[id[i]].train[0].timeAtReceivingArea + TECHNICAL_INSPECTION_TIME;
	if(eventTime[th].back()+HUMP_INTERVAL < time){
	    id.push_back(id[i]);
	    id.erase(id.begin()+i);
	    i--;
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

/*
* find the train combination  with rules of longest first
*/
vector<int> findTrainCombine(){
    int max = INT_MIN;
    bool ifAllTrackEmpty = true;
    vector<int> maxIdCombine;   //the long combine of id list
    vector<int> tmpIdCombine;
    vector<string> tmpNameCombine;
    vector<int>::iterator it=tmpIdCombine.begin();
    BlockValues bl;

    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
	int count = tmpIdCombine.size();
	for(int j=tmpIdCombine.size()-1 ; j>=0 ; j--){
	    if(classificationTracks[i].ifEmpty == false){
	        ifAllTrackEmpty = false;
		if(classificationTracks[i].train.size() > classificationTracks[tmpIdCombine[j]].train.size())
		    count = j;
	    }
    	}
	if(classificationTracks[i].ifEmpty == false){
	    ifAllTrackEmpty = false;
	    it = tmpIdCombine.begin();
	    tmpIdCombine.insert(it+count, i);
	}
    }
    if(ifAllTrackEmpty == true){   //increase the program
        return maxIdCombine;
    }

    for(int i=0 ; i<tmpIdCombine.size() ; i++){   //if single track can go out, than go out
	maxIdCombine.clear();
	vector<int>(maxIdCombine).swap(maxIdCombine);
	tmpNameCombine.clear();
	vector<string>(tmpNameCombine).swap(tmpNameCombine);
	
	maxIdCombine.push_back(tmpIdCombine[i]);
	tmpNameCombine.push_back(classificationTracks[tmpIdCombine[i]].train[0].blockName);
	max = classificationTracks[tmpIdCombine[i]].train.size();
	for(int j=i+1 ; j<tmpIdCombine.size() ; j++){
	    if(max >= MIN_OUTBOUND_TRAIN_NUMBER)   //if can go out, than go out
		return maxIdCombine;
	    else{
		tmpNameCombine.push_back(classificationTracks[tmpIdCombine[j]].train[0].blockName);
		if(bl.ifBlockCombination(tmpNameCombine)){   //judge if can combine
		    maxIdCombine.push_back(tmpIdCombine[j]);
		    max+=classificationTracks[tmpIdCombine[j]].train.size();
		}
		else{   //if can't combine
		    if(tmpNameCombine.size()>0)
			tmpNameCombine.pop_back();
		}
	    }
	}
	if(max>=MIN_OUTBOUND_TRAIN_NUMBER)   //if can go out
	    return maxIdCombine;
    }
    //min outbound train test
    if(max < MIN_OUTBOUND_TRAIN_NUMBER){
	maxIdCombine.clear();
	vector<int>(maxIdCombine).swap(maxIdCombine);
	maxIdCombine.push_back(-1);
    }

    return maxIdCombine;
}

/*
* to check for the block latesttime from classification area, when pull back.
*/
double findBlockLatestTime(vector<Block> tr){
    double latestT = 0.0f;
    for(vector<Block>::iterator b=tr.begin() ; b!=tr.end() ; ++b){
	if((*b).timeEndHump > latestT)
	    latestT = (*b).timeEndHump;
    }
    return latestT;
}

/*
* find the smallest time of empty track at outbound area, if argument is bigger than the time, than return argument
*/
double smallestPullBackTimeOfEmptyTrack(double pt){
    double min = INT_MAX*1.0f;
    bool empty = false;
    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
	if(classificationTracks[i].ifEmpty == true){
	    empty = true;
	    if(pt >= classificationTracks[i].pullBackTime)
		return pt;
	    if(classificationTracks[i].pullBackTime < min)
		min = classificationTracks[i].pullBackTime;
	}
    }
    if(empty == true)
	return min;
    else
	return pt;
}

/*
* classification area train to outbound area
*/
bool receivePullBackBlock(vector<Block> train){
    for(int i=0 ; i<DEPARTURE_TRACKS_NUMBER ; i++){
	if((outboundTracks[i].ifEmpty == true) && (train[0].timeStartPullBack >= outboundTracks[i].pullBackTime)){
	    outboundTracks[i].train = train;
	    outboundTracks[i].ifEmpty = false;
	    return true;
	}
    }
    return false;
}

/*
* find the first go out train track id
* return track id
*/
int firstGoOutTrainId(){
    double minTime = INT_MAX*1.0f;
    int goOutId = -1;
    for(int i=0 ; i<DEPARTURE_TRACKS_NUMBER ; i++){
	if(outboundTracks[i].ifEmpty==false){
	    if(outboundTracks[i].train[0].timeEndPullBack < minTime){
		minTime = outboundTracks[i].train[0].timeEndPullBack;
		goOutId = i;
	    }
	}
    }
    return goOutId;
}

/*
* out put inbound train information
*/
void outPutInboundTrainInfo(){
    fstream file;
    int s;   //for int to string tmp
    file.open("Inbound_Train_info.csv", fstream::out);
    file << "Inbound Train No\tDay No\tArriving time at receiving area\tReceiving track No\tNumber of blocks\
    \tNumber of cars\tDestination of blocks (number of cars)\tStarting time of humping job\tEnding time of humping job" << endl;
    for(int i=0 ; i<humpBlockList.size() ; i++){
	vector< vector<string> > tmpBlockName = countBlocks(humpBlockList[i]);
	string destinationOfBlocks;
	
	for(int j=0 ; j<tmpBlockName.size() ; j++){
	    s = tmpBlockName[j].size();
	    destinationOfBlocks+=tmpBlockName[j][0] + "(" + intToString(s) + ");";
	}
	file << "i" << humpBlockList[i][0].trainId << "\t" << humpBlockList[i][0].arrivalDay << "\t" << changeToFormatTime(humpBlockList[i][0].timeAtReceivingArea)
	    << "\tR" << (humpBlockList[i][0].receivingTrackNo+1) << "\t" << tmpBlockName.size() << "\t" << humpBlockList[i].size() << "\t" << destinationOfBlocks << "\t"
	    << changeToFormatTime(humpBlockList[i][0].timeStartHump) << "\t" << changeToFormatTime(humpBlockList[i][0].timeEndHump) << endl;
    }
    file.close();
}
/*
* count for [blockName] details
*/
vector< vector<string> > countBlocks(vector<Block> bl){
    vector< vector<string> > tmpBlockName;
    bool ifFind = false;
    for(int i=0 ; i<bl.size() ; i++){
	ifFind = false;
	vector<string>tmp;
	for(int j=0 ; j<tmpBlockName.size() ; j++){
	    if(tmpBlockName[j][0].find(bl[i].blockName) != string::npos){
		tmpBlockName[j].push_back(bl[i].blockName);
		ifFind = true;
		break;
	    }
	}
	if(ifFind == false){
	    tmp.push_back(bl[i].blockName);
	    tmpBlockName.push_back(tmp);
	}
    }
    return tmpBlockName;
}

/*
* change double time to format time
*/
string changeToFormatTime(double t){
    int day = ((int)t-1)/24+1;
    int hour = (int)(t-1)%24;
    double minute = (t-1.0f*(int)t)*60;
    int minu = (int)minute;
    string out = "Day" + intToString(day) + "," + intToString(hour) + ":" + intToString(minu);
    return out;
}

/*
* int to strnig, return string value
*/
string intToString(int &i){
    string s;
    stringstream ss(s);
    ss << i;

    return ss.str();
}

/*
* solution block to track output
*/
void outPutBlockToTrack(){
    
    fstream file; 
    file.open("block_to_track_assignment.csv", fstream::out);
    file << "Track\tBlock\tDay No\tStarting Time\tEnding Time" << endl;
    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++){
	for(int j=0 ; j<classificationTracks[i].btt.size() ; j++){
	    file << "C" << (classificationTracks[i].btt[j].trackNo+1) << "\t" << classificationTracks[i].btt[j].blockName << "\t" << classificationTracks[i].btt[j].dayNo << "\t"
	        << changeToFormatTime(classificationTracks[i].btt[j].startingTime) << "\t" << changeToFormatTime(classificationTracks[i].btt[j].endingTime) << endl;
	}
    }
    file.close();
}

/*
* output outbound train infomation
*/
void outPutOutboundTrainInfo(){
    fstream file;
    int s;   //for int to string
    file.open("outbound_train_info.csv", fstream::out);
    file << "outbound Train No\tDay No\tPullBack engine No\tStarting time of pullback job\tending time of pullback job\tdeparture time at departure area\t"
	<< "Departure track No\tNumber of blocks\tnumber of railcars\tdestination of blocks(associated number of cars)" << endl;
    for(int i=0 ; i<outboundBlockList.size() ; i++){
	vector< vector<string> > tmpBlockName = countBlocks(outboundBlockList[i]);
	string destinationOfBlocks;
	
	for(int j=0 ; j<tmpBlockName.size() ; j++){
	    s = tmpBlockName[j].size();
	    destinationOfBlocks+=tmpBlockName[j][0] + "(" + intToString(s) + ");";
	}
	file << "o" << outboundBlockList[i][0].outboundTrainNo << "\t" << outboundBlockList[i][0].departureDay << "\t" << outboundBlockList[i][0].pullBackEngineNo << "\t"
	    << changeToFormatTime(outboundBlockList[i][0].timeStartPullBack) << "\t" << changeToFormatTime(outboundBlockList[i][0].timeEndPullBack) << "\t"
	    << changeToFormatTime(outboundBlockList[i][0].timeDepartureAtDepartureArea) << "\t" << outboundBlockList[i][0].departureTrackNo+1 << "\t"
	    << tmpBlockName.size() << "\t" << outboundBlockList[i].size() << "\t" << destinationOfBlocks << endl;
    }
    file.close();
}

/*
* outbound train information details
*/
void outPutRailCarItinerary(){
    fstream file;
    file.open("railcar_itinerary.csv", fstream::out);
    file << "Car No\tDestination of block\tInbound Train No\tArrivalDay No\tarriving time at receivingarea\tReceiving track No\tStarting time of humping job\t\
    Ending time of humping job\tDeparture Day No\tPull back engine No\tStarting time of pull back job\tEnding time of pull back job\tDeparture time at departure area\t\
    outbound Train No\tDeparture track No" << endl;
    for(int i=0 ; i<outboundBlockList.size() ; i++)
	for(int j=0 ; j<outboundBlockList[i].size() ; j++){
	    file << outboundBlockList[i][j].carNo << "\t" << outboundBlockList[i][j].blockName << "\ti" << outboundBlockList[i][j].trainId << "\t" << outboundBlockList[i][j].arrivalDay
	    << "\t" << changeToFormatTime(outboundBlockList[i][j].timeAtReceivingArea) << "\tR" << outboundBlockList[i][j].receivingTrackNo+1 << "\t"
	    << changeToFormatTime(outboundBlockList[i][j].timeStartHump) << "\t" << changeToFormatTime(outboundBlockList[i][j].timeEndHump) << "\t"
	    << outboundBlockList[i][j].departureDay << "\t" << outboundBlockList[i][j].pullBackEngineNo << "\t" << changeToFormatTime(outboundBlockList[i][j].timeStartPullBack) << "\t"
	    << changeToFormatTime(outboundBlockList[i][j].timeEndPullBack) << "\t" << changeToFormatTime(outboundBlockList[i][j].timeDepartureAtDepartureArea) << "\to"
	    << outboundBlockList[i][j].outboundTrainNo << "\tD" << outboundBlockList[i][j].departureTrackNo+1 << endl;
    }
    file.close();
}

/*
* output the remains at classification area
*/
void outPutClassificationRemains(){
    vector< vector<Block> >classificationList;   //save the classification list at last
    for(int i=0 ; i<CLASSIFICATION_TRACKS_NUMBER ; i++)   //add remains to classificationList
	if(classificationTracks[i].train.empty()==false)
	    classificationList.push_back(classificationTracks[i].train);
	else{   //if no train
	    vector<Block> tmp;
	    classificationList.push_back(tmp);
	}
    
    fstream file;
    int s;
    file.open("Classification_Remains.csv", fstream::out);
    file << "Track No\tNumber of blocks\tnumber of railcars\tdestination of blocks" << endl;
    for(int i=0 ; i<classificationList.size() ; i++){
	vector< vector<string> > tmpBlockName = countBlocks(classificationList[i]);
	string destinationOfBlocks;
	
	for(int j=0 ; j<tmpBlockName.size() ; j++){
	    s = tmpBlockName[j].size();
	    destinationOfBlocks+=tmpBlockName[j][0] + "(" + intToString(s) + ");";
	}
	file << "C" << i+1 << "\t" << tmpBlockName.size() << "\t" << classificationList[i].size() << "\t" << destinationOfBlocks << endl;
    }
    file.close();
}

