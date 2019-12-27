#include "struct.h"

SelectPanel::SelectPanel(HDC des_hdc, HDC canvas_hdc)
{
	mWin_hdc = des_hdc;
	mCanvas_hdc = canvas_hdc;
	loadimage(&mSelect_player_image, "./res/big/select_player.gif");
	loadimage(&mTank[0], "./res/big/0Player/m0-2-1.gif");
	loadimage(&mTank[1], "./res/big/0Player/m0-2-2.gif");
	loadimage(&mBackground, "./res/big/bg_black.gif");
	mTankPos[0][0] = 60;
	mTankPos[1][0] = 60;
	mTankPos[2][0] = 60;
	mTankPos[0][1] = 123;
	mTankPos[1][1] = 139;
	mTankPos[2][1] = 156;
}

void SelectPanel::Init()
{
	mSelect_player_image_y = WIN_HEIGHT;
	mCounter = 0;
	mIndex = 0;
}

SelectResult SelectPanel::ShowPanel()
{
	cleardevice();
	Init();

	while (mSelect_player_image_y > 0)
	{
		mSelect_player_image_y -= 4;
		if (mSelect_player_image_y < 0)
			mSelect_player_image_y = 0;

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			mSelect_player_image_y = 0;

		Sleep(40);
		BitBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mSelect_player_image), 0, 0, SRCCOPY);
		StretchBlt(mWin_hdc, 0, mSelect_player_image_y, WIN_WIDTH, WIN_HEIGHT, mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
	}

	int keyctr = 0;		//防止读取按键过快
	while (true)
	{
		Sleep(20);
		mCounter = (mCounter + 1) % 2;
		keyctr--;

		BitBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mSelect_player_image), 0, 0, SRCCOPY);
		TransparentBlt(mCanvas_hdc, mTankPos[mIndex][_CX], mTankPos[mIndex][_CY], 16, 16,
			GetImageHDC(&mTank[mCounter]), 0, 0, 16, 16, 0x000000);
		StretchBlt(mWin_hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();

		if (GetAsyncKeyState(VK_DOWN) & 0x8000 && keyctr < 0)
		{
			keyctr = 6;
			mIndex = (mIndex + 1) % 3;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000 && keyctr < 0)
		{
			keyctr = 6;
			mIndex = mIndex - 1 >= 0 ? mIndex - 1 : 2;
		}
		else if (GetAsyncKeyState(VK_RETURN) & 0x8000 && keyctr < 0)
		{
			break;
		}
	}

	switch (mIndex)
	{
	case 0:
		return SinglePlayer;
	case 1:
		return DualPlayer;
	case 2:
	default:
		return Error;
	}

	return Error;
}

//---------------------------------------------------
int Bullet::cdevXY[4][2] = { {-8,0},{-1,-8},{7,0},{-1,7} };
int Bullet::cidevXY[4][2] = { {0,-1},{-1,0},{-3,-1},{-1,-3} };
int Bullet::bombdev[4][2] = { {1,1},{0,2},{0,1},{0,0} };

//---------------------------------------------------
IMAGE Bomb::mBombImage[3];

//---------------------------------------------------
IMAGE Blast::mBlastImage[5];
Blast::Blast()
{

	Timer.SetDeltaT(36);
	char c[50];
	for (int i = 0; i < 5; i++)
	{
		_stprintf_s(c, "./res/big/blast/%d.gif", i);
		loadimage(&mBlastImage[i], c);
	}
	Init();
}

void Blast::Init()
{
	cx = 0;
	cy = 0;
	blastflag = false;
	counter = 0;
}

