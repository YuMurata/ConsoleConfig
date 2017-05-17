#pragma once

#include"Config.h"
#include"Display.h"
#include<vector>
#include<memory>
#include"../../Ring/Ring/Ring.h"
#include<string>
#include<conio.h>

#include"Reader.h"



class ConsoleConfig
{
private:
	static void InputDeleter(HANDLE handle)
	{
		if (handle != INVALID_HANDLE_VALUE) {
			CloseHandle(handle);
		}
	}


	pRing<HANDLE> output_handle;
	std::unique_ptr<HANDLE,decltype(&InputDeleter)> input_handle;

	ConsoleConfig()
		:input_handle(nullptr,InputDeleter)
	{
		using namespace std;

		CONSOLE_CURSOR_INFO info;
		info.dwSize = 1;
		info.bVisible = FALSE;


		vector<pRing<HANDLE>> buff;
		auto deleater = [](Ring<HANDLE> *obj)
		{
			CloseHandle(obj->content);
		};

		for (int i = 0; i < 2; ++i)
		{
			HANDLE content;

			content =
				CreateConsoleScreenBuffer
				(
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
				);
			SetConsoleCursorInfo(content, &info);

			pRing<HANDLE> ptr(new Ring<HANDLE>, deleater);
			ptr->content = content;

			buff.push_back(ptr);
		}

		this->output_handle = MakeRing(buff);

		auto input_deleter = [](HANDLE *obj)
		{
			CloseHandle(*obj);
		};

		auto handle=  CreateFile
		(
			TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0
		);

		unique_ptr<HANDLE,decltype(&InputDeleter)> temp_ptr(new HANDLE(handle), InputDeleter);
		this->input_handle = move(temp_ptr);
		
		SetConsoleActiveScreenBuffer(this->output_handle->content);
	}

	void ErrorDisp()
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);
		// lpMsgBuf 内のすべての挿入シーケンスを処理する。
		// ...
		// 文字列を表示する。
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK | MB_ICONINFORMATION);
		// バッファを解放する。
		LocalFree(lpMsgBuf);
	}

public:
	static std::unique_ptr<ConsoleConfig>& Generate()
	{
		struct Impl :public ConsoleConfig
		{
			Impl() {}
		};

		static std::unique_ptr<ConsoleConfig> obj = std::make_unique<Impl>();
		return obj;
	}

	void SetCursorPos(const COORD &coord)
	{
		SetConsoleCursorPosition(this->output_handle->Next()->content, coord);
	}

	void SetCursorPos(const short &x, const short &y)
	{
		this->SetCursorPos(COORD{ x,y });
	}

	void Write(const tstring &str, const HANDLE &output)
	{
		DWORD length=lstrlen(str.c_str());
		WriteConsole(output, str.c_str(), length, &length, NULL);
	}
	void Write(const tstring &str)
	{
		this->Write(str, this->output_handle->Next()->content);
	}

	tstring LineRead(const bool &visible = true)
	{
		return Reader(*this->input_handle, this->output_handle->content).Line(visible);
	}

	tstring WordRead(const bool &visible=false)
	{
		return Reader(*this->input_handle, this->output_handle->content).Word(visible);
	}

	tstring PosRead(const COORD &coord)
	{
		return Reader(*this->input_handle, this->output_handle->content).PosLine(coord);
	}

	void PosWrite(const Display &disp)
	{
		this->SetCursorPos(disp.coord);
		this->Write(disp.text);
	}

	void Swap()
	{
		this->output_handle = this->output_handle->Next();
		SetConsoleActiveScreenBuffer(this->output_handle->content);
	}

	void Clear()
	{
		CONSOLE_SCREEN_BUFFER_INFOEX info{sizeof(CONSOLE_SCREEN_BUFFER_INFOEX)};
		if (!GetConsoleScreenBufferInfoEx(this->output_handle->Next()->content, &info))
		{
			this->ErrorDisp();
		}

		DWORD written;

		FillConsoleOutputCharacter(
			this->output_handle->Next()->content,
			' ',
			info.dwSize.X * info.dwSize.Y,
			COORD{ 0,0 },
			&written
		);
	}

	void Update()
	{
		this->Swap();
		this->Clear();
	}

	enum struct Color
	{
		BLACK = 0x00,
		DARK_BLUE = 0x01,
		DARK_GREEN = 0x02,
		DARK_CYAN = 0x03,
		DARK_RED = 0x04,
		DARK_VIOLET = 0x05,
		DARK_YELLOW = 0x06,
		GRAY = 0x07,
		LIGHT_GRAY = 0x08,
		BLUE = 0x09,
		GREEN = 0x0a,
		CYAN = 0x0b,
		RED = 0x0c,
		VIOLET = 0x0d,
		YELLOW = 0x0e,
		WHITE = 0x0f,
		INTENSITY = 0x08,		//	高輝度マスク
		RED_MASK = 0x04,
		GREEN_MASK = 0x02,
		BLUE_MASK = 0x01,
	};

	void SetColor(const Color &col)
	{
		WORD attr = 0;
		if (static_cast<int>(col) & static_cast<int>(Color::INTENSITY))
			attr |= FOREGROUND_INTENSITY;
		if (static_cast<int>(col) & static_cast<int>(Color::RED_MASK))
			attr |= FOREGROUND_RED;
		if (static_cast<int>(col) & static_cast<int>(Color::GREEN_MASK))
			attr |= FOREGROUND_GREEN;
		if (static_cast<int>(col) & static_cast<int>(Color::BLUE_MASK))
			attr |= FOREGROUND_BLUE;
		SetConsoleTextAttribute(this->output_handle->Next()->content, attr);
	}

	void SetColor(const Color &fg, const Color &bg)
	{
		WORD attr = 0;
		
		if (static_cast<int>(fg) & static_cast<int>(Color::INTENSITY))
			attr |= FOREGROUND_INTENSITY;
		if (static_cast<int>(fg) & static_cast<int>(Color::RED_MASK))
			attr |= FOREGROUND_RED;
		if (static_cast<int>(fg) & static_cast<int>(Color::GREEN_MASK))
			attr |= FOREGROUND_GREEN;
		if (static_cast<int>(fg) & static_cast<int>(Color::BLUE_MASK))
			attr |= FOREGROUND_BLUE;

		if (static_cast<int>(bg) & static_cast<int>(Color::INTENSITY))
			attr |= BACKGROUND_INTENSITY;
		if (static_cast<int>(bg) & static_cast<int>(Color::RED_MASK))
			attr |= BACKGROUND_RED;
		if (static_cast<int>(bg) & static_cast<int>(Color::GREEN_MASK))
			attr |= BACKGROUND_GREEN;
		if (static_cast<int>(bg) & static_cast<int>(Color::BLUE_MASK))
			attr |= BACKGROUND_BLUE;
		SetConsoleTextAttribute(this->output_handle->Next()->content, attr);
	}
};