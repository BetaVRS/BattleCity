#include "GameControl.h"

int GameControl::mCurrentStage = 1;

GameControl::GameControl(HDC win, HDC canvas)
{
	mWin_hdc = win;
	mCanvas_hdc = canvas;
	mCenterImage.Resize(CENTER_WIDTH, CENTER_HEIGHT);
	mCenter_hdc = GetImageHDC(&mCenterImage);
	mMarkBox = new MarkBox();
	mMainTimer.SetDeltaT(14);
	mGameOverTimer.SetDeltaT(30);
	mAddEnemyTimer.SetDeltaT(1000);

	loadimage(&mBlackBackgroundImage, "./res/big/bg_black.gif");	// ��ɫ����
	loadimage(&mGrayBackgroundImage, "./res/big/bg_gray.gif");		// ��ɫ����
	loadimage(&mStoneImage, "./res/big/stone.gif");					// 12*12��ʯͷ
	loadimage(&mForestImage, "./res/big/forest.gif");				// ����
	loadimage(&mIceImage, "./res/big/ice.gif");						// ����
	loadimage(&mRiverImage[0], "./res/big/river-0.gif");			// ����
	loadimage(&mRiverImage[1], "./res/big/river-1.gif");			//
	loadimage(&mWallImage, "./res/big/wall.gif");					// ��ǽ
	loadimage(&mCamp[0], "./res/big/camp0.gif");					// ��Ӫ
	loadimage(&mCamp[1], "./res/big/camp1.gif");					// 
	loadimage(&mEnemyTankIcoImage, "./res/big/enemytank-ico.gif");	// �л�ͼ��
	loadimage(&mFlagImage, "./res/big/flag.gif");					// ����
	loadimage(&mStageImage, "./res/big/stage.gif");
	loadimage(&mNumberImage, "./res/big/black-number.gif");	// 0123456789 ��ɫ����
	loadimage(&mGameOverImage, "./res/big/gameover.gif");
	loadimage(&mScoreBackground, "./res/big/scorepanel/background.gif");

	Init();
}

void GameControl::Init()
{
	mRemainEnemy = TOTAL_ENEMY_NUM;
	mCurrentEnemy = 0;
	mKilledEnemy = 0;
	mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
	mGameOverY = CENTER_HEIGHT;
	mGameOverCounter = 0;
	mRiverCounter = 0;

	mWin = mFail = false;
	mCampBlast = false;
	mBlast.Init();
	mShowScorePanel = false;
	mScore.Init();
	mProp.Init(mMarkBox);
	mGetShov=false;
	mShovCounter=0;
}

void GameControl::LoadMap()
{
	ifstream fin;
	fin.open("./res/data/map.dat",ios_base::binary);
	if (!fin.is_open())
		throw "�ļ���ʧ��!";
	fin.seekg(sizeof(Map)*(mCurrentStage - 1));
	fin.read((char*)&mMap, sizeof(Map));
	fin.close();

	InitMarkBox();
}

void GameControl::GameLoop()
{
	GameResult result = GameResult::Victory;
	StartAnimation();
	while (result != GameResult::Fail)
	{
		result = GameFrame();
		//Sleep(1);
	}
}

void GameControl::Gamemode(int player_num)
{
	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
		delete *itor;
	mPlayerList.clear();

	for (int i = 0; i < player_num; i++)
		mPlayerList.push_back(new PlayerClass(i, mMarkBox));
}

