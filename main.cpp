#include "Game_Menu.h"
#include "Game_SetLevel.h"
#include "Game_Play.h"
#include "Game_Setting.h"
# include "setjmp.h"

jmp_buf menu;
jmp_buf set_level;


int main() {
	setjmp(menu);
	switch (Menu()) {
	case 1:
		setjmp(set_level);
		Play(SetLevel());
		break;
	case 2:
		Setting();
		break;
	case 3:
		exit(0);
	}
	return 0;
}