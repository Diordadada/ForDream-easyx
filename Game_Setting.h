#pragma once
#include <windows.h>

#include <iostream>

void Setting();

struct bn* create(int x, int y, int w, int h, const wchar_t* pt, int a);

struct bnj* create(int x, int y, int w, int h, int t, wchar_t* pt);

struct bnj* create1(int x, int y, int w, int h, const wchar_t* pt);    //初始化按键按钮  坐标长宽音量 

struct bn1* create(int x, int y, int s, const wchar_t* pt);

struct bn     //按钮
{
	int x;      // 坐标  长宽
	int y;
	int w;
	int h;
	int t;
	COLORREF co[5];        //颜色
	wchar_t* pt;                //文字
};

struct bn1     //字按钮
{
	int x;      // 坐标 大小
	int y;
	int s;
	COLORREF co;        //颜色
	wchar_t* pt;                //文字
};

struct bnj     //按键按钮
{
	int x;      // 坐标  长宽
	int y;
	int w;
	int h;
	COLORREF co[5];        //颜色
	wchar_t pt;                //文字
	wchar_t* p;                //文字
};

void draww(struct bn1* bp);

void drawbn(struct bn* bp, int  zh, int zw, int zl);

void drawj(struct bnj* bp, int  zh, int zw, int zl);

void drawj1(struct bnj* bp, int  zh, int zw, int zl);      //画按键按钮

int mbn1(struct bn* bp, MOUSEMSG m);

int mbn(struct bnj* bp, MOUSEMSG m);

int dlickb(struct bnj* bp, MOUSEMSG m);

int dlick(struct bn* bp, MOUSEMSG m);
