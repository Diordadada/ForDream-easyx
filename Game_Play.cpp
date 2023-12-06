#include "Game_Play.h"
#include "Tools.h"
#include "setjmp.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <direct.h>

#define isKeyPressed(nVirtKey) (GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0

using namespace std;

//全局变量
bool game;

int level; //关卡数
static int rW = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度 像素
static int rH = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度 像素
static int B = min(rW / 31, rH / 19);
static int sW = (rW - B * 31) / 2;
static int sH = (rH - B * 19) / 2;
static int Lead_B = B / 4 * 3;
static int Bullet_B = B / 4 * 3;
static int Ball_B = B / 4 * 3;
static int Delivery_B = B * 3 - B / 2;
static double X = 2.5; //
static double Y = 1.75; //
static double G = 0.02; // 调参
static double V_X = X * sqrt(G * B) / sqrt(8 * Y);
static double V_Y = sqrt(2 * G * Y * B);
vector<Tip> tips;

constexpr int WaterBall_NUM = 11;

extern jmp_buf menu;
extern jmp_buf set_level;

jmp_buf play;

MyMusic play_bgm(L"resource//Music//play_bgm.mp3");
MyMusic jump(L"resource//Music//jump.mp3");
MyMusic fire(L"resource//Music//fire.mp3");
MyMusic explode(L"resource//Music//explode.mp3");
MyMusic death(L"resource//Music//death.mp3");

IMAGE 角色[4];
IMAGE 背景;
IMAGE 普通砖[2];
IMAGE 石砖;
IMAGE 铁砖;
IMAGE 玻璃砖;
//IMAGE 红球;
IMA 红球(L"resource//Item//RedBall");
//IMAGE 黑球;
IMA 黑球(L"resource//Item//BlackBall");
IMAGE 紫球;
IMAGE 灰球;
IMAGE 子弹;
IMAGE 子弹掩码图;
IMAGE 水球[WaterBall_NUM + 1];
IMAGE 水球掩码图[WaterBall_NUM + 1];
IMAGE 传送门提示;
IMAGE 传送门提示掩码图;
IMAGE 死亡提示;
IMAGE 死亡提示掩码图;
IMAGE 石砖变铁砖[5];
IMAGE 铁砖变石砖[5];
IMA 传送门(L"resource//Item//Delivery");

Lead lead;
Delivery delivery;
//十大链表
CommonBrick* CommonBrick_head = new CommonBrick;
SpecialBrick* StoneBrick_head = new SpecialBrick;
SpecialBrick* IronBrick_head = new SpecialBrick;
GlassBrick* GlassBrick_head = new GlassBrick;
BlastBall* RedBall_head = new BlastBall;
BlastBall* BlackBall_head = new BlastBall;
MoveBall* PurpleBall_head = new MoveBall;
MoveBall* GreyBall_head = new MoveBall;
WaterBall* WaterBall_head = new WaterBall;
Bullet* Bullet_head = new Bullet;

Lead::Lead()
{
	x = y = 0;
	v_y = 0;
	v_x = V_X;
	v0 = V_Y;
	p = 2;
	b = 30;
	g = G;
	f = 1;
	start = 0;
	fire = true;
	cd = 200;
	w = true;
	a = true;
	s = true;
	d = true;
	deli = false;
	death = false;
	tum = false;
	//在水平方向上移动为平移，在竖直方向上使用真实物理模拟
}

template <class T>
bool Delete(T head, T p)
{
	if (!head) return false;
	T tmp = head->next, pre = head;
	while (tmp)
	{
		if (tmp == p)
		{
			pre->next = tmp->next;
			delete tmp;
			return true;
		}
		pre = tmp;
		tmp = tmp->next;
	}
	return false;
}

template <class T>
bool Increase(T* head, double& x, double& y)
{
	T* tmp = new T;
	tmp->x = x;
	tmp->y = y;
	tmp->next = head->next;
	head->next = tmp;
	return true;
}

template <class T>
void print(T head)
{
	int i = 1;
	head = head->next;
	while (head)
	{
		cout << head->x << "   " << head->y << "   " << i << endl;
		head = head->next;
		i++;
	}
}

template <class T>
bool FreeList(T head)
{
	if (!head) return false;
	T p = head->next, q;
	head->next = NULL;
	while (p)
	{
		q = p;
		p = p->next;
		delete q;
	}
	return true;
}

template <class T>
void InitList(T*& head, int& x, int& y, char s)
{
	T* tmp = new T;
	tmp->x = B * x + sW;
	tmp->y = B * y + sH;
	tmp->s = s;
	tmp->next = head->next;
	head->next = tmp;
}

