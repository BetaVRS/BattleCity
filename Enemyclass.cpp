#include"stdafx.h"
#include"EnemyClass.h"

int EnemyClass::mDPXY[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };
int EnemyClass::moveT[4]= { 30, 15, 27, 25 };
int EnemyClass::E_IDcounter = 0;
bool EnemyClass::mPause = false;
TimeClock EnemyClass::mPauseTimer;
IMAGE EnemyClass::mScore[4];

EnemyClass::EnemyClass(TankType type, int level, MarkBox *MARKBOX)
{
	E_ID = E_IDcounter;
	E_type = type;
	E_level = level;
	E_MBP = MARKBOX;
	E_isdead = false;
	mPauseTimer.SetDeltaT(PAUSE_TIME);

	char c[50];
	for (int i = 0; i < 4; i++)
	{
		_stprintf_s(c, "./res/big/bullet-%d.gif", i);
		loadimage(&mBullet.mBulletImage[i], c);
		_stprintf_s(c, "./res/big/%d00.gif", i + 1);
		loadimage(&mScore[i], c);
	}

	int tempx[3] = { BOX_SIZE, 13 * BOX_SIZE, 25 * BOX_SIZE };	// 坦克随机出现的三个位置 x 坐标
	E_X = tempx[rand() % 3];
	E_Y = BOX_SIZE;
	E_dir = DIR_DOWN;
	E_ImageIndex = 0;
	E_ColorIndex = 0;

	for (int i = 0; i < 4; i++)
		mBullet.speed[i] = 3;

	E_step = rand() % 250;
	mTankTimer.SetDeltaT(moveT[E_level]);
	mShootTimer.SetDeltaT(rand() % 1000 + 700);
	mBulletTimer.SetDeltaT(30);
	E_IDcounter = (E_IDcounter + 1) % TOTAL_ENEMY_NUM;		//注意敌机一直生成,避免ID超过20造成bug

	mSpawn.SetSpawn(E_X, E_Y);
}

void EnemyClass::E_setbox_4(int val)
{
	int ajust_x, ajust_y;		//储存将地图划分为4*4的格子后的新坐标。
	if (E_X%SMALL_BOX_SIZE < 2)
		ajust_x = E_X / SMALL_BOX_SIZE;
	else
		ajust_x = E_X / SMALL_BOX_SIZE + 1;
	if (E_Y%SMALL_BOX_SIZE < 2)
		ajust_y = E_Y / SMALL_BOX_SIZE;
	else
		ajust_y = E_Y / SMALL_BOX_SIZE + 1;
	for (int i = ajust_y-2; i < ajust_y+2; i++)
		for (int j = ajust_x-2; j < ajust_x+2; j++)
			E_MBP->box_4[i][j] = val;//10000 + 1000 * E_level + 100 * E_type + E_ID;//五位数里每一位都具有意义。10000=ENEMYSIGN


}

void EnemyClass::E_movement()
{
	if (mPause)
		mPause = !mPauseTimer.IsTimeOut();
	if (E_isdead || !mTankTimer.IsTimeOut()||mSpawn.IsSpawning()||mPause)
		return;

	E_setbox_4(_EMPTY);

	if (E_step-- < 0)
		E_adjustdir();

	if (E_moveable())
	{
		E_X += mDPXY[E_dir][_CX];
		E_Y += mDPXY[E_dir][_CY];
	}
	else
		E_adjustdir();

	E_shoot();

	E_setbox_4(ENEMY_SIGN + E_level * 1000 + E_type * 100 + E_ID);
}

void EnemyClass::E_ajustlocation()
{
	if (E_dir == DIR_RIGHT || E_dir == DIR_LEFT)//左右转上下
	{
		if (E_X%BOX_SIZE > 3)
			E_X = (E_X / BOX_SIZE + 1)*BOX_SIZE;
		else
			E_X = (E_X / BOX_SIZE)*BOX_SIZE;
	}
	if (E_dir == DIR_UP || E_dir == DIR_DOWN)//上下转左右
	{
		if (E_Y%BOX_SIZE > 3)
			E_Y = (E_Y / BOX_SIZE + 1)*BOX_SIZE;
		else
			E_Y = (E_Y / BOX_SIZE)*BOX_SIZE;
	}
}

