#include "imgui_edited.hpp"
#include <vector>

using namespace ImGui;

namespace edited
{

    const char* keys[] =
    {
        "-",
        "M1",
        "M2",
        "CN",
        "M3",
        "M4",
        "M5",
        "-",
        "BACK",
        "TAB",
        "-",
        "-",
        "CLR",
        "ENTER",
        "-",
        "-",
        "SHIFT",
        "CTRL",
        "Menu",
        "Pause",
        "CAPS",
        "KAN",
        "-",
        "JUN",
        "FIN",
        "KAN",
        "-",
        "ESC",
        "CON",
        "NCO",
        "ACC",
        "MAD",
        "SPACE",
        "PGU",
        "PGD",
        "END",
        "HOME",
        "LEFT",
        "UP",
        "RIGHT",
        "DOWN",
        "SEL",
        "PRI",
        "EXE",
        "PRI",
        "INS",
        "DEL",
        "HEL",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "A",
        "B",
        "C",
        "D",
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K",
        "L",
        "M",
        "N",
        "O",
        "P",
        "Q",
        "R",
        "S",
        "T",
        "U",
        "V",
        "W",
        "X",
        "Y",
        "Z",
        "WIN",
        "WIN",
        "APP",
        "-",
        "SLE",
        "NUM0",
        "NUM1",
        "NUM2",
        "NUM3",
        "NUM4",
        "NUM5",
        "NUM6",
        "NUM7",
        "NUM8",
        "NUM9",
        "MUL",
        "ADD",
        "SEP",
        "MIN",
        "DEL",
        "DIV",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "F11",
        "F12",
        "F13",
        "F14",
        "F15",
        "F16",
        "F17",
        "F18",
        "F19",
        "F20",
        "F21",
        "F22",
        "F23",
        "F24",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "NUM",
        "SCR",
        "EQU",
        "MAS",
        "TOY",
        "OYA",
        "OYA",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "SHIFT",
        "SHIFT",
        "CTRL",
        "CTRL",
        "ALT",
        "ALT"
    };

#include <string>

    void RenderTextColor(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align)
    {
        PushFont(font);
        PushStyleColor(ImGuiCol_Text, col);
        RenderTextClipped(p_min, p_max, text, NULL, NULL, align, NULL);
        PopStyleColor();
        PopFont();
    }

    struct key_state
    {
        ImVec4 background, text;
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
    };

    bool Keybind(const char* label, const char* description, int* key)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const float width = (GetContentRegionMax().x - style.WindowPadding.x);