template <class T>
bool Traverse(T& head, T& tmp, double x, double y)
{
	if (!head) return false;
	tmp = head->next;
	while (tmp) {
		if (x >= tmp->x && x < tmp->x + B && y >= tmp->y && y < tmp->y + B) return true;
		tmp = tmp->next;
	}
	return false;
}

void Calculate()                            //计算像素单位大小
{
	B = min(rW / 31, rH / 19);
	sW = (rW - B * 31) / 2;
	sH = (rH - B * 19) / 2;
	Lead_B = Bullet_B = Ball_B = B / 4 * 3;
	Delivery_B = B * 3 - B / 2;
}

void LoadResource()
{
	loadimage(&角色[0], L"resource\\角色(左).png", Lead_B, Lead_B);
	loadimage(&角色[1], L"resource\\角色(右).png", Lead_B, Lead_B);
	loadimage(&背景, L"resource\\背景.png", rW, rH);
	loadimage(&普通砖[0], L"resource\\普通砖0.png", B, B);
	loadimage(&普通砖[1], L"resource\\普通砖1.png", B, B);
	loadimage(&石砖, L"resource\\石砖.png", B, B);
	loadimage(&铁砖, L"resource\\铁砖.png", B, B);
	loadimage(&玻璃砖, L"resource\\玻璃砖.png", B, B);
	//loadimage(&红球, L"resource\\红球.png", B, B);
	//loadimage(&黑球, L"resource\\黑球.png", B, B);
	loadimage(&紫球, L"resource\\紫球.png", B, B);
	loadimage(&灰球, L"resource\\灰球.png", B, B);
	loadimage(&水球[1], L"resource\\WaterBall\\1.png", B, B);
	loadimage(&水球[2], L"resource\\WaterBall\\2.png", B, B);
	loadimage(&水球[3], L"resource\\WaterBall\\3.png", B, B);
	loadimage(&水球[4], L"resource\\WaterBall\\4.png", B, B);
	loadimage(&水球[5], L"resource\\WaterBall\\5.png", B, B);
	loadimage(&水球[6], L"resource\\WaterBall\\6.png", B, B);
	loadimage(&水球[7], L"resource\\WaterBall\\7.png", B, B);
	loadimage(&水球[8], L"resource\\WaterBall\\8.png", B, B);
	loadimage(&水球[9], L"resource\\WaterBall\\9.png", B, B);
	loadimage(&水球[10], L"resource\\WaterBall\\10.png", B, B);
	loadimage(&水球[11], L"resource\\WaterBall\\11.png", B, B);
	loadimage(&水球掩码图[1], L"resource\\WaterBall\\掩码1.png", B, B);
	loadimage(&水球掩码图[2], L"resource\\WaterBall\\掩码2.png", B, B);
	loadimage(&水球掩码图[3], L"resource\\WaterBall\\掩码3.png", B, B);
	loadimage(&水球掩码图[4], L"resource\\WaterBall\\掩码4.png", B, B);
	loadimage(&水球掩码图[5], L"resource\\WaterBall\\掩码5.png", B, B);
	loadimage(&水球掩码图[6], L"resource\\WaterBall\\掩码6.png", B, B);
	loadimage(&水球掩码图[7], L"resource\\WaterBall\\掩码7.png", B, B);
	loadimage(&水球掩码图[8], L"resource\\WaterBall\\掩码8.png", B, B);
	loadimage(&水球掩码图[9], L"resource\\WaterBall\\掩码9.png", B, B);
	loadimage(&水球掩码图[10], L"resource\\WaterBall\\掩码10.png", B, B);
	loadimage(&水球掩码图[11], L"resource\\WaterBall\\掩码11.png", B, B);
	loadimage(&子弹, L"resource\\子弹.png", Bullet_B, Bullet_B);
	loadimage(&子弹掩码图, L"resource\\子弹掩码图.png", Bullet_B, Bullet_B);
	loadimage(&传送门提示, L"resource\\Delivery\\传送提示.png", rW / 6, rH / 30);
	loadimage(&传送门提示掩码图, L"resource\\Delivery\\传送提示掩码图.png", rW / 6, rH / 30);
	loadimage(&死亡提示, L"resource\\死亡提示.png", rW / 6 * 4, rH / 2);
	loadimage(&死亡提示掩码图, L"resource\\死亡提示掩码图.png", rW / 6 * 4, rH / 2);
	loadimage(&石砖变铁砖[1], L"resource\\石变铁始.png", B, B);
	loadimage(&石砖变铁砖[2], L"resource\\石变铁中.png", B, B);
	loadimage(&石砖变铁砖[3], L"resource\\石变铁末.png", B, B);
	loadimage(&铁砖变石砖[1], L"resource\\铁变石始.png", B, B);
	loadimage(&铁砖变石砖[2], L"resource\\铁变石中.png", B, B);
	loadimage(&铁砖变石砖[3], L"resource\\铁变石末.png", B, B);
	黑球.load(B, B);
	红球.load(B, B);
	传送门.load(125, 125);
}

