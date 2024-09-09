#include "includes.hpp"
#include <menu/Menu.hpp>

Menu* pMenu = new Menu;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    if (!pMenu->setup())
    {
        pMenu->destroy();
        ExitProcess(0);
    }
	
	return 0;
}