GameResult GameControl::GameFrame()
{
	
	if (mMainTimer.IsTimeOut())		//ʱ�ӿ�����Ϸ��ͼ����,����ˢ�²��������
	{
		//�Ƿְ���ʾ,�غϽ���
		if (mShowScorePanel)
		{
			mScore.GameMode(mPlayerList.size(),mCurrentStage);
			BitBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mScoreBackground), 0, 0, SRCCOPY);
			mScore.ShowPanel(mWin_hdc,mCanvas_hdc);

			//�ж�ʤ�����,���list
			if (mWin)
			{
				Init();

				for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
					delete *EnemyItor;
				mEnemyList.clear();

				for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
					if (!(*itor)->IsDead())		//����player�޷�����
						(*itor)->Init();

				mCurrentStage++;	//�ؿ��ƽ�
				LoadMap();			//������һ�صĵ�ͼ
				StartAnimation();
				return GameResult::Victory;
			}
			else if (mFail)
			{
				Init();

				for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
					delete *EnemyItor;

				PlayMusic(S_FAIL);
				mEnemyList.clear();
				mCurrentStage = 1;
				//
				return GameResult::Fail;
			}
		}

		AddEnemy();
		
		//���²�����
		RefreshSide();

		//������Ϸ����
		RefreshCenter();

		//����Ϸ����ӡ����������
		BitBlt(mCanvas_hdc, CENTER_X, CENTER_Y, CENTER_WIDTH, CENTER_HEIGHT, mCenter_hdc, 0, 0, SRCCOPY);
		//���Ż���������ʾ��������
		StretchBlt(mWin_hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
		//ShowBox();		//debug��
	}
	RefreshData();
	return GameResult::Victory;
}

void GameControl::ShowBox()
{
	std::system("cls");
	int i, j;
	for (i = 0; i < 52; i++)
	{
		for (j = 0; j < 52; j++)
			std::cout << mMarkBox->bullet[i][j]/100 ;
		std::cout << std::endl;
	}
	Sleep(500);
}

