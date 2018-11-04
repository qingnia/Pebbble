﻿//
//  player.cpp
//  testC
//
//  Created by 陈帅 on 2018/9/11.
//

#include "player.hpp"
#include "gameMgr.h"

/***************************内部常用函数**********************************/
gameMap* player::getMyMap()
{
	gameMgr* gm = gameMgr::getGameMgr();
	gameMap* myMap = gm->getMap(this->m_mapID);
	return myMap;
}

bool player::isActionDone()
{
	return this->actionDone;
}

bool player::isMyTurn()
{
	gameMap* map = getMyMap();
	if (map->getActionRoleID() == this->m_roleID)
	{
		return true;
	}
	return false;
}

/***************************内部常用函数**********************************/
player::player()
{
	this->moveNum = 0;
	this->m_floor = 1;
    this->pos = position(50, 50);
	this->actionDone = false;
}

player::player(int32_t roleID, int32_t mapID)
{
	this->m_roleID = roleID;
	this->m_mapID = mapID;
	this->m_ps = psEnter;
	this->actionDone = false;
}

player::player(int roleID, int mapID, map<string, string> playerConfig)
{
	stringstream ss;
	ss<<"初始化玩家 roleID:"<<roleID;
	logInfo(ss.str());
	ss.str("");
	this->moveNum = 0;
	this->m_mapID = mapID;
    this->m_roleID = roleID;
	this->m_floor = 1;
 	this->pos = position(50, 50);
	this->et2level[etDice] = 0;
	
	map<string, string>::iterator iter;
	string key, value;
	for (iter = playerConfig.begin(); iter != playerConfig.end(); iter++)
 	{
		key = iter->first;
		value = iter->second;
		if (key == "id")
		{
			this->m_id = stringToNum<int>(value);
		}
		else if (key == "originSpeedLevel")
		{
			this->et2level[etSpeed] = stringToNum<int>(value);
		}
		else if(key == "speedValue")
		{
			this->etLevel2value[etSpeed] = split<int>(value, "|");
		}
		else if(key == "originStrengthLevel")
		{
			this->et2level[etStrength] = stringToNum<int>(value);
		}
		else if(key == "strengthValue")
		{
			this->etLevel2value[etStrength] = split<int>(value, "|");
		}
		else if(key == "originSpiritLevel")
		{
			this->et2level[etSpirit] = stringToNum<int>(value);
		}
		else if(key == "spiritValue")
		{
			this->etLevel2value[etSpirit] = split<int>(value, "|");
		}
		else if(key == "originKnowledgeLevel")
		{
			this->et2level[etKnowledge] = stringToNum<int>(value);
		}
		else if(key == "knowledgeValue")
		{
			this->etLevel2value[etKnowledge] = split<int>(value, "|");
		}
 	}
	ss << "玩家速度：" << this->getETValue(etSpeed) <<"，力量："<< this->getETValue(etStrength);
	ss <<"，知识："<< this->getETValue(etKnowledge) <<"精神："<< this->getETValue(etSpirit);
	logInfo(ss.str());
}

/****************************玩家输入***********************************************/
direction player::inputDir()
{
    cout<<"输入：上w,下s,左a,右d"<<endl;

    char input;
    cin>>input;

	direction dir;
    switch(input)
    {
    case 'w':
		dir = dirUp;
        break;
    case 's':
		dir = dirDown;
        break;
    case 'a':
		dir = dirLeft;
        break;
    case 'd':
		dir = dirRight;
        break;
    default:
        break;
    }
    return dir;
}

list<int> player::rollDice(examType et, int forceDiceNum)
{
	stringstream ss;
	ss << "掷骰子，";
	int diceNum;
	if (forceDiceNum > 0)
	{
		diceNum = forceDiceNum;
		ss << "骰子数量强制为" << diceNum;
	}
	else
	{
		diceNum = this->getETValue(et);
		ss << "骰子数量为：" << diceNum;
		if (this->et2level[etDice] > 0)
		{
			diceNum = diceNum + this->et2level[etDice];
			ss << ",因玩家有特殊物品，骰子数量增加" << this->et2level[etDice];
		}
	}
	ss << "，每个骰子点数分别为：";
	list<int> diceNums(diceNum);
	for(int i = 0; i < diceNum; i++)
	{
		int num = random(2);
		ss<<num<<",  ";
		diceNums.push_back(num);
	}
	ss<<"掷骰结束。";
	logInfo(ss.str());
	return diceNums;
}

