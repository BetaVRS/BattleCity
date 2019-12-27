#pragma once
#include "stdafx.h"
#include "struct.h"

class PlayerClass
{
public:
	PlayerClass(int player,MarkBox* b);
	~PlayerClass() {};

	void Init();
	void DrawTank(const HDC &canvas);	//绘制坦克
	void DrawBullet(const HDC &canvas);	//绘制子弹
	void DrawData(const HDC&canvas);	//绘制侧边栏数据
	void Move(int new_dir);			//控制移动
	bool CheckMoveable();			//检测障碍物
	void PlayerControl();			//玩家控制
	void ShowBox();					//debug用
	bool ShootBullet(int bullet_id);//发射子弹
	int BulletMoving();				//子弹移动
	void Bombing(const HDC& center);//子弹爆炸
	void GetShot();					//玩家被击中
	void Blasting(const HDC& center);//坦克爆炸
	bool IsDead();					//检测死亡(生命耗光
	int GetID();
	void GetNewProp(PropType type);	//获取道具
private:
	void SignBullet(int x, int y, int dir, int val);	//标记子弹于碰撞箱
	void ClearBarrier(int bullet_id, int cx, int cy);	//清除墙/石头
	int CheckShot(int i);			//检测子弹击中物体
	void ReSpawn();					//重生
	bool CheckSpawn();			//检测出生区域是否被占据
	inline void CheckSliding(int dir);	//检测是否在冰块上滑行
	IMAGE* GetTankImage(int dir, int index);	
	void FillBox_4(int x, int y, int fval);		//根据坦克当前坐标填充box_4碰撞箱数据

private:
	int mPlayerID;
	static MarkBox* mbp;		//碰撞箱
	IMAGE mTankImage[4][4][2];
	int mMoveCounter;
	int mTankX, mTankY;			//坐标
	int mLife;
	unsigned char mTankDir : 2;
	int mTankLevel;				//分为4个等级
	static int mDPXY[4][2];
	
	IMAGE mNumber;
	IMAGE m12pImage;
	IMAGE mTankIcon;
	int mTankIconX, mTankIconY;
	int m12pImgX, m12pImgY;
	int mLifeImgX, mLifeImgY;
	
	Bullet mBullet[2];
	Bomb mBomb[2];
	Blast mBlast;
	Spawn mSpawn;

	TimeClock mTankTimer;			//坦克移动速度控制时钟
	static int mTankMoveDrt[4];		//更精细地控制速度,以mTankLevel为索引
	TimeClock mBulletTimer;			//子弹移动速度控制时钟
	static int mBulletMoveDrt[4];	//控制子弹速度,以mTankLevel为索引
	TimeClock mBulletShootTimer;    //待删除
	int mBulletShootCount;

	bool mMoving;
	bool mDead;
	bool mOnIce;
	bool mSliding;
	int mSlideCounter;
	int mSlideStep;
	Ring mRing;
	
};