void GameControl::StartAnimation()
{
	PlayMusic(S_START);

	StretchBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT,
		GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	while (mAnimCounter < 110)
	{
		Sleep(12);
		mAnimCounter += 3;
		StretchBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, mAnimCounter, GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
		StretchBlt(mCanvas_hdc, 0, CANVAS_HEIGHT - mAnimCounter, CANVAS_WIDTH, CANVAS_HEIGHT,
			GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);

		StretchBlt(mWin_hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
	}
	mAnimCounter = 0;

	StretchBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT,
		GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	TransparentBlt(mCanvas_hdc, 97, 103, 39, 7, GetImageHDC(&mStageImage), 0, 0, 39, 7, 0xffffff);

	if (mCurrentStage < 10)
		TransparentBlt(mCanvas_hdc, 157, 103, NUMBER_SIZE, NUMBER_SIZE,
			GetImageHDC(&mNumberImage), NUMBER_SIZE * mCurrentStage, 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
	else
	{
		TransparentBlt(mCanvas_hdc, 149, 103, NUMBER_SIZE, NUMBER_SIZE,
			GetImageHDC(&mNumberImage), NUMBER_SIZE * (mCurrentStage / 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);

		TransparentBlt(mCanvas_hdc, 157, 103, NUMBER_SIZE, NUMBER_SIZE,
			GetImageHDC(&mNumberImage), NUMBER_SIZE * (mCurrentStage % 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
	}
	StretchBlt(mWin_hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	Sleep(2000);
}

void GameControl::RefreshData()
{
	IsGetShovel();

	//������ݸ���
	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
	{
		//��Ϸ�������ֹͣ�ƶ�
		if(!mFail)
			(*itor)->PlayerControl();
		//����ӵ�����Ŀ��:enemy/camp
		int kind = (*itor)->BulletMoving();
		if (kind == BulletShootKind::Camp)
		{
			//std::cout << "Camp get shot!\n";
			PlayMusic(S_CAMP_BOMB);
			mFail = true;
			mCampBlast = true;
			mBlast.SetBlast(13 * BOX_SIZE, 25 * BOX_SIZE);
		}
		if (kind >= ENEMY_SIGN)
			if (KillEnemy(kind % 100))
			{
				PlayMusic(S_ENEMY_BOMB);
				mScore.AddKill((*itor)->GetID(), kind % 10000 / 1000);
				if (TankType::Prop == kind % 1000 / 100)
					mProp.Prop_Appear();//SetNewProp();
			}
		//������ʰȡ
		PropType type = mProp.Prop_Vanish((*itor)->GetID());
		if (type != PropType::Nothing)
		{
			PlayMusic(S_GETPROP);
			mScore.Add500((*itor)->GetID());
		}
		switch (type)
		{
		case PropType::Tank:
		case PropType::Star:
		case PropType::Helmet:
			(*itor)->GetNewProp(type);
			break;
		case PropType::Timer:
			EnemyClass::SetPause();
			break;
		case PropType::Grenade:
			PlayMusic(S_ENEMY_BOMB);
			for (list<EnemyClass*>::iterator  EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
			{		
				/*if (TankType::Prop == kind % 1000 / 100)			//����ը�������õ��ߣ�
					SetNewProp();*/
				if ((*EnemyItor)->E_beshot(true))
				{
					mKilledEnemy++;
					mCurrentEnemy--;
					mRemainEnemy--;
					mScore.AddKill((*itor)->GetID(), kind%10000/1000);
				}
			}
			break;
		case PropType::Shovel:
			SetShovel();
		case PropType::Nothing:break;
		default:break;
		}
	}

	//enemy���ݸ���
	for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
	{
		(*EnemyItor)->E_movement();
		//(*EnemyItor)->E_shoot();		//������E_movement()�е���
		BulletShootKind kind = (*EnemyItor)->E_bulletmoving();
		switch (kind)
		{
		case BulletShootKind::Player1:
		case BulletShootKind::Player2:
			//std::cout << "kind == Player == "<<kind<<std::endl;
			for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
				if ((*itor)->GetID() == ((int)kind % 100))
				{
					//std::cout << "Player" << kind / 100 << "->GetShot()!\n";
					(*itor)->GetShot();
				}
			break;
		case BulletShootKind::Camp:
			//std::cout << "Camp get shot!\n";
			PlayMusic(S_CAMP_BOMB);
			mFail = true;
			mCampBlast = true;
			mBlast.SetBlast(13 * BOX_SIZE, 25 * BOX_SIZE);
			for (int i = 12; i < 14; i++)
				for (int j = 24; j < 26; j++)
					FillBox_4(j, i, _CLEAR);
			break;
		default:break;
		}
	}

	
}

void GameControl::RefreshCenter()
{
	BitBlt(mCenter_hdc, 0, 0, CENTER_WIDTH, CENTER_HEIGHT, GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);

	//���Ƶ�ͼ
	int x = 0, y = 0;
	for (int i = 0; i < 26; i++)
		for (int j = 0; j < 26; j++)
		{
			x = j * BOX_SIZE;
			y = i * BOX_SIZE;
			switch (mMarkBox->box_8[i][j])
			{
			case _WALL:
				BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mWallImage), 0, 0, SRCCOPY);
				break;
			case _ICE:
				BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mIceImage), 0, 0, SRCCOPY);
				break;
			case _RIVER:
				mRiverCounter = mRiverCounter > 1000 ? 0: mRiverCounter + 1;
				BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mRiverImage[mRiverCounter/501]), 0, 0, SRCCOPY);
				break;
			case _STONE:
				BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mStoneImage), 0, 0, SRCCOPY);
				break;
			default:
				break;
			}
		}
	//��ⱻ���е�ǽ,���ƺ�ɫͼƬ����
	for (int i = 0; i < 52; i++)
		for (int j = 0; j < 52; j++)
			if (mMarkBox->clear[i][j] == _CLEAR)//|| mMarkBox->clear[i][j] == _EMPTY)		//��ͼ���(����)��ǽbug
				BitBlt(mCenter_hdc, j * SMALL_BOX_SIZE, i * SMALL_BOX_SIZE, SMALL_BOX_SIZE, SMALL_BOX_SIZE,
					GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);

	//�������̹�����ӵ�
	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
	{
		(*itor)->DrawTank(mCenter_hdc);
		(*itor)->DrawBullet(mCenter_hdc);
	}
	//����EnemyTank���ӵ�
	for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
	{
		(*EnemyItor)->DrawTank(mCenter_hdc);
		(*EnemyItor)->DrawBullet(mCenter_hdc);
	}
	//ɭ��Ҫ������̹��/�ӵ��Ϸ�
	for (int i = 0; i < 26; i++)
		for (int j = 0; j < 26; j++)
		{
			x = j * BOX_SIZE;
			y = i * BOX_SIZE;
			if (mMarkBox->box_8[i][j] == _FOREST)
				TransparentBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mForestImage), 0, 0, BOX_SIZE, BOX_SIZE, 0x000000);
		}

	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
	{
		(*itor)->Bombing(mCenter_hdc);
		(*itor)->Blasting(mCenter_hdc);	// ��������ù�
	}
	for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
	{
		(*EnemyItor)->Bombing(mCenter_hdc);
		(*EnemyItor)->E_blasting(mCenter_hdc);

	}

	mProp.Prop_Draw(mCenter_hdc);

	if(!mCampBlast)
		TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mCamp[0]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	else
		TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mCamp[1]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	mBlast.CampBlasting(mCenter_hdc);

	CheckGame();
	FlushBatchDraw();
}

