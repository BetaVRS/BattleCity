//main.cpp中为游戏循环设计
#include "stdafx.h"
#include "struct.h"
#include "GameControl.h"

#define SINGLE 1
#define DUAL 2

int main()
{
	srand((unsigned)time(0));		//确定随机数种子,在多个类与函数中用到rand();
	initgraph(WIN_WIDTH, WIN_HEIGHT);	//初始化游戏窗口,大小可以改变
	BeginBatchDraw();		//开始批量绘图
	OpenMusic();			//加载游戏音效文件
	IMAGE canvas(CANVAS_WIDTH, CANVAS_HEIGHT);	//游戏画布,游戏图像绘制其上,并缩放至主窗口

	HDC win_hdc = GetImageHDC();
	HDC canvas_hdc = GetImageHDC(&canvas);

	SelectPanel panel(win_hdc, canvas_hdc);		//控制游戏主界面选择模式
	GameControl *control = NULL;
	SelectResult result = Error;				//选择结果,为枚举类型
	
	while (true)		//游戏主循环
	{
		result = panel.ShowPanel();		//result为主界面选择结果,确定单双人
		control = new GameControl(win_hdc, canvas_hdc);		//新的GameControl类实例,控制游戏回合直到失败

		switch (result)
		{
		case SinglePlayer:
			control->LoadMap();			//加载地图
			control->Gamemode(SINGLE);	
			control->GameLoop();		//回合循环
			break;
		case DualPlayer:
			control->LoadMap();
			control->Gamemode(DUAL);
			control->GameLoop();
			break;
		default:
			closegraph();
			return 0;
		}
	}
	closegraph();		//关闭绘图窗口
	return 0;
}