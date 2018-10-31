﻿//
//  player.hpp
//  testC
//
//  Created by 陈帅 on 2018/9/11.
//

#ifndef player_hpp
#define player_hpp

#include <stdio.h>
#include <list>
#include <iostream>
#include "position.h"

#include "../utils/diyType.h"
#include "resCard.h"
#include "roomCard.h"

class gameMgr;
using namespace std;
class player {
    playerStatus m_ps;
    position pos;

    string name;
    int age, moveNum, m_floor;
    int m_id, m_mapID, m_roleID;
    
    map<examType, int> et2level;
    map<examType, vector<int> > etLevel2value;
    list<resCard*> resList;
    list<resCard*> infoList;
    list<int> skills;

	direction inputDir();
	list<int> rollDice(examType, int forceDiceNum = 0);
	template<class Type> Type inputFromList(const list<Type>&);
    int useWeapon();

    int gainNewItem(configType);

    int incrETLevel(examType, int);
    int getETValue(examType);
public:
    player();
    player(int32_t roleID, int32_t mapID);
    player(int roleID, int mapID, map<string, string>);

    int getID(), getRoleID();
    roomCard* getMyRoom();

//    oper getOperate();
    int start();
	int move();
    int stop();

    int attack(int id);
    int useSkill(int id);

	bool enterRoom(roomCard*, bool isNewRoom);
	bool leaveRoom(roomCard*);   //从上一回合进入，这一回合离开，是“离开房间”
    bool passRoom(roomCard*);    //这一回合进入，这一回合离开，是“通过房间”
    int moveTo(direction);

    int excuteExam(examine);
    int excutePunish(examType, int);

    bool gainBuff(cardUseType, card*);

    bool getReality();

    int changeNewRoomRotation(direction, roomCard*);

    ////////////组队房///////////////
    int32_t modifyStatus(int32_t);
};

#endif /* player_hpp */
