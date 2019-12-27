#include "PlayerClass.h"

int PlayerClass::mDPXY[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };
MarkBox* PlayerClass::mbp = NULL;
int PlayerClass::mTankMoveDrt[4] = { 21,19,17,15 };
int PlayerClass::mBulletMoveDrt[4] = { 20, 19, 18, 17 };

PlayerClass::PlayerClass(int player, MarkBox* b)
{
	mbp = b;
	mPlayerID = player;
	mLife = 3;
	mTankLevel = 0;
	mDead = false;

	//mScore.Init(player);
	Init();

	//加载图片
	char c[50];
	for(int k=0;k<4;k++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 2; j++)
			{
				_stprintf_s(c, "./res/big/%dPlayer/m%d-%d-%d.gif", player, k, i, j + 1);
				loadimage(&mTankImage[k][i][j], c);
			}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
		{
			_stprintf_s(c, "./res/big/bullet-%d.gif", j);
			loadimage(&mBullet[i].mBulletImage[j], c);
		}
	for (int i = 0; i < 3; i++)
	{
		_stprintf_s(c, "./res/big/bumb%d.gif", i);
		loadimage(&Bomb::mBombImage[i], c);
	}
	if (mPlayerID == 0)
	{
		loadimage(&m12pImage, "./res/big/1P.gif");
		mTankIconX = 232;
		mTankIconY=137;
		m12pImgX = 233;
		m12pImgY = 129;
		mLifeImgX = 240;
		mLifeImgY = 137;
	}
	else
	{
		loadimage(&m12pImage, "./res/big/2P.gif");
		mTankIconX = 232;
		mTankIconY = 161;
		m12pImgX = 233;
		m12pImgY = 153;
		mLifeImgX = 240;
		mLifeImgY = 161;
	}
	loadimage(&mNumber, "./res/big/black-number.gif");
	loadimage(&mTankIcon, "./res/big/playertank-ico.gif");
}

void PlayerClass::Init()
{
	mRing.Init();
	mSpawn.Init();
	mBlast.Init();
	mMoveCounter = 0;
	mTankDir = DIR_UP;
	mBulletShootCount = 6;
	mMoving = false;
	mOnIce=false;
	mSliding=false;
	mSlideCounter=0;
	mSlideStep = rand() % 20 + 80;

	for (int i = 0; i < 2; i++)
	{
		mBomb[i].cx = 0;
		mBomb[i].cy = 0;
		mBomb[i].bombflag = false;
		mBomb[i].counter = 0;
	}
	int temp[4] = { 2,3,3,4 };
	for (int i = 0; i < 2; i++)
	{
		mBullet[i].isShot = false;
		mBullet[i].killID = 0;
		mBullet[i].cx = 0;
		mBullet[i].cy = 0;
		mBullet[i].dir = DIR_UP;
		for (int j = 0; j < 4; j++)
			mBullet[i].speed[j] = temp[j];
	}
	if (mPlayerID == 0)
	{
		mTankX = 4 * 16 + BOX_SIZE;						//坦克首次出现时候的中心坐标
		mTankY = 12 * 16 + BOX_SIZE;

		mTankTimer.SetDeltaT(mTankMoveDrt[mTankLevel]);			//根据级别调整坦克移动速度
		mBulletTimer.SetDeltaT(mBulletMoveDrt[mTankLevel]);		//调整子弹速度
	}
	else
	{
		mTankX = 8 * 16 + BOX_SIZE;
		mTankY = 12 * 16 + BOX_SIZE;

		mTankTimer.SetDeltaT(mTankMoveDrt[mTankLevel]);
		mBulletTimer.SetDeltaT(mBulletMoveDrt[mTankLevel]);
	}
	mSpawn.SetSpawn(mTankX, mTankY);
	mSpawn.SetFlag();
	FillBox_4(mTankX, mTankY, PLAYER_SIGN + mPlayerID);		//bug??? 初始化无碰撞箱 -----main()中调用顺序问题
}

