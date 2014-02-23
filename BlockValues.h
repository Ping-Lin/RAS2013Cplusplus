/*
 * 此class定義有關block的類別和組合
 * 
 *  @author Ping
 */
#ifndef BLOCKVALUES_H
#define BLOCKVALUES_H

#include <string>
using namespace std;

class BlockValues {
	public:

		string *blockValues;
		int blockLength;
		string *blockCombination;
		int blockCombinationLength;
		BlockValues(){
		
			/**
			 * Big Case
			 */	
	/*		blockLength = 33;
			blockValues = new string[blockLength];
			blockValues[0] = "AA";blockValues[1] = "AB";blockValues[2] = "AC";blockValues[3] = "AD";
			blockValues[4] = "AE";blockValues[5] = "AF";blockValues[6] = "AG";blockValues[7] = "AH";
			blockValues[8] = "AI";blockValues[9] = "AJ";blockValues[10] = "AK";blockValues[11] = "AL";
			blockValues[12] = "AM";blockValues[13] = "AN";blockValues[14] = "AO";blockValues[15] = "AP";
			blockValues[16] = "AQ";blockValues[17] = "AR";blockValues[18] = "AS";blockValues[19] = "AT";
			blockValues[20] = "AU";blockValues[21] = "AV";blockValues[22] = "AW";blockValues[23] = "AX";
			blockValues[24] = "AY";blockValues[25] = "AZ";blockValues[26] = "BA";blockValues[27] = "BB";
			blockValues[28] = "BC";blockValues[29] = "BD";blockValues[30] = "BE";blockValues[31] = "BF";
			blockValues[32] = "BG";

			/**
			 * Data Sets 2 and 3 Combination:
			 */
	/*		blockCombination = new String[]{"AD,AF", "AF,AW,AY", "AH,AK", "AN,AP,AJ,BG", "AR,AW,AY"};
		
			/**
			 * Data Sets 4 and 5 Combination:
			 */
	/*		blockCombinationLength = 13;
			blockCombination = new string[blockCombinationLength];
			blockCombination[0] = "AA,AJ,AN,AP,BC,BE";blockCombination[1] = "AA,AJ,AN,AP,BG";blockCombination[2] = "AB,AD,AF,AS,AU,BA,BB";
	  		blockCombination[3] = "AB,AF,AW";blockCombination[4] = "AC,AG,AX";blockCombination[5] = "AE,AH,AK";blockCombination[6] = "AF,AW,AY";
	  		blockCombination[7] = "AH,AK,AT";blockCombination[8] = "AI,AL";blockCombination[9] = "AP,BE,BF";blockCombination[10] = "AQ,BD";
	  		blockCombination[11] = "AR,AW,AY";blockCombination[12] = "AV,AZ";
	
		
			/**
			 * Small Case, Illustrative Example
			 */
			blockLength = 6;
			blockValues = new string[blockLength];
			blockValues[0]="b1";
			blockValues[1]="b2";
			blockValues[2]="b3";
			blockValues[3]="b4";
			blockValues[4]="b5";
			blockValues[5]="b6";
			blockCombinationLength = 1;
			blockCombination = new string[blockCombinationLength];
			blockCombination[0] = "B5,B6";   //*/
		}
		/*
		 * 檢查是否可以結合，參數為block name集合
		 */
		bool ifBlockCombination(vector<string> blockSet){
			bool ifNoCombine = false;
			//這邊檢查同block name的結合
			if(!blockSet.empty()){
				string name = blockSet[0];
				for(int i=0 ; i<blockSet.size() ; i++){
					if(name.compare(blockSet[i]) != 0){
						ifNoCombine = true;
						break;
					}
				}
				if(ifNoCombine == false)
					return true;
		
				//這邊檢查不同block name的結合
				for(int i=0 ; i<blockCombinationLength ; i++){
					ifNoCombine = false;
					for(int j=0 ; j<blockSet.size() ; j++){
						if(blockCombination[i].find(blockSet[j]) == string::npos)
							ifNoCombine = true;
					}
					if(ifNoCombine == false)
						return true;
				}
			}
			return false;
		}
	
};

#endif

