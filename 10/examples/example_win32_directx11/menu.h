#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

class Menu
{
public:
    ImFont* nameCheat; ImFont* nameCheat2;
    ImFont* nameTable;
    ImFont* generalTable;
    ImFont* leftTable;
    ImFont* Icons;
    ImFont* font;
    ImFont* smallFont;
    ImFont* introFont;
};

extern Menu menu;
