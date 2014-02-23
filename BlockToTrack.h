/*
 * 輸出結果用類別
 *
 * @author Ping
 */
#ifndef BLOCKTOTRACK_H
#define BLOCKTOTRACK_H
#include <string>
using namespace std;

class BlockToTrack {
	public:
		int trackNo;
		string blockName;
		int dayNo;
		double startingTime;
		double endingTime;
	
		BlockToTrack(){
			trackNo = -1;
			dayNo = 0;
			startingTime = 0.0f;
			endingTime =0.0f;
		}
	
};

#endif
