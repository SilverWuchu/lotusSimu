

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <vector>
#include <map>
#include <conio.h> 

using namespace std;

int _hashTab[19];
const int MAXI = 9;
map<int, vector<int>> _scamGroup;
vector<int> _playerInputs;

int _token = 0;
int _level = 1;
//the global counter of opened blocks at the current level 
int _cc = 0;				

//1->3, 4->6, 7->9
//the key of the map's value, for the right list of preset
int _scamLv = 1;

bool _showLog = false;
bool _screenClear = false;
//bool _showLog = true;

void DisplayBlock (int theIndex, int* theHash) {
	int solo = 0;
	
	solo = theHash[theIndex];
	
	switch (solo) {
		case -1:
			cout << "x" << " ";
			break;
		case 0:
			cout << "*" << " ";
			break;
		
		}
	
}

void DisplayNine (int theHash[], int theCut) {
	//1st line
	cout << " ";
	
	for (int i = 1; i < MAXI + 1; i++) {
		
		DisplayBlock(i, theHash);
				
		if (i % theCut == 0) {
			cout << endl;
			cout << " ";
		}
	}
}

//start from 1
void DisplayList (vector<int> theList) {
	int solo = 0;
	
	//don't display the 0th index
	for (int i = 1; i < theList.size(); i++) {
		solo = theList[i];
		
		cout << solo << " ";
	}
	
	cout << endl;
}

//what: norm display
void DisplayListNorm (vector<int> theList) {
	int solo = 0;
	
	//don't display the 0th index
	for (int i = 0; i < theList.size(); i++) {
		solo = theList[i];
		
		cout << solo << " ";
	}
	
	cout << endl;
}

//what: read all percentage
//259
//1 0
void ReadIn () {
	int curInt = 0;
	int scamLv = 1;
	vector<int> curList;
	
	ifstream inFile("nineKickin.in");
	
	//total token to play the game
	inFile >> _token;
	
	inFile >> _level;
	inFile >> _cc;
	
	//the skip the first element (matching index to round, it shall start at 1 not 0)
	curList.push_back(-128);
	
	while (inFile >> curInt) {
//		_scamList.push_back(curInt);
		curList.push_back(curInt);
		
		//9+1 elements
		if (curList.size() % 10 == 0)  {
			_scamGroup.insert(pair<int, vector<int>>(scamLv, curList));
			
//			cout << ccKey << "->inserted"<< endl;
			
			curList.clear();
			
			//we don't use the first value
			curList.push_back(-128);
			scamLv++;
		}
	}
	
	inFile.close();

//	cout << "3 Groups of preset percentages:" << endl; 
//	//tested
//	for (const auto soloPair: _scamGroup) {
//		DisplayList(soloPair.second);	
//	}
	
}


//what: calc all the token you need to open all levels
int CalcTokenRmd (int curLv, int blockOpen) {
	int blockRmd = MAXI - blockOpen;
	int res = 0;
	
	res = curLv * blockRmd;
	
	for (int i = curLv + 1; i < MAXI + 1; i++) {
		res += i * MAXI;
	}
	
	return res;
}

//what: 88.88889 -> 88.88
double PurgeWithTwoDigits (double theRes) {
	//keeping 2 digits -> 88.88
	theRes = (int)(theRes * 100);
	theRes /= 100;
	
	return theRes;
}

//what: true->OK input; false->WRONG input, will ignore
bool CheckInput (string theInput) {
	//user behaviour control
	if (theInput.length() > 1) {
		return false;
	}
		
	if (theInput[0] > '9') {
		return false;
	}
		
	if (theInput[0] < '1') {
		return false;
	}
	
	return true;
}

//what: the reserve value (need to have this amount to finish the game)
//memo: skip the current level
int CalcReserve (int theMaxi, int curLv) {
	//need to calc lvToFinish's minimum value
	int lvToFinish = theMaxi - curLv; 
	int reserve = 0;
	
	for (int i = curLv + 1; i < curLv + lvToFinish + 1; i++) {
		reserve += i;
	}
	
	return reserve;
}

//what: find the preset list and get the one with the right index
int FindScamPreset (int theLevel, int theIndex) {
	
	int curScamLv = (theLevel - 1) / 3 + 1;	
//	auto curScamList = _scamGroup[curScamLv];
	vector<int> curScamList = _scamGroup[curScamLv];
		
	//the first one
	int scamPreset = curScamList[theIndex];	
	
	return scamPreset;
}

