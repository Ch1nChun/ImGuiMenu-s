#include "imgui.h"

struct ImFontAtlas;
struct ImFontBuilderIO;

enum ImGuiFreeTypeBuilderFlags
{
    ImGuiFreeTypeBuilderFlags_NoHinting = 1 << 0,   
    ImGuiFreeTypeBuilderFlags_NoAutoHint = 1 << 1,  
    ImGuiFreeTypeBuilderFlags_ForceAutoHint = 1 << 2,
    ImGuiFreeTypeBuilderFlags_LightHinting = 1 << 3,
    ImGuiFreeTypeBuilderFlags_MonoHinting = 1 << 4,  
    ImGuiFreeTypeBuilderFlags_Bold = 1 << 5,
    ImGuiFreeTypeBuilderFlags_Oblique = 1 << 6,  
    ImGuiFreeTypeBuilderFlags_Monochrome = 1 << 7,   
    ImGuiFreeTypeBuilderFlags_LoadColor = 1 << 8,   
    ImGuiFreeTypeBuilderFlags_Bitmap = 1 << 9    
};

namespace ImGuiFreeType
{
    IMGUI_API const ImFontBuilderIO*    GetBuilderForFreeType();
    IMGUI_API void                      SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data = NULL);

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    static inline bool                  BuildFontAtlas(ImFontAtlas* atlas, unsigned int flags = 0) { atlas->FontBuilderIO = GetBuilderForFreeType(); atlas->FontBuilderFlags = flags; return atlas->Build(); }
#endif
}