bool CreateList()		//创建链表
{
	char filename[100];
	sprintf_s(filename, 100,"resource\\Level\\%d.txt", level);
	char s;
	FILE* fp;
	fopen_s(&fp, filename, "r");
	if (fp == NULL)
	{
		cout << "关卡文件打开失败" << endl;
		exit(0);
	}

	CommonBrick* CommonBrick_tmp = CommonBrick_head;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head;
	SpecialBrick* IronBrick_tmp = IronBrick_head;
	GlassBrick* GlassBrick_tmp = GlassBrick_head;
	BlastBall* RedBall_tmp = RedBall_head;
	BlastBall* BlackBall_tmp = BlackBall_head;
	MoveBall* PurpleBall_tmp = PurpleBall_head;
	MoveBall* GreyBall_tmp = GreyBall_head;
	WaterBall* WaterBall_tmp = WaterBall_head;
	Bullet* Bullet_tmp = Bullet_head;

	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 31; j++)
		{
			s = fgetc(fp);
			if (s == 'q' || s == 'w') InitList(CommonBrick_tmp, j, i, s);
			else if (s == 'e') InitList(StoneBrick_tmp, j, i, s);
			else if (s == 'r') InitList(IronBrick_tmp, j, i, s);
			else if (s == 't') InitList(GlassBrick_tmp, j, i, s);
			else if (s == 'y') InitList(RedBall_tmp, j, i, s);
			else if (s == 'u') InitList(BlackBall_tmp, j, i, s);
			else if (s == 'i') InitList(PurpleBall_tmp, j, i, s);
			else if (s == 'o') InitList(PurpleBall_tmp, j, i, s);
			else if (s == 'p') InitList(GreyBall_tmp, j, i, s);
			else if (s == 'a') InitList(GreyBall_tmp, j, i, s);
			else if (s == 's') InitList(WaterBall_tmp, j, i, s);
			else if (s == '*')
			{
				delivery.x = B * j + sW;
				delivery.y = B * i + sH;
			}
			else if (s == 'g')
			{
				Lead start;
				lead = start;
				lead.x = B * j + sW;
				lead.y = B * i + sH;
			}
		}
		fseek(fp, 2L, 1);
	}
	fclose(fp);
	return true;
}