template<class Type>
Type player::inputFromList(const list<Type> &l)
{
	stringstream ss;
	ss << "输入" << list2String(l) << "中的一个";
	//list<Type>::const_iterator begin = l.begin();
	//list<Type>::const_iterator end = l.end();
	auto begin = l.begin();
	auto end = l.begin();
	Type input;
	while (cin >> input)
	{
		while (begin != end && *begin != input)
		{
			++begin;
		}
		if(begin != end)
		{
			break;
		}
	}
	return input;
}

/****************************玩家输入***********************************************/

int player::excuteExam(examine exam)
{
	if (exam.et == etNone)
    {
        return 0;
    }
    exam.showMsg();
    list<int> diceNums = this->rollDice(exam.et);
    int score = accumulate(diceNums.begin(), diceNums.end(), 0);
    if(exam.attackValue > 0)
    {
        int compareNum = random(2 * exam.attackValue);
        if(score < compareNum)
        {
            this->excutePunish(exam.attackEffect.et, compareNum - score);
        }
    }
    else
    {
        list<effect>::iterator iter;
        for (iter = exam.efList.begin(); iter != exam.efList.end(); iter++)
	    {
           effect ef = *iter;
           if (score >= ef.min && score <= ef.max)
           {
               this->excutePunish(ef.et, ef.eNum);
               break;
           }
	    }
    }
	return 0;
}

int player::excutePunish(examType et, int num)
{
	stringstream ss;
	string efAttr = getETString(et);
	ss << "你受到" << num << "点" << efAttr << "损伤";
	logInfo(ss.str());
	ss.clear();

	examType attribute = et;
    if (et == etPhysicalDamage)
    {
        ss<<"你收到%d点物理损伤，请选择1:速度，2:力量";
		logInfo(ss.str());
        //选择
        attribute = etSpeed;
    }
    else if (et == etMindDamage)
    {
        //选择
        attribute = etSpirit;
    }
	this->incrETLevel(attribute, num);
    return true;
}

bool player::getReality()
{
	gameMap* myMap = getMyMap();
    if (myMap->getProcess() > 0)
    {
        return true;
    }

	stringstream ss;
    ss<<"尝试揭露真相吧";
	logInfo(ss.str());
	ss.str("");
    int infoNum = myMap->getInfoNum();
    list<int> diceNums = this->rollDice(etNone, infoNum);
    int score = accumulate(diceNums.begin(), diceNums.end(), 0);
    if (score > 6)
    {
		ss << "真相出现了！";
		logInfo(ss.str());
		myMap->unravelRiddle(this->pos, this->m_id);
        return true;
    }
	ss << "真相还被隐藏着";
	logInfo(ss.str());
    return false;
}
bool player::enterRoom(roomCard* room, bool isNewRoom)
{
	//执行房间事件、考验、
	if (room->needExam(mrtEnter))
	{
		this->excuteExam(room->cardExam);
	}

	if (isNewRoom)
	{
		//进新房间要拿东西
		this->gainNewItem(room->type);
		//新房间的事件、考验等
		this->moveNum = this->getETValue(etSpeed);
	}
	else
	{
		gameMap* myMap = getMyMap();
		list<int> canAttackList = myMap->getCanAttackRoleIDList(this);
		if (canAttackList.size() > 0)
		{
			//询问玩家是否要攻击
			stringstream ss;
			ss << "你当前可以攻击 " << list2String(canAttackList) << " 这些玩家，请输入要攻击目标，0为放弃攻击";
			logInfo(ss.str());

			canAttackList.push_back(0);
			int target = this->inputFromList(canAttackList);
			if (target != 0)
			{
				this->attack(target);
			}
		}
		this->moveNum++;
	}
	return true;
}

