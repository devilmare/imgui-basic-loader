#pragma once
#include <includes.hpp>

namespace window {
	inline ImVec2 pos{ 500, 500 };
	inline ImVec2 size{ 400, 270 };
	inline DWORD  flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	inline const char* title = "Loader base";
	inline bool is_open = true;
	inline char szUsername[56] = "";
	inline char szPassword[56] = "";
}

class Menu {
public:
	int setup();
	void destroy();
	void render();
};

extern Menu* pMenu;