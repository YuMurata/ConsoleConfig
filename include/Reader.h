#pragma once

#include"Config.h"
#include<algorithm>

class Reader
{
private:
	HANDLE input;
	HANDLE output;

	void Write(const tstring &str)
	{
		DWORD length = lstrlen(str.c_str());
		WriteConsole(this->output, str.c_str(), length, &length, NULL);
	}

	template<typename Pre1,typename Pre2>
	tstring PreRead(const Pre1 &loop_func, const Pre2 &body)
	{
		INPUT_RECORD input_record;
		DWORD buff;

		tstring ret;

		if (ReadConsoleInput(this->input, &input_record, 1, &buff))
		{
			auto KeyEvent = [&input_record]()
			{
				auto key_event = input_record.EventType == KEY_EVENT;
				auto key_down = input_record.Event.KeyEvent.bKeyDown;
				auto ret = key_event&&key_down;

				return ret;
			};

			if (KeyEvent())
			{
				while (loop_func(input_record))
				{
					bool is_shift = false;
					if (KeyEvent())
					{
						auto temp = body(&input_record);
						if (*temp.c_str() == VK_BACK)
						{
							if (!ret.empty())
							{
								ret.pop_back();
							}
						}
						else
						{
							ret += temp;
						}
					}
					ReadConsoleInput(this->input, &input_record, 1, &buff);
				}
			}
		}

		FlushConsoleInputBuffer(this->input);

		return ret;
	}

public:
	Reader(const HANDLE &input, const HANDLE &output)
		:input(input), output(output) {}

	tstring Line(const bool &visible=true)
	{
			auto loop_func = [](const INPUT_RECORD &input_record)
			{
				auto key_enter = input_record.Event.KeyEvent.wVirtualKeyCode == VK_RETURN;
				return !key_enter;
			};
		

			auto body = [this, &visible](INPUT_RECORD *input_record)
			{
				DWORD buff;
				tstring ret;
				
				auto word = input_record->Event.KeyEvent.wVirtualKeyCode;
				if (visible)
				{
					if (word == VK_BACK)
					{
						CONSOLE_SCREEN_BUFFER_INFOEX info{ sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
						GetConsoleScreenBufferInfoEx(this->output, &info);
						
						DWORD written;
						info.dwCursorPosition.X -= 1;
					
						FillConsoleOutputCharacter(
							this->output,
							' ',
							1,
							info.dwCursorPosition,
							&written
						);
						SetConsoleCursorPosition(this->output, info.dwCursorPosition);
						ret = word;
					}
					if(0x30<=word&&word<=0x5a)
					{
						auto is_shift = input_record->Event.KeyEvent.dwControlKeyState == SHIFT_PRESSED;
						ret = is_shift ? toupper(word) : tolower(word);

						this->Write(ret);
					}
				}

				return ret;
			};

		auto ret = this->PreRead(loop_func,body);

		return ret;
	}

	tstring Word(const bool &visible = false)
	{
		auto loop_func = [](const INPUT_RECORD &input_handle)
		{
			return false;
		};

		auto body = [this, &visible](INPUT_RECORD *input_record)
		{
			tstring ret;

			ret =tolower(input_record->Event.KeyEvent.wVirtualKeyCode);
			if (visible)
			{
				this->Write(ret);
			}

			return ret;
		};

		auto ret = this->PreRead(loop_func,body);
		return ret;
	}

	void SetCursorPos(const COORD &coord)
	{
		SetConsoleCursorPosition(this->output, coord);
	}

	tstring PosLine(const COORD &coord)
	{
		this->SetCursorPos(coord);
		return this->Line();
	}
};