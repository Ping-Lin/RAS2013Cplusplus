/*
 * 此class定義所有與每一個car所需要記載的內容
 * 
 *  @author Ping
 */
#ifndef BLOCK_H
#define BLOCK_H
 
#include <string>
using namespace std;
class Block {
	public:
		string blockName;   //block name

		/*
		 * primary key
		 */
	
		int carNo;   //sequence number
		int trainId;   //inbound train number
	
		/*
		 *  about track
		 */
	
		int receivingTrackNo;   //stay at which receiving track	
		int pullBackEngineNo;   //which pull back pull the block
		int outboundTrainNo;   //outbound train number
		int departureTrackNo;   //departure track number
	
		/*
		 * about time
		 */
	
		int arrivalDay;   //arrival day
		int departureDay;   //departure day
		double timeAtReceivingArea;   //arriving time at receiving area
		double timeStartHump;   //starting time of humping job
		double timeEndHump;   //ending time of humping job
		double timeStartPullBack;   //starting time of pull back job
		double timeEndPullBack;   //ending time of pull back job
		double timeDepartureAtDepartureArea;   //departure time at departure area
};

#endif