bool player::leaveRoom(roomCard* room)
{
	if (room->needExam(mrtLeave))
	{
		this->excuteExam(room->cardExam);
	}

	//执行房间内事件，如果遭遇敌人，步数再+1
	return true;
}

bool player::passRoom(roomCard* room)
{
	if (room->needExam(mrtPass))
	{
		this->excuteExam(room->cardExam);
	}

	//执行房间内事件，如果遭遇敌人，步数再+1
	return true;
}

int player::start()
{
	roomCard* room = this->getMyRoom();
	if (room->needExam(mrtStart))
	{
		this->excuteExam(room->cardExam);
	}
	return 0;
}

int player::move()
{
	stringstream ss;
	ss << "轮到玩家" << this->m_roleID << "移动,玩家速度：" << this->getETValue(etSpeed) << "当前移动步数：" << this->moveNum;
	logInfo(ss.str());
	ss.clear();
    //行动值在停止行动时清零
    //一次移动一格，移动距离达到速度停止
	while (this->moveNum < this->getETValue(etSpeed))
	{
		direction dir = this->inputDir();
		if (dir == dirStop)
		{
			break;
		}
		gameMap* myMap = getMyMap();
		roomCard* thisRoom = myMap->getRoom(this->pos);

		//检查这个位置从当前房间能不能通过
		if (!thisRoom->canPass(dir))
		{
			return -1;
		}
		bool canPass = false;
		if (this->moveNum == 0)
		{
			canPass = this->leaveRoom(thisRoom);
		}
		else
		{
			canPass = this->passRoom(thisRoom);
		}
		if (canPass)
		{
			this->moveTo(dir);
		}
	}
	return 0;
}

int player::stop()
{
	//检查房间，看是否有停止时生效的
	roomCard* room = this->getMyRoom();
	if (room->needExam(mrtStay))
	{
		this->excuteExam(room->cardExam);
	}
	//移动结束后把行动力恢复
	this->moveNum = 0;
	this->actionDone = true;
	return 0;
}

int player::moveTo(direction dir)
{
	stringstream ss;

	gameMap* myMap = getMyMap();
	position* nextPos = (this->pos).getNeibourPos(dir);
	roomCard* nextRoom = myMap->getRoom(*nextPos);

	bool enterNewRoom = false;
	if (nextRoom == nullptr)
	{
		//玩家要走的位置没有房间,从牌库中拿到一个新房间,将新房间放进地图
		nextRoom = myMap->bindNewRoom(this->m_floor, *nextPos);
		if (nextRoom == nullptr)
		{
			//新房间也没，卡池已空
			return -2;
		}
		this->changeNewRoomRotation(dir, nextRoom);
		enterNewRoom = true;
		ss << "玩家进入新房间";
	}
	else
	{
		//这个位置已经有了房间
		//1.检查两个房间是否互通
		direction revDir = reverseDir(dir);
		if (!nextRoom->canPass(revDir))
		{
			return -1;
		}
		ss << "这个房间已经被人开发过，可以进入";
	}
	ss << "，剩余移动步数：" << this->getETValue(etSpeed) - this->moveNum << "房间名：" << nextRoom->getName() << "\n\t   " << nextRoom->getDesc();
	logInfo(ss.str());

	this->pos.x = nextPos->x;
	this->pos.y = nextPos->y;

	this->enterRoom(nextRoom, enterNewRoom);

	return 0;
}

int player::changeNewRoomRotation(direction fromDir, roomCard* room)
{
	direction dir = dirUp;
	while(! room->changeRotation(fromDir, dir))
	{
		if (room->canChangeRotation())
		{
			dir = this->inputDir();
		}
	}
	return 0;
}

