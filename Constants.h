/**
 * Constants 介面定義了一些常數的數值
 * 
 *	@author Ping
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants{
	/**
	 * case 1
	 */
	const int RECEIVING_TRACKS_NUMBER = 4;
	const int RECEIVING_TRACKS_CAPACITY = 40;
	const int CLASSIFICATION_TRACKS_NUMBER = 6;
	const int CLASSIFICATION_TRACKS_CAPACITY = 50;
	const int DEPARTURE_TRACKS_NUMBER = 4;
	const int DEPARTURE_TRACKS_CAPACITY = 40;
	const int MAX_OUTBOUND_TRAIN_NUMBER = 40;
	const int MIN_OUTBOUND_TRAIN_NUMBER = 30;
	const double HUMP_RATE = (1*1.0f/2.5)/60;   // 每拉一節車要多久，單位:小時
	const double HUMP_INTERVAL = 8*1.0f/60; //單位:小時
	const double TECHNICAL_INSPECTION_TIME = 30*1.0f/60;   //單位:小時
	const double COMBINING_BLOCKS_PER_TRACK = 3*1.0f/60;   //一個track的每有一block結合所需時間,單位:小時
	const double PULL_BACK_INTERVAL = 10*1.0f/60;   //單位:小時
	const double PULL_BACK_MULTI_EACH_ADDITIONAL = 15*1.0f/60;   //單位:小時
	const double OUTBOUND_TRAIN_INTERVAL = 8*1.0f/60;   //單位:小時
	/*
	 * case 5
	 */
/*    const int RECEIVING_TRACKS_NUMBER = 10;
    const int RECEIVING_TRACKS_CAPACITY = 185;
    const int CLASSIFICATION_TRACKS_NUMBER = 42;
    const int CLASSIFICATION_TRACKS_CAPACITY = 60;   
    const int DEPARTURE_TRACKS_NUMBER = 7;
    const int DEPARTURE_TRACKS_CAPACITY = 207;
    const int MAX_OUTBOUND_TRAIN_NUMBER = 150;
    const int MIN_OUTBOUND_TRAIN_NUMBER = 25;
    const double HUMP_RATE = (1*1.0f/2.2)/60;   // 每拉一台車，單位:小時
    const double HUMP_INTERVAL = 10*1.0f/60; //單位:小時
    const double TECHNICAL_INSPECTION_TIME = 45*1.0f/60;   //單位:小時
    const double PULL_BACK_INTERVAL = 10*1.0f/60;   //單位:小時
    const double PULL_BACK_MULTI_EACH_ADDITIONAL = 15*1.0f/60;   //單位:小時
    const double OUTBOUND_TRAIN_INTERVAL = 10*1.0f/60;   //單位:小時*/	
}

#endif
