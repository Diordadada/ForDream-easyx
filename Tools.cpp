#include "Tools.h"
#include "graphics.h"
#include <iostream>
#include <stack>
#include <io.h>
#include <direct.h>
#pragma comment (lib,"winmm.lib")
using namespace std;

static constexpr double PI = 3.14159265359;
static constexpr int  Default_Size = 30;

static constexpr int ROW = 3;
static constexpr int COLUMN = 5;

static constexpr int LENGTH = 100;
static constexpr int WIDTH = 100;

MOUSEMSG MyMouse::mouse;
HCURSOR MyMouse::normal;
HCURSOR MyMouse::check;

void MyMouse::init_mouse(const wchar_t* Normal_Path, double Normal_Size, const wchar_t* Check_Path, double Check_Size) {
	MyMouse::normal = (HCURSOR)LoadImage(nullptr, Normal_Path, IMAGE_CURSOR, Default_Size * Normal_Size, Default_Size, LR_LOADFROMFILE);
	MyMouse::check = (HCURSOR)LoadImage(nullptr, Check_Path, IMAGE_CURSOR, Default_Size * Check_Size, Default_Size, LR_LOADFROMFILE);
}
void MyMouse::SetStyle(char sign) {
	HWND hwnd = GetForegroundWindow();
	if (sign == 'n') {
		SetClassLong(hwnd, -12, (long)MyMouse::normal);
	}
	else if (sign == 'c') {
		SetClassLong(hwnd, -12, (long)MyMouse::check);
	}
	else {
		cout << "SetStyle()函数不能识别该标志" << endl;
	}
}

IMA::IMA(const wchar_t* path) :path(path), radium(0) {};

void IMA::load(int nWidth = 0, int nHeight = 0)
{
	wchar_t top_path[100];
	wchar_t bottom_path[100];
	swprintf_s(top_path, 100, L"%ls\\self.png", path);
	swprintf_s(bottom_path, 100, L"%ls\\mask.png", path);
	top = new IMAGE;
	bottom = new IMAGE;
	loadimage(top, top_path, nWidth, nHeight, false);
	loadimage(bottom, bottom_path, nWidth, nHeight, false);
}

void IMA::put(int dstX, int dstY)
{
	putimage(dstX, dstY, bottom, SRCAND);
	putimage(dstX, dstY, top, SRCPAINT);
}

void IMA::rotate(double degree, int dstX, int dstY)
{
	IMAGE* newtop = new IMAGE;
	IMAGE* newbottom = new IMAGE;
	double x = degree * PI / 180;
	rotateimage(newbottom, bottom, radium, WHITE);
	rotateimage(newtop, top, radium, BLACK);
	radium += x;
	putimage(dstX, dstY, newbottom, SRCAND);
	putimage(dstX, dstY, newtop, SRCPAINT);
	delete newtop;
	delete newbottom;
}

IMA::~IMA() {
	if (top != nullptr)
		delete top;
	if (bottom != nullptr)
		delete bottom;
}

MciMusic::MciMusic(const wchar_t* file_name) :file_name(file_name) {
	mci_dgv_saudio_parms.dwItem = MCI_DGV_SETAUDIO_VOLUME;
}

void MciMusic::open() {
	mci_open_parms.lpstrElementName = file_name;
	if (0 != mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_open_parms)) {
		printf("Path: %ls\nopen error!\n", file_name);
	}
	wDeviceID = mci_open_parms.wDeviceID;
}

void MciMusic::play()
{
	seek_to_start();
	if (0 != mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&mci_play_parms)) {
		printf("Path: %ls\nplay error!\n", mci_open_parms.lpstrElementName);
	}
};

void MciMusic::repeat()
{
	if (0 != mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_DGV_PLAY_REPEAT, (DWORD_PTR)&mci_play_parms))
		printf("Path: %ls\nrepeat error!\n", mci_open_parms.lpstrElementName);
}

void MciMusic::pause() {
	if (0 != mciSendCommand(wDeviceID, MCI_PAUSE, MCI_NOTIFY | MCI_WAIT, (DWORD_PTR)&mci_generic_parms))
		printf("Path: %ls\npause error!\n", mci_open_parms.lpstrElementName);
}