void GameControl::RefreshSide()
{
	StretchBlt(mCanvas_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);
	
	//�л�ͼ��
	int x[2] = { 233,241 };
	for (int i = 0; i < mRemainEnemy; i++)
	{
		TransparentBlt(mCanvas_hdc, x[i % 2], 19 + i / 2 * 8, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE,
			GetImageHDC(&mEnemyTankIcoImage), 0, 0, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE, 0xffffff);
	}

	//�����Ϣ
	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
		(*itor)->DrawData(mCanvas_hdc);

	//����
	TransparentBlt(mCanvas_hdc, 232, 177, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y,
		GetImageHDC(&mFlagImage), 0, 0, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y, 0xffffff);
	//�ؿ�
	if (mCurrentStage < 10)
		TransparentBlt(mCanvas_hdc, 238, 193, 7, 7, GetImageHDC(&mNumberImage),
			NUMBER_SIZE * mCurrentStage, 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
	else
	{
		TransparentBlt(mCanvas_hdc, 233, 193, 7, 7, GetImageHDC(&mNumberImage),
			NUMBER_SIZE * (mCurrentStage / 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
		TransparentBlt(mCanvas_hdc, 241, 193, 7, 7, GetImageHDC(&mNumberImage),
			NUMBER_SIZE * (mCurrentStage % 10), 0, NUMBER_SIZE, NUMBER_SIZE, 0xffffff);
	}
}

void GameControl::AddEnemy()
{
	if (!mAddEnemyTimer.IsTimeOut())
		return;
	int num = mEnemyList.size();
	if (mCurrentEnemy > 5 || TOTAL_ENEMY_NUM - num < 1)
		return;
	mCurrentEnemy++;
	
	TankType type = TankType::Common;
	int level;
	if (num < 8)
		level = 0;
	else if (num < 13)
		level = 1;
	else if (num < 17)
		level = 2;
	else
		level = 3;

	//��4����,5��һ������̹��
	if (num % 5 == 4)
		type = TankType::Prop;
	if(level<3)
		switch (type)
		{
		case TankType::Prop: mEnemyList.push_back(new PropTank(level, mMarkBox)); break;
		case TankType::Common: mEnemyList.push_back(new CommonTank(level, mMarkBox)); break;
		default:break;
		}
	else
		mEnemyList.push_back(new BossTank(type,mMarkBox));
	
}

bool GameControl::KillEnemy(int id)
{
	for (list<EnemyClass*>::iterator EnemyItor = mEnemyList.begin(); EnemyItor != mEnemyList.end(); EnemyItor++)
		if ((*EnemyItor)->GetID() == id)
			if((*EnemyItor)->E_beshot(false))
			{	
				mKilledEnemy++;
				mCurrentEnemy--;
				mRemainEnemy--;
				return true;
			}
	return false;
}

void GameControl::CheckGame()
{
	bool temp = true;
	for (list<PlayerClass*>::iterator itor = mPlayerList.begin(); itor != mPlayerList.end(); itor++)
		if (!(*itor)->IsDead())
			temp = false;
	if (!mFail)
		mFail = temp;

	if (mFail)
	{
		//std::cout << "Printing GAMEOVER image.\n";
		TransparentBlt(mCenter_hdc, mGameOverX, mGameOverY, GAMEOVER_WIDTH, GAMEOVER_HEIGHT,
			GetImageHDC(&mGameOverImage), 0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, 0x000000);

		if (mGameOverTimer.IsTimeOut() && mGameOverY >= CENTER_HEIGHT * 0.45)
			mGameOverY -= 2;
		else if (mGameOverY < CENTER_HEIGHT * 0.45)
			mGameOverCounter++;

		if (mGameOverCounter > 300)
			mShowScorePanel = true;
	}

	if (mKilledEnemy == TOTAL_ENEMY_NUM)
		if (mWinCounter > 500)
		{
			mShowScorePanel = true;
			mWin = true;
			mWinCounter = 0;
		}
		else
		{
			mWinCounter++;
			//std::cout << "winnig\n";
		}
}

void GameControl::InitMarkBox()
{
	//��ͼ
	for (int i = 0; i < 26; i++)
		for (int j = 0; j < 26; j++)
		{
			mMarkBox->box_8[i][j] = mMap.map[i][j] - '0';
			FillBox_4(i, j, mMap.map[i][j] - '0');
		}
	//��Ӫ
	for (int i = 23; i < 26; i++)
		for (int j = 11; j < 15; j++)
		{
			if (i >= 24 && j >= 12 && j <= 13)
				mMarkBox->box_8[i][j] = CAMP_SIGN;
			else
				mMarkBox->box_8[i][j] = _WALL;
			FillBox_4(i, j, mMarkBox->box_8[i][j]);
		}

	//box_4 clear bullet
	for (int i = 0; i < 52; i++)
	{
		for (int j = 0; j < 52; j++)
		{
			mMarkBox->clear[i][j] = _EMPTY;
			mMarkBox->bullet[i][j] = _EMPTY;
			std::cout << mMarkBox->box_4[i][j];
		}
		std::cout << std::endl;
	}

}

//void GameControl::SetNewProp()
//{
//	//int x = rand() % 25;
//	//int y = rand() % 25;
//	//for (int k=0;k<625;k++)
//	//	for (int i = y; i < y + 2; i++)
//	//		for (int j = x; j < x + 2; j++)
//	//			if (mMarkBox->box_8[i][j] < _RIVER)		// ��֤���߿��Ի��(���
//	//				break;
//	mProp.Prop_Appear(/*x, y*/);
//		
//}

//void GameControl::CheckProp()
//{
//	
//	
//}

void GameControl::SetShovel()
{
	mGetShov=true;
	mShovCounter=0;
}

void GameControl::IsGetShovel()
{
	if (mGetShov)
	{
		if (mShovCounter == 0)
			SetCamp(_STONE);
		else if (mShovCounter > 31000 && mShovCounter < 36000)
			if (mShovCounter % 12 < 6)
				SetCamp(_STONE);
			else
				SetCamp(_WALL);
		else if (mShovCounter > 36000)
		{
			mGetShov = false;
			mShovCounter = 0;
			SetCamp(_WALL);
		}
	}
}

void GameControl::SetCamp(int val)
{
	for (int i = 23; i < 26; i++)
		for (int j = 11; j < 15; j++)
			if (i < 24 || j < 12 || j > 13)
			{
				mMarkBox->box_8[i][j] = val;
				for (int m = 2 * i; m < 2 * i + 2; m++)
					for (int n = 2 * j; n < 2 * j + 2; n++)
						mMarkBox->box_4[m][n] = val;
					
			}
}

//i==y, j==x box_8����
void GameControl::FillBox_4(int i, int j, int fval)
{
	for (int k = 0; k < 2; k++)
		for (int l = 0; l < 2; l++)
			mMarkBox->box_4[i * 2 + k][j * 2 + l] = fval;
}