void PlayerClass::DrawTank(const HDC & canvas)
{
	if (mDead || mBlast.IsBlasting())
		return;
	if (mSpawn.IsSpawning())
	{
		if(!mSpawn.CanSpawn())
			CheckSpawn();
		else
			mSpawn.Spawnig(canvas);
	}
	else
	{
		mMoveCounter = mMoving ? (mMoveCounter + 1) % 2 : mMoveCounter;
		TransparentBlt(canvas, mTankX - BOX_SIZE, mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mTankImage[mTankLevel][mTankDir][mMoveCounter]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		if (mRing.IsRinging())
			mRing.ShowRing(canvas, mTankX, mTankY);
	}
}

void PlayerClass::DrawBullet(const HDC & canvas)
{
	int ix, iy, dir;
	for (int i = 0; i < 2; i++)
		if(mBullet[i].isShot)
		{
			dir = mBullet[i].dir;
			ix = mBullet[i].cx + Bullet::cidevXY[dir][_CX];
			iy = mBullet[i].cy + Bullet::cidevXY[dir][_CY];
			IMAGE bullet = mBullet[i].mBulletImage[dir];
			TransparentBlt(canvas, ix, iy, bullet.getwidth(), bullet.getheight(), 
				GetImageHDC(&bullet),0, 0, bullet.getwidth(), bullet.getheight(), 0x000000);
		}
}

void PlayerClass::DrawData(const HDC & canvas)
{
	//1p/2p图标
	TransparentBlt(canvas, m12pImgX, m12pImgY, PLAYER_12P_ICO_SIZE_X, PLAYER_12P_ICO_SIZE_Y,
		GetImageHDC(&m12pImage), 0, 0, PLAYER_12P_ICO_SIZE_X, PLAYER_12P_ICO_SIZE_Y, 0xffffff);
	// 坦克图标
	TransparentBlt(canvas, mTankIconX, mTankIconY, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y,
		GetImageHDC(&mTankIcon), 0, 0, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y, 0x00000);
	// 剩余生命
	TransparentBlt(canvas, mLifeImgX, mLifeImgY, NUMBER_SIZE, NUMBER_SIZE,
		GetImageHDC(&mNumber), NUMBER_SIZE * mLife, 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
}

void PlayerClass::Move(int new_dir)
{
	if (!mTankTimer.IsTimeOut())
		return;

	FillBox_4(mTankX, mTankY, _EMPTY);
	if (mTankDir != new_dir)
	{
		//转向前调整位置,对齐box_4碰撞箱
		/*if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
			if (mTankX > (mTankX / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 - 1)
				mTankX = (mTankX / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
			else
				mTankX = (mTankX / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;
		else
			if (mTankY > (mTankY / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 - 1)
				mTankY = (mTankY / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
			else
				mTankY = (mTankY / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;*/
		if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
			mTankX = (mTankX + 1) / SMALL_BOX_SIZE * SMALL_BOX_SIZE;
		else
			mTankY = (mTankY + 1) / SMALL_BOX_SIZE * SMALL_BOX_SIZE;
		
		//FillClearBox(mTankX, mTankY, _CLEAR);
		mTankDir = new_dir;
	}
	else if(CheckMoveable())
	{
		mTankX += mDPXY[mTankDir][_CX];
		mTankY += mDPXY[mTankDir][_CY];
	}
	
	FillBox_4(mTankX, mTankY, PLAYER_SIGN + mPlayerID);
}

bool PlayerClass::CheckMoveable()
{
	int tempx = mTankX + mDPXY[mTankDir][_CX];
	int tempy = mTankY + mDPXY[mTankDir][_CY];

	//边界检测
	if (tempx<BOX_SIZE || tempy<BOX_SIZE || tempx>CENTER_WIDTH - BOX_SIZE || tempy>CENTER_HEIGHT - BOX_SIZE)
	{
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = mTankX / BOX_SIZE * BOX_SIZE; break;
		case DIR_UP:	mTankY = mTankY / BOX_SIZE * BOX_SIZE; break;
		case DIR_RIGHT:	mTankX = mTankX / BOX_SIZE * BOX_SIZE; break;
		case DIR_DOWN:	mTankY = mTankY / BOX_SIZE * BOX_SIZE; break;
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
	int check1, check2, check3, check4;
	temp1 = (check1 = mbp->box_4[yj + dev_4[mTankDir][0][_CY]][xi + dev_4[mTankDir][0][_CX]]) < _WALL;
	temp2 = (check2 = mbp->box_4[yj + dev_4[mTankDir][1][_CY]][xi + dev_4[mTankDir][1][_CX]]) < _WALL;
	temp3 = (check3 = mbp->box_4[yj + dev_4[mTankDir][2][_CY]][xi + dev_4[mTankDir][2][_CX]]) < _WALL;
	temp4 = (check4 = mbp->box_4[yj + dev_4[mTankDir][3][_CY]][xi + dev_4[mTankDir][3][_CX]]) < _WALL;
	t1 = mbp->box_4[yj + dev_4[mTankDir][0][_CY]][xi + dev_4[mTankDir][0][_CX]] >= PLAYER_SIGN;
	t2 = mbp->box_4[yj + dev_4[mTankDir][1][_CY]][xi + dev_4[mTankDir][1][_CX]] >= PLAYER_SIGN;
	t3 = mbp->box_4[yj + dev_4[mTankDir][2][_CY]][xi + dev_4[mTankDir][2][_CX]] >= PLAYER_SIGN;
	t4 = mbp->box_4[yj + dev_4[mTankDir][3][_CY]][xi + dev_4[mTankDir][3][_CX]] >= PLAYER_SIGN;
	if (!temp1 || !temp2 || !temp3 || !temp4)
	{
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = mTankX / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;	//注意边界判断,左上与右下不同
		case DIR_UP:	mTankY = mTankY / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		case DIR_RIGHT:	mTankX = tempx / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		case DIR_DOWN:	mTankY = tempy / SMALL_BOX_SIZE * SMALL_BOX_SIZE; break;
		default:break;
		}
		/*std::cout << '(' << xi << ',' << yj << ')' << std::endl;
		std::cout << '(' << mTankX << ',' << mTankY << ')' << std::endl;
		std::cout << temp1 << ' ' << temp2 << ' ' << temp3 << ' ' << temp4 << ' ' << std::endl;*/
		return false;
	}
	else if (t1 || t2 || t3 || t4)		//坦克相撞不用调整位置
		return false;

	if (!mOnIce && (check1 == _ICE || check2 == _ICE || check3 == _ICE || check4 == _ICE))
		mOnIce = true;
	if (mOnIce && check1 != _ICE && check2 != _ICE && check3 != _ICE && check4 != _ICE)
		mOnIce = false;
	
	return true;
}

void PlayerClass::PlayerControl()
{
	if (mDead||mBlast.IsBlasting()||mSpawn.IsSpawning())
		return;

	if (mSliding)
	{
		if (mSlideCounter < mSlideStep)
			Move(mTankDir);
		else
		{
			mSlideCounter = 0;
			mSliding = false;
		}
		mSlideCounter++;
	}

	switch (mPlayerID)
	{
	case 0:
		//坦克移动
		if (GetAsyncKeyState('W') & 0x8000)
		{
			CheckSliding(DIR_UP);
			mMoving = true;
			Move(DIR_UP);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			CheckSliding(DIR_DOWN);
			mMoving = true;
			Move(DIR_DOWN);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			CheckSliding(DIR_LEFT);
			mMoving = true;
			Move(DIR_LEFT);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			CheckSliding(DIR_RIGHT);
			mMoving = true;
			Move(DIR_RIGHT);
		}
		else if (mMoving)
		{
			mMoving = false;
		}

		//子弹发射
		if (GetAsyncKeyState('F') & 0x8000)
		{

			if (!ShootBullet(0))
			{
				if (ShootBullet(1))
					PlayMusic(S_SHOOT1);
			}
			else
				PlayMusic(S_SHOOT0);
		}
		break;
	case 1:
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			CheckSliding(DIR_UP);
			mMoving = true;
			Move(DIR_UP);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			CheckSliding(DIR_DOWN);
			mMoving = true;
			Move(DIR_DOWN);
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{ 
			CheckSliding(DIR_LEFT);
			mMoving = true;
			Move(DIR_LEFT);
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			CheckSliding(DIR_RIGHT);
			mMoving = true;
			Move(DIR_RIGHT);
		}
		else if (mMoving)
		{
			mMoving = false;
		}

		if (GetAsyncKeyState('J') & 0x8000)
		{
			if (!ShootBullet(0))
			{
				if (ShootBullet(1))
					PlayMusic(S_SHOOT3);
			}
			else
				PlayMusic(S_SHOOT2);
		}
		break;
	default:
		break;
	}

}

void PlayerClass::ShowBox()
{
	system("cls");
	int i, j;
	for (i = 0; i < 52; i++)
	{
		for (j = 0; j < 52; j++)
			std::cout << mbp->box_4[i][j]<<"\t\t";
		std::cout << std::endl << std::endl;
	}
}

bool PlayerClass::ShootBullet(int id)
{
	switch (id)
	{
	case 0:
		if (mBullet[0].isShot||mBomb[id].bombflag)
			return false;
		mBullet[0].dir = mTankDir;
		mBullet[0].isShot = true;
		mBullet[0].cx = mTankX + Bullet::cdevXY[mTankDir][_CX];
		mBullet[0].cy = mTankY + Bullet::cdevXY[mTankDir][_CY];
		/*mBullet[0].ix = mBullet[0].cx + Bullet::cidevXY[mTankDir][_CX];
		mBullet[0].iy = mBullet[0].cy + Bullet::cidevXY[mTankDir][_CY];*/
		//std::cout << "Bullet[" << 0 << "] is shot!\n";
		mBulletShootCount = 25;
		SignBullet(mBullet[0].cx, mBullet[0].cy, mBullet[0].dir, PB_SIGN + 10 * mPlayerID + id);
		return true;
	case 1:
		if (mBullet[1].isShot || mTankLevel < 2 || mBomb[id].bombflag||mBulletShootCount>0)
			return false;
		mBullet[1].dir = mTankDir;
		mBullet[1].isShot = true;
		mBullet[1].cx = mTankX + Bullet::cdevXY[mTankDir][_CX];
		mBullet[1].cy = mTankY + Bullet::cdevXY[mTankDir][_CY];
		/*mBullet[1].ix = mBullet[1].cx + Bullet::cidevXY[mTankDir][_CX];
		mBullet[1].iy = mBullet[1].cy + Bullet::cidevXY[mTankDir][_CY];*/
		//std::cout << "Bullet[" << 1 << "] is shot!\n";
		SignBullet(mBullet[1].cx, mBullet[1].cy, mBullet[1].dir, PB_SIGN + 10 * mPlayerID + id);
		return true;
		break;
	}
	return false;
}

int PlayerClass::BulletMoving()
{
	if (!mBulletTimer.IsTimeOut())
		return BulletShootKind::None;

	/*if (mBulletShootCount > 0)
		mBulletShootCount--;*/
	for (int i = 0; i < 2; i++)
		if (mBullet[i].isShot)
		{
			int kind = CheckShot(i);
			if (kind == BulletShootKind::Other)
				continue;
			else if (kind >= ENEMY_SIGN || kind == BulletShootKind::Camp)
				return kind;

			int dir = mBullet[i].dir;
			SignBullet(mBullet[i].cx, mBullet[i].cy, dir, _EMPTY);
			mBullet[i].cx += mDPXY[dir][_CX] * mBullet[i].speed[mTankLevel];
			mBullet[i].cy += mDPXY[dir][_CY] * mBullet[i].speed[mTankLevel];
			/*mBullet[i].ix = mBullet[i].cx + Bullet::cidevXY[mBullet[i].dir][_CX];
			mBullet[i].iy = mBullet[i].cy + Bullet::cidevXY[mBullet[i].dir][_CY];*/
			
			if (i == 0)
				mBulletShootCount--;
			SignBullet(mBullet[i].cx, mBullet[i].cy, dir, PB_SIGN + mPlayerID * 10 + i);
		}
	return BulletShootKind::None;
}

void PlayerClass::SignBullet(int cx, int cy, int dir, int val)
{
	// 转换弹头坐标
	/*int hx = x + Bullet::cidevXY[dir][_CX];
	int hy = y + Bullet::cidevXY[dir][_CY];*/

	// 转换成 4*4 格子下标索引
	int b4i = cy / SMALL_BOX_SIZE;
	int b4j = cx / SMALL_BOX_SIZE;
	if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
		return;

	mbp->bullet[b4i][b4j] = val;
}

void PlayerClass::ClearBarrier(int bullet_id, int cx, int cy)
{
	int t4i, t4j;
	int b4i = cy / SMALL_BOX_SIZE;
	int b4j = cx / SMALL_BOX_SIZE;
	int check[2][4][2] = { { {0,-2},{0,-1},{0,0},{0,1} },{ {-1,0},{0,0},{1,0},{2,0} } };	//不能放入case中,报错
	switch (mBullet[bullet_id].dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
		for (int i = 0; i < 4; i++)
		{
			t4i = b4i + check[0][i][_CY];
			t4j = b4j + check[0][i][_CX];

			if(mbp->box_4[t4i][t4j]==_WALL)
			{ 
				mbp->box_4[t4i][t4j] = _CLEAR;
				mbp->clear[t4i][t4j] = _CLEAR;
			}
			else if (mTankLevel < 3 && mbp->box_4[t4i][t4j] == _STONE)
			{
				PlayMusic(S_BIN);
			}
			else if (mTankLevel == 3 && mbp->box_4[t4i][t4j] == _STONE)
			{
				mbp->box_4[t4i][t4j] = _CLEAR;
				mbp->clear[t4i][t4j] = _CLEAR;
			}

		}
		break;
	case DIR_UP:
	case DIR_DOWN:
		for (int i = 0; i < 4; i++)
		{
			t4i = b4i + check[1][i][_CY];
			t4j = b4j + check[1][i][_CX];

			if (mbp->box_4[t4i][t4j] == _WALL)
			{
				mbp->box_4[t4i][t4j] = _CLEAR;
				mbp->clear[t4i][t4j] = _CLEAR;
			}
			else if (mTankLevel < 3 && mbp->box_4[t4i][t4j] == _STONE)
			{
				PlayMusic(S_BIN);
			}
			else if (mTankLevel == 3 && mbp->box_4[t4i][t4j] == _STONE)
			{
				mbp->box_4[t4i][t4j] = _CLEAR;
				mbp->clear[t4i][t4j] = _CLEAR;
			}

		}
		break;
	default:
		break;
	}

}

int PlayerClass::CheckShot(int i)
{
	int cx = mBullet[i].cx;
	int cy = mBullet[i].cy;
	int dir = mBullet[i].dir;
	if (cx <= 0 || cy <= 0 || cx >= CENTER_WIDTH - 1 || cy >= CENTER_HEIGHT - 1)
	{
		mBullet[i].isShot = false;
		mBomb[i].Boom(cx, cy, dir);
		SignBullet(cx, cy, dir, _EMPTY);
		PlayMusic(S_BIN);
		return BulletShootKind::Other;
	}

	int tempi, tempj;
	int b4i = cy / SMALL_BOX_SIZE;
	int b4j = cx / SMALL_BOX_SIZE;
	int b8i = cy / BOX_SIZE;
	int b8j = cx / BOX_SIZE;

	if (mbp->bullet[b4i][b4j] == PB_SIGN + (1 - mPlayerID) * 10 + 0 ||
		mbp->bullet[b4i][b4j] == PB_SIGN + (1 - mPlayerID) * 10 + 1 ||
		mbp->bullet[b4i][b4j] == EB_SIGN)
	{
		mBullet[i].isShot = false;
		mbp->bullet[b4i][b4j] = BD_SIGN;
		mBomb[i].Boom(cx, cy, dir);
		return BulletShootKind::Other;
	}
	else if (mbp->bullet[b4i][b4j] == BD_SIGN)
	{
		mBullet[i].isShot = false;
		mbp->bullet[b4i][b4j] = _EMPTY;
		mBomb[i].Boom(cx, cy, dir);
		return BulletShootKind::Other;
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

			if (mbp->box_4[tempi][tempj] == CAMP_SIGN)
			{
				mBullet[i].isShot = false;
				mBomb[i].Boom(cx, cy, dir);
				FillBox_4(13 * BOX_SIZE, 25 * BOX_SIZE, _CLEAR);
				return BulletShootKind::Camp;
			}
			else if (mbp->box_4[tempi][tempj] == _WALL || mbp->box_4[tempi][tempj] == _STONE)
			{
				mBullet[i].isShot = false;
				ClearBarrier(i, cx, cy);
				mBomb[i].Boom(cx, cy, dir);
				return BulletShootKind::Other;
			}
			else if (mbp->box_4[tempi][tempj] >= ENEMY_SIGN)
			{
				mBullet[i].isShot = false;
				mBomb[i].Boom(cx, cy, dir);
				//mScore.AddKill(mbp->box_4[tempi][tempj] / 1000 - 10);
				return mbp->box_4[tempi][tempj];
				//return BulletShootKind::Other;
			}
			else if (mbp->box_4[tempi][tempj] == PLAYER_SIGN && mPlayerID != 0 || mbp->box_4[tempi][tempj] == PLAYER_SIGN + 1 && mPlayerID != 1)
			{

				return (BulletShootKind)mbp->box_4[tempi][tempj];
			}
		}
		break;
	case DIR_UP:
	case DIR_DOWN:
		for (int k = 0; k < 2; k++)
		{
			tempi = b4i + check[1][k][0];
			tempj = b4j + check[1][k][1];

			if (mbp->box_4[tempi][tempj] == CAMP_SIGN)
			{
				mBullet[i].isShot = false;
				mBomb[i].Boom(cx, cy, dir);
				FillBox_4(13 * BOX_SIZE, 25 * BOX_SIZE, _CLEAR);
				return BulletShootKind::Camp;
			}
			else if (mbp->box_4[tempi][tempj] == _WALL || mbp->box_4[tempi][tempj] == _STONE)
			{
				mBullet[i].isShot = false;
				ClearBarrier(i, cx, cy);
				mBomb[i].Boom(cx, cy, dir);
				return BulletShootKind::Other;
			}
			else if (mbp->box_4[tempi][tempj] >= ENEMY_SIGN)
			{
				mBullet[i].isShot = false;
				mBomb[i].Boom(cx, cy, dir);
				//mScore.AddKill(mbp->box_4[tempi][tempj] / 1000 - 10);
				return mbp->box_4[tempi][tempj];
				//return BulletShootKind::Other;
			}
			else if (mbp->box_4[tempi][tempj] == PLAYER_SIGN && mPlayerID != 0 || mbp->box_4[tempi][tempj] == PLAYER_SIGN + 1 && mPlayerID != 1)
			{

				return (BulletShootKind)mbp->box_4[tempi][tempj];
			}
		}
		break;
	default:break;
	}
	return BulletShootKind::None;
}

void PlayerClass::ReSpawn()
{
	if (mPlayerID == 0)
	{
		mTankX = 4 * 16 + BOX_SIZE;	
		mTankY = 12 * 16 + BOX_SIZE;
	}
	else
	{
		mTankX = 8 * 16 + BOX_SIZE;
		mTankY = 12 * 16 + BOX_SIZE;
	}
	mSpawn.SetSpawn(mTankX, mTankY);
	//FillBox_4(mTankX, mTankY, PLAYER_SIGN + mPlayerID);
}

bool PlayerClass::CheckSpawn()
{
	if (mPlayerID == 0)
	{
		for (int i = 48; i < 52; i++)
			for (int j = 16; j < 20; j++)
				if (mbp->box_4[i][j] > _EMPTY && mbp->box_4[i][j] - 100 != mPlayerID)
					return false;
	}
	else
	{
		for (int i = 48; i < 52; i++)
			for (int j = 32; j < 36; j++)
				if (mbp->box_4[i][j] > _EMPTY && mbp->box_4[i][j] - 100 != mPlayerID)
					return false;
	}

	if (mbp->box_4[mTankY / SMALL_BOX_SIZE][mTankX / SMALL_BOX_SIZE] != PLAYER_SIGN + mPlayerID)
	{
		FillBox_4(mTankX, mTankY, PLAYER_SIGN + mPlayerID);
		mSpawn.SetFlag();
		mRing.SetRing();
	}
	return true;
}

inline void PlayerClass::CheckSliding(int dir)
{
	if (mOnIce && (mTankDir == dir))
	{
		mSliding = true;
		mSlideCounter = 0;
		mSlideStep = rand() % 20 + 80;
	}
}

void PlayerClass::Bombing(const HDC & center)
{
	int index[6] = { 0,1,1,2,2,1 };
	for (int i = 0; i < 2; i++)
		if (mBomb[i].bombflag)
		{
			TransparentBlt(center, mBomb[i].cx - BOX_SIZE, mBomb[i].cy - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&Bomb::mBombImage[index[mBomb[i].counter % 6]]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
			if (!(mBomb[i].counter = ++mBomb[i].counter % 6))
					mBomb[i].bombflag = false;
		}
}

void PlayerClass::GetShot()
{
	if (mDead||mSpawn.IsSpawning()||mRing.IsRinging())
		return;

	FillBox_4(mTankX, mTankY, _EMPTY);
	if(!mBlast.IsBlasting())
		mBlast.SetBlast(mTankX, mTankY);
	PlayMusic(S_PLAYER_BOMB);
	mLife--;
	if (mLife == 0)
		mDead = true;
	else
		ReSpawn();
}

void PlayerClass::Blasting(const HDC & center)
{
	mBlast.PlayerBlasting(center);
}

bool PlayerClass::IsDead()
{
	return mDead;
}

int PlayerClass::GetID()
{
	//std::cout << "GetID()!\n";
	return mPlayerID;
}

void PlayerClass::GetNewProp(PropType type)
{
	switch (type)
	{
	case PropType::Tank:
		PlayMusic(S_ADDLIFE);
		mLife =( (mLife+1) > 9 ? 9 : mLife + 1);
		break;
	case PropType::Star:
		mTankLevel =((mTankLevel+1)> 3 ? 3 : mTankLevel + 1);
		mTankTimer.SetDeltaT(mTankMoveDrt[mTankLevel]);
		mBulletTimer.SetDeltaT(mBulletMoveDrt[mTankLevel]);
		break;
	case PropType::Helmet:
		mRing.SetRing(RING_TIME);
		break;
	case PropType::Timer:
	case PropType::Grenade:
	case PropType::Shovel:
	case PropType::Nothing:
	default:break;
	}
}

/*bool PlayerClass::ShowScorePanel(const HDC & canvas)
{
	return mScore.ShowPanel(canvas);
}*/

IMAGE* PlayerClass::GetTankImage(int dir, int index = 0)
{

	return &mTankImage[mTankLevel][dir][index];
}

void PlayerClass::FillBox_4(int x, int y, int fval)
{
	//调整坦克中心到占据百分比最多的16个4*4的格子中心
	if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
		if (x > (x / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	//如果是靠近右边节点, 
			x = (x / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else 
			x = (x / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					//靠近格子线上的左边节点
	else
		if (y > (y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	//如果是靠近格子下边节点,
			y = (y / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			y = (y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					//靠近格子线上的上边节点

	//填充box_4
	int iy = y / SMALL_BOX_SIZE - 2;
	int jx = x / SMALL_BOX_SIZE - 2;
	for (int i = iy; i < iy + 4; i++)
		for (int j = jx; j < jx + 4; j++)
			mbp->box_4[i][j] = fval;
}

/*void PlayerClass::FillClearBox(int x, int y, int val)
{
	if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
		if (x > (x / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2) 
			x = (x / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			x = (x / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;	
	else
		if (y > (y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	
			y = (y / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			y = (y / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					

	int iy = y / SMALL_BOX_SIZE - 2;
	int jx = x / SMALL_BOX_SIZE - 2;
	for (int i = iy; i < iy + 4; i++)
		for (int j = jx; j < jx + 4; j++)
			mbp->clear[i][j] = val;
}*/
