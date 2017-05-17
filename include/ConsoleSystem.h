#pragma once

#include<Windows.h>

struct ConsoleSystem
{
	static void SetCursorPos(const HANDLE &output,const COORD &coord)
	{
		SetConsoleCursorPosition(output, coord);
	}

};