bool Draw()
{
	putimage(0, 0, &背景);

	CommonBrick* CommonBrick_tmp = CommonBrick_head->next;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	SpecialBrick* IronBrick_tmp = IronBrick_head->next;
	GlassBrick* GlassBrick_tmp = GlassBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;
	MoveBall* PurpleBall_tmp = PurpleBall_head->next;
	MoveBall* GreyBall_tmp = GreyBall_head->next;
	WaterBall* WaterBall_tmp = WaterBall_head->next;
	Bullet* Bullet_tmp = Bullet_head->next;

	while (CommonBrick_tmp)
	{
		if (CommonBrick_tmp->s == 'q') putimage(CommonBrick_tmp->x, CommonBrick_tmp->y, &普通砖[0]);
		if (CommonBrick_tmp->s == 'w') putimage(CommonBrick_tmp->x, CommonBrick_tmp->y, &普通砖[1]);
		CommonBrick_tmp = CommonBrick_tmp->next;
	}
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->invert) putimage(StoneBrick_tmp->x, StoneBrick_tmp->y, &石砖变铁砖[StoneBrick_tmp->index / 200]);
		else putimage(StoneBrick_tmp->x, StoneBrick_tmp->y, &石砖);
		StoneBrick_tmp = StoneBrick_tmp->next;
	}
	while (IronBrick_tmp)
	{
		if (IronBrick_tmp->invert) putimage(IronBrick_tmp->x, IronBrick_tmp->y, &铁砖变石砖[IronBrick_tmp->index / 200]);
		else putimage(IronBrick_tmp->x, IronBrick_tmp->y, &铁砖);
		IronBrick_tmp = IronBrick_tmp->next;
	}
	while (GlassBrick_tmp)
	{
		putimage(GlassBrick_tmp->x, GlassBrick_tmp->y, &玻璃砖);
		GlassBrick_tmp = GlassBrick_tmp->next;
	}
	while (RedBall_tmp)
	{
		红球.put(RedBall_tmp->x, RedBall_tmp->y);
		//putimage(RedBall_tmp->x, RedBall_tmp->y, &红球);
		RedBall_tmp = RedBall_tmp->next;
	}
	while (BlackBall_tmp)
	{
		黑球.put(BlackBall_tmp->x, BlackBall_tmp->y);
		//putimage(BlackBall_tmp->x, BlackBall_tmp->y, &黑球);
		BlackBall_tmp = BlackBall_tmp->next;
	}
	while (PurpleBall_tmp)
	{
		putimage(PurpleBall_tmp->x, PurpleBall_tmp->y, &紫球);
		PurpleBall_tmp = PurpleBall_tmp->next;
	}
	while (GreyBall_tmp)
	{
		putimage(GreyBall_tmp->x, GreyBall_tmp->y, &灰球);
		GreyBall_tmp = GreyBall_tmp->next;
	}
	while (WaterBall_tmp)
	{
		putimage(WaterBall_tmp->x, WaterBall_tmp->y, &水球掩码图[((WaterBall_tmp->index / 40) % WaterBall_NUM) + 1], SRCAND);
		putimage(WaterBall_tmp->x, WaterBall_tmp->y, &水球[((WaterBall_tmp->index / 40) % WaterBall_NUM) + 1], SRCPAINT);
		WaterBall_tmp = WaterBall_tmp->next;
	}
	while (Bullet_tmp)
	{
		putimage(Bullet_tmp->x, Bullet_tmp->y, &子弹掩码图, SRCAND);
		putimage(Bullet_tmp->x, Bullet_tmp->y, &子弹, SRCPAINT);
		Bullet_tmp = Bullet_tmp->next;
	}

	//传送门.put(delivery.x, delivery.y);
	传送门.rotate(0.4, delivery.x, delivery.y);

	if (lead.f == -1 && !lead.death)
		putimage(lead.x, lead.y, &角色[0]);
	if (lead.f == 1 && !lead.death)
		putimage(lead.x, lead.y, &角色[1]);

	if (lead.deli)
	{
		putimage(delivery.x - 55, delivery.y - 25, &传送门提示掩码图, SRCAND);
		putimage(delivery.x - 55, delivery.y - 25, &传送门提示, SRCPAINT);
	}
	
	LOGFONT f_tip;                                 //字体变量
	gettextstyle(&f_tip);						   // 获取当前字体设置

	f_tip.lfHeight = rH / 20;			           // 设置字体高度
	_tcscpy_s(f_tip.lfFaceName, _T("微软雅黑"));   // 设置字体华文彩云
	f_tip.lfQuality = ANTIALIASED_QUALITY;		   // 设置输出效果为抗锯齿  
	settextstyle(&f_tip);						   // 设置字体样式
	settextcolor(RGB(75,183,197));                 // 设置字体颜色 RGB(128, 42, 42)
	setbkmode(TRANSPARENT);                        // 设置字体背景为透明
	for (auto& i : tips) {
		i.draw();
	}
	return true;
}

char GetPosition(double x, double y)
{
	CommonBrick* CommonBrick_tmp = NULL;
	SpecialBrick* StoneBrick_tmp = NULL;
	SpecialBrick* IronBrick_tmp = NULL;
	GlassBrick* GlassBrick_tmp = NULL;
	BlastBall* RedBall_tmp = NULL;
	BlastBall* BlackBall_tmp = NULL;
	MoveBall* PurpleBall_tmp = NULL;
	MoveBall* GreyBall_tmp = NULL;
	WaterBall* WaterBall_tmp = NULL;

	if (Traverse(CommonBrick_head, CommonBrick_tmp, x, y)) return 'q';
	if (Traverse(StoneBrick_head, StoneBrick_tmp, x, y)) return 'e';
	if (Traverse(IronBrick_head, IronBrick_tmp, x, y)) return 'r';
	if (Traverse(GlassBrick_head, GlassBrick_tmp, x, y)) return 't';
	if (Traverse(RedBall_head, RedBall_tmp, x, y)) return 'y';
	if (Traverse(BlackBall_head, BlackBall_tmp, x, y)) return 'u';
	if (Traverse(PurpleBall_head, PurpleBall_tmp, x, y)) return 'i';
	if (Traverse(GreyBall_head, GreyBall_tmp, x, y)) return 'p';
	if (Traverse(WaterBall_head, WaterBall_tmp, x, y)) return 's';

	if (x >= delivery.x && x < delivery.x + Delivery_B && y >= delivery.y && y < delivery.y + Delivery_B) return '*';

	if (x <= 0 || x >= rW || y <= 0 || y >= rH) return 'X';

	return '0';
}

