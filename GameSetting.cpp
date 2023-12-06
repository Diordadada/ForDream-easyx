#include "graphics.h"
#include "setjmp.h"
#include "Game_Setting.h"
#include "Tools.h"
#include<Windows.h>

#pragma warning (disable:4996)
//����
extern MyMusic menu_bgm;
extern MyMusic play_bgm;
//��Ч
extern MyMusic jump;
extern MyMusic fire;
extern MyMusic explode;
extern MyMusic death;

extern jmp_buf menu;

static int Win_W = 0;
static int Win_H = 0;
extern int v = 0;

void Setting()
{   //��ȡ��Ļ���
	double cx = GetSystemMetrics(SM_CXSCREEN); // ��Ļ��� ����
	double cy = GetSystemMetrics(SM_CYSCREEN); // ��Ļ�߶� ����

	int x1 = int(cx / 10), x2 = int(cx / 20), xl = int(cx / 50), x3 = int(cx / 5), x4 = int(cx / 30), y2 = int(cy / 20), y3 = int(cy / 25), y1 = int(cy / 10), y4 = int(cy / 100), ym = int(cy / 50), ymi = int(cy / 100);
	int a = 1, t = 1;
	int yl = 5; int yx = 5;  //��Ч ����
	wchar_t d[6] = { 'A','D','I','0','J','K' };
	wchar_t d1[6] = { 'A','D','I','O','J','K' };
	wchar_t y[3] = { ' ','-','+' };

	InitFullGraph(Win_W, Win_H);
	


	
	IMAGE background;
	loadimage(&background, L"resource\\Background\\OIG.lEM.png", cx, cy);

	while (t)
	{
		BeginBatchDraw();     //ˢ��
		MOUSEMSG m = GetMouseMsg();     //��ȡ�����Ϣ 
		
		putimage(0, 0, &background);
		// ���ñ���ɫΪ��ɫ
		//setbkcolor(RGB(0, 60, 106));
		//cleardevice();
		//setfillcolor(RGB(16, 66, 76));         //��ɫ
		//fillrectangle(x1, y2, cx - x1, 4 * y2 + y2);   //��
		//fillrectangle(x1, 5 * y2 + y2, cx - x1, 8 * y1 + y3);   //��
		struct bn1* mis1 = create(2 * x1, 2 * y2 - ym + ymi, y2, L"����");
		struct bn1* mis2 = create(2 * x1, 3 * y2 + ymi, y2, L"��Ч");
		struct bn1* lift1 = create(2 * x1, 5 * y2 + y3 + y2, y3, L"����");
		struct bn1* right1 = create(2 * x1, 5 * y2 + 3 * y3 + y2, y3, L"����");
		struct bn1* save1 = create(2 * x1, 5 * y2 + 5 * y3 + y2, y3, L"����");
		struct bn1* next1 = create(2 * x1, 5 * y2 + 7 * y3 + y2, y3, L"��һ��");
		struct bn1* shoot1 = create(2 * x1, 5 * y2 + 9 * y3 + y2, y3, L"���");
		struct bn1* jump1 = create(2 * x1, 5 * y2 + 11 * y3 + y2, y3, L"��Ծ");
		draww(lift1);           //����
		draww(right1);           //����
		draww(save1);           //����
		draww(next1);           //����
		draww(shoot1);           //����
		draww(jump1);  //����
		draww(mis1);   //����   
		draww(mis2);   //����   
		struct bnj* lift = create(7 * x1 + x2, 5 * y2 + y3 + y2, 30, 30, 0, d1);   //���곤��
		struct bnj* right = create(7 * x1 + x2, 5 * y2 + 3 * y3 + y2, 30, 30, 1, d1);
		struct bnj* save = create(7 * x1 + x2, 5 * y2 + 5 * y3 + y2, 30, 30, 2, d1);
		struct bnj* next = create(7 * x1 + x2, 5 * y2 + 7 * y3 + y2, 30, 30, 3, d1);
		struct bnj* shoot = create(7 * x1 + x2, 5 * y2 + 9 * y3 + y2, 30, 30, 5, d1);
		struct bnj* Jump = create(7 * x1 + x2, 5 * y2 + 11 * y3 + y2, 30, 30, 4, d1);
		struct bnj* mus3 = create(4 * x1 + x2, 2 * y2 - ym, 2 * x1 + x2, y2, 0, y);  //������
		struct bnj* mlow = create(4 * x1, 2 * y2 + y4 - ym, 28, 30, 1, y);  //��������
		struct bnj* madd = create(7 * x1 + x4, 2 * y2 + y4 - ym, 28, 30, 2, y);  //��������
		struct bnj* mus4 = create(4 * x1 + x2, 3 * y2 + y4, 2 * x1 + x2, y2, 0, y);  //��Ч��
		struct bnj* mlow1 = create(4 * x1, 3 * y2 + 2 * y4, 28, 30, 1, y);  //��Ч����
		struct bnj* madd1 = create(7 * x1 + x4, 3 * y2 + 2 * y4, 28, 30, 2, y);  //��Ч����

		drawj(mus3, 29, 6, 2);           //�� ���� ��ť�������С���ఴť����ߣ�
		drawj(mlow, 29, 7, 3);           //�� ������ ��ť�������С���ఴť����ߣ�
		drawj(madd, 29, 7, 2);           //�� ������ ��ť�������С���ఴť����ߣ�
		drawj(mus4, 29, 6, 2);           //�� ��Ч ��ť�������С���ఴť����ߣ�
		drawj(mlow1, 29, 7, 3);           //�� ��Ч�� ��ť�������С���ఴť����ߣ�
		drawj(madd1, 29, 7, 2);           //�� ��Ч�� ��ť�������С���ఴť����ߣ�
		drawj(lift, 29, 6, 2);           //����ť�������С���ఴť����ߣ�
		drawj(right, 29, 6, 2);           //����ť�������С���ఴť����ߣ�
		drawj(save, 29, 6, 2);           //����ť�������С���ఴť����ߣ�
		drawj(next, 29, 6, 2);           //����ť�������С���ఴť����ߣ�
		drawj(shoot, 29, 6, 2);           //����ť�������С���ఴť����ߣ�
		drawj(Jump, 29, 6, 2);           //����ť�������С���ఴť����ߣ�

		//���Ϊ��ɫ
		setfillcolor(WHITE);

		//������    //���Ͻ� ���½� Բ�߿�� �߶�
		fillroundrect(4 * x1 + 2 * x4, 2 * y2 + ym - ym, 6 * x1 + x2 + x4, 2 * y2 + ym + y4 - ym, y4, y4);

		//��Ч��    //���Ͻ� ���½� Բ�߿�� �߶�
		fillroundrect(4 * x1 + 2 * x4, 3 * y2 + y4 + ym, 6 * x1 + x2 + x4, 3 * y2 + y4 + ym + y4, y4, y4);

		struct bn* music = create(3 * x1 + x2, 2 * y2, 200, 40, L"������", a);   //���곤�� ״̬
		struct bn* unmusic = create(6 * x1 + x2, 2 * y2, 200, 40, L"�ر�����", a + 1);   //���곤�� ״̬

		struct bn* Save = create(7 * x1 + x4, 8 * y1 + y2 + y3, 90, 40, L"����", a + 6);   //���곤�� ״̬
		struct bn* resave = create(x1 + x2, 8 * y1 + y2 + y3, 200, 40, L"�ָ�Ĭ��", a + 6);   //���곤�� ״̬
		drawbn(Save, 36, 12, 5);        //�����ذ�ť�������С���ఴť����ߣ�
		drawbn(resave, 36, 26, 5);        //���ָ�Ĭ�ϰ�ť�������С���ఴť����ߣ�

		//���Ϊ��ɫ
		setfillcolor(BLUE);

		//������    //���Ͻ� ���½� Բ�߿�� �߶�
		fillroundrect(4 * x1 + 2 * x4, 2 * y2 + ym - ym, 4 * x1 + 2 * x4 + yl * xl, 2 * y2 + ym + y4 - ym, y4, y4);
		//��Ч��    //���Ͻ� ���½� Բ�߿�� �߶�
		fillroundrect(4 * x1 + 2 * x4, 3 * y2 + y4 + ym, 4 * x1 + 2 * x4 + yx * xl, 3 * y2 + y4 + ym + y4, y4, y4);
	    // ��������
		   //����
		menu_bgm.set_volume(yl * 90);
		play_bgm.set_volume(yl * 90);
		   //��Ч
		jump.set_volume(yx * 90);
		fire.set_volume(yx * 90);
		explode.set_volume(yx * 90);
		death.set_volume(yx * 90);
	
		
		FlushBatchDraw();
		EndBatchDraw();       //ˢ��

		if (dlickb(lift, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"�޸İ���", L"A");
			d1[0] = *s;
		}
		if (dlickb(right, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"V�޸İ���", L"D");
			d1[1] = *s;
		}
		if (dlickb(save, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"�޸İ���", L"I");
			d1[2] = *s;
		}
		if (dlickb(next, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"�޸İ���", L"O");
			d1[3] = *s;
		}
		if (dlickb(Jump, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"�޸İ���", L"J");
			d1[4] = *s;
		}
		if (dlickb(shoot, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"�������޸ĺ�İ���", L"�޸İ���", L"K");
			d1[5] = *s;
		}
		if (dlick(music, m))
		{
			//Music.set_volume(m1);
			a = 1;

		}
		if (dlick(unmusic, m))
		{
			//Music.set_volume(m0);
			a = 0;
		}
		if (dlick(resave, m))
		{
			//Music.set_volume(m1);
			a = 1;
			d1[0] = d[0];
			d1[1] = d[1];
			d1[2] = d[2];
			d1[3] = d[3];
			d1[4] = d[4];
			d1[5] = d[5];
			yx = 5;
			yl = 5;
		}
		if (dlick(Save, m))   //tuichu
		{
			longjmp(menu, 1);
			break;
		}
		//����
		if (dlickb(mlow, m))   //������
		{
			if (yl > 0)
				yl--;
		}
		if (dlickb(madd, m))                 //������
		{
			if (yl <= 10)
				yl++;

		}
		//��Ч
		if (dlickb(mlow1, m))   //������
		{
			if (yx > 0)
				yx--;
		}
		if (dlickb(madd1, m))                 //������
		{
			if (yx <= 10)
				yx++;

		}


	}

}