void MciMusic::seek_to_start()
{
	if (0 != mciSendCommand(wDeviceID, MCI_SEEK, MCI_WAIT | MCI_NOTIFY | MCI_SEEK_TO_START, ((DWORD_PTR)&mci_seek_parms)))
		printf("Path: %ls\nstop error!\n", mci_open_parms.lpstrElementName);
}

void MciMusic::close() {
	if (0 != mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL))
		printf("Path: %ls\nclose error!\n", mci_open_parms.lpstrElementName);
}

void MciMusic::set_volume(DWORD volume_value) {
	if (volume_value > 1000)
		volume_value = 1000;
	else if (volume_value < 0)
		volume_value = 0;
	mci_dgv_saudio_parms.dwValue = volume_value;
	if (0 != mciSendCommand(wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)&mci_dgv_saudio_parms))
		printf("Path: %ls\nset_volume error!\n", mci_open_parms.lpstrElementName);
}

MyMusic::MyMusic(const wchar_t* file_name) :music(file_name) { music.open(); }
void MyMusic::play_once() { music.play(); }
void MyMusic::play() { music.repeat(); }
void MyMusic::pause() { music.pause(); }
void MyMusic::close() { music.close(); }
void MyMusic::set_volume(DWORD volume_value) { music.set_volume(volume_value); }
void MyMusic::seek_to_start() { music.seek_to_start(); }
wchar_t __num_to_wchar(size_t num) {
	switch (num) {
	case 0: return L'0';
	case 1: return L'1';
	case 2: return L'2';
	case 3: return L'3';
	case 4: return L'4';
	case 5: return L'5';
	case 6: return L'6';
	case 7: return L'7';
	case 8: return L'8';
	case 9: return L'9';
	default: std::cout << "error!!!\n";
	}
}

int readf_level()
{
	char folderName[] = "C:\\ProgramData\\ForDream";

	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
		FILE* fp;
		fopen_s(&fp, "C:\\ProgramData\\ForDream\\user.txt", "w");
		if (fp == NULL)
		{
			cout << "文件打开失败" << endl;
			exit(0);
		}
		fputc('1', fp);
		fputc('0', fp);
		fclose(fp);
		return 10;
	}
	//文件存在则读取数据
	else
	{
		char guanqia[5];
		int x;
		FILE* fp;
		fopen_s(&fp, "C:\\ProgramData\\ForDream\\user.txt", "r");
		if (fp == NULL)
		{
			cout << "文件打开失败" << endl;
			exit(0);
		}
		fgets(guanqia, 5, fp);
		x = atoi(guanqia);
		fclose(fp);
		return x;
	}
}


wchar_t* num_to_wchar(long long num) {
	bool is_nega;
	if (num < 0) {
		is_nega = true;
		num = 0 - num;
	}
	else if (num == 0) {
		wchar_t* text = new wchar_t[2];
		text[0] = L'0';
		text[1] = L'\0';
		return text;
	}
	else
		is_nega = false;
	int number;
	std::stack<wchar_t> stack;
	int wchar_cnt = is_nega ? 1 : 0;
	while (num != 0) {
		number = num % 10;
		stack.push(__num_to_wchar(number));
		num = num / 10;
		++wchar_cnt;
	}
	wchar_t* text = new wchar_t[wchar_cnt + 1];
	int i = 0;
	if (is_nega) {
		text[0] = L'-';
		i = 1;
	}
	for (; i < wchar_cnt; i++) {
		text[i] = stack.top();
		stack.pop();
	}
	text[wchar_cnt] = L'\0';
	return text;
}

void InitFullGraph(int& Win_W, int& Win_H) { // 初始化一个全屏窗口
	Win_W = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度 像素
	Win_H = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度 像素

	HWND hwnd = initgraph(Win_W, Win_H, NOCLOSE); // 初始化绘图窗口并获取窗口句柄（以 EasyX 为例）
	LONG l_WinStyle = GetWindowLong(hwnd, GWL_STYLE); // 获取窗口信息
	SetWindowLong(hwnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER); // 直接修改窗口样式：最大化 取消标题栏及边框
	SetWindowPos(hwnd, HWND_TOP, 0, 0, Win_W, Win_H, 0);
	SetWindowText(hwnd, L"For Dream"); // 修改窗口标题
}

void GetGraphWH(int& Win_W, int& Win_H) {
	Win_W = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度 像素
	Win_H = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度 像素
}