bool Lead::Judge()
{
	char awa, aww, dwd, dww, asa, ass, dsd, dss;//八个像素点位用于移动与跳跃下落
	char aw, as, dw, ds;//四个像素点位用于判断死亡
	char asd, dsa;//两个像素点位用于判断跌倒

	awa = GetPosition(x - 1, y);
	aww = GetPosition(x, y - 1);
	dwd = GetPosition(x + Lead_B + 1, y);
	dww = GetPosition(x + Lead_B, y - 1);
	asa = GetPosition(x - 1, y + Lead_B);
	ass = GetPosition(x, y + Lead_B + 1);
	dsd = GetPosition(x + Lead_B + 1, y + Lead_B);
	dss = GetPosition(x + Lead_B, y + Lead_B + 1);

	aw = GetPosition(x, y);
	as = GetPosition(x + Lead_B, y);
	dw = GetPosition(x, y + Lead_B);
	ds = GetPosition(x + Lead_B, y + Lead_B);

	asd = GetPosition(x + 1, y + Lead_B);
	dsa = GetPosition(x + Lead_B - 1, y + Lead_B);

	if (dwd == 'q' || dwd == 'e' || dwd == 'r' || dwd == 't' || dwd == 'X' || dsd == 'q' || dsd == 'e' || dsd == 'r' || dsd == 't' || dsd == 'X' || lead.x + Lead_B > rW - sW)
		d = false;
	else d = true;

	if (awa == 'q' || awa == 'e' || awa == 'r' || awa == 't' || awa == 'X' || asa == 'q' || asa == 'e' || asa == 'r' || asa == 't' || asa == 'X' || lead.x < sW)
		a = false;
	else a = true;

	if (dww == 'q' || dww == 'e' || dww == 'r' || dww == 't' || aww == 'q' || aww == 'e' || aww == 'r' || aww == 't')
		w = false;
	else w = true;

	if (dss == 'q' || dss == 'e' || dss == 'r' || dss == 't' || ass == 'q' || ass == 'e' || ass == 'r' || ass == 't')
		s = false;
	else s = true;

	//传送判断
	if (awa == '*' || asa == '*' || dwd == '*' || dsd == '*' || aww == '*' || ass == '*' || dww == '*' || dss == '*')
		deli = true;
	else deli = false;

	//死亡判断
	if (aw == 'y' || as == 'y' || dw == 'y' || ds == 'y') death = true;
	if (aw == 'u' || as == 'u' || dw == 'u' || ds == 'u') death = true;
	if (aw == 'i' || as == 'i' || dw == 'i' || ds == 'i') death = true;
	if (aw == 'p' || as == 'p' || dw == 'p' || ds == 'p') death = true;
	if (aw == 's' || as == 's' || dw == 's' || ds == 's') death = true;
	if (y >= rH) death = true;

	//跌倒判断
	if (asd == 'q' || asd == 'e' || asd == 'r' || asd == 't' || dsa == 'q' || dsa == 'e' || dsa == 'r' || dsa == 't')
		tum = true;
	else tum = false;

	return true;
}

bool Lead::FireCD()
{
	if (clock() - start > cd)
		fire = true;
	return true;
}

bool Lead::FireBullets()     //返回0表示没有发射子弹
{
	start = clock();
	Bullet* Bullet_tmp = new Bullet;
	Bullet_tmp->x = x;
	Bullet_tmp->y = y;
	Bullet_tmp->f = f;
	Bullet_tmp->next = Bullet_head->next;
	Bullet_head->next = Bullet_tmp;
	fire = false;
	return true;
}

bool BlastBall::Explode(int unit1, int unit2) // 红球u1 = 1, u2 = 2, 黑球u1 = u2 = 3;
{
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;

	if (!StoneBrick_tmp) return false;
	unit1 *= B;
	unit2 *= B;
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->x >= x - unit1 && StoneBrick_tmp->x <= x + unit1 && StoneBrick_tmp->y >= y - unit1 && StoneBrick_tmp->y <= y + unit1)
			StoneBrick_tmp->del = true;
		StoneBrick_tmp = StoneBrick_tmp->next;
	}

	while (RedBall_tmp)
	{
		if (RedBall_tmp->x >= x - unit1 && RedBall_tmp->x <= x + unit1 && RedBall_tmp->y >= y - unit1 && RedBall_tmp->y <= y + unit1)
			RedBall_tmp->del = RedBall_tmp->ex = true;
		RedBall_tmp = RedBall_tmp->next;
	}

	while (BlackBall_tmp)
	{
		if (BlackBall_tmp->x >= x - unit1 && BlackBall_tmp->x <= x + unit1 && BlackBall_tmp->y >= y - unit1 && BlackBall_tmp->y <= y + unit1)
			BlackBall_tmp->del = BlackBall_tmp->ex = true;
		BlackBall_tmp = BlackBall_tmp->next;
	}

	if (lead.x >= x - unit2 && lead.x <= x + unit2 && lead.y >= y - unit1 && lead.y <= y + unit1)
		lead.death = true;
	return true;
}