void EnemyClass::E_shoot()
{
	if (!mShootTimer.IsTimeOut() || mBullet.isShot)
		return;
	mBullet.isShot = true;
	mBullet.cx = E_X + Bullet::cdevXY[E_dir][_CX];
	mBullet.cy = E_Y + Bullet::cdevXY[E_dir][_CY];
	mBullet.dir = E_dir;

	SignBullet(mBullet.cx, mBullet.cy, mBullet.dir, EB_SIGN);
}

void EnemyClass::Bombing(const HDC & center)
{
	int index[6] = { 0,1,1,2,2,1 };
	for (int i = 0; i < 2; i++)
		if (mBomb.bombflag)
		{
			TransparentBlt(center, mBomb.cx - BOX_SIZE, mBomb.cy - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&Bomb::mBombImage[index[mBomb.counter % 6]]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
			if (!(mBomb.counter = ++mBomb.counter % 6))
				mBomb.bombflag = false;
		}
}

bool EnemyClass::E_beshot(bool kill)
{
	if (E_isdead)
		return false;
	E_isdead = true;
	mBlast.SetBlast(E_X, E_Y);

	E_setbox_4(_EMPTY);
	return true;
}

void EnemyClass::E_blasting(const HDC & center)
{
	mBlast.EnemyBlasting(center, &mScore[E_level]);
}

BulletShootKind EnemyClass::E_bulletmoving()
{
	if (!mBulletTimer.IsTimeOut() || !mBullet.isShot)
		return BulletShootKind::None;

	/*if (mBulletShootCount > 0)
		mBulletShootCount--;*/
	BulletShootKind kind = CheckShot();
	
	switch (kind)
	{
	case BulletShootKind::Camp:
	case BulletShootKind::Other:
	case BulletShootKind::Player1:
	case BulletShootKind::Player2:
		return kind;
	case BulletShootKind::None:
		break;
	default:
		break;
	}

	int dir = mBullet.dir;
	SignBullet(mBullet.cx, mBullet.cy, dir, _EMPTY);
	mBullet.cx += mDPXY[dir][_CX] * mBullet.speed[E_level];
	mBullet.cy += mDPXY[dir][_CY] * mBullet.speed[E_level];
	/*mBullet[i].ix = mBullet[i].cx + Bullet::cidevXY[mBullet[i].dir][_CX];
	mBullet[i].iy = mBullet[i].cy + Bullet::cidevXY[mBullet[i].dir][_CY];*/
	SignBullet(mBullet.cx, mBullet.cy, dir, EB_SIGN);

	return BulletShootKind::None;
}

int EnemyClass::GetID()
{
	return E_ID;
}

void EnemyClass::DrawBullet(const HDC & canvas)
{
	int ix, iy, dir;
	for (int i = 0; i < 2; i++)
		if (mBullet.isShot)
		{
			dir = mBullet.dir;
			ix = mBullet.cx + Bullet::cidevXY[dir][_CX];
			iy = mBullet.cy + Bullet::cidevXY[dir][_CY];
			IMAGE bullet = mBullet.mBulletImage[dir];
			TransparentBlt(canvas, ix, iy, bullet.getwidth(), bullet.getheight(),
				GetImageHDC(&bullet), 0, 0, bullet.getwidth(), bullet.getheight(), 0x000000);
		}
}

void EnemyClass::CheckSpawn()
{
	int li = E_Y / SMALL_BOX_SIZE - 2;
	int lj = E_X / SMALL_BOX_SIZE - 2;
	for (int i = li; i < li+4; i++)
		for (int j = li; j < li+4; j++)
			if (E_MBP->box_4[i][j] > _EMPTY && E_MBP->box_4[i][j]%100 != E_ID )
				return;
	if (E_MBP->box_4[li][lj] == _EMPTY)
	{
		E_setbox_4(ENEMY_SIGN + E_level * 1000 + E_type * 100 + E_ID);
		mSpawn.SetFlag();
	}

}

void EnemyClass::SetPause()
{
	mPause = true;
	mPauseTimer.Init();
}

void EnemyClass::SignBullet(int x, int y, int dir, int val)
{
	int b4i = y / SMALL_BOX_SIZE;
	int b4j = x / SMALL_BOX_SIZE;
	if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
		return;

	E_MBP->bullet[b4i][b4j] = val;
}

BulletShootKind EnemyClass::CheckShot()
{
	int cx = mBullet.cx;
	int cy = mBullet.cy;
	int dir = mBullet.dir;
	if (cx <= 0 || cy <= 0 || cx >= CENTER_WIDTH - 1 || cy >= CENTER_HEIGHT - 1)
	{
		mBullet.isShot = false;
		mBomb.Boom(cx, cy, dir);
		SignBullet(cx, cy, dir, _EMPTY);
		return BulletShootKind::Other;
	}

	int tempi, tempj;
	int b4i = cy / SMALL_BOX_SIZE;
	int b4j = cx / SMALL_BOX_SIZE;
	int b8i = cy / BOX_SIZE;
	int b8j = cx / BOX_SIZE;

	if (E_MBP->bullet[b4i][b4j] == PB_SIGN + 0 * 10 + 0 ||
		E_MBP->bullet[b4i][b4j] == PB_SIGN + 0 * 10 + 1 ||
		E_MBP->bullet[b4i][b4j] == PB_SIGN + 1 * 10 + 0 ||
		E_MBP->bullet[b4i][b4j] == PB_SIGN + 1 * 10 + 1)
	{
		mBullet.isShot = false;
		E_MBP->bullet[b4i][b4j] = BD_SIGN;
		mBomb.Boom(cx, cy, dir);
		return BulletShootKind::Other;
	}
	else if (E_MBP->bullet[b4i][b4j] == BD_SIGN)
	{
		mBullet.isShot = false;
		E_MBP->bullet[b4i][b4j] = _EMPTY;
		mBomb.Boom(cx, cy, dir);
		return BulletShootKind::None;
	}

	int check[2][2][2] = { { {0,0},{-1,0} }, {{0,0},{0,1}} };
	switch (dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
		for (int k = 0; k < 2; k++)
		{
			tempi = b4i + check[0][k][0];
			tempj = b4j + check[0][k][1];

			if (E_MBP->box_4[tempi][tempj] == CAMP_SIGN)
			{
				mBullet.isShot = false;
				mBomb.Boom(cx, cy, dir);
				return BulletShootKind::Camp;
			}
			else if (E_MBP->box_4[tempi][tempj] == _WALL || E_MBP->box_4[tempi][tempj] == _STONE)
			{
				mBullet.isShot = false;
				ClearBarrier(cx, cy);
				mBomb.Boom(cx, cy, dir);
				return BulletShootKind::Other;
			}
			else if (E_MBP->box_4[tempi][tempj] >= ENEMY_SIGN)
			{

				return BulletShootKind::None;
			}
			else if (E_MBP->box_4[tempi][tempj] == PLAYER_SIGN || E_MBP->box_4[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBullet.isShot = false;
				mBomb.Boom(cx, cy, dir);
				return BulletShootKind(E_MBP->box_4[tempi][tempj]);
			}
		}
		break;
	case DIR_UP:
	case DIR_DOWN:
		for (int k = 0; k < 2; k++)
		{
			tempi = b4i + check[1][k][0];
			tempj = b4j + check[1][k][1];

			if (E_MBP->box_4[tempi][tempj] == CAMP_SIGN)
			{
				mBullet.isShot = false;
				mBomb.Boom(cx, cy, dir);
				return BulletShootKind::Camp;
			}
			else if (E_MBP->box_4[tempi][tempj] == _WALL || E_MBP->box_4[tempi][tempj] == _STONE)
			{
				mBullet.isShot = false;
				ClearBarrier(cx, cy);
				mBomb.Boom(cx, cy, dir);
				return BulletShootKind::Other;
			}
			else if (E_MBP->box_4[tempi][tempj] >= ENEMY_SIGN)
			{
				return BulletShootKind::None;
			}
			else if (E_MBP->box_4[tempi][tempj] == PLAYER_SIGN || E_MBP->box_4[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBullet.isShot = false;
				mBomb.Boom(cx, cy, dir);
				return (BulletShootKind)E_MBP->box_4[tempi][tempj];
			}
		}
		break;
	default:break;
	}
	return BulletShootKind::None;
}

void EnemyClass::ClearBarrier(int cx, int cy)
{
	int t4i, t4j;
	int b4i = cy / SMALL_BOX_SIZE;
	int b4j = cx / SMALL_BOX_SIZE;
	int check[2][4][2] = { { {0,-2},{0,-1},{0,0},{0,1} },{ {-1,0},{0,0},{1,0},{2,0} } };	//不能放入case中,报错
	switch (mBullet.dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
		for (int i = 0; i < 4; i++)
		{
			t4i = b4i + check[0][i][_CY];
			t4j = b4j + check[0][i][_CX];

			if (E_MBP->box_4[t4i][t4j] == _WALL)
			{
				E_MBP->box_4[t4i][t4j] = _CLEAR;
				E_MBP->clear[t4i][t4j] = _CLEAR;
			}
			else if (E_level < 3 && E_MBP->box_4[t4i][t4j] == _STONE)
			{

			}
			else if (E_level < 3 && E_MBP->box_4[t4i][t4j] == _STONE)
			{

			}

		}
		break;
	case DIR_UP:
	case DIR_DOWN:
		for (int i = 0; i < 4; i++)
		{
			t4i = b4i + check[1][i][_CY];
			t4j = b4j + check[1][i][_CX];

			if (E_MBP->box_4[t4i][t4j] == _WALL)
			{
				E_MBP->box_4[t4i][t4j] = _CLEAR;
				E_MBP->clear[t4i][t4j] = _CLEAR;
			}
			else if (E_level < 3 && E_MBP->box_4[t4i][t4j] == _STONE)
			{

			}
			else if (E_level < 3 && E_MBP->box_4[t4i][t4j] == _STONE)
			{

			}

		}
		break;
	default:
		break;
	}
}

void EnemyClass::E_adjustdir()
{
	E_step = rand() % 250;

	//E_ajustlocation();
	if (E_dir == DIR_LEFT || E_dir == DIR_RIGHT)
		if (E_X > (E_X / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 - 1)
			E_X = (E_X / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			E_X = (E_X / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;
	else
		if (E_Y > (E_Y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 - 1)
			E_Y = (E_Y / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			E_Y = (E_Y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;

	if (E_dir == DIR_LEFT || E_dir == DIR_RIGHT)
	{
		if (rand() % 100 > 70)
			E_dir = DIR_DOWN;
		else
			E_dir = rand() % 4;
	}
	else
		E_dir = rand() % 4;
}

bool EnemyClass::E_moveable()
{
	int tempx = E_X + mDPXY[E_dir][_CX];
	int tempy = E_Y + mDPXY[E_dir][_CY];

	//边界检测
	if (tempx<BOX_SIZE || tempy<BOX_SIZE || tempx>CENTER_WIDTH - BOX_SIZE || tempy>CENTER_HEIGHT - BOX_SIZE)
	{
		switch (E_dir)
		{
		case DIR_LEFT:	E_X = E_X / BOX_SIZE * BOX_SIZE; break;
		case DIR_UP:	E_Y = E_Y / BOX_SIZE * BOX_SIZE; break;
		case DIR_RIGHT:	E_X = E_X / BOX_SIZE * BOX_SIZE; break;
		case DIR_DOWN:	E_Y = E_Y / BOX_SIZE * BOX_SIZE; break;
		default:break;
		}
		return false;
	}

	//障碍物检测
	int xi = tempx / SMALL_BOX_SIZE;
	int yj = tempy / SMALL_BOX_SIZE;

	//四个方向需要检测的4个4*4的格子相对坦克中心的box_4下标移量
	int dev_4[4][4][2] = { {{-2,-2},{-2,-1},{-2,0},{-2,1}},		//注意边界判断,左右与上下不同
							{{1,-2},{0,-2},	{-1,-2},{-2,-2}},
							{{2,1},	{2,0},	{2,-1},	{2,-2}},
							{{-2,2},{-1,2},	{0, 2},	{1,2}} };
	bool temp1, temp2, temp3, temp4, t1, t2, t3, t4;
	temp1 = E_MBP->box_4[yj + dev_4[E_dir][0][_CY]][xi + dev_4[E_dir][0][_CX]] < _WALL;
	temp2 = E_MBP->box_4[yj + dev_4[E_dir][1][_CY]][xi + dev_4[E_dir][1][_CX]] < _WALL;
	temp3 = E_MBP->box_4[yj + dev_4[E_dir][2][_CY]][xi + dev_4[E_dir][2][_CX]] < _WALL;
	temp4 = E_MBP->box_4[yj + dev_4[E_dir][3][_CY]][xi + dev_4[E_dir][3][_CX]] < _WALL;
	t1 = E_MBP->box_4[yj + dev_4[E_dir][0][_CY]][xi + dev_4[E_dir][0][_CX]] >= PLAYER_SIGN;
	t2 = E_MBP->box_4[yj + dev_4[E_dir][1][_CY]][xi + dev_4[E_dir][1][_CX]] >= PLAYER_SIGN;
	t3 = E_MBP->box_4[yj + dev_4[E_dir][2][_CY]][xi + dev_4[E_dir][2][_CX]] >= PLAYER_SIGN;
	t4 = E_MBP->box_4[yj + dev_4[E_dir][3][_CY]][xi + dev_4[E_dir][3][_CX]] >= PLAYER_SIGN;
	if (!temp1 || !temp2 || !temp3 || !temp4)
	{
		switch (E_dir)
		{
		case DIR_LEFT:	E_X = E_X / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;	//注意边界判断,左上与右下不同
		case DIR_UP:	E_Y = E_Y / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		case DIR_RIGHT:	E_X = tempx / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		case DIR_DOWN:	E_Y = tempy / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		default:break;
		}
		return false;
	}
	else if (t1 || t2 || t3 || t4)		//坦克相撞不用调整位置
		return false;

	return true;;
}


//------------------------------------------------------------

PropTank::PropTank(int level, MarkBox * mbp):
	EnemyClass(TankType::Prop,level,mbp)
{
	char c[50];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 2; j++)
		{
			_stprintf_s(c, "./res/big/gray-tank/%d-%d-%d.gif", level + 1, i + 1, j + 1);
			loadimage(&mTankImage[0][i][j], c);
			_stprintf_s(c, "./res/big/red-tank/%d-%d-%d.gif", level + 1, i + 1, j + 1);
			loadimage(&mTankImage[1][i][j], c);
		}

}

void PropTank::DrawTank(const HDC & canvas)
{
	if (E_isdead)
		return;
	if (mSpawn.IsSpawning())
	{
		if (!mSpawn.CanSpawn())
			CheckSpawn();
		else
			mSpawn.Spawnig(canvas);
	}
	else
	{
		E_ImageIndex = (E_ImageIndex + 1) % 2;
		E_ColorIndex = (E_ColorIndex + 1) % 12;
		TransparentBlt(canvas, E_X - BOX_SIZE, E_Y - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mTankImage[E_ColorIndex > 5 ? 0 : 1][E_dir][E_ImageIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	}
}

BossTank::BossTank(TankType type, MarkBox * mbp):
	EnemyClass(type, 3, mbp)
{
	hp = 3;

	char c[50];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 2; j++)
		{
			_stprintf_s(c, "./res/big/gray-tank/%d-%d-%d.gif", 4, i + 1, j + 1);
			loadimage(&mTankImage[0][i][j], c);
			_stprintf_s(c, "./res/big/red-tank/%d-%d-%d.gif", 4, i + 1, j + 1);
			loadimage(&mTankImage[1][i][j], c);
			_stprintf_s(c, "./res/big/other-tank/y%d-%d-%d.gif", 4, i + 1, j + 1);
			loadimage(&mTankImage[2][i][j], c);
			_stprintf_s(c, "./res/big/other-tank/g%d-%d-%d.gif", 4, i + 1, j + 1);
			loadimage(&mTankImage[3][i][j], c);
		}
}

void BossTank::DrawTank(const HDC & canvas)
{
	if (E_isdead)
		return;
	if (mSpawn.IsSpawning())
	{
		if (!mSpawn.CanSpawn())
			CheckSpawn();
		else
			mSpawn.Spawnig(canvas);
	}
	else {
		E_ImageIndex = (E_ImageIndex + 1) % 2;
		E_ColorIndex = (E_ColorIndex + 1) % 12;
		IMAGE temp[2][4][2];
		switch (E_type)
		{
		case TankType::Common:
			switch (hp)
			{
			case 3:GetTempImage(temp, GREEN_TANK, GRAY_TANK); break;
			case 2:GetTempImage(temp, GRAY_TANK, YELLOW_TANK); break;
			case 1:GetTempImage(temp, YELLOW_TANK, GREEN_TANK); break;
			case 0:GetTempImage(temp, GRAY_TANK, GRAY_TANK); break;
			default:break;
			}
			break;
		case TankType::Prop:
			switch (hp)
			{
			case 3:GetTempImage(temp, RED_TANK, GRAY_TANK); break;
			case 2:GetTempImage(temp, RED_TANK, YELLOW_TANK); break;
			case 1:GetTempImage(temp, YELLOW_TANK, GRAY_TANK); break;
			case 0:GetTempImage(temp, RED_TANK, GRAY_TANK); break;
			default:break;
			}
			break;
		default:
			break;
		}
		TransparentBlt(canvas, E_X - BOX_SIZE, E_Y - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&temp[E_ColorIndex > 5 ? 0 : 1][E_dir][E_ImageIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	}
}

bool BossTank::E_beshot(bool kill)
{
	if (E_isdead)
		return false;
	PlayMusic(S_BIN);
	if (kill || hp == 0)
	{
		E_isdead = true;
		mBlast.SetBlast(E_X, E_Y);
		E_setbox_4(_EMPTY);
		return true;
	}
	hp--;
	return false;
}


CommonTank::CommonTank(int level, MarkBox * mbp) :
	EnemyClass(TankType::Common, level, mbp)
{
	char c[50];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 2; j++)
		{
			_stprintf_s(c, "./res/big/gray-tank/%d-%d-%d.gif", level + 1, i + 1, j + 1);
			loadimage(&mTankImage[i][j], c);
		}
}

void CommonTank::DrawTank(const HDC & canvas)
{
	if (E_isdead)
		return;
	if (mSpawn.IsSpawning())
	{
		if (!mSpawn.CanSpawn())
			CheckSpawn();
		else
			mSpawn.Spawnig(canvas);
	}
	else
	{
		E_ImageIndex = (E_ImageIndex + 1) % 2;
		TransparentBlt(canvas, E_X - BOX_SIZE, E_Y - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mTankImage[E_dir][E_ImageIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	}
}