int player::gainNewItem(configType ct)
{
	gameMap* myMap = getMyMap();
	issueCard* newIssue;
	resCard* newRes;
	resCard* newInfo;

	stringstream ss;
	switch (ct)
	{
	case ctIssue:
		newIssue = myMap->getNewIssue();
		ss<<"房间类型为：事件\n\t     "<<newIssue->getName()<<"\n\t  "<<newIssue->getDesc();
		logInfo(ss.str());
		//一次性的考验直接不保存，如果是持续性的，需要保存状态
		//事件一定有考验
		this->excuteExam(newIssue->cardExam);
//		newIssue->cardExam.affect(*this);
		break;
	case ctRes:
		newRes = myMap->getNewRes();
		ss<<"房间类型为：物品\n\t     "<<newRes->getName()<<"\n\t  "<<newRes->getDesc();
		logInfo(ss.str());
		this->resList.push_back(newRes);
		this->gainBuff(cutGain, newRes);
		break;
	case ctInfo:
		newInfo = myMap->getNewInfo();
		ss<<"房间类型为：预兆\n\t     "<<newInfo->getName()<<"\n\t  "<<newInfo->getDesc();
		logInfo(ss.str());
		this->infoList.push_back(newInfo);
		this->gainBuff(cutGain, newInfo);
		//如果不是作祟阶段，需要进行揭露真相
		this->getReality();
		break;
	default:
		break;
	};
	return 0;
}

int player::getID()
{
    return this->m_id;
}

int player::getRoleID()
{
	return this->m_roleID;
}

roomCard* player::getMyRoom()
{
	gameMap* myMap = getMyMap();
	roomCard* room = myMap->getRoom(this->pos);
	return room;
}

/*************************属性相关***********************************/
int player::incrETLevel(examType et, int num)
{
	stringstream ss;
	this->et2level[et] += num;
	if (num != 0)
	{
		ss << "玩家" << this->m_roleID << "健康发生变化:属性" << getETString(et) << "变化" << num << "个等级，新等级为" << this->et2level[et] << "，新数值为" << this->getETValue(et);
		logInfo(ss.str());
		ss.str("");
	}
	if(this->et2level[et] < 0)
	{
		ss << "玩家" << this->m_roleID << getETString(et) << "变为0，死亡！";
		logInfo(ss.str());
		//死亡
		gameMap* myMap = getMyMap();
		myMap->tryEnd();
	}
	return this->et2level[et];
}

int player::getETValue(examType et)
{
	int level = this->et2level[et];
	vector<int> Level2value = this->etLevel2value[et];
	return Level2value[level];
}

bool player::gainBuff(cardUseType cut, card* c)
{
	map<examType, int> buff = c->getBuff(cut);

	examType et;
	int value;
	map<examType, int>::iterator iter;
	for(iter = buff.begin(); iter != buff.end(); iter++)
	{
		et = iter->first;
		value = iter->second;
		this->incrETLevel(et, value);
		//数值加减
	}
	return true;
}
/*************************属性相关***********************************/

/************************攻击相关**************************************/
int player::attack(int roleID)
{
	//1.先检查能不能攻击，考虑特殊道具（枪）影响攻击范围
	//1.选择要不要使用武器
	//2.掷骰，检查物品和特殊房间，看是否影响骰数
	//3.被攻击方掷骰
	//4.判定结果，根据攻击类型对输的一方惩罚物理/精神损伤
	return 0;
}

int player::useWeapon()
{
	stringstream ss;
	list<int> l;
	list<resCard*>::iterator resIter;
	for(resIter = this->resList.begin(); resIter != this->resList.end(); resIter++)
	{
		resCard* res = *resIter;
		if (res->isWeapon())
		{
			l.push_back(res->getID());
		}
	}
	if (l.size() > 0)
	{
		ss<<"选择你使用的武器，0为不用"<< list2String(l);
	}
	int weapon = inputFromList(l);
	//使用武器逻辑
	return weapon;
}
/************************攻击相关**************************************/

int32_t player::modifyStatus(int32_t status)
{
	playerStatus ps = (playerStatus)status;
	switch(ps)
	{
	case psReady:
		this->m_ps = ps;
		break;
	case psStart:
		//需要所有人都准备好才能开始
		break;
	default:
		break;
	}
return 0;
}
