#pragma once
#include<Windows.h>
#include"Display.h"
#include"SceneItem.h"
#include<memory>

class Cursor
{
private:
	std::unique_ptr<struct CursorImpl> pimpl;

	Cursor();

	enum struct Direction
	{
		UP,
		LEFT,
		DOWN,
		RIGHT,
		NaN,
	};

	struct Mover;

public:
	~Cursor()=default;

	static Cursor& Generate();

	void Init(const COORD &coord, const COORD &item,const std::string &cursor);

	void Input();

	void Move(const COORD &delta);

	void Move(const ItemMatrix &matrix);

	bool Enter();

	COORD GetItem()const;

	Displayer Disp();
};