//what: check if having enough token
//memo: -1->can't complete; other than -1 ->CAN complete
double CalcWinRate (int token, int theCc) {
	
	int lvToFinish = 0;
	int curLevel = 0;
	int reserve = 0;				//for the next level(1 per level)
	int curCc = theCc;				//we need our own counter 
	double loseRate = 100;
	
	//if currently 2nd level, then the lvToFinish is 9 - 2 = 7
	//if currently 8th level, then the lvToFinish is 9 - 8 = 1
	int prevLv = 0;
	
	curLevel = _level;
	prevLv = curLevel;
	
	reserve = CalcReserve(MAXI, curLevel);
	
	if (_showLog) {
		cout << "inited curLevel->" << curLevel << endl;
	}
	
	//last level, no reserve needed
	if (curLevel == MAXI) {
		reserve = 0;
	}
	
	if (_showLog) {
		//the least you should have to clear all levels
		cout << "reserve->" << reserve << endl;
	}
	
	
	//cut reserve away, to finish the game later 
	token -= reserve;
	
		//mini is the MINI amount to finish all the level
	if (token < 0) {
		cout << "Don't have enough token->" << token << "/" << reserve << endl;
		return 0;
	}
	
	int curScamLv = 0;
	vector<int> curScamList;
	int scamPresetWinningRate = 0;
	double curScamRate = 1;
	int prevScamLv = 0;
	
	if (_showLog) {
		cout << "CUR-LV=>" << curLevel << endl;
	}
	
	//>= calc 
	//play until don't have enough token
	while ((token > curLevel - 1) && curLevel < MAXI + 1) {
//		cout << "while:curLevel->" << curLevel << endl;
		curScamLv = (curLevel - 1) / 3 + 1;	
		if (prevScamLv < curScamLv) {
			curScamList = _scamGroup[curScamLv];	
			
			if (_showLog) {
				DisplayList(curScamList);	
			}
			
			
			prevScamLv = curScamLv;
			//can't do continue, some other code need to be executed
		}
		
		else {
			if (prevLv < curLevel) {
				
				if (_showLog) {
					cout << "CUR-LV=>" << curLevel << endl;
					DisplayList(curScamList);
				}
			
				
				prevLv = curLevel;
			}
		}
		
		
		//10 for 10%
		scamPresetWinningRate = curScamList[curCc + 1];
//		curScamRate = (double)scamPreset / 100;
		

		//100 * 10 / 100
		//losing rate 
		//init->100 (100%)
		//init->100-8 = 92
		
		if (_showLog) {
			cout << "scamPresetRate " << scamPresetWinningRate << endl;
//			cout << "loseRate->" << PurgeWithTwoDigits(loseRate) << "%" << endl;
		}
		
		//the last one should be 100 (designer should make sure that happens)
		loseRate *= (double)(100 - scamPresetWinningRate);
		loseRate /= 100;
		
		
		//82.8 <-- 92 * 0.9 (where 92 is current losing rate; 0.9 came from 10% for the [1] scamRate, )
		//97.02 <-- 99 * 98 (/100) = 97.02
		if (_showLog) {
			cout << "round " << curCc + 1 << ":"; 
			cout << "loseRate->" << PurgeWithTwoDigits(loseRate) << "%" << endl;
		}
		
		if (_showLog) {
			cout << "scamWinRate->[" << curCc + 1<< "]->";
			cout << scamPresetWinningRate << "%";
			
			//meaning, current round win rate at CURRENT MOMENT
			cout << ",roundWinRate(" << PurgeWithTwoDigits(100 - loseRate) << "%);"; 
			cout << token; 
		
			cout << endl;	
		}
		
		
		//the last preset value shall always be 100
//		if (scamPresetWinningRate == 100) {
//			loseRate = 100;
//		}
//		cout << "resRate" << "->" << resRate << endl;
		
		curCc ++;
		token -= curLevel;
		
		//init: 0, then, 0->1
		//done for the level
		if (curCc % MAXI == 0) {
			curLevel++;
			curCc = 0;
			//finished one level, so resRate is automatically 100
			//just in case designer fucked up with data 
			loseRate = 100;			
			
			if (_showLog) {
				cout << "curToken->" << token << endl;	
			}
			
			
			//finished it
			if (curLevel > MAXI) {
				continue;
			}
			
			//add reserve back
			token += reserve;
			
			if (_showLog) {
				cout << "+reserve->" << reserve << ",token->" << token << endl;
			}
			
			
			//from 8 to 9 level
			if (curLevel < MAXI) {
				reserve = CalcReserve(MAXI, curLevel);
				token -= reserve;
				
				if (_showLog) {
					cout << "+neoReserve->" << reserve << ", neoToken->" << token << endl;	
				}
			}
			
			
			if (token < 0) {
				
				if (_showLog) {
					cout << "Don't have enough token [reserve]->" << token << "/" << reserve << endl;
				}
				
				
				return 0;
			}
			
			//can't perform for the current level
			if (token < curLevel) {
				
				if (_showLog) {
					cout << "Don't have enough token [cur level]->" << token << "/" << curLevel << endl;
				}
				
				
				return 0;
			}
			continue;
		}
		
		//did the maxi at the current level
	}
	
	if (_showLog) {
		cout << "curLevel=" << curLevel << endl;
		cout << "curLosingRate=" << loseRate << endl;	
//		cout << "doing next level->" << endl;
	}
	
	//won all level
	if (curLevel > MAXI) {
		loseRate = 0;
	}
	
	//calc the final rate
	for (int i = curLevel + 1; i < MAXI + 1; i++) {
		if (_showLog) {
			cout << "doing next level->" << i << endl;
		}
		
		//the first one
		scamPresetWinningRate = FindScamPreset(i, 1);
		
		if (_showLog) {
			cout << "scamPreset=" << scamPresetWinningRate << endl;
		}
		
		//the rate for you to lose
		loseRate *= (100 - scamPresetWinningRate);
		
		if (_showLog) {
			cout << loseRate << endl;
		}
		
		loseRate /= 100;
//		resRate = scamPreset;
	}
	
	if (_showLog) {
		cout << "loseRate" << "->" << loseRate << endl;
		cout << "winRate" << "->" << (100 - loseRate) << endl;
	}
	

	
	//current unveiled box (0 for the 1st)
	curCc = _cc;	

	//this is the winRate	
	return 100 - loseRate;	
}