struct bn* create(int x, int y, int w, int h, const wchar_t* pt, int a)     //��ʼ����ť
{
	struct bn* pb = (struct bn*)malloc(sizeof(struct bn));
	pb->x = x;
	pb->y = y;
	pb->w = w;
	pb->h = h;
	pb->pt = (wchar_t*)malloc(wcslen(pt) + 1);
	wcscpy(pb->pt, pt);
	//strcpy(pb->pt, pt);
	if (a == 1)
	{
		pb->co[0] = RGB(256, 128, 66);

	}
	else
	{

		pb->co[0] = RGB(186, 186, 186);
	}
	pb->co[1] = WHITE;
	pb->co[2] = BLACK;
	return pb;
}

struct bnj* create(int x, int y, int w, int h, int t, wchar_t* pt)     //��ʼ��������ť
{
	struct bnj* pb = (struct bnj*)malloc(sizeof(struct bnj));
	pb->x = x;
	pb->y = y;
	pb->w = w;
	pb->h = h;
	pb->pt = pt[t];
	pb->co[0] = RGB(186, 186, 186);
	pb->co[1] = WHITE;
	pb->co[2] = BLACK;
	return pb;
}

struct bnj* create1(int x, int y, int w, int h, const wchar_t* pt)     //��ʼ��������ť  ���곤������ 
{
	struct bnj* pb = (struct bnj*)malloc(sizeof(struct bnj));
	pb->x = x;
	pb->y = y;
	pb->w = w;
	pb->h = h;
	pb->p = (wchar_t*)malloc(wcslen(pt) + 1);
	wcscpy(pb->p, pt);
	pb->co[0] = RGB(186, 186, 186);
	pb->co[1] = WHITE;
	pb->co[2] = BLACK;
	return pb;
}
struct bn1* create(int x, int y, int s, const wchar_t* pt)     //��ʼ���ְ�ť
{
	struct bn1* pb = (struct bn1*)malloc(sizeof(struct bn1));
	pb->x = x;
	pb->y = y;
	pb->s = s;
	pb->pt = (wchar_t*)malloc(wcslen(pt) + 1);
	wcscpy(pb->pt, pt);
	//strcpy(pb->pt, pt);