bool NaturalLaw()
{
	CommonBrick* CommonBrick_tmp = CommonBrick_head->next;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	SpecialBrick* IronBrick_tmp = IronBrick_head->next;
	GlassBrick* GlassBrick_tmp = GlassBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;
	MoveBall* PurpleBall_tmp = PurpleBall_head->next;
	MoveBall* GreyBall_tmp = GreyBall_head->next;
	WaterBall* WaterBall_tmp = WaterBall_head->next;
	Bullet* Bullet_tmp = Bullet_head->next;

	//主角
	lead.v_y = lead.v_y - lead.g;

	if (!lead.s)
	{
		if (lead.v_y < 0)
		{
			lead.v_y = 0;
			lead.p = 2;
		}
	}

	if (!lead.w)
	{
		lead.v_y = 0;
		lead.y++;//脱离上方墙体
	}

	//上边界限制
	if (lead.y <= sH)
	{
		lead.v_y = 0;
		lead.y++;
	}

	lead.y = lead.y - lead.v_y;

	if (lead.tum)//自动扶起
		lead.y--;

	if (lead.death)
	{
		death.play_once();
		putimage(200, 200, &死亡提示掩码图, SRCAND);
		putimage(200, 200, &死亡提示, SRCPAINT);
		FlushBatchDraw();
		while (1)
		{
			ExMessage msg;
			peekmessage(&msg, 2 );
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == 'r' || msg.vkcode == 'R')
				{
					longjmp(play, 1);
				}
			}
			if (_kbhit()) // Esc键特殊处理，否则出现不稳定问题
			{
				int k = _getch();
				if (k == 27) // ESC键
				{
					game = false;
				}
			}
		}
	}

	//子弹
	while (Bullet_tmp)
	{
		if (Bullet_tmp->del == true) {
			Delete(Bullet_head, Bullet_tmp);
			break;
		}
		else Bullet_tmp->x += Bullet_tmp->v * Bullet_tmp->f;
		Bullet_tmp = Bullet_tmp->next;
	}

	//紫球
	while (PurpleBall_tmp)
	{
		if (PurpleBall_tmp->fly) {
			MoveBall* FloatBall = PurpleBall_tmp;
			PurpleBall_tmp = PurpleBall_tmp->next;
			FloatBall->showy(IronBrick_head, PurpleBall_head, 'r');
			continue;
		}
		else PurpleBall_tmp->move();
		PurpleBall_tmp = PurpleBall_tmp->next;
	}

	//灰球
	while (GreyBall_tmp)
	{
		if (GreyBall_tmp->fly) {
			MoveBall* FloatBall = GreyBall_tmp;
			GreyBall_tmp = GreyBall_tmp->next;
			FloatBall->showy(StoneBrick_head, GreyBall_head, 'e');
			continue;
		}
		else GreyBall_tmp->move();
		GreyBall_tmp = GreyBall_tmp->next;
	}

	//铁砖
	while (IronBrick_tmp)
	{
		if (IronBrick_tmp->invert) {
			IronBrick_tmp->index += 4;
			if (IronBrick_tmp->index >= 800) {
				Increase(StoneBrick_head, IronBrick_tmp->x, IronBrick_tmp->y);
				SpecialBrick* cur = IronBrick_tmp;
				IronBrick_tmp = IronBrick_tmp->next;
				Delete(IronBrick_head, cur);
				continue;
			}
		}
		IronBrick_tmp = IronBrick_tmp->next;
	}

	//石砖
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->invert)
		{
			StoneBrick_tmp->index += 5;
			if (StoneBrick_tmp->index >= 800)
			{
				Increase(IronBrick_head, StoneBrick_tmp->x, StoneBrick_tmp->y);
				SpecialBrick* cur = StoneBrick_tmp;
				StoneBrick_tmp = StoneBrick_tmp->next;
				Delete(StoneBrick_head, cur);
				continue;
			}
		}
		if (StoneBrick_tmp->del) {
			SpecialBrick* cur = StoneBrick_tmp;
			StoneBrick_tmp = StoneBrick_tmp->next;
			Delete(StoneBrick_head, cur);
			continue;
		}
		StoneBrick_tmp = StoneBrick_tmp->next;
	}

	//红球
	while (RedBall_tmp)
	{
		if (RedBall_tmp->ex) RedBall_tmp->Explode(1, 2);
		if (RedBall_tmp->del)
		{
			explode.play_once();
			Delete(RedBall_head, RedBall_tmp);
			break;
		}
		RedBall_tmp = RedBall_tmp->next;
	}

	//黑球
	while (BlackBall_tmp)
	{
		if (BlackBall_tmp->ex) BlackBall_tmp->Explode(3, 3);
		if (BlackBall_tmp->del)
		{
			explode.play_once();
			Delete(BlackBall_head, BlackBall_tmp);
			break;
		}
		BlackBall_tmp = BlackBall_tmp->next;
	}

	//玻璃砖
	while (GlassBrick_tmp)
	{
		if (GlassBrick_tmp->del)
		{
			Delete(GlassBrick_head, GlassBrick_tmp);
			break;
		}
		GlassBrick_tmp = GlassBrick_tmp->next;
	}

	//水球
	while (WaterBall_tmp)
	{
		WaterBall_tmp->index++;
		WaterBall_tmp = WaterBall_tmp->next;
	}

	//传送门
	delivery.index++;

	return true;

}