void DisplayLvInfo () {
	
	cout << "LV" << _level << ": ";
//	cout << " PLAYER->" << playerIn << ";";
	cout << "UNBLOCKED->" << _cc << endl;
	
}

//current token 
//104
//level, current unveiled block
//9 0

//level 1->3
//10 15 25
//30 50 65 
//75 85 100

//level 4->6
//5 7 12
//15 25 32
//33 43 100

//level 7->9
//1 2 3
//4 5 6 
//7 8 100
//what: scam game
void Game () {
	int playerIn = 0;
	int theCut = 3;
//	int cc = 0;
	double honestMissingRate = 0;
	int maxi = 9;

	int scamPreset = 0;
	int scamRand = 0;
	int scamPer = 0;
	double scamRes = 0;
	
	string curStr = "Error";
	
//	scamRes = 100 - _scamList[0];

	int trashScamLv = (_level - 1) / 3 + 1;	
	vector<int> curList = _scamGroup[trashScamLv];	
	
	//starting for the 1st group
	scamRes = 100 - curList[_cc + 1];	
	
	int tokenRmd = 0;
	double curWinRate = 0;
	
	//reset _cc
	_cc = 0;
	
//	system("cls"); 
	DisplayLvInfo();
	
	while (_token > _level) {
		
		
		
		///		
		trashScamLv = (_level - 1) / 3 + 1;	
		vector<int> trashScamList = _scamGroup[trashScamLv];	
		
		cout << "Current rates:";
		DisplayList(trashScamList);	
		///
		
		cout << "-Pick a number from 1 to 9" << endl;
		
		//initially, there is NO opened block (therefore cc->0)
		tokenRmd = CalcTokenRmd(_level, _cc);
		curWinRate = CalcWinRate(_token, _cc);				// maxi decres//		maxi -= cc;
//		res = (maxi - 1) * 100 / maxi;

		//cout << " LV->" << _level << endl;
		cout << " TOKEN=>" << _token << "/";
		cout << tokenRmd;
		cout << "(curWining:" << PurgeWithTwoDigits(curWinRate) << "%)";
		cout << endl;
		//honest rate
		
		//9 - (0 + 1) => if guessed wrong, the proba
		honestMissingRate = (maxi - (_cc + 1)) * 100;
		honestMissingRate = (double)honestMissingRate / (double)(maxi);
		honestMissingRate = PurgeWithTwoDigits(honestMissingRate);
		
		
		cout << " NORM.LOSING %=>" << honestMissingRate << "%" << endl;
		
		if (curList[_cc + 1] == 100) {
			scamRes = 0;
		}
		
		//scam rate
		cout << " SCAM LOSING %=>" << scamRes << "%" << endl;
		
		DisplayNine(_hashTab, theCut);
			
		cin >> curStr;
					
		if (!CheckInput(curStr)) {
			continue;
		}
		
		//purge the playerIn
		playerIn = curStr[0] - '0';
		
		cout << "input recorded" << endl;
		//record the input
		_playerInputs.push_back(playerIn);
		
			
		//checked already
		if (_hashTab[playerIn] == -1) {
			cout << " CHECKED->" << playerIn << endl;
			continue;
		}
		
		//finding the scam preset
		curList = _scamGroup[_scamLv];
//		scamPreset = _scamList[cc];
	
		//initially, it was 0, so adjust it into 1 (+1)
		scamPreset = curList[_cc + 1];
		
		
		//[1, 100]
		scamRand = rand() % 100 + 1;
		
		//scam
		if (scamRand < scamPreset + 1) {
//			if (_screenClear) {
//				system("cls"); 	
//			}
			
			
			system("cls"); 	
//			cout << endl;	
			cout << "Level " << _level;
			cout << ", CLEARED! " << "(" << _cc + 1 << " shots)";
			cout << endl;
			
			cout << "INPUT:";
			DisplayListNorm(_playerInputs);
			_playerInputs.clear();
			cout << endl;
			
			_token -= _level;
			_level++;
			
			if (_level > MAXI) {
				return;
			}
			
			
			cout << " GO TO NEXT LEVEL->" << _level << endl << endl;
			
			//123 -> 1
			//456 -> 2
			//(lv - 1) / 3 + 1
			_scamLv = (_level - 1) / 3 + 1;
			
			curList = _scamGroup[_scamLv];
			
//			cout << " PRESET list:";
//			DisplayList(curList);	
			
			memset(_hashTab, 0, sizeof(_hashTab));
			
			//reset scam val
			scamPer = 0;
			scamRes = 0;
			_cc = 0;
//			return;
			
	
			//starting for the 1st element, the 0th is a plug
			scamRes = 100 - curList[1];	
			
			DisplayLvInfo();
			continue;
		}


		//you have to PAY!
		_token -= _level;
		
		//missed
		
		if (_screenClear) {	system("cls"); 	}
		DisplayLvInfo();
//		cout << "LV" << _level << ";";
//		cout << " PLAYER->" << playerIn << ";";
//		cout << "cc->" << _cc + 1 << endl;
		
		_hashTab[playerIn] = -1;

		//next scamPer
		scamPer = 100 - curList[_cc + 2];
		
		if (_showLog) {
			cout << "cur Per->" << scamPer << endl;	
		}
		
		
		scamRes *= (double)scamPer;
		scamRes /= 100;
		
		if (_showLog) {
			cout << "cur ScamRes->" << scamRes << endl;
		}
		
		
		scamRes = PurgeWithTwoDigits(scamRes);
		
		_cc++;		
	}
	
}


void Core () {
	int theCut = 3;
	
	//seed
	srand(time(NULL));
	
	cout << "---Lotus Simulation---" << endl;
	
	
	while (true) {
		Game();
		
		if (_level > MAXI) {
			
			cout << "-Well, you finished it. Congrats." << endl;
			cout << "-Let's always KEEEE fuKunNan, shall we?" << endl;
			return;
		}
		
		//_level -> MAXI
		if (_cc < MAXI) {
			cout << "-You couldn't finish it." << endl;
			cout << "-You will have to KEEE fuKunNan, isn't it..." << endl;
			
			cout << "INPUT:";
			DisplayListNorm(_playerInputs);
			_playerInputs.clear();
			cout << endl;
			return;
		}
//		cout << "Stuck here" << endl;
	}
	
	
}



int main () {
	ReadIn();
	Core();
	
	return 0;	
}
