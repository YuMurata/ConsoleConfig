#include"../include/ConsoleConfig.h"

int main()
{
	Display disp(10,10,TEXT("‚È‚ñ‚Å‚à‚¢‚¢‚©‚ç‘‚­‚ñ‚¾"));
	tstring str;
	while (1)
	{
		
		//ConsoleConfig::Generate()->SetColor(ConsoleConfig::Color::BLUE,ConsoleConfig::Color::YELLOW);
		ConsoleConfig::Generate()->PosWrite(disp);
		
		ConsoleConfig::Generate()->Update();

		auto buff=ConsoleConfig::Generate()->PosRead(COORD{ 10,15 });
		if (!buff.empty())
		{
			str = buff;
		}
		ConsoleConfig::Generate()->PosWrite(Display{ COORD{10,20},str.c_str() });

	}
	
    return 0;
}