bool FreeAllList()
{
	FreeList(CommonBrick_head);
	FreeList(StoneBrick_head);
	FreeList(IronBrick_head);
	FreeList(GlassBrick_head);
	FreeList(RedBall_head);
	FreeList(BlackBall_head);
	FreeList(PurpleBall_head);
	FreeList(GreyBall_head);
	FreeList(WaterBall_head);
	FreeList(Bullet_head);
	return true;
}

bool Lead::Move()
{
	if (isKeyPressed(68) && d)
	{
		if (isKeyPressed(65) && a)
		{
			x = x - v_x;
			f = -1;
		}
		x = x + v_x;
		f = 1;
	}
	else if (isKeyPressed(65) && a)
	{
		if (isKeyPressed(68) && d)
		{
			x = x + v_x;
			f = 1;
		}
		x = x - v_x;
		f = -1;
	}
	return true;
}

bool Lead::Jump()
{
	y--;
	v_y = v0;
	p--;
	return true;
}

bool MoveBall::showy(SpecialBrick* special_head, MoveBall* moveball_head, char c)
{
	char aw, dw;
	y -= 0.5;
	aw = GetPosition(x, y - 1);
	dw = GetPosition(x + B - 1, y - 1);
	if ((aw == '0' && dw == '0') || (aw == 's' && dw == 's')) return false;
	else {
		if (aw == c || dw == c) {
			SpecialBrick* topl = NULL, * topr = NULL;
			if (Traverse(special_head, topl, x, y - 1)) topl->invert = true;
			if (Traverse(special_head, topr, x + B - 1, y - 1)) topr->invert = true;
		}
		Delete(moveball_head, this);
	}
	return true;
}

bool MoveBall::move()
{
	char awa, asa, dwd, dsd;
	x += v_move * t;
	double a = x, a1 = y + B + 0.1, d = x + B;
	awa = GetPosition(a, y);
	dwd = GetPosition(d, y);
	asa = GetPosition(a, a1);
	dsd = GetPosition(d, a1);
	if ((awa != '0' && awa != '*' && awa != 'i' && awa != 's' && awa != 'p'))
	{
		t = -t;
		return false;
	}
	else if ((dwd != '0' && dwd != '*' && dwd != 'i' && dwd != 's' && dwd != 'p'))
	{
		t = -t;
		return false;
	}
	else if (asa == '0' || dsd == '0')
	{
		t = -t;
		return false;
	}
	return true;

}

bool Bullet_Judge()
{
	CommonBrick* CommonBrick_tmp = NULL;
	SpecialBrick* StoneBrick_tmp = NULL;
	SpecialBrick* IronBrick_tmp = NULL;
	GlassBrick* GlassBrick_tmp = NULL;
	BlastBall* RedBall_tmp = NULL;
	BlastBall* BlackBall_tmp = NULL;
	MoveBall* PurpleBall_tmp = NULL;
	MoveBall* GreyBall_tmp = NULL;
	WaterBall* WaterBall_tmp = NULL;
	Bullet* Bullet_tmp = Bullet_head->next;

	int dx, dy[2] = { 0,1 };

	while (Bullet_tmp)
	{
		dx = Bullet_tmp->f;
		if (dx < 0) dx++;
		for (int i = 0; i < 2; ++i)
		{
			char sym = '0';
			double x = Bullet_tmp->x + dx * Bullet_B;     //方便表示子弹坐标
			double y = Bullet_tmp->y + dy[i] * Bullet_B;
			//确定子弹状态
			if (Traverse(CommonBrick_head, CommonBrick_tmp, x, y)) sym = 'q';
			else if (Traverse(StoneBrick_head, StoneBrick_tmp, x, y)) sym = 'e';
			else if (Traverse(IronBrick_head, IronBrick_tmp, x, y)) sym = 'r';
			else if (Traverse(GlassBrick_head, GlassBrick_tmp, x, y)) sym = 't';
			else if (Traverse(RedBall_head, RedBall_tmp, x, y)) sym = 'y';
			else if (Traverse(BlackBall_head, BlackBall_tmp, x, y)) sym = 'u';
			else if (Traverse(PurpleBall_head, PurpleBall_tmp, x, y)) sym = 'i';
			else if (Traverse(GreyBall_head, GreyBall_tmp, x, y)) sym = 'p';
			else if (Traverse(WaterBall_head, WaterBall_tmp, x, y)) sym = 's';
			//判断
			if (sym == '0');
			else if (sym == 'q' || sym == 'w' || sym == 'e' || sym == 'r') Bullet_tmp->del = true;
			else if (sym == 't') GlassBrick_tmp->del = Bullet_tmp->del = true;
			else if (sym == 'y') RedBall_tmp->del = RedBall_tmp->ex = Bullet_tmp->del = true;
			else if (sym == 'u') BlackBall_tmp->del = BlackBall_tmp->ex = Bullet_tmp->del = true;
			else if (sym == 'i' || sym == 'o') PurpleBall_tmp->fly = Bullet_tmp->del = true;
			else if (sym == 'p' || sym == 'a') GreyBall_tmp->fly = Bullet_tmp->del = true;
			else if (sym == 's');
		}
		Bullet_tmp = Bullet_tmp->next;
	}
	return true;
}