        const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 50));
        const ImRect clickable(window->DC.CursorPos + ImVec2(width - 80, 10), window->DC.CursorPos + ImVec2(width - 10, 40));

        ItemSize(ImRect(rect.Min, rect.Max));
        if (!ImGui::ItemAdd(rect, id)) return false;

        char buf_display[64] = "None";


        bool value_changed = false;
        int k = *key;

        std::string active_key = "";
        active_key += keys[*key];

        if (*key != 0 && g.ActiveId != id) {
            strcpy_s(buf_display, active_key.c_str());
        }
        else if (g.ActiveId == id) {
            strcpy_s(buf_display, "...");
        }

        const ImVec2 label_size = CalcTextSize(buf_display, NULL, true);

        bool hovered = ItemHoverable(rect, id, NULL);

        static std::map<ImGuiID, key_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, key_state() });
            it_anim = anim.find(id);
        }

        it_anim->second.text = ImLerp(it_anim->second.text, g.ActiveId == id ? c::elements::text_active : hovered ? c::elements::text_hov : c::elements::text, ImGui::GetIO().DeltaTime * 6.f);

        window->DrawList->AddRectFilled(clickable.Min, clickable.Max, GetColorU32(c::elements::background), c::elements::rounding);

        RenderTextColor(font::lexend_regular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));

        GetWindowDrawList()->AddRectFilled(rect.Min + ImVec2(width - 80, 0), rect.Max, GetColorU32(c::elements::background), c::elements::rounding);
        GetWindowDrawList()->AddRectFilledMultiColor(rect.Min, rect.Max - ImVec2(80, 0), GetColorU32(c::elements::background, 0.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 0.f), c::elements::rounding);

        window->DrawList->AddRectFilled(clickable.Min, clickable.Max, GetColorU32(c::elements::background_widget), c::elements::rounding);

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));

        RenderTextColor(font::lexend_bold, clickable.Min, clickable.Max, GetColorU32(c::elements::text_active), buf_display, ImVec2(0.5f, 0.5f));

        if (hovered && io.MouseClicked[0])
        {
            if (g.ActiveId != id) {

                memset(io.MouseDown, 0, sizeof(io.MouseDown));
                memset(io.KeysDown, 0, sizeof(io.KeysDown));
                *key = 0;
            }
            ImGui::SetActiveID(id, window);
            ImGui::FocusWindow(window);
        }
        else if (io.MouseClicked[0]) {

            if (g.ActiveId == id)
                ImGui::ClearActiveID();
        }

        if (g.ActiveId == id) {
            for (auto i = 0; i < 5; i++) {
                if (io.MouseDown[i]) {
                    switch (i) {
                    case 0:
                        k = 0x01;
                        break;
                    case 1:
                        k = 0x02;
                        break;
                    case 2:
                        k = 0x04;
                        break;
                    case 3:
                        k = 0x05;
                        break;
                    case 4:
                        k = 0x06;
                        break;
                    }
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
            if (!value_changed) {
                for (auto i = 0x08; i <= 0xA5; i++) {
                    if (io.KeysDown[i]) {
                        k = i;
                        value_changed = true;
                        ImGui::ClearActiveID();
                    }
                }
            }

            if (IsKeyPressedMap(ImGuiKey_Escape)) {
                *key = 0;
                ImGui::ClearActiveID();
            }
            else {
                *key = k;
            }
        }

        return value_changed;
    }

    struct checkp_state
    {
        ImVec4 background;
    };

    bool checkboxP(const char* label, bool* v, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true), pos = window->DC.CursorPos;

        ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);

        ItemSize(size, 0.f);
        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);
        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        static std::map<ImGuiID, checkp_state> anim;
        checkp_state& state = anim[id];

        state.background = ImLerp(state.background, *v ? c::accent : c::elements::background, g.IO.DeltaTime * 6.f);

        ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(state.background, 0.1f), 3.f);
        ImGui::GetWindowDrawList()->AddRect(bb.Min, bb.Max, GetColorU32(state.background), 2.f);

        RenderTextColor(font::lexend_regular, bb.Min, bb.Max - ImVec2(0, 2), GetColorU32(c::elements::text_active), label, ImVec2(0.5, 0.5));

        return pressed;
    }

    struct tab_state
    {
        ImVec4 background, text, icon, shadow;
        float bg_alpha;
    };

    bool edited::Tab(bool selected, const char* icon, const char* label, const char* description, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true), pos = window->DC.CursorPos;

        ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);

        ItemSize(size, 0.f);
        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);

        static std::map<ImGuiID, tab_state> anim;
        tab_state& state = anim[id];

        state.icon = ImLerp(state.icon, selected ? c::accent : hovered ? c::elements::text_hov : c::elements::text, g.IO.DeltaTime * 6.f);
        state.text = ImLerp(state.text, selected ? c::elements::text_active : hovered ? c::elements::text_hov : c::elements::text, g.IO.DeltaTime * 6.f);
        state.bg_alpha = ImLerp(state.bg_alpha, selected ? 1.f : 0.f, g.IO.DeltaTime * 6.f);
        state.shadow = ImLerp(state.shadow, selected ? c::tab::tab_active : c::tab::border, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(c::tab::tab_active, state.bg_alpha), c::elements::rounding);

        RenderTextColor(font::lexend_regular, bb.Min + ImVec2(size.y, 0), bb.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.7));

        GetWindowDrawList()->AddRectFilledMultiColor(bb.Min + ImVec2(size.x / 2, 0), bb.Max, GetColorU32(state.shadow, 0.f), GetColorU32(state.shadow, 1.f), GetColorU32(state.shadow, 1.f), GetColorU32(state.shadow, 0.f), c::elements::rounding);

        RenderTextColor(font::lexend_bold, bb.Min + ImVec2(size.y, 0), bb.Max, GetColorU32(state.text), label, ImVec2(0.0, 0.3));
        RenderTextColor(font::icomoon, bb.Min, bb.Min + ImVec2(size.y, size.y), GetColorU32(state.icon), icon, ImVec2(0.5, 0.5));

        return pressed;
    }

    bool edited::BeginChild(const char* str_id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
    {
        ImGuiID id = GetCurrentWindow()->GetID(str_id);

        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

        return edited::BeginChildEx(str_id, id, size_arg, child_flags, window_flags | ImGuiWindowFlags_AlwaysUseWindowPadding);
    }

    bool edited::BeginChild(ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
    {
        return edited::BeginChildEx(NULL, id, size_arg, child_flags, window_flags);
    }

    bool edited::BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* parent_window = g.CurrentWindow;
        IM_ASSERT(id != 0);

        const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle;
        IM_UNUSED(ImGuiChildFlags_SupportedMask_);
        IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
        IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
        if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
        {
            IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
            IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding) child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
#endif
        if (child_flags & ImGuiChildFlags_AutoResizeX) child_flags &= ~ImGuiChildFlags_ResizeX;
        if (child_flags & ImGuiChildFlags_AutoResizeY) child_flags &= ~ImGuiChildFlags_ResizeY;

        window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar;
        window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);
        if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
        if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0) window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

        if (child_flags & ImGuiChildFlags_FrameStyle)
        {
            PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
            PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
            PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
            PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
            child_flags |= ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding;
            window_flags |= ImGuiWindowFlags_NoMove;
        }

        g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
        g.NextWindowData.ChildFlags = child_flags;

        const ImVec2 size_avail = GetContentRegionAvail();
        const ImVec2 size_default((child_flags & ImGuiChildFlags_AutoResizeX) ? 0.0f : size_avail.x, (child_flags & ImGuiChildFlags_AutoResizeY) ? 0.0f : size_avail.y);
        const ImVec2 size = CalcItemSize(size_arg, size_default.x, size_default.y);
        SetNextWindowSize(size);

   //     GetWindowDrawList()->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + size_arg, GetColorU32(c::accent), 0);

        const char* temp_window_name;

        if (name)
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
        else
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

        const float backup_border_size = g.Style.ChildBorderSize;
        if ((child_flags & ImGuiChildFlags_Border) == 0) g.Style.ChildBorderSize = 0.0f;

        const bool ret = Begin(temp_window_name, NULL, window_flags);

        g.Style.ChildBorderSize = backup_border_size;
        if (child_flags & ImGuiChildFlags_FrameStyle)
        {
            PopStyleVar(3);
            PopStyleColor();
        }

        ImGuiWindow* child_window = g.CurrentWindow;
        child_window->ChildId = id;

        if (child_window->BeginCount == 1) parent_window->DC.CursorPos = child_window->Pos;

        const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
        if (g.ActiveId == temp_id_for_activation) ClearActiveID();
        if (g.NavActivateId == id && !(window_flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
        {
            FocusWindow(child_window);
            NavInitWindow(child_window, false);
            SetActiveID(temp_id_for_activation, child_window);
            g.ActiveIdSource = g.NavInputSource;
        }
        return ret;
    }

    void edited::EndChild()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* child_window = g.CurrentWindow;
        PopStyleVar(2);
        IM_ASSERT(g.WithinEndChild == false);
        IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);

        g.WithinEndChild = true;
        ImVec2 child_size = child_window->Size;
        End();
        if (child_window->BeginCount == 1)
        {
            ImGuiWindow* parent_window = g.CurrentWindow;
            ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
            ItemSize(child_size);
            if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !(child_window->Flags & ImGuiWindowFlags_NavFlattened))
            {
                ItemAdd(bb, child_window->ChildId);
                RenderNavHighlight(bb, child_window->ChildId);

                if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow) RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
            }
            else
            {
                ItemAdd(bb, 0);

                if (child_window->Flags & ImGuiWindowFlags_NavFlattened) parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
            }
            if (g.HoveredWindow == child_window) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
        }
        g.WithinEndChild = false;
        g.LogLinePosY = -FLT_MAX;
    }

    struct check_state
    {
        ImVec4 background, text;
        float alpha, mark_pos;
    };

    bool edited::Checkbox(const char* label, const char* description, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const float square_sz = 20.f;
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect rect(pos, pos + ImVec2(w, 50));

        ItemSize(rect, 0.f);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        static std::map<ImGuiID, check_state> anim;
        check_state& state = anim[id];

        state.background = ImLerp(state.background, *v ? c::accent : c::elements::background_widget, g.IO.DeltaTime * 6.f);
        state.alpha = ImLerp(state.alpha, *v ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        state.mark_pos = ImClamp(state.mark_pos + (35.f * g.IO.DeltaTime * (*v ? -3.f : 3.f)), 0.f, 35.f);

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background), c::elements::rounding);

        RenderTextColor(font::lexend_regular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));

        GetWindowDrawList()->AddRectFilled(rect.Min + ImVec2(w - 40, 0), rect.Max, GetColorU32(c::elements::background), c::elements::rounding);
        GetWindowDrawList()->AddRectFilledMultiColor(rect.Min, rect.Max - ImVec2(20, 0), GetColorU32(c::elements::background, 0.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 0.f), c::elements::rounding);

        GetWindowDrawList()->AddRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU32(state.background), 2.f);
        GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU32(state.background, 0.5f), 2.f);

        PushClipRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), true);
        RenderCheckMark(GetWindowDrawList(), rect.Max - ImVec2(35 - (square_sz / 2) / 2, 35 - (square_sz / 2 + state.mark_pos) / 2), GetColorU32(c::elements::mark, state.alpha), square_sz / 2);
        PopClipRect();

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));

        return pressed;
    }

    struct slider_state
    {
        ImVec4 text;
        float slow, blick;
    };

    bool SliderScalar(const char* label, const char* description, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos + ImVec2(180, 29), window->DC.CursorPos + ImVec2(w - 10, 39));
        const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, 50));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ItemSize(rect, 0.f);
        if (!ItemAdd(frame_bb, id, &frame_bb)) return false;

        if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

        bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(frame_bb, id, &hovered, &held, NULL);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);

        static std::map<ImGuiID, slider_state> anim;
        slider_state& state = anim[id];

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background), c::elements::rounding);

        RenderTextColor(font::lexend_regular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));

        GetWindowDrawList()->AddRectFilled(rect.Min + ImVec2(w - 140, 0), rect.Max, GetColorU32(c::elements::background), c::elements::rounding);
        GetWindowDrawList()->AddRectFilledMultiColor(rect.Min, rect.Max - ImVec2(120, 0), GetColorU32(c::elements::background, 0.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 0.f), c::elements::rounding);

        GetWindowDrawList()->AddRectFilled(frame_bb.Min, frame_bb.Max, GetColorU32(c::elements::background_widget), 2.f);

        ImRect grab_bb;
        const bool value_changed = SliderBehavior(ImRect(frame_bb.Min - ImVec2(0, 0), frame_bb.Max + ImVec2(6, 0)), id, data_type, p_data, p_min, p_max, format, flags, & grab_bb);

        if (value_changed) MarkItemEdited(id);

        state.slow = ImLerp(state.slow, grab_bb.Min.x - (frame_bb.Min.x + 5), g.IO.DeltaTime * 25.f);
        GetWindowDrawList()->AddRectFilled(frame_bb.Min, ImVec2(state.slow + (frame_bb.Min.x + 9), grab_bb.Max.y), GetColorU32(c::accent), 2.f);

        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));
        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(0, 7), rect.Max - ImVec2(15, 10), GetColorU32(c::elements::text_active), value_buf, ImVec2(1.0, 0.0f));

        return value_changed;
    }

    bool SliderFloat(const char* label, const char* description, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, description, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool SliderInt(const char* label, const char* description, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, description, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    struct begin_state
    {
        ImVec4 background, text, outline;
        float open, alpha, combo_size = 0.f, shadow_opticaly;
        bool opened_combo = false, hovered = false;
        float arrow_roll;
    };

    static float CalcMaxPopupHeightFromItemCount(int items_count)
    {
        ImGuiContext& g = *GImGui;
        if (items_count <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    int rotation_start_index;
    void ImRotateStart()
    {
        rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
    }

    ImVec2 ImRotationCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

        const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
    }

    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
    {
        float s = sin(rad), c = cos(rad);
        center = ImRotate(center, s, c) - center;

        auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
    }

    static const char* Items_ArrayGetter(void* data, int idx)
    {
        const char* const* items = (const char* const*)data;
        return items[idx];
    }

    bool BeginCombo(const char* label, const char* description, const char* preview_value, int val, bool multi, ImGuiComboFlags flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();

        g.NextWindowData.ClearFlags();
        if (window->SkipItems) return false;

        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const float w = ((GetContentRegionMax().x - style.WindowPadding.x));
        const float y_size = 50;

        const ImRect bb(window->DC.CursorPos + ImVec2(0, 0), window->DC.CursorPos + ImVec2(w, 50));
        const ImRect rect(window->DC.CursorPos + ImVec2(180, (50 - 30) / 2), window->DC.CursorPos + ImVec2(w - 10, (50 + 30) / 2));

        const ImRect total_bb(bb.Min, bb.Max);
        ItemSize(bb, 0.f);

        if (!ItemAdd(bb, id, &bb)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held);

        static std::map<ImGuiID, begin_state> anim;
        begin_state& state = anim[id];

        if (hovered && g.IO.MouseClicked[0] || state.opened_combo && g.IO.MouseClicked[0] && !state.hovered) state.opened_combo = !state.opened_combo;

        state.arrow_roll = ImLerp(state.arrow_roll, state.opened_combo ? -1.f : 1.f, g.IO.DeltaTime * 6.f);
        state.text = ImLerp(state.text, state.opened_combo ? c::elements::text_active : hovered ? c::elements::text_hov : c::elements::text, g.IO.DeltaTime * 6.f);
        state.combo_size = ImLerp(state.combo_size, state.opened_combo ? (val * 33) + 17 : 0.f, g.IO.DeltaTime * 12.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(c::elements::background), c::elements::rounding);

        RenderTextColor(font::lexend_regular, bb.Min + ImVec2(10, 0), bb.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));

        GetWindowDrawList()->AddRectFilled(bb.Min + ImVec2(w - 140, 0), bb.Max, GetColorU32(c::elements::background), c::elements::rounding);
        GetWindowDrawList()->AddRectFilledMultiColor(bb.Min, bb.Max - ImVec2(120, 0), GetColorU32(c::elements::background, 0.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 0.f), c::elements::rounding);

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background_widget), c::elements::rounding);

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text_active), preview_value, ImVec2(0.0, 0.5));
        GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(30, 30), rect.Max, GetColorU32(c::elements::background_widget), 2.f);

        GetWindowDrawList()->AddRectFilledMultiColor(rect.Min, rect.Max - ImVec2(30, 0), GetColorU32(c::elements::background_widget, 0.f), GetColorU32(c::elements::background_widget, 1.f), GetColorU32(c::elements::background_widget, 1.f), GetColorU32(c::elements::background_widget, 0.f), 2.f);
        
        RenderTextColor(font::icomoon_widget, rect.Min, rect.Max - ImVec2(7, 0), GetColorU32(c::accent), "z", ImVec2(1.0, 0.5));
        RenderTextColor(font::lexend_bold, bb.Min + ImVec2(10, 0), bb.Max, GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));

        GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(y_size, y_size), rect.Max, GetColorU32(state.background), c::elements::rounding);

        if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
        {
            state.opened_combo = false;
            state.combo_size = 0.f;
        }

        if (!state.opened_combo && state.combo_size < 2.f) return false;

        ImGui::SetNextWindowPos(ImVec2(rect.Min.x, rect.Max.y + 5));
        ImGui::SetNextWindowSize(ImVec2(rect.GetWidth(), state.combo_size));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;

        PushStyleColor(ImGuiCol_WindowBg, c::elements::background);
        PushStyleColor(ImGuiCol_Border, c::elements::background_widget);
        PushStyleVar(ImGuiStyleVar_WindowRounding, c::elements::rounding);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);

        bool ret = Begin(label, NULL, window_flags);

        PopStyleVar(3);
        PopStyleColor(2);

        state.hovered = IsWindowHovered();

        if (multi && state.hovered && g.IO.MouseClicked[0]) state.opened_combo = false;

        return true;
    }

    void EndCombo()
    {
        End();
    }

    void MultiCombo(const char* label, const char* description, bool variable[], const char* labels[], int count)
    {
        ImGuiContext& g = *GImGui;

        std::string preview = "None";

        for (auto i = 0, j = 0; i < count; i++)
        {
            if (variable[i])
            {
                if (j)
                    preview += (", ") + (std::string)labels[i];
                else
                    preview = labels[i];

                j++;
            }
        }

        if (BeginCombo(label, description, preview.c_str(), count, NULL, NULL))
        {
            for (auto i = 0; i < count; i++)
            {
                PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 15));
                PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
                edited::Selectable(labels[i], &variable[i], ImGuiSelectableFlags_DontClosePopups);
                PopStyleVar(2);
            }
            End();
        }

        preview = ("None");
    }

    bool BeginComboPreview()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

        if (window->SkipItems || !(g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)) return false;

        IM_ASSERT(g.LastItemData.Rect.Min.x == preview_data->PreviewRect.Min.x && g.LastItemData.Rect.Min.y == preview_data->PreviewRect.Min.y);

        if (!window->ClipRect.Overlaps(preview_data->PreviewRect)) return false;

        preview_data->BackupCursorPos = window->DC.CursorPos;
        preview_data->BackupCursorMaxPos = window->DC.CursorMaxPos;
        preview_data->BackupCursorPosPrevLine = window->DC.CursorPosPrevLine;
        preview_data->BackupPrevLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
        preview_data->BackupLayout = window->DC.LayoutType;
        window->DC.CursorPos = preview_data->PreviewRect.Min + g.Style.FramePadding;
        window->DC.CursorMaxPos = window->DC.CursorPos;
        window->DC.LayoutType = ImGuiLayoutType_Horizontal;
        window->DC.IsSameLine = false;
        PushClipRect(preview_data->PreviewRect.Min, preview_data->PreviewRect.Max, true);

        return true;
    }

    void EndComboPreview()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

        ImDrawList* draw_list = window->DrawList;
        if (window->DC.CursorMaxPos.x < preview_data->PreviewRect.Max.x && window->DC.CursorMaxPos.y < preview_data->PreviewRect.Max.y)
            if (draw_list->CmdBuffer.Size > 1)
            {
                draw_list->_CmdHeader.ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 2].ClipRect;
                draw_list->_TryMergeDrawCmds();
            }
        PopClipRect();
        window->DC.CursorPos = preview_data->BackupCursorPos;
        window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, preview_data->BackupCursorMaxPos);
        window->DC.CursorPosPrevLine = preview_data->BackupCursorPosPrevLine;
        window->DC.PrevLineTextBaseOffset = preview_data->BackupPrevLineTextBaseOffset;
        window->DC.LayoutType = preview_data->BackupLayout;
        window->DC.IsSameLine = false;
        preview_data->PreviewRect = ImRect();
    }

    static const char* Items_SingleStringGetter(void* data, int idx)
    {
        const char* items_separated_by_zeros = (const char*)data;
        int items_count = 0;
        const char* p = items_separated_by_zeros;
        while (*p)
        {
            if (idx == items_count)
                break;
            p += strlen(p) + 1;
            items_count++;
        }
        return *p ? p : NULL;
    }

    bool Combo(const char* label, const char* description, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
    {
        ImGuiContext& g = *GImGui;

        const char* preview_value = NULL;
        if (*current_item >= 0 && *current_item < items_count)
            preview_value = getter(user_data, *current_item);

        if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
            SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

        if (!BeginCombo(label, description, preview_value, items_count, ImGuiComboFlags_None, NULL)) return false;

        bool value_changed = false;
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 15));
        for (int i = 0; i < items_count; i++)
        {
            const char* item_text = getter(user_data, i);
            if (item_text == NULL)
                item_text = "*Unknown item*";

            PushID(i);
            const bool item_selected = (i == *current_item);
            if (edited::Selectable(item_text, item_selected) && *current_item != i)
            {
                value_changed = true;
                *current_item = i;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }
        PopStyleVar();

        EndCombo();

        if (value_changed)
            MarkItemEdited(g.LastItemData.ID);

        return value_changed;
    }

    bool Combo(const char* label, const char* description, int* current_item, const char* const items[], int items_count, int height_in_items)
    {
        const bool value_changed = Combo(label, description, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
        return value_changed;
    }

    bool Combo(const char* label, const char* description, int* current_item, const char* items_separated_by_zeros, int height_in_items)
    {
        int items_count = 0;
        const char* p = items_separated_by_zeros;
        while (*p)
        {
            p += strlen(p) + 1;
            items_count++;
        }
        bool value_changed = Combo(label, description, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
        return value_changed;
    }

    struct select_state
    {
        ImVec4 text;
        float opticaly;
    };

    bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        ImGuiID id = window->GetID(label);
        ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
        const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
        const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
        if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth)) size.x = ImMax(label_size.x, max_x - min_x);

        const ImVec2 text_min = pos;
        const ImVec2 text_max(min_x + size.x, pos.y + size.y);

        ImRect bb(min_x, pos.y, text_max.x, text_max.y);
        if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
        {
            const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
            const float spacing_y = style.ItemSpacing.y;
            const float spacing_L = IM_TRUNC(spacing_x * 0.50f);
            const float spacing_U = IM_TRUNC(spacing_y * 0.50f);
            bb.Min.x -= spacing_L;
            bb.Min.y -= spacing_U;
            bb.Max.x += (spacing_x - spacing_L);
            bb.Max.y += (spacing_y - spacing_U);
        }

        const float backup_clip_rect_min_x = window->ClipRect.Min.x;
        const float backup_clip_rect_max_x = window->ClipRect.Max.x;
        if (span_all_columns)
        {
            window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
            window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
        }

        const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
        const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
        if (span_all_columns)
        {
            window->ClipRect.Min.x = backup_clip_rect_min_x;
            window->ClipRect.Max.x = backup_clip_rect_max_x;
        }

        if (!item_add) return false;

        const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
        if (disabled_item && !disabled_global) BeginDisabled();

        if (span_all_columns && window->DC.CurrentColumns) PushColumnsBackground();
        else if (span_all_columns && g.CurrentTable) TablePushBackgroundChannel();

        ImGuiButtonFlags button_flags = 0;
        if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
        if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
        if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
        if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
        if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
        if ((flags & ImGuiSelectableFlags_AllowOverlap) || (g.LastItemData.InFlags & ImGuiItemFlags_AllowOverlap)) { button_flags |= ImGuiButtonFlags_AllowOverlap; }

        const bool was_selected = selected;
        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

        if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
            if (g.NavJustMovedToId == id)  selected = pressed = true;

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
        {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
            {
                SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
                g.NavDisableHighlight = true;
            }
        }
        if (pressed) MarkItemEdited(id);

        if (selected != was_selected)  g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;


        if (g.NavId == id) RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

        if (span_all_columns && window->DC.CurrentColumns) PopColumnsBackground();
        else if (span_all_columns && g.CurrentTable) TablePopBackgroundChannel();

        static std::map<ImGuiID, select_state> anim;
        select_state& state = anim[id];

        state.text = ImLerp(state.text, selected ? c::accent : c::elements::text, g.IO.DeltaTime * 6.f);
        state.opticaly = ImLerp(state.opticaly, selected ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max + ImVec2(0, 1), GetColorU32(c::elements::background_widget, state.opticaly), 2.f);

        RenderTextColor(font::lexend_bold, text_min + ImVec2(1, 1), text_max, GetColorU32(state.text), label, ImVec2(0.f, 0.5f));

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup)) CloseCurrentPopup();

        if (disabled_item && !disabled_global) EndDisabled();

        return pressed;
    }

    bool Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
    {
        if (Selectable(label, *p_selected, flags, size_arg))
        {
            *p_selected = !*p_selected;
            return true;
        }
        return false;
    }

    int ParseFormatPrecision(const char* fmt, int default_precision)
    {
        int precision = default_precision;
        while ((fmt = strchr(fmt, '%')) != NULL)
        {
            fmt++;
            if (fmt[0] == '%') { fmt++; continue; } // Ignore "%%"
            while (*fmt >= '0' && *fmt <= '9')
                fmt++;
            if (*fmt == '.')
            {
                precision = atoi(fmt + 1);
                if (precision < 0 || precision > 10)
                    precision = default_precision;
            }
            break;
        }
        return precision;
    }

    float RoundScalar(float value, int decimal_precision)
    {
        static const float min_steps[10] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
        float min_step = (decimal_precision >= 0 && decimal_precision < 10) ? min_steps[decimal_precision] : powf(10.0f, (float)-decimal_precision);
        bool negative = value < 0.0f;
        value = fabsf(value);
        float remainder = fmodf(value, min_step);
        if (remainder <= min_step * 0.5f)
            value -= remainder;
        else
            value += (min_step - remainder);
        return negative ? -value : value;
    }

    static inline float SliderBehaviorCalcRatioFromValue(float v, float v_min, float v_max, float power, float linear_zero_pos)
    {
        if (v_min == v_max)
            return 0.0f;

        const bool is_non_linear = (power < 1.0f - 0.00001f) || (power > 1.0f + 0.00001f);
        const float v_clamped = (v_min < v_max) ? ImClamp(v, v_min, v_max) : ImClamp(v, v_max, v_min);
        if (is_non_linear)
        {
            if (v_clamped < 0.0f)
            {
                const float f = 1.0f - (v_clamped - v_min) / (ImMin(0.0f, v_max) - v_min);
                return (1.0f - powf(f, 1.0f / power)) * linear_zero_pos;
            }
            else
            {
                const float f = (v_clamped - ImMax(0.0f, v_min)) / (v_max - ImMax(0.0f, v_min));
                return linear_zero_pos + powf(f, 1.0f / power) * (1.0f - linear_zero_pos);
            }
        }

        return (v_clamped - v_min) / (v_max - v_min);
    }

    struct range_state
    {
        ImVec4 text;
        float slow1, slow2, alpha_text, text_offset;
    };

    bool RangeSliderBehavior(const ImRect& frame_bb, ImGuiID id, float* v1, float* v2, float v_min, float v_max, float power, int decimal_precision, ImGuiSliderFlags flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();
        const ImGuiStyle& style = g.Style;

        const bool is_non_linear = (power < 1.0f - 0.00001f) || (power > 1.0f + 0.00001f);
        const bool is_horizontal = (flags & ImGuiSliderFlags_Vertical) == 0;

        const float grab_padding = 2.0f;
        const float slider_sz = is_horizontal ? (frame_bb.GetWidth() - grab_padding * 2.0f) : (frame_bb.GetHeight() - grab_padding * 2.0f);
        float grab_sz;
        if (decimal_precision > 0)
            grab_sz = ImMin(style.GrabMinSize, slider_sz);
        else
            grab_sz = ImMin(ImMax(1.0f * (slider_sz / ((v_min < v_max ? v_max - v_min : v_min - v_max) + 1.0f)), style.GrabMinSize), slider_sz);
        const float slider_usable_sz = slider_sz - grab_sz;
        const float slider_usable_pos_min = (is_horizontal ? frame_bb.Min.x : frame_bb.Min.y) + grab_padding + grab_sz * 0.5f;
        const float slider_usable_pos_max = (is_horizontal ? frame_bb.Max.x : frame_bb.Max.y) - grab_padding - grab_sz * 0.5f;

        float linear_zero_pos = 0.0f;
        if (v_min * v_max < 0.0f)
        {
            const float linear_dist_min_to_0 = powf(fabsf(0.0f - v_min), 1.0f / power);
            const float linear_dist_max_to_0 = powf(fabsf(v_max - 0.0f), 1.0f / power);
            linear_zero_pos = linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0);
        }
        else
        {
            linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
        }

        window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, GetColorU32(c::elements::background_widget), 30.f);

        bool value_changed = false;
        if (g.ActiveId == id)
        {
            if (g.IO.MouseDown[0])
            {
                const float mouse_abs_pos = is_horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
                float clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (!is_horizontal)
                    clicked_t = 1.0f - clicked_t;

                float new_value;
                if (is_non_linear)
                {
                    if (clicked_t < linear_zero_pos)
                    {
                        float a = 1.0f - (clicked_t / linear_zero_pos);
                        a = powf(a, power);
                        new_value = ImLerp(ImMin(v_max, 0.0f), v_min, a);
                    }
                    else
                    {
                        float a;
                        if (fabsf(linear_zero_pos - 1.0f) > 1.e-6f)
                            a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                        else
                            a = clicked_t;
                        a = powf(a, power);
                        new_value = ImLerp(ImMax(v_min, 0.0f), v_max, a);
                    }
                }
                else
                {
                    new_value = ImLerp(v_min, v_max, clicked_t);
                }
                new_value = RoundScalar(new_value, decimal_precision);
                if (*v1 != new_value || *v2 != new_value)
                {
                    if (fabsf(*v1 - new_value) < fabsf(*v2 - new_value))
                    {
                        *v1 = new_value;
                    }
                    else
                    {
                        *v2 = new_value;
                    }
                    value_changed = true;
                }
            }
            else
            {
                SetActiveID(0, NULL);
            }
        }

        float grab_t = SliderBehaviorCalcRatioFromValue(*v1, v_min, v_max, power, linear_zero_pos);

        static std::map<ImGuiID, range_state> anim;
        range_state& state = anim[id];

        ImRect grab_bb2, grab_bb1;

        if (!is_horizontal) grab_t = 1.0f - grab_t;
        float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (is_horizontal)
            grab_bb1 = ImRect(ImVec2(grab_pos - grab_sz * 0.5f, frame_bb.Min.y + grab_padding), ImVec2(grab_pos + grab_sz * 0.5f, frame_bb.Max.y - grab_padding));
        else
            grab_bb1 = ImRect(ImVec2(frame_bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f), ImVec2(frame_bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f));

        state.slow1 = ImLerp(state.slow1, grab_bb1.Min.x - frame_bb.Min.x - 1, g.IO.DeltaTime * 25.f);
        grab_t = SliderBehaviorCalcRatioFromValue(*v2, v_min, v_max, power, linear_zero_pos);
        if (!is_horizontal) grab_t = 1.0f - grab_t;

        grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);

        if (is_horizontal)
            grab_bb2 = ImRect(ImVec2(grab_pos - grab_sz * 0.5f, frame_bb.Min.y + grab_padding), ImVec2(grab_pos + grab_sz * 0.5f, frame_bb.Max.y - grab_padding));
        else
            grab_bb2 = ImRect(ImVec2(frame_bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f), ImVec2(frame_bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f));

        state.slow2 = ImLerp(state.slow2, grab_bb2.Min.x - frame_bb.Min.x + 14, g.IO.DeltaTime * 25.f);

        window->DrawList->AddRectFilled(ImVec2(frame_bb.Min.x + state.slow1, frame_bb.Min.y), ImVec2(frame_bb.Min.x + state.slow2, frame_bb.Max.y), GetColorU32(c::accent), 2.f);

        return value_changed;
    }

    bool RangeSliderFloat(const char* label, const char* description, float* v1, float* v2, float v_min, float v_max, const char* display_format, float power)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const float y_size = 50;

        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, y_size));
        const ImRect slider_bb(window->DC.CursorPos + ImVec2(180, 31), window->DC.CursorPos + ImVec2(w - 10, 39));

        if (!ItemAdd(slider_bb, id))
        {
            ItemSize(ImRect(rect.Min, rect.Max));
            return false;
        }

        const bool hovered = ItemHoverable(slider_bb, id, g.LastItemData.InFlags);

        if (hovered) SetHoveredID(id);

        if (!display_format) display_format = "(%.3f, %.3f)";

        ItemSize(ImRect(rect.Min, rect.Max));

        int decimal_precision = ParseFormatPrecision(display_format, 3);

        if (hovered && g.IO.MouseClicked[0])
        {
            SetActiveID(id, window);
            FocusWindow(window);
        }

        static std::map<ImGuiID, range_state> anim;
        range_state& state = anim[id];

        state.text = ImLerp(state.text, IsItemActive() ? c::elements::text_active : hovered ? c::elements::text_hov : c::elements::text, g.IO.DeltaTime * 8.f);
        state.alpha_text = ImLerp(state.alpha_text, IsItemActive() ? 1.f : 0.f, g.IO.DeltaTime * 15.f);

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background), c::elements::rounding);

        RenderTextColor(font::lexend_regular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));

        GetWindowDrawList()->AddRectFilled(rect.Min + ImVec2(w - 140, 0), rect.Max, GetColorU32(c::elements::background), c::elements::rounding);
        GetWindowDrawList()->AddRectFilledMultiColor(rect.Min, rect.Max - ImVec2(120, 0), GetColorU32(c::elements::background, 0.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 1.f), GetColorU32(c::elements::background, 0.f), c::elements::rounding);

        const bool value_changed = RangeSliderBehavior(slider_bb, id, v1, v2, v_min, v_max, power, decimal_precision);

        char value_buf[64];
        const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v1, *v2);

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(0, 7), rect.Max - ImVec2(15, 10), GetColorU32(c::elements::text_active), value_buf, ImVec2(1.0, 0.0f));
        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));

        return value_changed;

    }

    struct quand_state
    {
        float slow0, slow1, tickness;
        int move_y, move_x;
        float move_yy, move_xx;
    };

    bool QuandScalar(const char* label, const char* description, ImGuiDataType data_type, void* p_data0, void* p_data1, const void* p_min0, const void* p_max0, const void* p_min1, const void* p_max1, const char* format0, const char* format1, ImGuiSliderFlags flags)
    {

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const float w = GetContentRegionMax().x - style.WindowPadding.x;

        ImVec2 pos = window->DC.CursorPos;
        const ImRect frame_bb(pos + ImVec2(10, 55), pos + ImVec2(w - 10, 145));
        const ImRect rect(pos, pos + ImVec2(w, 155));
        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

        ItemSize(rect, 0.f);
        if (!ItemAdd(rect, id)) return false;

        if (format0 == NULL) format0 = DataTypeGetInfo(data_type)->PrintFmt;
        if (format1 == NULL) format1 = DataTypeGetInfo(data_type)->PrintFmt;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);

        ImRect grab_bb0, grab_bb1;

        static std::map<ImGuiID, quand_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, quand_state() });
            it_anim = anim.find(id);
        }

        const bool value_changed0 = SliderBehavior(ImRect(frame_bb.Min - ImVec2(7, 0), frame_bb.Max + ImVec2(5, 0)), id, data_type, p_data0, p_min0, p_max0, format0, flags, &grab_bb0);
        const bool value_changed1 = SliderBehavior(ImRect(frame_bb.Min - ImVec2(0, 7), frame_bb.Max + ImVec2(0, 4)), id, data_type, p_data1, p_max1, p_min1, format1, flags | ImGuiSliderFlags_Vertical, &grab_bb1);

        if (value_changed0) MarkItemEdited(id);

        char value_buf0[64];
        char value_buf1[64];
        const char* value_buf_end0 = value_buf0 + DataTypeFormatString(value_buf0, IM_ARRAYSIZE(value_buf0), data_type, p_data0, format0);
        const char* value_buf_end1 = value_buf1 + DataTypeFormatString(value_buf1, IM_ARRAYSIZE(value_buf1), data_type, p_data1, format1);

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background), c::elements::rounding);

        it_anim->second.slow0 = ImLerp(it_anim->second.slow0, grab_bb0.Min.x - (frame_bb.Min.x) + 7, g.IO.DeltaTime * 25.f);
        it_anim->second.slow1 = ImLerp(it_anim->second.slow1, grab_bb1.Min.y - (frame_bb.Min.y) + 7, g.IO.DeltaTime * 25.f);

        GetWindowDrawList()->AddLine(ImVec2(it_anim->second.slow0 + frame_bb.Min.x, grab_bb0.Min.y), ImVec2(it_anim->second.slow0 + frame_bb.Min.x, grab_bb0.Max.y), GetColorU32(c::accent), 0.5f);
        GetWindowDrawList()->AddShadowRect(ImVec2(it_anim->second.slow0 + frame_bb.Min.x, grab_bb0.Min.y), ImVec2(it_anim->second.slow0 + frame_bb.Min.x, grab_bb0.Max.y), GetColorU32(c::accent, 0.5f), 25.f, ImVec2(0, 0), 100.f);

        GetWindowDrawList()->AddLine(ImVec2(grab_bb1.Min.x, it_anim->second.slow1 + frame_bb.Min.y), ImVec2(grab_bb1.Max.x, it_anim->second.slow1 + frame_bb.Min.y), GetColorU32(c::accent), 0.5f);
        GetWindowDrawList()->AddShadowRect(ImVec2(grab_bb1.Min.x, it_anim->second.slow1 + frame_bb.Min.y), ImVec2(grab_bb1.Max.x, it_anim->second.slow1 + frame_bb.Min.y), GetColorU32(c::accent, 0.5f), 25.f, ImVec2(0, 0), 100.f);

        GetWindowDrawList()->AddText(font::lexend_bold, 17, ImVec2(rect.Min.x + 10, rect.Min.y + 10), GetColorU32(c::elements::text_active), label);

        std::string blah0(value_buf0), blah1(value_buf1);

        std::string label_x = "[X, " + blah0 + " Y, " + blah1 + "]";

        GetWindowDrawList()->AddText(font::lexend_bold, 17, ImVec2(rect.Min.x + 10, rect.Min.y + 30), GetColorU32(c::elements::text), description);

        RenderTextColor(font::lexend_bold, rect.Min + ImVec2(0, 7), rect.Max - ImVec2(15, 10), GetColorU32(c::elements::text_active), label_x.c_str(), ImVec2(1.0, 0.0f));

    }

    bool QuandFloat(const char* label, const char* description, float* v0, float* v1, float v_min0, float v_max0, float v_min1, float v_max1, const char* format0, const char* format1, ImGuiSliderFlags flags)
    {
        return QuandScalar(label, description, ImGuiDataType_Float, v0, v1, &v_min0, &v_max0, &v_min1, &v_max1, format0, format1, flags);
    }

    bool QuandInt(const char* label, const char* description, int* v0, int* v1, int v_min0, int v_max0, int v_min1, int v_max1, const char* format0, const char* format1, ImGuiSliderFlags flags)
    {
        return QuandScalar(label, description, ImGuiDataType_S32, v0, v1, &v_min0, &v_max0, &v_min1, &v_max1, format0, format1, flags);
    }

    static void ColorEditRestoreH(const float* col, float* H)
    {
        ImGuiContext& g = *GImGui;
        IM_ASSERT(g.ColorEditCurrentID != 0);
        if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
            return;
        *H = g.ColorEditSavedHue;
    }

    static void ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
    {
        ImGuiContext& g = *GImGui;
        IM_ASSERT(g.ColorEditCurrentID != 0);
        if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) return;

        if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
            *H = g.ColorEditSavedHue;

        if (*V == 0.0f) *S = g.ColorEditSavedSat;
    }

    struct iconbox_state
    {
        float line;
    };

    bool icon_box(const char* icon, ImVec2 size, ImU32 color_bg, ImU32 color_icon, ImU32 color_border)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        const ImVec2 pos = window->DC.CursorPos;
        const ImRect rect(pos, pos + size);
        const ImGuiID id = window->GetID(icon);

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        ItemSize(rect, 0.f);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held, NULL);

        static std::map<ImGuiID, iconbox_state> anim;
        iconbox_state& state = anim[id];

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(color_bg), c::elements::rounding);
        GetWindowDrawList()->AddRect(rect.Min, rect.Max, GetColorU32(color_border), c::elements::rounding);

        RenderTextColor(font::icomoon_widget, rect.Min, rect.Max, GetColorU32(color_icon), icon, ImVec2(0.5, 0.5));

        return pressed;
    }

    bool color_button(const char* name, ImVec2 size, ImU32 color_bg)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        const ImVec2 pos = window->DC.CursorPos;
        const ImRect rect(pos, pos + size);

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(name);

        ItemSize(rect, 0.f);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held, NULL);

        static std::map<ImGuiID, iconbox_state> anim;
        iconbox_state& state = anim[id];

        GetWindowDrawList()->AddCircleFilled(rect.Min + ImVec2(size / 2), size.x / 2, GetColorU32(color_bg), 30.f);
        GetWindowDrawList()->AddShadowCircle(rect.Min + ImVec2(size / 2), size.x / 2, GetColorU32(color_bg), 18.f, ImVec2(0, 0), 30.f);

        GetWindowDrawList()->AddCircle(rect.Min + ImVec2(size / 2), size.x / 3, GetColorU32(c::elements::background, 0.3f), 30.f, 3.f);

        return pressed;
    }



    struct edit_state
    {
        ImVec4 text;
        float alpha = 0.f, alpha_search;
        bool hovered, active;
    };

    bool ColorEdit4(const char* label, const char* description, float col[4], ImGuiColorEditFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float square_sz = 18.f;
        const float w_full = CalcItemWidth();
        const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : square_sz;
        const float w_inputs = w_full - w_button;
        const char* label_display_end = FindRenderedTextEnd(label);
        g.NextItemData.ClearFlags();

        char buf[64];
        static bool search_col = false;

        BeginGroup();
        PushID(label);
        const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
        if (set_current_color_edit_id) g.ColorEditCurrentID = window->IDStack.back();

        const ImGuiColorEditFlags flags_untouched = flags;

        if (flags & ImGuiColorEditFlags_NoInputs) flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

        if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorEditOptionsPopup(col, flags);

        if (!(flags & ImGuiColorEditFlags_DisplayMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
        if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
        if (!(flags & ImGuiColorEditFlags_PickerMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
        if (!(flags & ImGuiColorEditFlags_InputMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
        flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_));
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));

        const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
        const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
        const int components = alpha ? 4 : 3;

        float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
        if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
        else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
        {
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
            ColorEditRestoreHS(col, &f[0], &f[1], &f[2]);
        }
        int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

        bool value_changed = false;
        bool value_changed_as_float = false;

        const ImVec2 pos = window->DC.CursorPos;
        const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
        window->DC.CursorPos.x = pos.x + inputs_offset_x;

        if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
        {
            const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
            const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

            const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
            static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
            static const char* fmt_table_int[3][4] =
            {
                {   "%3d",   "%3d",   "%3d",   "%3d" },
                { "R:%3d", "G:%3d", "B:%3d", "A:%3d" },
                { "H:%3d", "S:%3d", "V:%3d", "A:%3d" }
            };

            static const char* fmt_table_float[3][4] =
            {
                {   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" },
                { "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" },
                { "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }
            };

            const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_DisplayHSV) ? 2 : 1;

            for (int n = 0; n < components; n++)
            {
                if (n > 0) SameLine(0, style.ItemInnerSpacing.x);
                SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

                if (flags & ImGuiColorEditFlags_Float)
                {
                    value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
                    value_changed_as_float |= value_changed;
                }
                else
                {
                    value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
                }
                if (!(flags & ImGuiColorEditFlags_NoOptions)) OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            }
        }

        static std::map<ImGuiID, edit_state> anim;
        edit_state& state = anim[ImGui::GetID(label)];

        ImGuiWindow* picker_active_window = NULL;
        if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
        {
            const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
            window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

            const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
            if (ColorButton("##ColorButton", col_v4, flags, ImVec2(20, 20)))
            {
                if (!(flags & ImGuiColorEditFlags_NoPicker))
                {
                    g.ColorPickerRef = col_v4;
                    OpenPopup("picker");
                    SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
                }
            }

            if (!(flags & ImGuiColorEditFlags_NoOptions)) OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

            if (ItemHoverable(g.LastItemData.Rect, g.LastItemData.ID, NULL) && g.IO.MouseClicked[0] || state.active && !search_col && g.IO.MouseClicked[0] && !state.hovered)
                state.active = !state.active;

            state.alpha_search = ImLerp(state.alpha_search, search_col ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

            PushStyleColor(ImGuiCol_WindowBg, GetColorU32(c::elements::background));
            PushStyleColor(ImGuiCol_Border, GetColorU32(c::elements::background_widget));

            PushStyleVar(ImGuiStyleVar_WindowRounding, c::elements::rounding);
            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
            PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);

            POINT cursorPos;
            COLORREF color;

            if (state.active)
            {
                SetNextWindowPos(g.LastItemData.Rect.GetTR() + ImVec2(-45, -5));

                Begin("picker", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
                {
                    state.hovered = IsWindowHovered();

                    ImVec4 col_v4(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);

                    GetCursorPos(&cursorPos);
                    HDC hdc = GetDC(NULL);
                    color = GetPixel(hdc, cursorPos.x, cursorPos.y);

                    if (search_col)
                    {

                        static DWORD dwTickStart = GetTickCount();
                        if (GetTickCount() - dwTickStart > 150)
                        {
                            col[0] = GetRValue(color) / 255.f;
                            col[1] = GetGValue(color) / 255.f;
                            col[2] = GetBValue(color) / 255.f;
                            dwTickStart = GetTickCount();

                            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) search_col = false;

                        }
                    }
                    
                    if (alpha)
                        ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
                    else
                        ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

                    GetWindowDrawList()->AddText(font::lexend_bold, 17.f, GetCursorScreenPos() - ImVec2(0, 2), GetColorU32(c::elements::text_active), (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel)) ? label : "Color picker");

                    PushFont(font::icomoon_widget2);
                    ImGui::GetWindowDrawList()->AddText(GetCursorScreenPos() + ImVec2(GetContentRegionMax().x - (CalcTextSize("l").x + 15), 0), GetColorU32(c::elements::text_hov), "l");
                    PopFont();

                    ImGui::SetCursorPosY(GetCursorPosY() + 30);

                    picker_active_window = g.CurrentWindow;
                    ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                    ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;

                    SetNextItemWidth(square_sz * 11.5f);
                    value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);

                    if (edited::icon_box("u", ImVec2(35, 35), GetColorU32(c::elements::background_widget), GetColorU32(c::accent), GetColorU32(c::accent, state.alpha_search)))
                    {
                        search_col = true;
                    };

                    ImGui::SameLine(0, 15);

                    if (InputTextEx("v", "HEX COLOR", buf, IM_ARRAYSIZE(buf), ImVec2(!(flags & ImGuiColorEditFlags_NoAlpha) ? 128 : 118, 35), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
                    {
                        value_changed = true;
                        char* p = buf;
                        while (*p == '#' || ImCharIsBlankA(*p)) p++;
                        i[0] = i[1] = i[2] = 0;
                        i[3] = 0xFF;
                        int r;
                        if (alpha)
                            r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
                        else
                            r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
                        IM_UNUSED(r);
                    }

                    ImGui::SameLine(0, 3);

                    static std::vector<float> color_x, color_y, color_z, color_a;

                    static bool add_status = true;

                    if (edited::icon_box(add_status ? "g" : "k", ImVec2(35, 35), GetColorU32(c::elements::background_widget), GetColorU32(c::accent), GetColorU32(c::accent, 0.f)))
                    {

                        if (add_status) {

                            color_x.push_back(col[0]);
                            color_y.push_back(col[1]);
                            color_z.push_back(col[2]);
                            color_a.push_back(col[3]);

                        }
                        else if (color_x.size() > 0) {

                            color_x.pop_back();
                            color_y.pop_back();
                            color_z.pop_back();
                            color_a.pop_back();

                        }
                    };

                    if (IsItemHovered() && GetIO().MouseClicked[1]) add_status = !add_status;

                    for (int i = 0; i < color_x.size(); i++)
                    {
                        std::string number_x = std::to_string(color_x[i]),
                            number_y = std::to_string(color_y[i]),
                            number_z = std::to_string(color_z[i]),
                            number_a = std::to_string(color_a[i]);

                        std::string name_box = std::to_string(i);

                        if (edited::color_button(name_box.c_str(), ImVec2(17, 17), ImColor(color_x[i], color_y[i], color_z[i], color_a[i])))
                        {
                            col[0] = color_x[i], col[1] = color_y[i], col[2] = color_z[i], col[3] = color_a[i];
                        };

                        if ((i + 1) % 7 != 0) ImGui::SameLine(0, 18.f);

                    }

                }
                End();
            }
        }
        PopStyleColor(2);
        PopStyleVar(3);

        state.text = ImLerp(state.text, state.active ? c::elements::text_active : c::elements::text, g.IO.DeltaTime * 6.f);

        if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
        {
            SameLine(0.0f, style.ItemInnerSpacing.x);
            window->DC.CursorPos.x = pos.x - w_button + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full);

            RenderTextColor(font::lexend_regular, pos + ImVec2(10, 0), pos + ImVec2(GetContentRegionMax().x, 50), GetColorU32(c::elements::text), description, ImVec2(0.0, 0.8));
            RenderTextColor(font::lexend_bold, pos + ImVec2(10, 0), pos + ImVec2(GetContentRegionMax().x, 50), GetColorU32(c::elements::text_active), label, ImVec2(0.0, 0.2));

        }

        PopID();
        EndGroup();

        return value_changed;
    }

    static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
    {
        ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
        ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
    }

    struct picker_state
    {
        float hue_bar;
        float alpha_bar;
        float circle;
        ImVec2 circle_move;
    };

    bool ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImDrawList* draw_list = window->DrawList;
        ImGuiStyle& style = g.Style;
        ImGuiIO& io = g.IO;

        const float width = CalcItemWidth();
        g.NextItemData.ClearFlags();

        PushID(label);
        const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
        if (set_current_color_edit_id)
            g.ColorEditCurrentID = window->IDStack.back();
        BeginGroup();

        if (!(flags & ImGuiColorEditFlags_NoSidePreview)) flags |= ImGuiColorEditFlags_NoSmallPreview;

        if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorPickerOptionsPopup(col, flags);

        if (!(flags & ImGuiColorEditFlags_PickerMask_)) flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
        if (!(flags & ImGuiColorEditFlags_InputMask_)) flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_));
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));
        if (!(flags & ImGuiColorEditFlags_NoOptions)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

        int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
        bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
        ImVec2 picker_pos = window->DC.CursorPos;
        float square_sz = GetFrameHeight();
        float bars_width = 15.f;
        float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width));
        float bar0_pos_x = picker_pos.x + sv_picker_size + 10;
        float bar1_pos_x = bar0_pos_x + bars_width + 10;
        float bars_triangles_half_sz = IM_TRUNC(bars_width * 0.20f);

        float backup_initial_col[4];
        memcpy(backup_initial_col, col, components * sizeof(float));

        float H = col[0], S = col[1], V = col[2];
        float R = col[0], G = col[1], B = col[2];
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            ColorEditRestoreHS(col, &H, &S, &V);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }

        bool value_changed = false, value_changed_h = false, value_changed_sv = false;

        InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
        if (IsItemActive())
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            ColorEditRestoreH(col, &H);
            value_changed = value_changed_sv = true;
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions)) OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }

        if (alpha_bar)
        {
            SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
            InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
            if (IsItemActive())
            {
                col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
                value_changed = true;
            }
        }

        if (!(flags & ImGuiColorEditFlags_NoLabel))
        {
            const char* label_display_end = FindRenderedTextEnd(label);
            if (label != label_display_end)
            {
                if ((flags & ImGuiColorEditFlags_NoSidePreview))
                    SameLine(0, style.ItemInnerSpacing.x);
                TextEx(label, label_display_end);
            }
        }

        if (value_changed_h || value_changed_sv)
        {
            if (flags & ImGuiColorEditFlags_InputRGB)
            {
                ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
                g.ColorEditSavedHue = H;
                g.ColorEditSavedSat = S;
                g.ColorEditSavedID = g.ColorEditCurrentID;
                g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
            }
            else if (flags & ImGuiColorEditFlags_InputHSV)
            {
                col[0] = H;
                col[1] = S;
                col[2] = V;
            }
        }

        if (value_changed)
        {
            if (flags & ImGuiColorEditFlags_InputRGB)
            {
                R = col[0];
                G = col[1];
                B = col[2];
                ColorConvertRGBtoHSV(R, G, B, H, S, V);
                ColorEditRestoreHS(col, &H, &S, &V);
            }
            else if (flags & ImGuiColorEditFlags_InputHSV)
            {
                H = col[0];
                S = col[1];
                V = col[2];
                ColorConvertHSVtoRGB(H, S, V, R, G, B);
            }
        }

        const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
        const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
        const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
        const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
        const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

        ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
        ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
        ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha));

        static std::map<ImGuiID, picker_state> anim;
        picker_state& state = anim[ImGui::GetID(label)];

        ImVec2 sv_cursor_pos;

        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white, 5.f);
        draw_list->AddRectFilledMultiColor(picker_pos - ImVec2(1, 1), picker_pos + ImVec2(sv_picker_size + 1, sv_picker_size + 1), 0, 0, col_black, col_black, 5.f);

        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2);
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        for (int i = 0; i < 6; ++i) draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6) - (i == 5 ? 1 : 0)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6) + (i == 0 ? 1 : 0)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1], 10.f, i == 0 ? ImDrawFlags_RoundCornersTop : i == 5 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);

        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        bar0_line_y = ImClamp(bar0_line_y, picker_pos.y + 3.f, picker_pos.y + (sv_picker_size - 13));

        state.hue_bar = ImLerp(state.hue_bar, bar0_line_y + 5, g.IO.DeltaTime * 24.f);
        draw_list->AddCircleFilled(ImVec2(bar0_pos_x + 7.5f, state.hue_bar), 4.5f, col_black, 100.f);

        float sv_cursor_rad = value_changed_sv ? 10.0f : 6.0f;
        int sv_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(sv_cursor_rad);

        state.circle_move = ImLerp(state.circle_move, sv_cursor_pos, g.IO.DeltaTime * 10.f);
        state.circle = ImLerp(state.circle, value_changed_sv ? 6.0f : 4.0f, g.IO.DeltaTime * 24.f);

        draw_list->AddCircle(state.circle_move, state.circle, col_white, sv_cursor_segments, 2.f);

        if (alpha_bar)
        {
            float alpha = ImSaturate(col[3]);
            ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);

            draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, 100.f);

            float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
            bar1_line_y = ImClamp(bar1_line_y, picker_pos.y + 3.f, picker_pos.y + (sv_picker_size - 13));

            state.alpha_bar = ImLerp(state.alpha_bar, bar1_line_y + 5, g.IO.DeltaTime * 24.f);
            draw_list->AddCircleFilled(ImVec2(bar1_pos_x + 7.5f, state.alpha_bar), 4.5f, col_black, 100.f);
        }

        EndGroup();

        if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) value_changed = false;
        if (value_changed) MarkItemEdited(g.LastItemData.ID);

        PopID();

        return value_changed;
    }


    bool ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(desc_id);
        const float default_size = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const float width = GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x;
        const ImRect rect(pos, pos + ImVec2(width, 50));

        const ImRect clickable(rect.Min + ImVec2(width - 35, (50 - 20) / 2), rect.Max - ImVec2(15, (50 - 20) / 2));

        ItemSize(ImRect(rect.Min, rect.Max - ImVec2(0, 0)));
        if (!ItemAdd((flags & ImGuiColorEditFlags_NoLabel) ? clickable : rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(clickable, id, &hovered, &held);

        if (flags & ImGuiColorEditFlags_NoAlpha) flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

        ImVec4 col_rgb = col;
        if (flags & ImGuiColorEditFlags_InputHSV) ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::elements::background), c::elements::rounding);

        GetWindowDrawList()->AddRectFilled(clickable.Min, clickable.Max, GetColorU32(col_rgb), c::elements::rounding);

        RenderColorRectWithAlphaCheckerboard(window->DrawList, clickable.Min, clickable.Max, GetColorU32(col_rgb), ImMin(20, 20) / 2.99f, ImVec2(0.f, 0.f), c::elements::rounding);

        return pressed;
    }

    struct preview_state
    {
        ImVec4 box_color, nick_color, weapon_color, zoom_color, bomb_color, c4_color, money_color, hit_color, hp_color, hp_line_color;
        ImVec2 nick_pos;
    };

    struct OverlayTextElement {
        bool* condition;
        ImVec2 position_offset;
        ImU32 color;
        std::string text;
        ImVec2 measurement_offset;
    };

    void esp_preview(ImTextureID player_preview, bool* nickname, float nick_color[4], bool* weapon, float weapon_color[4], int* hp, float hp_color[4], bool* zoom, float zoom_color[4], bool* bomb, float bomb_color[4], bool* c4, float c4_color[4], bool* money, float money_color[4], bool* hit, float hit_color[4], bool* esp_box, float box_color[4], bool* hp_line, float hp_line_color[4])
    {
        ImGuiWindow* window = GetCurrentWindow();
        ImGuiContext& g = *GImGui;

        const ImGuiID id = window->GetID(player_preview);
        const ImVec2 pos = window->DC.CursorPos;
        const float width = GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x;
        const ImRect player(pos + ImVec2((width - 175) / 2, 20), pos + ImVec2((width + 175) / 2, 322));
        const ImRect box(pos + ImVec2((width - 210) / 2, 20), pos + ImVec2((width + 210) / 2, 327));

        static std::vector<ImVec2> position;

        ItemSize(ImRect(box.Min + ImVec2(0, 0), box.Max + ImVec2(0, 45)));

        static std::map<ImGuiID, preview_state> anim;
        preview_state& state = anim[id];

        state.hp_line_color = ImLerp(state.hp_line_color, ImColor(hp_line_color[0], hp_line_color[1], hp_line_color[2], hp_line_color[3]), g.IO.DeltaTime * 6.f);
        state.weapon_color = ImLerp(state.weapon_color, ImColor(weapon_color[0], weapon_color[1], weapon_color[2], weapon_color[3]), g.IO.DeltaTime * 6.f);
        state.money_color = ImLerp(state.money_color, ImColor(money_color[0], money_color[1], money_color[2], money_color[3]), g.IO.DeltaTime * 6.f);
        state.bomb_color = ImLerp(state.bomb_color, ImColor(bomb_color[0], bomb_color[1], bomb_color[2], bomb_color[3]), g.IO.DeltaTime * 6.f);
        state.zoom_color = ImLerp(state.zoom_color, ImColor(zoom_color[0], zoom_color[1], zoom_color[2], zoom_color[3]), g.IO.DeltaTime * 6.f);
        state.nick_color = ImLerp(state.nick_color, ImColor(nick_color[0], nick_color[1], nick_color[2], nick_color[3]), g.IO.DeltaTime * 6.f);
        state.hit_color = ImLerp(state.hit_color, ImColor(hit_color[0], hit_color[1], hit_color[2], hit_color[3]), g.IO.DeltaTime * 6.f);
        state.box_color = ImLerp(state.box_color, ImColor(box_color[0], box_color[1], box_color[2], box_color[3]), g.IO.DeltaTime * 6.f);
        state.hp_color = ImLerp(state.hp_color, ImColor(hp_color[0], hp_color[1], hp_color[2], hp_color[3]), g.IO.DeltaTime * 6.f);
        state.c4_color = ImLerp(state.c4_color, ImColor(c4_color[0], c4_color[1], c4_color[2], c4_color[3]), g.IO.DeltaTime * 6.f);

        int hp_result = 307 * *hp / 100;

        GetWindowDrawList()->AddImage(player_preview, player.Min, player.Max, ImVec2(0, 0), ImVec2(1, 1), GetColorU32(c::elements::text_active));

        static std::vector<std::string> partyNames = { "Nickname", "95%", "Zoomed", "Weapon", "Bomb", "50$", "HIT", "C4" };

        PushFont(font::lexend_regular);
        PushStyleColor(ImGuiCol_Text, GetColorU32(c::elements::text_active));

        for (int n = 0; n < partyNames.size(); n++)
        {
            position = {

            ImVec2((width - CalcTextSize(partyNames[n].c_str()).x) / 2, 50),
            ImVec2(55, 50),
            ImVec2(55, 385),
            ImVec2((width - CalcTextSize(partyNames[n].c_str()).x) / 2, 385),
            ImVec2(285, 70),
            ImVec2(285, 95),
            ImVec2(285, 145),
            ImVec2(285, 120)

            };

            ImGui::SetCursorPos(position[n]);
            ImGui::Button(partyNames[n].c_str());

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                int payloadData = n;
                ImGui::SetDragDropPayload("DND_DEMO_CELL", &payloadData, sizeof(int));

                ImGui::Text(partyNames[n].c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    const int payload_n = *(const int*)payload->Data;

                    std::swap(partyNames[n], partyNames[payload_n]);

                }
                ImGui::EndDragDropTarget();
            }
        }

        PopStyleColor();
        PopFont();

        if (*esp_box)  GetWindowDrawList()->AddRect(box.Min, box.Max, GetColorU32(state.box_color), 0.f, NULL, 2.f);
        if (*hp_line)  GetWindowDrawList()->AddLine(box.Min + ImVec2(-10, 0), box.Min + ImVec2(-10, 307), GetColorU32(c::elements::background_widget), 3.f);
        if (*hp_line)  GetWindowDrawList()->AddLine(box.Min + ImVec2(-10, 307 - hp_result), box.Min + ImVec2(-10, 307), GetColorU32(c::accent), 3.f);

    }
}