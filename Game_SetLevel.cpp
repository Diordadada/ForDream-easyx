#include "Game_SetLevel.h"
#include "graphics.h"
#include "stdio.h"
#include "conio.h"
#include "setjmp.h"
#include <stack>
#include <io.h>
#include <direct.h>
#include <iostream>

using namespace std;

static constexpr int ROW = 3;
static constexpr int COLUMN = 5;

static constexpr int LENGTH = 100;
static constexpr int WIDTH = 100;

static int Win_W = 0;
static int Win_H = 0;

std::vector<MyButton> MyButton::buttons;

extern jmp_buf menu;
extern LOGFONT f_title;

int SetLevel() {
	GetGraphWH(Win_W, Win_H);
	IMAGE BG;
	loadimage(&BG, L"resource\\Background\\setlevel_bg.png", Win_W, Win_H);   // 加载背景图片
	putimage(0, 0, &BG);

	settextstyle(&f_title);						   // 设置字体样式
	settextcolor(RGB(128, 42, 42));            // 设置字体颜色
	setbkmode(TRANSPARENT);                    // 设置字体背景为透明

	myMouse::init_mouse(L"resource\\Mouse\\normal.cur", 1.0, L"resource\\Mouse\\check.cur", 1.0);
	MyButton::init_button();
	MyButton::draw_buttons();
	while (true) {
		int level = myMouse::get_level();
		myMouse::change_style(level);
		if (MyMouse::mouse.uMsg == WM_LBUTTONDOWN) {
			if (level > 0) {
				extern MyMusic menu_bgm;
				menu_bgm.pause();
				return level;
			}
			if (level == -1) {
				longjmp(menu, 1);
			}
		}

		if (_kbhit())
		{
			int k = _getch();
			if (k == 27)//ESC键
			{
				longjmp(menu, 1);
			}
		}

	}
	return 0;
}

void MyButton::draw() const {
	putimage(x - LENGTH / 2, y - WIDTH / 2, &img);
	outtextxy(x - LENGTH / 4, y - WIDTH / 4, text);
	if (MyButton::flag == ButtonFlag::lock) {
		putimage(x - LENGTH / 2, y - WIDTH / 2, &lock);
	}
}

void MyButton::get_coord() {
	int Win_W = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度 像素
	int Win_H = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度 像素
	buttons.resize(15);
	for (int i = 1; i <= ROW; i++) {
		for (int j = 1; j <= COLUMN; j++) {
			buttons[(i - 1) * COLUMN + j - 1].x = Win_W / (COLUMN + 1) * j;
			buttons[(i - 1) * COLUMN + j - 1].y = Win_H / (ROW + 1) * i;
		}
	}
}

void MyButton::init_button() {
	MyButton button;
	int level = readf_level();
	loadimage(&button.img, _T("resource\\Item\\button_img.png"), WIDTH, LENGTH);
	loadimage(&button.lock, _T("resource\\Item\\button_lock.png"), WIDTH, LENGTH);
	for (int i = 1; i <= level; i++)
	{
		button.level = i;
		button.flag = ButtonFlag::open;
		button.text = num_to_wchar(i);
		buttons.push_back(button);
	}
	for (int j = level + 1; j <= ROW * COLUMN; j++) {
		button.level = j;
		button.flag = ButtonFlag::lock;
		button.text = num_to_wchar(j);
		buttons.push_back(button);
	}

	get_coord();
}

void MyButton::draw_buttons() {
	for (const auto& item : buttons) {
		item.draw();
	}
	MyButton::draw_Return_buttons();
}

void MyButton::draw_Return_buttons() {
	IMAGE Return;
	int win_width, win_hight;
	GetGraphWH(win_width, win_hight);
	loadimage(&Return, _T("resource\\Item\\button_return.png"), 100, 79);
	putimage(win_width - 100, 0, &Return);
}

int myMouse::get_level() {
	int win_width, win_hight;
	GetGraphWH(win_width, win_hight);
	MyMouse::mouse = GetMouseMsg();
	for (const auto& item : MyButton::buttons) {
		if (mouse.x <= item.x + LENGTH / 2 && mouse.x >= item.x - LENGTH / 2 && mouse.y <= item.y + WIDTH / 2 &&
			mouse.y >= item.y - WIDTH / 2 && item.flag == ButtonFlag::open) {
			return item.level;
		}
		else if (mouse.x <= win_width && mouse.x > win_width - 100 && mouse.y < 79 && mouse.y>0) {
			return -1;
		}
	}
	return 0;
}

void myMouse::change_style(int level) {
	if (level == 0) {
		MyMouse::SetStyle('n');
	}
	if (level > 0 || level == -1) {
		MyMouse::SetStyle('c');
	}
}