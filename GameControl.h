#pragma once
#include"stdafx.h"
#include"struct.h"
#include"PlayerClass.h"
#include"EnemyClass.h"
class GameControl
{
public:
	GameControl(HDC win,HDC canvas);
	~GameControl() {};
	void Init();
	void LoadMap();
	void GameLoop();
	void Gamemode(int player_num);
	GameResult GameFrame();		//<-此函数为游戏中一帧,循环调用※
	void ShowBox();
public:
	static int mCurrentStage;
private:
	void StartAnimation();
	void RefreshData();		//<-此函数控制数据刷新※
	void RefreshCenter();	//<-此函数控制图像刷新※
	void RefreshSide();
	void AddEnemy();
	bool KillEnemy(int id);
	void CheckGame();
	void InitMarkBox();		//LoadMap()内调用,初始化mMarkBox(碰撞箱数据)
	void SetShovel();		//设置铲子道具
	void IsGetShovel();		//检测铲子
	void SetCamp(int val);	//设置Camp周围为石头/墙
	void FillBox_4(int i, int j, int fval);		//通过fval标记4*4格子
private:
	HDC mWin_hdc, mCenter_hdc, mCanvas_hdc;
	IMAGE mCenterImage;
	MarkBox *mMarkBox;					//碰撞箱数据
	list<PlayerClass*> mPlayerList;		//储存玩家
	list<EnemyClass*> mEnemyList;		//储存敌机

	IMAGE mGrayBackgroundImage;		// 游戏灰色背景图
	IMAGE mBlackBackgroundImage;	// 黑色背景图

	Map mMap;						// 存储地图数据结构
	IMAGE mStoneImage;				// 石头
	IMAGE mForestImage;				// 树林
	IMAGE mIceImage;				// 冰块
	IMAGE mRiverImage[2];			// 河流
	int mRiverCounter;
	IMAGE mWallImage;				// 泥墙
	IMAGE mCamp[2];					// 大本营
	IMAGE mScoreBackground;			// 记分板背景
	IMAGE mEnemyTankIcoImage;		// 敌机坦克图标
	IMAGE mFlagImage;				// 旗子

	int mAnimCounter;
	IMAGE mStageImage;				// STAGE 字样
	IMAGE mNumberImage;				// 0123456789 当前关卡数
	int mGameOverX;
	int mGameOverY;
	IMAGE mGameOverImage;			//GameOver
	TimeClock mGameOverTimer;
	int mGameOverCounter;

	int mRemainEnemy;
	int mCurrentEnemy;
	int mKilledEnemy;
	
	TimeClock mMainTimer;
	TimeClock mAddEnemyTimer;

	int mWinCounter;
	bool mWin;
	bool mFail;

	Blast mBlast;
	bool mCampBlast;
	bool mShowScorePanel;

	ScorePanel mScore;
	PropClass mProp;
	bool mGetShov;
	int mShovCounter;
};