#include "Game_Menu.h"
#include "graphics.h"
#include "Tools.h"
#include "time.h"

static int Win_W = 0;
static int Win_H = 0;

#define font_Magnitude 5 //字体变化最大幅度
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)  //用来检测按键的点击事件

MyMusic menu_bgm(L"resource\\Music\\menu_bgm.mp3");

LOGFONT f_title;                                 //字体变量

int Menu() {
	InitFullGraph(Win_W, Win_H);

	IMAGE Menu; // 背景图片
	IMAGE Title1, Title2; // 艺术字游戏名
	MyMouse::init_mouse(L"resource\\Mouse\\normal.cur", 1.0, L"resource\\Mouse\\check.cur", 1.0);
	MyMouse::SetStyle('n');

	menu_bgm.seek_to_start();
	menu_bgm.play();

	loadimage(&Menu, L"resource\\Background\\menu_bg.png", Win_W, Win_H);   // 加载背景图片
	putimage(0, 0, &Menu);

	int title_width = Win_W / 2;
	int title_height = Win_H / 4;      // 定义游戏名的宽度和高度

	int title_xpos = (Win_W - title_width) / 2;
	int title_ypos = Win_H / 5;        // 定义游戏名位置

	loadimage(&Title1, L"resource\\Item\\title.png", title_width, title_height); // 加载艺术字游戏名
	loadimage(&Title2, L"resource\\Item\\title_mask.png", title_width, title_height);
	putimage(title_xpos, title_ypos, &Title2, SRCAND);
	putimage(title_xpos, title_ypos, &Title1, SRCPAINT);

	
	gettextstyle(&f_title);						   // 获取当前字体设置

	f_title.lfHeight = Win_H / 20;			   // 设置字体高度
	_tcscpy_s(f_title.lfFaceName, _T("华文彩云"));   // 设置字体华文彩云
	f_title.lfQuality = ANTIALIASED_QUALITY;		   // 设置输出效果为抗锯齿  
	settextstyle(&f_title);						   // 设置字体样式
	settextcolor(RGB(128, 42, 42));            // 设置字体颜色
	setbkmode(TRANSPARENT);                    // 设置字体背景为透明


	TCHAR s[3][10] = { _T("开始游戏"),_T("游戏设置"),_T("退出游戏") };// 选项名称

	int option_height = Win_H / 15;                              // 定义选项间的高度

	int option_xpos = Win_W / 2, option_ypos = Win_H / 2;    // 定义选项位置

	int r[3][4];                                                      // 选项的二维数组

	for (int i = 0; i < 3; ++i)                                       // 初始化选项的位置
	{
		r[i][0] = option_xpos - textwidth(s[i]) / 2;
		r[i][1] = option_ypos + (textheight(s[i]) + option_height) * i;
		r[i][2] = r[i][0] + textwidth(s[i]);
		r[i][3] = r[i][1] + textheight(s[i]);
	}

	for (int i = 0; i < 3; ++i)
		outtextxy(r[i][0], r[i][1], s[i]);                           // 打印选项

	MOUSEMSG m;                                                      // 鼠标指针
	int event = 0, tmp = 0;                                          // 鼠标事件变量
	while (true)
	{
		m = GetMouseMsg();                                           // 获取鼠标消息
		tmp = button_judge(m.x, m.y, r);
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:                                           // 鼠标移动
			setfillstyle(BS_DIBPATTERN, NULL, &Menu);               // 设置背景图片填充
			if (tmp && event != tmp)
			{
				if (event) font_reset(event - 1, r, s);
				event = tmp;                                     // 记录当前鼠标事件
				font_special(event - 1, r, s);
			}
			else if (!tmp && event) {
				font_reset(event - 1, r, s);
				event = 0;
			}
			break;
		case WM_LBUTTONDOWN:                                          // 鼠标左键按下
			if (tmp) return tmp;
			break;
		}
		flushmessage(EM_MOUSE);                                      // 清空鼠标消息缓存区
	}
	menu_bgm.close();
	return 0;
}

int button_judge(int x, int y, int r[][4])   // 鼠标位置判断
{
	for (int i = 0; i < 3; ++i)
		if (x > r[i][0] && x<r[i][2] && y>r[i][1] && y < r[i][3]) return i + 1;
	return 0;
}

void font_special(int op, int r[][4], TCHAR s[][10])  //字体动态特效
{
	int lfgap;
	BeginBatchDraw();
	clearrectangle(r[op][0], r[op][1], r[op][2], r[op][3]);
	solidrectangle(r[op][0], r[op][1], r[op][2], r[op][3]);
	for (int i = 1; i <= font_Magnitude; ++i)
	{
		++f_title.lfHeight, lfgap = i;
		settextstyle(&f_title);
		int rect_xpos = r[op][0] - textwidth(s[op]) / f_title.lfHeight / 2 * lfgap;    // 定义动态选项位置
		int rect_ypos = r[op][1] - lfgap / 2;
		outtextxy(rect_xpos, rect_ypos, s[op]);
		FlushBatchDraw();
		Sleep(20);
		if (i != font_Magnitude) {
			clearrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
			solidrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
		}
	}
	EndBatchDraw();
}

void font_reset(int op, int r[][4], TCHAR s[][10])    //重置选项大小
{
	int rect_xpos = r[op][0] - textwidth(s[op]) / f_title.lfHeight / 2 * font_Magnitude;
	int rect_ypos = r[op][1] - (font_Magnitude + 1) / 2;                //确定重置前选项位置
	f_title.lfHeight -= font_Magnitude;                                 //重置字体大小
	BeginBatchDraw();
	clearrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
	solidrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
	settextstyle(&f_title);
	outtextxy(r[op][0], r[op][1], s[op]);
	FlushBatchDraw();
	EndBatchDraw();
}