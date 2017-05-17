#pragma once
#include"Config.h"


struct Display
{
	COORD coord;
	tstring text;

	Display() {}
	Display(const COORD &coord, const tstring &text)
		:coord(coord), text(text) {}
	Display(const short &x, const short &y, const tstring &text)
		:Display(COORD{ x,y }, text) {}
};