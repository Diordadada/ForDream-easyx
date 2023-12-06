#include "graphics.h"
#include "setjmp.h"
#include "Game_Setting.h"
#include "Tools.h"
#include<Windows.h>

#pragma warning (disable:4996)
//音乐
extern MyMusic menu_bgm;
extern MyMusic play_bgm;
//音效
extern MyMusic jump;
extern MyMusic fire;
extern MyMusic explode;
extern MyMusic death;

extern jmp_buf menu;

static int Win_W = 0;
static int Win_H = 0;
extern int v = 0;

void Setting()
{   //获取屏幕宽度
	double cx = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度 像素
	double cy = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度 像素

	int x1 = int(cx / 10), x2 = int(cx / 20), xl = int(cx / 50), x3 = int(cx / 5), x4 = int(cx / 30), y2 = int(cy / 20), y3 = int(cy / 25), y1 = int(cy / 10), y4 = int(cy / 100), ym = int(cy / 50), ymi = int(cy / 100);
	int a = 1, t = 1;
	int yl = 5; int yx = 5;  //音效 音量
	wchar_t d[6] = { 'A','D','I','0','J','K' };
	wchar_t d1[6] = { 'A','D','I','O','J','K' };
	wchar_t y[3] = { ' ','-','+' };

	InitFullGraph(Win_W, Win_H);
	


	
	IMAGE background;
	loadimage(&background, L"resource\\Background\\OIG.lEM.png", cx, cy);

	while (t)
	{
		BeginBatchDraw();     //刷新
		MOUSEMSG m = GetMouseMsg();     //获取鼠标消息 
		
		putimage(0, 0, &background);
		// 设置背景色为蓝色
		//setbkcolor(RGB(0, 60, 106));
		//cleardevice();
		//setfillcolor(RGB(16, 66, 76));         //颜色
		//fillrectangle(x1, y2, cx - x1, 4 * y2 + y2);   //框
		//fillrectangle(x1, 5 * y2 + y2, cx - x1, 8 * y1 + y3);   //框
		struct bn1* mis1 = create(2 * x1, 2 * y2 - ym + ymi, y2, L"音乐");
		struct bn1* mis2 = create(2 * x1, 3 * y2 + ymi, y2, L"音效");
		struct bn1* lift1 = create(2 * x1, 5 * y2 + y3 + y2, y3, L"向左");
		struct bn1* right1 = create(2 * x1, 5 * y2 + 3 * y3 + y2, y3, L"向右");
		struct bn1* save1 = create(2 * x1, 5 * y2 + 5 * y3 + y2, y3, L"复活");
		struct bn1* next1 = create(2 * x1, 5 * y2 + 7 * y3 + y2, y3, L"下一关");
		struct bn1* shoot1 = create(2 * x1, 5 * y2 + 9 * y3 + y2, y3, L"射击");
		struct bn1* jump1 = create(2 * x1, 5 * y2 + 11 * y3 + y2, y3, L"跳跃");
		draww(lift1);           //画字
		draww(right1);           //画字
		draww(save1);           //画字
		draww(next1);           //画字
		draww(shoot1);           //画字
		draww(jump1);  //画字
		draww(mis1);   //画字   
		draww(mis2);   //画字   
		struct bnj* lift = create(7 * x1 + x2, 5 * y2 + y3 + y2, 30, 30, 0, d1);   //坐标长宽
		struct bnj* right = create(7 * x1 + x2, 5 * y2 + 3 * y3 + y2, 30, 30, 1, d1);
		struct bnj* save = create(7 * x1 + x2, 5 * y2 + 5 * y3 + y2, 30, 30, 2, d1);
		struct bnj* next = create(7 * x1 + x2, 5 * y2 + 7 * y3 + y2, 30, 30, 3, d1);
		struct bnj* shoot = create(7 * x1 + x2, 5 * y2 + 9 * y3 + y2, 30, 30, 5, d1);
		struct bnj* Jump = create(7 * x1 + x2, 5 * y2 + 11 * y3 + y2, 30, 30, 4, d1);
		struct bnj* mus3 = create(4 * x1 + x2, 2 * y2 - ym, 2 * x1 + x2, y2, 0, y);  //音量条
		struct bnj* mlow = create(4 * x1, 2 * y2 + y4 - ym, 28, 30, 1, y);  //音量条减
		struct bnj* madd = create(7 * x1 + x4, 2 * y2 + y4 - ym, 28, 30, 2, y);  //音量条加
		struct bnj* mus4 = create(4 * x1 + x2, 3 * y2 + y4, 2 * x1 + x2, y2, 0, y);  //音效条
		struct bnj* mlow1 = create(4 * x1, 3 * y2 + 2 * y4, 28, 30, 1, y);  //音效条减
		struct bnj* madd1 = create(7 * x1 + x4, 3 * y2 + 2 * y4, 28, 30, 2, y);  //音效条加

		drawj(mus3, 29, 6, 2);           //画 音量 按钮（字体大小，距按钮宽与高）
		drawj(mlow, 29, 7, 3);           //画 音量减 按钮（字体大小，距按钮宽与高）
		drawj(madd, 29, 7, 2);           //画 音量加 按钮（字体大小，距按钮宽与高）
		drawj(mus4, 29, 6, 2);           //画 音效 按钮（字体大小，距按钮宽与高）
		drawj(mlow1, 29, 7, 3);           //画 音效减 按钮（字体大小，距按钮宽与高）
		drawj(madd1, 29, 7, 2);           //画 音效加 按钮（字体大小，距按钮宽与高）
		drawj(lift, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）
		drawj(right, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）
		drawj(save, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）
		drawj(next, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）
		drawj(shoot, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）
		drawj(Jump, 29, 6, 2);           //画按钮（字体大小，距按钮宽与高）

		//填充为白色
		setfillcolor(WHITE);

		//音量条    //左上角 右下角 圆边宽度 高度
		fillroundrect(4 * x1 + 2 * x4, 2 * y2 + ym - ym, 6 * x1 + x2 + x4, 2 * y2 + ym + y4 - ym, y4, y4);

		//音效条    //左上角 右下角 圆边宽度 高度
		fillroundrect(4 * x1 + 2 * x4, 3 * y2 + y4 + ym, 6 * x1 + x2 + x4, 3 * y2 + y4 + ym + y4, y4, y4);

		struct bn* music = create(3 * x1 + x2, 2 * y2, 200, 40, L"打开音乐", a);   //坐标长宽 状态
		struct bn* unmusic = create(6 * x1 + x2, 2 * y2, 200, 40, L"关闭音乐", a + 1);   //坐标长宽 状态

		struct bn* Save = create(7 * x1 + x4, 8 * y1 + y2 + y3, 90, 40, L"返回", a + 6);   //坐标长宽 状态
		struct bn* resave = create(x1 + x2, 8 * y1 + y2 + y3, 200, 40, L"恢复默认", a + 6);   //坐标长宽 状态
		drawbn(Save, 36, 12, 5);        //画返回按钮（字体大小，距按钮宽与高）
		drawbn(resave, 36, 26, 5);        //画恢复默认按钮（字体大小，距按钮宽与高）

		//填充为蓝色
		setfillcolor(BLUE);

		//音量条    //左上角 右下角 圆边宽度 高度
		fillroundrect(4 * x1 + 2 * x4, 2 * y2 + ym - ym, 4 * x1 + 2 * x4 + yl * xl, 2 * y2 + ym + y4 - ym, y4, y4);
		//音效条    //左上角 右下角 圆边宽度 高度
		fillroundrect(4 * x1 + 2 * x4, 3 * y2 + y4 + ym, 4 * x1 + 2 * x4 + yx * xl, 3 * y2 + y4 + ym + y4, y4, y4);
	    // 设置音量
		   //音乐
		menu_bgm.set_volume(yl * 90);
		play_bgm.set_volume(yl * 90);
		   //音效
		jump.set_volume(yx * 90);
		fire.set_volume(yx * 90);
		explode.set_volume(yx * 90);
		death.set_volume(yx * 90);
	
		
		FlushBatchDraw();
		EndBatchDraw();       //刷新

		if (dlickb(lift, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"修改按键", L"A");
			d1[0] = *s;
		}
		if (dlickb(right, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"V修改按键", L"D");
			d1[1] = *s;
		}
		if (dlickb(save, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"修改按键", L"I");
			d1[2] = *s;
		}
		if (dlickb(next, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"修改按键", L"O");
			d1[3] = *s;
		}
		if (dlickb(Jump, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"修改按键", L"J");
			d1[4] = *s;
		}
		if (dlickb(shoot, m))
		{
			char s[1];
			InputBox(LPTSTR(s), 10, L"请输入修改后的按键", L"修改按键", L"K");
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
		//音乐
		if (dlickb(mlow, m))   //减音量
		{
			if (yl > 0)
				yl--;
		}
		if (dlickb(madd, m))                 //加音量
		{
			if (yl <= 10)
				yl++;

		}
		//音效
		if (dlickb(mlow1, m))   //减音量
		{
			if (yx > 0)
				yx--;
		}
		if (dlickb(madd1, m))                 //加音量
		{
			if (yx <= 10)
				yx++;

		}


	}

}

struct bn* create(int x, int y, int w, int h, const wchar_t* pt, int a)     //初始化按钮
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

struct bnj* create(int x, int y, int w, int h, int t, wchar_t* pt)     //初始化按键按钮
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

struct bnj* create1(int x, int y, int w, int h, const wchar_t* pt)     //初始化按键按钮  坐标长宽音量 
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
struct bn1* create(int x, int y, int s, const wchar_t* pt)     //初始化字按钮
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

void draww(struct bn1* bp)        //画文字
{
	settextstyle(bp->s, 0, L"楷体");    //字体
	settextcolor(bp->co);      //字体颜色
	setbkmode(TRANSPARENT);
	outtextxy(bp->x, bp->y, LPCTSTR(bp->pt));
}

void drawbn(struct bn* bp, int  zh, int zw, int zl)        //画按钮
{
	setfillcolor(bp->co[0]);         //颜色
	settextstyle(zh, 0, L"楷体");    //字体
	setlinecolor(bp->co[1]);      //边条
	settextcolor(bp->co[2]);      //字体
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, LPCTSTR(bp->pt));
}

void drawj(struct bnj* bp, int  zh, int zw, int zl)        //画按键按钮
{
	setfillcolor(bp->co[0]);         //颜色
	settextstyle(zh, 0, L"楷体");    //字体
	setlinecolor(bp->co[1]);      //边条
	settextcolor(bp->co[2]);      //字体
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, (bp->pt));
}

void drawj1(struct bnj* bp, int  zh, int zw, int zl)        //画按键按钮
{
	setfillcolor(bp->co[0]);         //颜色
	settextstyle(zh, 0, L"楷体");    //字体
	setlinecolor(bp->co[1]);      //边条
	settextcolor(bp->co[2]);      //字体
	setbkmode(TRANSPARENT);
	fillrectangle(bp->x, bp->y, bp->w + bp->x, bp->y + bp->h);
	outtextxy(bp->x + zw, bp->y + zl, (bp->p));
}

int mbn1(struct bn* bp, MOUSEMSG m)                    //判断鼠标是否在按钮内按钮               
{
	if (bp->x <= m.x && bp->y <= m.y && bp->x + bp->w >= m.x && bp->y + bp->h >= m.y)
		return  1;
	return 0;
}

int mbn(struct bnj* bp, MOUSEMSG m)                    //判断鼠标是否在按钮内按钮               
{
	if (bp->x <= m.x && bp->y <= m.y && bp->x + bp->w >= m.x && bp->y + bp->h >= m.y)
		return  1;
	return 0;
}

int dlickb(struct bnj* bp, MOUSEMSG m)                  //判断鼠标点击按键按钮
{
	if (mbn(bp, m) && m.uMsg == WM_LBUTTONDOWN)
	{


		return 1;
	}
	return 0;
}

int dlick(struct bn* bp, MOUSEMSG m)                  //判断鼠标点击按钮
{
	if (mbn1(bp, m) && m.uMsg == WM_LBUTTONDOWN)
	{


		return 1;
	}
	return 0;
}
