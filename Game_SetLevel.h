#pragma once
#include <vector>
#include "graphics.h"
#include "Tools.h"

class MyButton;

class myMouse :public MyMouse {
public:
	static int get_level();
	static void change_style(int level);
};

enum class ButtonFlag {
	lock,
	open,
};

class MyButton {
private:
	double x = 0;
	double y = 0;
	int level;
	ButtonFlag flag = ButtonFlag::lock;
	IMAGE img;
	IMAGE lock;
	const wchar_t* text = nullptr;
public:
	static std::vector<MyButton> buttons;
	MyButton() = default;
	void draw() const;
	static void get_coord();
	static void init_button();
	static void draw_buttons();
	friend static int myMouse::get_level();
	static void draw_Return_buttons();
};

int SetLevel();