void Blast::CampBlasting(const HDC & center)
{
	if (blastflag)
	{
		int index[17] = { 0,0,0,1,1,2,2,3,3,4,4,4,4,3,2,1,0 };
		TransparentBlt(center, cx- BOX_SIZE*2, cy-BOX_SIZE*2, BOX_SIZE * 4, BOX_SIZE * 4,
			GetImageHDC(&Blast::mBlastImage[index[counter % 17]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		if (Timer.IsTimeOut() && counter++ == 17)
		{
			blastflag = false;
			return;
		}
		return;
	}
	return;
}

void Blast::PlayerBlasting(const HDC& center)
{
	int index[13] = { 0,1,1,2,2,3,3,4,4,3,2,1,0 };
	if (blastflag)
	{
		TransparentBlt(center,cx - BOX_SIZE * 2, cy - BOX_SIZE * 2, BOX_SIZE * 4, BOX_SIZE * 4,
			GetImageHDC(&Blast::mBlastImage[index[counter % 13]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		if (Timer.IsTimeOut())
			if (counter++ >= 12)
			{
				Init();
				return;
			}
		return;
	}
	return;
}

void Blast::EnemyBlasting(const HDC &center, IMAGE* score)
{
	int index[13] = { 0,1,1,2,2,3,3,4,4,3,2,1,0 };
	if (blastflag)
	{
		if (counter < 13)
			TransparentBlt(center, cx - BOX_SIZE * 2, cy - BOX_SIZE * 2, BOX_SIZE * 4, BOX_SIZE * 4,
				GetImageHDC(&Blast::mBlastImage[index[counter % 13]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		else
			TransparentBlt(center, cx - 7, cy - 3, 14, 7,GetImageHDC(score), 0, 0, 14, 7, 0x000000);
		if (Timer.IsTimeOut())
			if (counter++ >= 30)
				Init();
	}
}

void Blast::SetBlast(int x, int y)
{
	cx = x;
	cy = y;
	blastflag = true;
}

bool Blast::IsBlasting()
{
	return blastflag;
}

//---------------------------------------------------
IMAGE Spawn::mStarImage[4];
Spawn::Spawn()
{
	char c[50];
	for (int i = 0; i < 4; i++)
	{
		_stprintf_s(c, "./res/big/star%d.gif", i);
		loadimage(&mStarImage[i], c);
	}

	Init();
}

void Spawn::Init()
{
	mImageIndex = 0;
	mImageCounter = 0;
	spawning = false;
	spawnflag = false;
}

void Spawn::Spawnig(const HDC & center)
{
	if (!spawning)
		return;
	mImageCounter++;
	if (mImageCounter / 3 % 2 == 0)
		mImageIndex++;
	else
		mImageIndex--;
	TransparentBlt(center, cx - BOX_SIZE, cy - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mStarImage[mImageIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);	
	if (mImageCounter == 50)
		Init();
}

void Spawn::SetSpawn(int x, int y)
{
	cx = x;
	cy = y;
	spawning = true;
}

bool Spawn::IsSpawning()
{
	return spawning;
}

void Spawn::SetFlag()
{
	spawnflag = true;
}

bool Spawn::CanSpawn()
{
	return spawnflag;
}

//---------------------------------------------------
IMAGE ScorePanel::yellow_number;
IMAGE ScorePanel::number;
IMAGE ScorePanel::bonus;
const int ScorePanel::scoreY[2][4][2] = { {{88,88},{112,112},{136,136},{160,160}},
							{{88,88},{112,112},{136,136},{160,160}} };


ScorePanel::ScorePanel()
{
	playernum = 0;
	stage = 1;
	loadimage(&yellow_number, "./res/big/yellow-number.gif");
	loadimage(&number, "./res/big/white-number.gif");
	loadimage(&bonus, "./res/big/scorepanel/bunds.gif");
	loadimage(&player[0], "./res/big/scorepanel/player-0.gif");
	loadimage(&pts[0], "./res/big/scorepanel/pts-0.gif");
	loadimage(&player[1], "./res/big/scorepanel/player-1.gif");
	loadimage(&pts[1], "./res/big/scorepanel/pts-1.gif");

	playerX[0] = 24;
	playerY[0] = 47;
	ptsX[0] = 25;
	ptsY[0] = 85;
	for (int i = 0; i < 4; i++)
	{
		scoreX[0][i][0] = 34;
		scoreX[0][i][1] = 103;
		//scoreY[0][i][0] = 88 + i * 24;
		//scoreY[0][i][1] = 88 + i * 24;
		totalkillX[0] = 103;
		totalkillY[0] = 178;
	}
	totalscoreX[0] = 78;
	totalscoreY[0] = 58;


	playerX[1] = 170;
	playerY[1] = 47;
	ptsX[1] = 137;
	ptsY[1] = 85;
	for (int i = 0; i < 4; i++)
	{
		scoreX[1][i][0] = 177;
		scoreX[1][i][1] = 154;
		//scoreY[1][i][0] = 88 + i * 24;
		//scoreY[1][i][1] = 88 + i * 24;
		totalkillX[1] = 154;
		totalkillY[1] = 178;
	}

	totalscoreX[1] = 224;
	totalscoreY[1] = 58;
	Init();
}

void ScorePanel::AddKill(int player_id,int enemy_level)
{
	kills[player_id][enemy_level]++;
	totalscore[player_id] += (enemy_level + 1) * 100;
	totalkill[player_id]++;
	/*std::cout <<player_id<< "kills now=";
	for (int i = 0; i < 4; i++)
		std::cout << kills[player_id][enemy_level]<<' ';
	std::cout << "\ntotal score=" << totalscore[player_id];
	std::cout << "\ntotal kill=" << totalkill[player_id]<<std::endl;*/
}

bool ScorePanel::ShowPanel(const HDC& win,const HDC & canvas)
{
	//背景
	for (int i = 0; i < playernum; i++)
	{
		BitBlt(canvas, playerX[i], playerY[i], player[i].getwidth(), player[i].getheight(), GetImageHDC(&player[i]), 0, 0, SRCCOPY);
		BitBlt(canvas, ptsX[i], ptsY[i], pts[i].getwidth(), pts[i].getheight(), GetImageHDC(&pts[i]), 0, 0, SRCCOPY);
	}

	//stage字样
	TransparentBlt(canvas, 154, 32, NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
		NUMBER_SIZE * (stage % 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0x000000);
	if (stage > 10)
		TransparentBlt(canvas, 154 - 8, 32, NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
			NUMBER_SIZE * (stage / 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0x000000);
	//total score
	for (int i = 0; i < playernum; i++)
	{
		BitBlt(canvas, totalscoreX[i], totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
			0, 0, SRCCOPY);
		if (totalscore[i] > 90)
		{
			BitBlt(canvas, totalscoreX[i]-8, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				0, 0, SRCCOPY);
			BitBlt(canvas, totalscoreX[i]-16, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				totalscore[i] % 1000 / 100 * NUMBER_SIZE, 0, SRCCOPY);
		}
		if (totalscore[i] > 900)
			BitBlt(canvas, totalscoreX[i]-24, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				totalscore[i] / 1000 * NUMBER_SIZE, 0, SRCCOPY);
	}
	StretchBlt(win, 0, 0, WIN_WIDTH, WIN_HEIGHT,canvas, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();	//前三个静态图像

	//各自分数
	int score, temp, digit;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k <= kills[0][i] || k <= kills[1][i];k++)
		{
			PlayMusic(S_SCOREPANEL_DI);
			for (int j = 0; j < playernum; j++)
			{
				
				BitBlt(canvas, scoreX[j][i][1], scoreY[j][i][1], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
					NUMBER_SIZE * (kills[j][i] > k ? k : kills[j][i]),0,SRCCOPY);

				score = (i + 1) * 100 * (kills[j][i] > k ? k : kills[j][i]);
				temp = score;
				digit = 1;
				while ((temp /= 10) > 0)
					digit++;
				//std::cout << "score=" << score << "\tdigit=" << digit << std::endl;

				switch(digit)
				{
				case 1:
					BitBlt(canvas, scoreX[j][i][0]+16, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						0, 0, SRCCOPY);
					break;
				case 2:break;
				case 3:
					BitBlt(canvas, scoreX[j][i][0], scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						score/100*NUMBER_SIZE, 0, SRCCOPY);
					BitBlt(canvas, scoreX[j][i][0] + 8, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						0, 0, SRCCOPY);
					BitBlt(canvas, scoreX[j][i][0] + 16, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						0, 0, SRCCOPY);
					break;
				case 4:
					BitBlt(canvas, scoreX[j][i][0] - 8, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						score / 1000 * NUMBER_SIZE, 0, SRCCOPY);
					BitBlt(canvas, scoreX[j][i][0], scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						score % 1000 / 100 * NUMBER_SIZE, 0, SRCCOPY);
					BitBlt(canvas, scoreX[j][i][0] + 8, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						0, 0, SRCCOPY);
					BitBlt(canvas, scoreX[j][i][0] + 16, scoreY[j][i][0], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
						0, 0, SRCCOPY);
					break;
				default:break;
				}
				//std::cout << "temp=" << k << std::endl;
			}

			StretchBlt(win, 0, 0, WIN_WIDTH, WIN_HEIGHT, canvas, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
			FlushBatchDraw();
			Sleep(200);
		}
	}

	for (int i = 0; i < playernum; i++)
	{
		if (totalkill[i] < 10)
			BitBlt(canvas, totalkillX[i], totalkillY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
				totalkill[i] * NUMBER_SIZE, 0, SRCCOPY);
		else
		{
			BitBlt(canvas, totalkillX[i], totalkillY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
				totalkill[i] % 10 * NUMBER_SIZE, 0, SRCCOPY);
			BitBlt(canvas, totalkillX[i]-8, totalkillY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&number),
				totalkill[i] / 10 * NUMBER_SIZE, 0, SRCCOPY);
		}
	}
	StretchBlt(win, 0, 0, WIN_WIDTH, WIN_HEIGHT, canvas, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();
	Sleep(500);

	if(playernum==2&&totalscore[0]>totalscore[1]&&totalscore[0]>1000)
	{ 
		totalscore[0] += 1000;
		PlayMusic(S_BONUS1000);
		TransparentBlt(canvas, 26, 190, 63, 15, GetImageHDC(&bonus), 0, 0, 63, 15, 0x000000);
	}
	else if (playernum == 2 && totalscore[1] > totalscore[0] && totalscore[1] > 1000)
	{
		totalscore[1] += 1000;
		PlayMusic(S_BONUS1000);
		TransparentBlt(canvas, 170, 190, 63, 15, GetImageHDC(&bonus), 0, 0, 63, 15, 0x000000);
	}
	//加上bonus后totalscore
	for (int i = 0; i < playernum; i++)
	{
		BitBlt(canvas, totalscoreX[i], totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
			0, 0, SRCCOPY);
		if (totalscore[i] > 90)
		{
			BitBlt(canvas, totalscoreX[i] - 8, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				0, 0, SRCCOPY);
			BitBlt(canvas, totalscoreX[i] - 16, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				totalscore[i] % 1000 / 100 * NUMBER_SIZE, 0, SRCCOPY);
		}
		if (totalscore[i] > 900)
			BitBlt(canvas, totalscoreX[i] - 24, totalscoreY[i], NUMBER_SIZE, NUMBER_SIZE, GetImageHDC(&yellow_number),
				totalscore[i] / 1000 * NUMBER_SIZE, 0, SRCCOPY);
	}
	StretchBlt(win, 0, 0, WIN_WIDTH, WIN_HEIGHT, canvas, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	std::system("pause");

	return false;
}

void ScorePanel::GameMode(int mode,int stg)
{
	playernum = mode;
	stage = stg;
}

void ScorePanel::Init()
{
	totalscore[0] = totalscore[1] = 0;
	totalkill[0] = totalkill[1] = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 2; j++)
			kills[j][i] = 0;

}

void ScorePanel::Add500(int player_id)
{
	totalscore[player_id] += 500;
}

//---------------------------------------------------
PropClass::PropClass()
{
	Prop_Timer.SetDeltaT(13000);
	char c[50];
	for (int i = 0; i < 6; i++)
	{
		_stprintf_s(c, "./res/big/prop/p%d.gif", i);
		loadimage(&Prop_Image[i], c);
	}
	loadimage(&score, "./res/big/500.gif");

}
void PropClass::Prop_Sign(int type)
{
	int i = Prop_x / BOX_SIZE;
	int j = Prop_y / BOX_SIZE;
	for (int k = i; k < i + 2; k++)
		for (int l = j; l < j + 2; l++)
			Prop_MBP->prop[k][l] = type;
}
void PropClass::Init(MarkBox *mbp)
{
	Prop_MBP = mbp;
	Image_Counter = 0;
	ScoreCounter = 0;
	Prop_x = 0;
	Prop_y = 0;
	propflag = false;
}
void PropClass::Prop_Draw(const HDC &canvas)
{
	if (!propflag&&ScoreCounter < 0)
		return;

	if (propflag)
	{
		Image_Counter = Image_Counter < 70 ? Image_Counter + 1 : 0;
		if (Image_Counter / 35)
			TransparentBlt(canvas, Prop_x, Prop_y, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&Prop_Image[Prop_Type]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	}
	else if (ScoreCounter-- > 0)
	{
		TransparentBlt(canvas, Prop_x + 8, Prop_y + 3, 14, 7, GetImageHDC(&score), 0, 0, 14, 7, 0x000000);
	}

	if (Prop_Timer.IsTimeOut())
	{
		Image_Counter = 0;
		propflag = false;
		Prop_Sign(_EMPTY);
	}
}
void PropClass::Prop_Appear()
{
	
	//Prop_Sign(_EMPTY);				//先利用未更新的道具坐标将地图清空
	
	int x, y;
	bool is_in_camp,can_set;
	/*bool is_in_stone = (Prop_MBP->box_8[Prop_x][Prop_y] == _STONE || Prop_MBP->box_8[Prop_x + 1][Prop_y] == _STONE
		  || Prop_MBP->box_8[Prop_x][Prop_y + 1] == _STONE || Prop_MBP->box_8[Prop_x + 1][Prop_y + 1] == _STONE);*/
	for (int k = 0; k < 625; k++)
	{
		can_set = true;
		x = rand() % 25;
		y = rand() % 25;
		is_in_camp = (x >= 10 && x <= 14 && y >= 22 && y <= 25);
		for (int i = y; i < y + 2; i++)
			for (int j = x; j < x + 2; j++)
				if (Prop_MBP->box_8[i][j] >= _RIVER || is_in_camp)		// 保证道具可以获得(大概
					can_set=false;
		if (can_set)
			break;
	}

	Prop_x = x*BOX_SIZE;
	Prop_y = y * BOX_SIZE;
	propflag = true;
	Prop_Timer.Init();
	Prop_Type = PropType(rand() % 6);
	PlayMusic(S_PROPOUT);
	Prop_Sign(PROP_SIGN+Prop_Type);

}
PropType PropClass::Prop_Vanish(int player_id)
{
	if (!propflag)
		return PropType::Nothing;
		int i4y = Prop_y / SMALL_BOX_SIZE;
		int j4x = Prop_x / SMALL_BOX_SIZE;
		for (int j = j4x; j < j4x + 4; j++)
			for (int i = i4y; i < i4y + 4; i++)
				if (Prop_MBP->box_4[i][j] == PLAYER_SIGN + player_id)
				{
					ScoreCounter = 50;
					Image_Counter = 0;
					propflag = false;
					Prop_Sign(_EMPTY);
					return Prop_Type;
				}
	return PropType::Nothing;
}
//-------------------------------------------------
Ring::Ring()
{
	char c[50];
	for (int i = 0; i < 2; i++)
	{
		_stprintf_s(c, "./res/big/ring%d.gif", i);
		loadimage(&Ring::ring[i], c);
	}
	Init();
}

void Ring::Init()
{
	ringflag = false;
	timer.SetDeltaT(3500);
	ImageCounter = 0;
}

void Ring::ShowRing(const HDC & center, int cx, int cy)
{
	if (timer.IsTimeOut())
	{
		ImageCounter = 0;
		ringflag = false;
		return;
	}
	ImageCounter = ImageCounter > 4 ? 0 : ImageCounter + 1;
	TransparentBlt(center, cx - BOX_SIZE, cy - BOX_SIZE, BOX_SIZE * 2,
		BOX_SIZE * 2, GetImageHDC(&ring[ImageCounter/2]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

void Ring::SetRing(int time)
{
	ringflag = true;
	timer.SetDeltaT(time);
	timer.Init();
}

bool Ring::IsRinging()
{
	return ringflag;
}
