//main.cpp��Ϊ��Ϸѭ�����
#include "stdafx.h"
#include "struct.h"
#include "GameControl.h"

#define SINGLE 1
#define DUAL 2

int main()
{
	srand((unsigned)time(0));		//ȷ�����������,�ڶ�����뺯�����õ�rand();
	initgraph(WIN_WIDTH, WIN_HEIGHT);	//��ʼ����Ϸ����,��С���Ըı�
	BeginBatchDraw();		//��ʼ������ͼ
	OpenMusic();			//������Ϸ��Ч�ļ�
	IMAGE canvas(CANVAS_WIDTH, CANVAS_HEIGHT);	//��Ϸ����,��Ϸͼ���������,��������������

	HDC win_hdc = GetImageHDC();
	HDC canvas_hdc = GetImageHDC(&canvas);

	SelectPanel panel(win_hdc, canvas_hdc);		//������Ϸ������ѡ��ģʽ
	GameControl *control = NULL;
	SelectResult result = Error;				//ѡ����,Ϊö������
	
	while (true)		//��Ϸ��ѭ��
	{
		result = panel.ShowPanel();		//resultΪ������ѡ����,ȷ����˫��
		control = new GameControl(win_hdc, canvas_hdc);		//�µ�GameControl��ʵ��,������Ϸ�غ�ֱ��ʧ��

		switch (result)
		{
		case SinglePlayer:
			control->LoadMap();			//���ص�ͼ
			control->Gamemode(SINGLE);	
			control->GameLoop();		//�غ�ѭ��
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
	closegraph();		//�رջ�ͼ����
	return 0;
}