bool Key_Detection()//按键检测
{
	ExMessage msg;
	peekmessage(&msg, 2);

	if (msg.message == WM_KEYDOWN)
	{
		if (msg.vkcode == 'R' || msg.vkcode == 'r')
			longjmp(play, 1);
		else if ((msg.vkcode == 'J' || msg.vkcode == 'j') && lead.fire)
		{
			fire.play_once();
			lead.FireBullets();
		}
		else if ((msg.vkcode == 'K' || msg.vkcode == 'k' || msg.vkcode == ' ') && lead.p)
		{
			jump.play_once();
			lead.Jump();
		}
		else if ((msg.vkcode == 'L' || msg.vkcode == 'l') && lead.deli)
		{
			level++;
			longjmp(play, 1);
		}
	}
	if (_kbhit()) // Esc键特殊处理，否则出现不稳定问题
	{
		int k = _getch();
		if (k == 27)//ESC键
		{
			game = false;
		}
	}
	return true;
}

void down_shift_once()
{
	static int key = 0;
	key++;
	if (key == 1)
	{
		keybd_event(VK_SHIFT, 0, 0, 0);
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	}
}

std::vector<Tip> outtext(int level) {
	TCHAR k[1024];
	char _buf[1024];
	wchar_t buf[1024]={L'\0'};
	wchar_t text[1024];
	char addr[1024];
	sprintf_s(addr, 1024, "resource\\Level\\%d.txt", level);
	std::vector<Tip> tips;
	int x, y;
	FILE* file;
	fopen_s(&file, addr, "r");
	if (file == NULL) {
		printf("file open error!");
		exit(1);
	}
	fread(_buf, 1, 1024, file);
	MultiByteToWideChar(CP_ACP, 0, _buf,-1,buf,1024); // 
	buf[1023] = L'\0';
	for (unsigned int i = 0; buf[i] != L'&'; i++) {
		if (buf[i] == L'#') {
			unsigned int j = ++i;
			while (buf[i++] != L'#');
			wcsncpy(text, &buf[j], i - j - 1);
			text[i - j - 1] = L'\0';
			x = _wtoi(&buf[i]);
			while (buf[i++] != L'#');
			y = _wtoi(&buf[i]);
			tips.push_back(Tip(x, y, text));
			while (1) {
				while (buf[i++] !=L'#') {
					if (buf[i] == L'&')
					{
						fclose(file);
						return std::move(tips);
					}	
				}
				unsigned int j = i;
				while (buf[i++] != L'#');
				wcsncpy(text, &buf[j], i - j - 1);
				text[i - j - 1] = L'\0';
			
				x = _wtoi(&buf[i]);
				while (buf[i++] != L'#');
				y = _wtoi(&buf[i]);
			
				tips.push_back(Tip(x, y, text));
			}
		}
	}
	fclose(file);
	return std::move(tips);
}

void Play(int x) {
	game = true; //判断游戏是否结束

	down_shift_once();
	MyMouse::SetStyle('n');
	play_bgm.seek_to_start();
	play_bgm.play();
	level = x;
	LoadResource();//加载资源

	setjmp(play);//起始标志

	FreeAllList();

	//创建链表
	CreateList();

	tips = outtext(level);

	clock_t st, ed;
	double spt, fps = 0.003, tim;	//0.004	越小越快

	while (1)
	{
		st = clock();

		//给标志变量赋值
		lead.FireCD();
		lead.Judge();
		lead.Move();

		Bullet_Judge();

		//根据标志变量计算
		NaturalLaw();

		Key_Detection();

		BeginBatchDraw();
		cleardevice();
		Draw();
		FlushBatchDraw();

		if (!game)
		{
			EndBatchDraw();
			FreeAllList();
			play_bgm.pause();
			longjmp(set_level, 1);
			break;
		}

		ed = clock();
		spt = double(ed - st) / CLOCKS_PER_SEC;
		tim = (fps - spt) * 1000;
		if (tim > 0) Sleep(tim);
	}
}