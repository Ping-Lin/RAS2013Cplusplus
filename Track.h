/**
 * 此Class為ReceivingArea的軌道
 * 
 * @author Ping
 */
 #ifndef TRACK_H
 #define TRACK_H
 
 #include <vector>
 #include "BlockToTrack.h"
 using namespace std;
 
class Track {
	public:
		bool ifEmpty;   //判斷軌道是否為空
		vector<Block> train;   //track上面的火車
		double humpTime;   //被hump拉的時間
		vector<BlockToTrack> btt;   //for print out
	
		Track(){
			ifEmpty = true;
			humpTime = 0.0f;
		}
};

#endif
