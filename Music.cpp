#include "Music.h"

void OpenMusic()
{
	mciSendString("open ./res/music/bgm.mp3 alias	bgm", NULL, 0, NULL);
	mciSendString("open ./res/music/start.wav alias	start", NULL, 0, NULL);
	mciSendString("open ./res/music/shoot.wav alias	shoot0", NULL, 0, NULL);		// Ò»¸öÍæ¼ÒÁ½¸ö×Óµ¯ <-谜之乱码注释.....
	mciSendString("open ./res/music/shoot.wav alias	shoot1", NULL, 0, NULL);
	mciSendString("open ./res/music/shoot.wav alias	shoot2", NULL, 0, NULL);
	mciSendString("open ./res/music/shoot.wav alias	shoot3", NULL, 0, NULL);

	mciSendString("open ./res/music/bin.wav alias             bin", NULL, 0, NULL);
	mciSendString("open ./res/music/bk_sound.mp3 alias        bk_sound", NULL, 0, NULL);
	mciSendString("open ./res/music/get-prop.wav alias        get_prop", NULL, 0, NULL);
	mciSendString("open ./res/music/enemy-bomb.wav alias      enemy_bomb", NULL, 0, NULL);
	mciSendString("open ./res/music/scorepanel_di.wav alias   scorepanel_di", NULL, 0, NULL);
	mciSendString("open ./res/music/bouns1000.wav alias       bouns1000", NULL, 0, NULL);
	mciSendString("open ./res/music/addlife.wav alias         addlife", NULL, 0, NULL);
	mciSendString("open ./res/music/fail.wav alias			  fail", NULL, 0, NULL);
	mciSendString("open ./res/music/player_bomb.wav alias	  player_bomb", NULL, 0, NULL);
	mciSendString("open ./res/music/player_move.wav alias	  player_move", NULL, 0, NULL);
	mciSendString("open ./res/music/prop-out.wav alias		  prop_out", NULL, 0, NULL);
}

void CloseMusic()
{
	mciSendString("close	bgm", NULL, 0, NULL);
	mciSendString("close	shoot0", NULL, 0, NULL);
	mciSendString("close	shoot1", NULL, 0, NULL);
	mciSendString("close	shoot2", NULL, 0, NULL);
	mciSendString("close	shoot3", NULL, 0, NULL);
	mciSendString("close	bin", NULL, 0, NULL);
	mciSendString("close	bk_sound", NULL, 0, NULL);
	mciSendString("close	get_prop", NULL, 0, NULL);
	mciSendString("close	enemy_bomb", NULL, 0, NULL);
	mciSendString("close	scorepanel_di", NULL, 0, NULL);
	mciSendString("close	bouns1000", NULL, 0, NULL);
	mciSendString("close	addlife", NULL, 0, NULL);
	mciSendString("close	fail", NULL, 0, NULL);
	mciSendString("close	player_bomb", NULL, 0, NULL);
	mciSendString("close	player_move", NULL, 0, NULL);
	mciSendString("close	prop_out", NULL, 0, NULL);
}

void PlayMusic(int Kind)
{
	switch (Kind)
	{
	case BGM:
		mciSendString("play bgm repeat", NULL, 0, NULL);
		break;
	case S_START:
		mciSendString("play start from 0", NULL, 0, NULL);
		break;
	case S_SHOOT0:
		mciSendString("play shoot0 from 0", NULL, 0, NULL);
		break;
	case S_SHOOT1:
		mciSendString("play shoot1 from 0", NULL, 0, NULL);
		break;
	case S_SHOOT2:
		mciSendString("play shoot2 from 0", NULL, 0, NULL);
		break;
	case S_SHOOT3:
		mciSendString("play shoot3 from 0", NULL, 0, NULL);
		break;
	case S_BIN:
		mciSendString("play bin from 0", NULL, 0, NULL);
		break;
	case S_BK:
		mciSendString("play bk_sound repeat", NULL, 0, NULL);
		break;
	case S_GETPROP:
		mciSendString("play get_prop from 0", NULL, 0, NULL);
		break;
	case S_PROPOUT:
		mciSendString("play prop_out from 0", NULL, 0, NULL);
		break;
	case S_ENEMY_BOMB:
		mciSendString("play enemy_bomb from 0", NULL, 0, NULL);
		break;
	case S_CAMP_BOMB:
		mciSendString("play player_bomb from 0", NULL, 0, NULL);
		break;
	case S_SCOREPANEL_DI:
		mciSendString("play scorepanel_di from 0", NULL, 0, NULL);
		break;
	case S_BONUS1000:
		mciSendString("play bouns1000 from 0", NULL, 0, NULL);
		break;
	case S_ADDLIFE:
		mciSendString("play addlife from 0", NULL, 0, NULL);
		break;
	case S_FAIL:
		mciSendString("play fail from 0", NULL, 0, NULL);
		break;
	case S_PLAYER_BOMB:
		mciSendString("play player_bomb from 0", NULL, 0, NULL);
		break;
	case S_PLAYER_MOVE:
		mciSendString("play player_move from 0", NULL, 0, NULL);
		break;

	default:
		break;
	}
}