	pb->co = WHITE;
	return pb;
}

void draww(struct bn1* bp)        //������
{
	settextstyle(bp->s, 0, L"����");    //����
	settextcolor(bp->co);      //������ɫ
	setbkmode(TRANSPARENT);
	outtextxy(bp->x, bp->y, LPCTSTR(bp->pt));
}

void drawbn(struct bn* bp, int  zh, int zw, int zl)        //����ť
{
	setfillcolor(bp->co[0]);         //��ɫ
	settextstyle(zh, 0, L"����");    //����
	setlinecolor(bp->co[1]);      //����
	settextcolor(bp->co[2]);      //����
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, LPCTSTR(bp->pt));
}

void drawj(struct bnj* bp, int  zh, int zw, int zl)        //��������ť
{
	setfillcolor(bp->co[0]);         //��ɫ
	settextstyle(zh, 0, L"����");    //����
	setlinecolor(bp->co[1]);      //����
	settextcolor(bp->co[2]);      //����
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, (bp->pt));
}

void drawj1(struct bnj* bp, int  zh, int zw, int zl)        //��������ť
{
	setfillcolor(bp->co[0]);         //��ɫ
	settextstyle(zh, 0, L"����");    //����
	setlinecolor(bp->co[1]);      //����
	settextcolor(bp->co[2]);      //����
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, (bp->p));
}

int mbn1(struct bn* bp, MOUSEMSG m)                    //�ж�����Ƿ��ڰ�ť�ڰ�ť               
{
	if (bp->x <= m.x && bp->y <= m.y && bp->x + bp->w >= m.x && bp->y + bp->h >= m.y)
		return  1;
	return 0;
}

int mbn(struct bnj* bp, MOUSEMSG m)                    //�ж�����Ƿ��ڰ�ť�ڰ�ť               
{
	if (bp->x <= m.x && bp->y <= m.y && bp->x + bp->w >= m.x && bp->y + bp->h >= m.y)
		return  1;
	return 0;
}

int dlickb(struct bnj* bp, MOUSEMSG m)                  //�ж������������ť
{
	if (mbn(bp, m) && m.uMsg == WM_LBUTTONDOWN)
	{


		return 1;
	}
	return 0;
}

int dlick(struct bn* bp, MOUSEMSG m)                  //�ж��������ť
{
	if (mbn1(bp, m) && m.uMsg == WM_LBUTTONDOWN)
	{


		return 1;
	}
	return 0;
}
