#pragma once
#include"stdafx.h"
#include"struct.h"

class EnemyClass
{	
public:
	EnemyClass(TankType type, int level, MarkBox* MARKBOX);
	void E_setbox_4(int val);	//设置16个4*4格子为目标值。注:不是所有调用都标记坦克自身,需要参数
	void E_movement();			//设置敌人坦克的随机移动。
	void E_ajustlocation();		//坦克转向的时候将坦克位置调整到8*8的格子内。
	void E_shoot();				//发射子弹
	void Bombing(const HDC& center);	//绘制子弹爆炸
	virtual bool E_beshot(bool kill);	//子弹击中,虚函数,不同坦克实现不同
	void E_blasting(const HDC& center);	//绘制坦克爆炸
	BulletShootKind E_bulletmoving();	//子弹移动
	int GetID();
	void DrawBullet(const HDC &canvas);	//绘制子弹
	virtual void DrawTank(const HDC &canvas) {};	//绘制坦克,虚函数
	void CheckSpawn();			//检查出生点
	static void SetPause();		//暂停,与道具有关
private:
	void SignBullet(int x, int y, int dir, int val);	//标记子弹
	BulletShootKind CheckShot();		//检测子弹是否击中物体
	void ClearBarrier(int cx, int cy);	//清理击中墙壁
	void E_adjustdir();					//随机转向
	bool E_moveable();					//检查障碍物
	
protected:
	TankType E_type;			//坦克类型
	static int E_IDcounter;
	int E_ID;					//每个敌人坦克有自己的ID，区别不同的敌人。
	int E_level;				//坦克等级
	bool E_isdead;				//是否死亡，死：true；活：false。
	int E_X, E_Y;				//坦克的中心坐标。
	MarkBox* E_MBP;
	int E_step;					//步数
	int E_dir;					//方向
	int E_ImageIndex;			//移动时切换图片。
	int E_ColorIndex;			//移动时切换颜色
	static int mDPXY[4][2];
	int E_speed;				//速度

	Bullet mBullet;
	int mShootCounter;			//随机计数差后发射子弹
	Bomb mBomb;					
	Blast mBlast;
	Spawn mSpawn;

	static int moveT[4];
	TimeClock mTankTimer;		// 敌机移动速度
	TimeClock mBulletTimer;		// 子弹移动速度
	TimeClock mShootTimer;		// 发射子弹频率
	TimeClock mBombTimer;		// 子弹爆炸速度

	static TimeClock mPauseTimer;
	static bool mPause;

	static IMAGE mScore[4];	//击毁坦克显示得分
};

//三种不同坦克类型,继承自EnemyClass
class CommonTank :public EnemyClass
{
public:
	CommonTank(int level, MarkBox *mbp);
	~CommonTank() {};
	void DrawTank(const HDC &canvas);
private:
	IMAGE mTankImage[4][2];
};

class PropTank :public EnemyClass
{
public:
	PropTank(int level, MarkBox* mbp);
	~PropTank() {};

	void DrawTank(const HDC &canvas);
private:
	IMAGE mTankImage[2][4][2];
};

class BossTank :public EnemyClass
{
public:
	BossTank(TankType type, MarkBox* mbp);
	~BossTank() {};

	void DrawTank(const HDC &canvas);
	bool E_beshot(bool kill);
private:
	inline void GetTempImage(IMAGE temp[2][4][2],int color1, int color2)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 2; j++)
			{
				temp[0][i][j] = mTankImage[color1][i][j];
				temp[1][i][j] = mTankImage[color2][i][j];
			}
	}
private:
	int hp;
	IMAGE mTankImage[4][4][2];
};