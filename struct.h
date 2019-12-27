#pragma once
#include "stdafx.h"
#include "Music.h"
#include "TimeClock.h"


#define _CLEAR			-1		//子弹击中墙后标记
#define _EMPTY			0
#define _FOREST			1
#define _ICE			2
#define _WALL			3
#define _RIVER			4
#define _STONE			5

#define CAMP_SIGN		200
#define PLAYER_SIGN		100		//PLAYER_SIGN + mPlayerID
#define ENEMY_SIGN		10000	//ENEMY_SIGN + E_level * 1000 + E_type * 100 + E_ID
#define PB_SIGN			400		//玩家子弹标记 PB_SIGN+10*mPlayerID+bullet_id
#define EB_SIGN			300		//敌军子弹标记 
#define BD_SIGN			444		//子弹相消标记
#define PROP_SIGN		5000	//道具标记PROP_SIGN+0~5

#define _CX				0
#define _CY				1
#define DIR_LEFT		0
#define DIR_UP			1
#define DIR_RIGHT		2
#define DIR_DOWN		3

#define WIN_WIDTH		512		//窗口大小，可更改
#define WIN_HEIGHT		448
#define CENTER_WIDTH	208		//游戏区域大小
#define CENTER_HEIGHT	208
#define CANVAS_WIDTH	256		//画布大小
#define CANVAS_HEIGHT	224
#define CENTER_X		16		// 黑色游戏区域相对左上角的坐标
#define CENTER_Y		9

#define BOX_SIZE		8
#define SMALL_BOX_SIZE  4

#define GAMEOVER_WIDTH				31		//GMAEOVER字样大小
#define GAMEOVER_HEIGHT				15
#define NUMBER_SIZE					7		// 数字大小7*7
#define ENEMY_TANK_ICO_SIZE			7		// 敌机图标大小
#define PLAYER_TANK_ICO_SIZE_X		7		// 玩家图标大小
#define PLAYER_TANK_ICO_SIZE_Y		8
#define PLAYER_12P_ICO_SIZE_X		14		// 1P/2P 图标大小
#define PLAYER_12P_ICO_SIZE_Y		7
#define FLAG_ICO_SIZE_X				16		// 旗子大小
#define FLAG_ICO_SIZE_Y				15

#define TOTAL_ENEMY_NUM		20		//敌机总数
#define	GRAY_TANK			0
#define RED_TANK			1
#define YELLOW_TANK			2
#define GREEN_TANK			3
enum TankType { Prop, Common };

struct MarkBox
{
	int box_4[52][52];
	int box_8[26][26];
	int bullet[52][52];
	int clear[52][52];
	int prop[26][26];
};

struct Map
{
	char map[26][27];
};

enum GameResult { Victory, Fail };

enum SelectResult
{
	SinglePlayer = 1,
	DualPlayer,
	Error
};
class SelectPanel
{
public:
	SelectPanel(HDC des_hdc, HDC canvas_hdc);
	~SelectPanel() {}

	void Init();
	SelectResult ShowPanel();

private:
	int mSelect_player_image_y;
	HDC mWin_hdc, mCanvas_hdc;
	IMAGE mSelect_player_image;
	IMAGE mBackground;
	IMAGE mTank[2];
	int mTankPos[3][2];
	int mIndex, mCounter;
};

enum BulletShootKind { None = 99, Player1 = PLAYER_SIGN, Player2, Camp, Other };
struct Bullet
{
	int cx, cy;						//中心坐标(弹头)
	//int ix, iy;						//图像坐标(待删除)
	int dir;
	int speed[4];					//子弹速度,玩家与敌机不同
	int killID;
	bool  isShot=false;
	IMAGE mBulletImage[4];	
	static int cdevXY[4][2];		//子弹中心相对坦克中心偏移量,上下,左右分别要对齐
	static int cidevXY[4][2];		//子弹图像坐标相对子弹中心偏移量
	static int bombdev[4][2];		//子弹爆炸中心偏移量(待删除)
};

struct Bomb
{
	int cx, cy;
	static IMAGE mBombImage[3];
	bool bombflag;
	int counter;
	inline void Boom(int x, int y, int dir)
	{
		bombflag = true;
		counter = 0;
		cx = x + Bullet::bombdev[dir][_CX];
		cy = y + Bullet::bombdev[dir][_CY];
	}
};

class Blast
{
public:
	Blast();
	~Blast(){}

	void Init();
	void CampBlasting(const HDC& center);
	void PlayerBlasting(const HDC& center);
	void EnemyBlasting(const HDC& center,IMAGE* score);
	void SetBlast(int x, int y);
	bool IsBlasting();
private:
	int cx, cy;
	static IMAGE mBlastImage[5];
	bool blastflag;
	int counter;
	TimeClock Timer;
};

class Spawn
{
public:
	Spawn();
	~Spawn(){}
	void Init();
	void Spawnig(const HDC& center);
	void SetSpawn(int cx,int cy);
	bool IsSpawning();
	void SetFlag();
	bool CanSpawn();
private:
	int cx, cy;
	static IMAGE mStarImage[4];
	bool spawning;
	bool spawnflag;
	int mImageIndex;
	int mImageCounter;


};

class ScorePanel
{
public:
	ScorePanel();
	//~ScorePanel() {};
	void AddKill(int player_id, int enemy_level);
	bool ShowPanel(const HDC& win,const HDC& canvas);
	void GameMode(int mode,int stg);
	void Init();
	void Add500(int player_id);

private:
	static IMAGE yellow_number;
	static IMAGE number;
	static IMAGE bonus;		
	IMAGE player[2];
	IMAGE pts[2];
	
	int playernum;
	int playerX[2], playerY[2];
	int ptsX[2], ptsY[2];
	int scoreX[2][4][2];	//分数位置scoreX[玩家0/1][坦克等级0,1,2,3][0为分数,1为击杀数]
	static const int scoreY[2][4][2];	//bug???scoreY[][3][0/1]值会变

	int kills[2][4];		//击杀数...[player][enemy_level]
	int totalkill[2];
	int totalkillX[2], totalkillY[2];
	int totalscore[2];
	int totalscoreX[2], totalscoreY[2];
	int stage;
};

#define RING_TIME		12000	//ring保护时间
#define PAUSE_TIME		10000	//敌机暂停时间
enum PropType { Tank, Star, Timer, Grenade, Shovel, Helmet, Nothing };
class PropClass
{
public:
	PropClass();
	~PropClass() {};
	void Init(MarkBox*);
	void Prop_Draw(const HDC &canvas);
	//bool Prop_Check_for_vanish();	//控制一个道具是否应该消失。被玩家获取而消失返回ture，时间过长而消失返回false。
	void Prop_Sign(int type);		//标记prop地图上道具位置。
	void Prop_Appear();		//一个道具出现。
	PropType Prop_Vanish(int);		//一个道具消失。

private:
	MarkBox* Prop_MBP;
	IMAGE Prop_Image[6];
	IMAGE score;
	int ScoreCounter;
	TimeClock Prop_Timer;
	int Image_Counter;
	int Prop_x;		//在8*8地图中道具图片的左上角坐标。
	int Prop_y;
	PropType Prop_Type;
	bool propflag;
};

class Ring
{
public:
	Ring();
	~Ring() {};
	void Init();
	void ShowRing(const HDC& center, int cx,int cy);
	void SetRing(int time=3500);			//默认为出生时,时间较短
	bool IsRinging();
private:
	IMAGE ring[2];
	TimeClock timer;
	int ImageCounter;
	bool ringflag;

};

