#include "imgui_edited.hpp"

namespace edited
{

    bool BeginPopupW(const char* id, const char* hint)
    {
        if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1]) ImGui::OpenPopup(id);

        if (IsItemHovered() && hint[0]) SetTooltip(hint);

        if (ImGui::BeginPopup(id, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) return true;

        return false;
    }

    void EndPopupW()
    {
        ImGui::EndPopup();
    }

    void RenderTextColor(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align)
    {
        PushFont(font);
        PushStyleColor(ImGuiCol_Text, col);
        RenderTextClipped(p_min, p_max, text, NULL, NULL, align, NULL);
        PopStyleColor();
        PopFont();
    }

    void TextColoredWrapped(const char* label, ImU32 color, float wrap_size)
    {
        PushTextWrapPos(wrap_size);
        ImGui::TextColored(ImColor(GetColorU32(color)), label);
        PopTextWrapPos();
    }

    struct page_state
    {
        ImVec4 background, text, icon;
        float alpha;
    };

    struct ICObutton_state
    {
        ImVec4 text;
    };

    bool ICObutton(ImFont* icon_font, const char* icon, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(icon);
        const ImVec2 label_size = CalcTextSize(icon, NULL, true), pos = window->DC.CursorPos;

        ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);

        ItemSize(size, 0.f);
        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);

        static std::map<ImGuiID, ICObutton_state> anim;
        ICObutton_state& state = anim[id];

        state.text = ImLerp(state.text, hovered ? c::accent : c::element::text, g.IO.DeltaTime * 6.f);

        RenderTextColor(icon_font, bb.Min, bb.Max, GetColorU32(state.text), icon, ImVec2(0.5, 0.48f));

        return pressed;
    }

    bool Pages(bool selected, const char* label, ImTextureID image_icon, const ImVec2& size_arg)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true), pos = window->DC.CursorPos;

        ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);

        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));
        ItemSize(size, 0.f);
        PopStyleVar();

        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);

        static std::map<ImGuiID, page_state> anim;
        page_state& state = anim[id];

        state.text = ImLerp(state.text, selected ? c::element::text_active : c::element::text, g.IO.DeltaTime * 6.f);
        state.icon = ImLerp(state.icon, selected ? c::accent : c::element::text, g.IO.DeltaTime * 6.f);
        state.alpha = ImLerp(state.alpha, selected ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(c::element::page_active, state.alpha), c::element::rounding / 2);

        GetWindowDrawList()->AddImage(image_icon, bb.Min + ImVec2(10, 10), bb.Min + ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), GetColorU32(state.icon));

        RenderTextColor(font::inter_medium_widget, bb.Min + ImVec2(size_arg.y, 0), bb.Max, GetColorU32(state.text), label, ImVec2(0.0, 0.5));

        return pressed;
    }

    const char* keys[] =
    {
        "None",
        "Mouse 1",
        "Mouse 2",
        "CN",
        "Mouse 3",
        "Mouse 4",
        "Mouse 5",
        "-",
        "Back",
        "Tab",
        "-",
        "-",
        "CLR",
        "Enter",
        "-",
        "-",
        "Shift",
        "CTL",
        "Menu",
        "Pause",
        "Caps Lock",
        "KAN",
        "-",
        "JUN",
        "FIN",
        "KAN",
        "-",
        "Escape",
        "CON",
        "NCO",
        "ACC",
        "MAD",
        "Space",
        "PGU",
        "PGD",
        "End",
        "Home",
        "Left",
        "Up",
        "Right",
        "Down",
        "SEL",
        "PRI",
        "EXE",
        "PRI",
        "INS",
        "Delete",
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
        "Numpad 0",
        "Numpad 1",
        "Numpad 2",
        "Numpad 3",
        "Numpad 4",
        "Numpad 5",
        "Numpad 6",
        "Numpad 7",
        "Numpad 8",
        "Numpad 9",
        "MUL",
        "ADD",
        "SEP",
        "MIN",
        "Delete",
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
        "Shift",
        "Shift",
        "Ctrl",
        "Ctrl",
        "Alt",
        "Alt"
    };

#include <string>

    struct key_state
    {
        ImVec4 background, text;
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
    };

    bool Keybind(const char* label, int* key, int* mode, bool* show)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const float width = (GetContentRegionMax().x - style.WindowPadding.x);

        const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 35));

        const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(CalcTextSize(label).x + 110, 35));

        ItemSize(total_bb, 0.f);

        if (!ImGui::ItemAdd(rect, id)) return false;

        char buf_display[64] = "'None'";
     
        bool value_changed = false;
        int k = *key;

        std::string active_key = "'";
        active_key += keys[*key] + active_key;

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

        it_anim->second.text = ImLerp(it_anim->second.text, g.ActiveId == id ? c::element::text_active : hovered ? c::element::text_hov : c::element::text, ImGui::GetIO().DeltaTime * 6.f);

        window->DrawList->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::element::popup_elements::filling), c::element::rounding);

        RenderTextColor(font::inter_medium_widget, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::element::text_active), buf_display, ImVec2(0.0f, 0.5f));
        RenderTextColor(font::icomoon, rect.Max - ImVec2(35, 35), rect.Max, GetColorU32(c::element::text_active), "l", ImVec2(0.5f, 0.5f));

        if (edited::button(0 == *mode, "Toggle", ImVec2(width / 2, 30))) *mode = 0;
        SameLine(0, 10);
        if (edited::button(1 == *mode, "Hold", ImVec2(80, 30))) *mode = 1;

        edited::CheckboxWithPopup(false, "Show In Binds", "-", show, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

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
   
     bool BeginChild(const char* str_id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
    {
        ImGuiID id = GetCurrentWindow()->GetID(str_id);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));
        return BeginChildEx(str_id, id, size_arg, child_flags, window_flags | ImGuiWindowFlags_AlwaysUseWindowPadding);
    }

    bool BeginChild(ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
    {
        return BeginChildEx(NULL, id, size_arg, child_flags, window_flags);
    }

    struct child_state
    {
        bool collapsed;
        float size_offset = 0.f;
    };

    bool BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
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

        ImGui::SetCursorPos(GetCursorPos() + ImVec2(0.f, 35.f));

        static std::map<ImGuiID, child_state> anim;
        child_state& state = anim[id];

        GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos - ImVec2(0, 35), parent_window->DC.CursorPos + ImVec2(size_arg.x - 0.4f, 0), GetColorU32(c::accent), c::child::rounding, ImDrawFlags_RoundCornersTop);
        GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos - ImVec2(0, 34), parent_window->DC.CursorPos + ImVec2(size_arg.x, 0), GetColorU32(c::child::top), c::child::rounding, ImDrawFlags_RoundCornersTop);
        GetWindowDrawList()->AddRect(parent_window->DC.CursorPos - ImVec2(0, 0), parent_window->DC.CursorPos + ImVec2(size_arg.x, 0), GetColorU32(c::child::stroke), 1.f);

        RenderTextColor(font::inter_bold, parent_window->DC.CursorPos - ImVec2(-12, 35), parent_window->DC.CursorPos + ImVec2(size_arg.x, 0), GetColorU32(c::child::child_text), name, ImVec2(0.0, 0.5f));

        const char* temp_window_name;

        if (name)
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
        else
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

        const float backup_border_size = g.Style.ChildBorderSize;
        if ((child_flags & ImGuiChildFlags_Border) == 0) g.Style.ChildBorderSize = 0.0f;

        const bool ret = Begin(temp_window_name, NULL, window_flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGuiWindow* child_window = g.CurrentWindow;
        child_window->ChildId = id;

        state.size_offset = ImLerp(state.size_offset, child_window->ContentSize.y > 0 ? child_window->ContentSize.y + 30 : 1.f, g.IO.DeltaTime * 20.f);

        SetWindowSize(ImVec2(size_arg.x, state.size_offset));

        parent_window->DrawList->AddRectFilled(child_window->Pos, child_window->Pos + ImVec2(size_arg.x, state.size_offset), GetColorU32(c::child::filling), c::child::rounding, ImDrawFlags_RoundCornersBottom);

        g.Style.ChildBorderSize = backup_border_size;
        if (child_flags & ImGuiChildFlags_FrameStyle)
        {
            PopStyleVar(3);
            PopStyleColor();
        }

        if (child_window->BeginCount == 1) parent_window->DC.CursorPos = child_window->Pos;

        const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
        if (g.ActiveId == temp_id_for_activation)
            ClearActiveID();
        if (g.NavActivateId == id && !(window_flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
        {
            FocusWindow(child_window);
            NavInitWindow(child_window, false);
            SetActiveID(temp_id_for_activation, child_window);
            g.ActiveIdSource = g.NavInputSource;
        }
        return ret;
    }

    void EndChild()
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
            if (g.HoveredWindow == child_window)
                g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
        }
        g.WithinEndChild = false;
        g.LogLinePosY = -FLT_MAX;
    }

    struct button_state
    {
        ImVec4 background;
    };

    bool button(bool select, const char* label, const ImVec2& size_arg)
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

        static std::map<ImGuiID, button_state> anim;
        button_state& state = anim[id];

        state.background = ImLerp(state.background, select ? c::accent : c::element::popup_elements::filling, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(state.background), c::element::rounding / 2);

        RenderTextColor(font::inter_medium_small, bb.Min, bb.Max, GetColorU32(c::element::text_active), label, ImVec2(0.5, 0.48f));

        return pressed;
    }

    struct check_state
    {
        std::vector<std::string> pageTexts = { "Copy Lua Path", "Description", "New Bind", "Colors", "Reset"};
        std::vector<std::string> icons = { "z", "r", "u", "e", "w" };

        ImVec4 background, circle, text;

        float background_opticaly, circle_offset, popup_opticaly, alpha;

        int page_selector = 0;
    };

    bool CheckboxWithPopup(bool popup_box, const char* label, const char* hint, bool* v, int* key, int* mode, bool* value, bool keybind, bool picker, float color[4], ImGuiColorEditFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const float square_sz = 18, y_size = 18;
        const ImVec2 pos = window->DC.CursorPos;
        const float w = GetContentRegionMax().x - style.WindowPadding.x;

        const ImRect rect(pos + ImVec2(0, 0), pos + ImVec2(w, y_size));
        const ImRect total_bb(pos, pos + ImVec2(CalcTextSize(label).x + 110, y_size));

        ItemSize(total_bb, 0.f);

        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

        static std::map<ImGuiID, check_state> anim;
        check_state& state = anim[id];

        state.background_opticaly = ImLerp(state.background_opticaly, *v ? 0.2f : 1.0f, g.IO.DeltaTime * 12.f);
        state.circle_offset = ImLerp(state.circle_offset, *v ? 0 : -square_sz * 2, g.IO.DeltaTime * 12.f);
        state.background = ImLerp(state.background, *v ? c::accent : c::element::filling, g.IO.DeltaTime * 12.f);
        state.circle = ImLerp(state.circle, *v ? c::accent : c::element::circle_mark, g.IO.DeltaTime * 12.f);
        state.text = ImLerp(state.text, *v ? c::element::text_active : hovered ? c::element::text_hov : c::element::text, g.IO.DeltaTime * 12.f);

        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        if (popup_box)
        {

            if (keybind) RenderTextColor(font::icomoon, rect.Min, rect.Max - ImVec2(47, 0), GetColorU32(c::element::popup_elements::cog), "p", ImVec2(1.0, 0.49f));
            if (picker) RenderTextColor(font::icomoon, rect.Min, rect.Max - ImVec2(keybind ? 75 : 47, 0), ImColor(ImVec4(color[0], color[1], color[2], 1.f)), "e", ImVec2(1.0, 0.49f));
            if (!picker && !keybind) RenderTextColor(font::icomoon, rect.Min, rect.Max - ImVec2(47, 0), GetColorU32(c::element::popup_elements::cog), "n", ImVec2(1.0, 0.49f));

            if (edited::BeginPopupW(label, ""))
            {
                if (g.CurrentWindow->BeginCount == 1)
                {
                    for (int i = 0; i < state.pageTexts.size(); i++)
                    {
                        if (i == 3 && !picker || i == 2 && !keybind || i == 1 && hint == "") continue;
                        
                        if (ImGui::BeginMenuEx(state.pageTexts[i].c_str(), state.icons[i].c_str()))
                        {
                            if (i == 0) edited::button(1 == 1, "Copy to Clipboard", ImVec2(150, 30));

                            if (i == 1) TextColoredWrapped(hint, GetColorU32(c::element::text_active), (GetIO().DisplaySize.x / 2));

                            if (i == 2) Keybind("keybind", key, mode, value);

                            if (i == 3 && picker) edited::ColorPicker4("picker", color, flags);

                            if (i == 4) edited::button(1 == 1, "Reset All Settings", ImVec2(150, 30));

                            ImGui::EndMenu();
                        }
                    }
                }
                EndPopupW();
            }
        }

        const ImRect check_bb(pos + ImVec2(w - (square_sz * 2), (y_size - square_sz) / 2), pos + ImVec2(w, (y_size + square_sz) / 2));

        GetWindowDrawList()->AddRectFilled(check_bb.Min + ImVec2(0, 3), check_bb.Max - ImVec2(0, 3), GetColorU32(state.background, state.background_opticaly), 30);
        GetWindowDrawList()->AddCircleFilled(check_bb.Max - ImVec2(y_size - state.circle_offset, y_size) / 2, 9.f, GetColorU32(state.circle), 30);
        if (*v) GetWindowDrawList()->AddShadowCircle(check_bb.Max - ImVec2(y_size - state.circle_offset, y_size) / 2, 9.f, GetColorU32(state.circle), 30.f, ImVec2(0, 0), 30);

        RenderTextColor(font::inter_medium_widget, rect.Min, rect.Max, GetColorU32(state.text), label, ImVec2(0.0, 0.5f));

        return pressed;
    }

    struct iconbox_state
    {
        float line;
    };

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

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(color_bg), 3.f);

        return pressed;
    }

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

        ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(color_bg), 4);
        ImGui::GetWindowDrawList()->AddRect(rect.Min, rect.Max, GetColorU32(color_border), 4);

        edited::RenderTextColor(font::icomoon, rect.Min, rect.Max, GetColorU32(color_icon), icon, ImVec2(0.5, 0.5));

        return pressed;
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

        if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
            *H = g.ColorEditSavedHue;

        if (*V == 0.0f) *S = g.ColorEditSavedSat;
    }

    struct color_state
    {
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
    };

    bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float square_sz = GetFrameHeight();
        const float w_full = CalcItemWidth();
        const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
        const float w_inputs = w_full - w_button;
        const char* label_display_end = FindRenderedTextEnd(label);
        g.NextItemData.ClearFlags();

        POINT cursorPos;
        COLORREF color;

        BeginGroup();
        PushID(label);

        const ImGuiColorEditFlags flags_untouched = flags;
        if (flags & ImGuiColorEditFlags_NoInputs) flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

        if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorEditOptionsPopup(col, flags);

        if (!(flags & ImGuiColorEditFlags_DisplayMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
        if (!(flags & ImGuiColorEditFlags_DataTypeMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
        if (!(flags & ImGuiColorEditFlags_PickerMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
        if (!(flags & ImGuiColorEditFlags_InputMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);

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

        ImGuiWindow* picker_active_window = NULL;
        if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
        {
            const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
            window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

            const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);


        if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
        {

            if (!(flags & ImGuiColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
        }
            if (ColorButton(label, col_v4, flags))
            {
                if (!(flags & ImGuiColorEditFlags_NoPicker))

                {
                    // Store current color and open a picker
                    g.ColorPickerRef = col_v4;
                    OpenPopup("picker");
                    SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, 0.f));
                }
            }
            if (!(flags & ImGuiColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

            static std::map<ImGuiID, color_state> anim;
            color_state& state = anim[GetID(label)];

            PushStyleColor(ImGuiCol_PopupBg, GetColorU32(c::black, 0.4f));
            PushStyleVar(ImGuiStyleVar_PopupRounding, 6.f);
            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
            PushStyleColor(ImGuiCol_WindowShadow, GetColorU32(c::accent, 0.4f));

            if (BeginPopup("picker"))
            {
                if (g.CurrentWindow->BeginCount == 1)
                {
                    picker_active_window = g.CurrentWindow;

                    ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                    ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                    SetNextItemWidth(square_sz * 12.0f);
                    value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);

                    static std::vector<float> color_x, color_y, color_z, color_a;

                    static bool add_status = true;

                    if (edited::icon_box(add_status ? "t" : "9", ImVec2(190, 25), GetColorU32(c::element::combo::filling), GetColorU32(c::accent), GetColorU32(c::accent, 0.f)))
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

                        if (edited::color_button(name_box.c_str(), ImVec2(22.8, 15), ImColor(color_x[i], color_y[i], color_z[i], color_a[i])))
                        {
                            col[0] = color_x[i], col[1] = color_y[i], col[2] = color_z[i], col[3] = color_a[i];
                        };

                        if ((i + 1) % 7 != 0) ImGui::SameLine(0, 5);

                    }

                }
                EndPopup();
            }
            PopStyleColor(2);
            PopStyleVar(3);
        }

        if (value_changed && picker_active_window == NULL)
        {
            if (!value_changed_as_float)
                for (int n = 0; n < 4; n++)
                    f[n] = i[n] / 255.0f;
            if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
            {
                g.ColorEditSavedHue = f[0];
                g.ColorEditSavedSat = f[1];
                ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
                g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
            }
            if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
                ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

            col[0] = f[0];
            col[1] = f[1];
            col[2] = f[2];
            if (alpha)
                col[3] = f[3];
        }

        PopID();
        EndGroup();

        if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
            g.LastItemData.ID = g.ActiveId;

        if (value_changed && g.LastItemData.ID != 0)
            MarkItemEdited(g.LastItemData.ID);

        return value_changed;
    }

    bool ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
    {
        float col4[4] = { col[0], col[1], col[2], 1.0f };
        if (!ColorPicker4(label, col4, flags | ImGuiColorEditFlags_NoAlpha, NULL))
            return false;
        col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];
        return true;
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
        ImVec2 sv;
        float hue, alpha;
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
        BeginGroup();
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));

        if (!(flags & ImGuiColorEditFlags_NoSidePreview)) flags |= ImGuiColorEditFlags_NoSmallPreview;

        if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorPickerOptionsPopup(col, flags);

        if (!(flags & ImGuiColorEditFlags_PickerMask_))
            flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
        if (!(flags & ImGuiColorEditFlags_InputMask_))
            flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_));
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_)); 
        if (!(flags & ImGuiColorEditFlags_NoOptions)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

        int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
        bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
        ImVec2 picker_pos = window->DC.CursorPos + ImVec2(0, 0);
        ImVec2 bar_pos = window->DC.CursorPos + ImVec2(0, 135);

        float picker_size = 190.f;
        float bar_width = 190.f;
        float bar_height = 4.f;

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

        PushItemFlag(ImGuiItemFlags_NoNav, true);

        SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y));
        InvisibleButton("sv", ImVec2(picker_size, picker_size));
        if (IsItemActive())
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (picker_size - 1));

            if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) H = g.ColorEditSavedHue;
            value_changed = value_changed_sv = true;
        }

        SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 204.f));
        InvisibleButton("hue", ImVec2(bar_width, bar_height + 8));
        if (IsItemActive())
        {
            H = 1.f - ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
            value_changed = value_changed_h = true;
        }

        if (alpha_bar)
        {
            SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 227.f));
            InvisibleButton("alpha", ImVec2(bar_width, bar_height + 8));
            if (IsItemActive())
            {
                col[3] = ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
                value_changed = true;
            }
        }
        PopItemFlag();

        if (value_changed_h || value_changed_sv)
        {
            if (flags & ImGuiColorEditFlags_InputRGB)
            {
                ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
                g.ColorEditSavedHue = H;
                g.ColorEditSavedSat = S;
                g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
            }
            else if (flags & ImGuiColorEditFlags_InputHSV)
            {
                col[0] = H;
                col[1] = S;
                col[2] = V;
            }
        }

        bool value_changed_fix_hue_wrap = false;
            ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
            ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;

            if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                if (ImGui::ColorEdit4("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
                {
                    value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
                    value_changed = true;
                }
            if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                value_changed |= ImGui::ColorEdit4("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);
            if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                value_changed |= ImGui::ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);

        if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
        {
            float new_H, new_S, new_V;
            ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
            if (new_H <= 0 && H > 0)
            {
                if (new_V <= 0 && V != new_V)
                    ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
                else if (new_S <= 0)
                    ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
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
        const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(0,0,255,style_alpha8),IM_COL32(0,255,255,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

        ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
        ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
        ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha));

        ImVec2 sv_cursor_pos;

        const int vtx_idx_0 = draw_list->VtxBuffer.Size;
        draw_list->AddRectFilled(picker_pos, picker_pos + ImVec2(picker_size, picker_size - 2), col_white, 3.0f);
        const int vtx_idx_1 = draw_list->VtxBuffer.Size;
        ShadeVertsLinearColorGradientKeepAlpha(draw_list, vtx_idx_0, vtx_idx_1, picker_pos, picker_pos + ImVec2(picker_size, 0.0f), col_white, hue_color32);

        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(picker_size, picker_size), 0, 0, col_black, col_black, 3.f);

        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * picker_size), picker_pos.x + 2, picker_pos.x + picker_size - 2);
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * (picker_size)), picker_pos.y + 2, picker_pos.y + picker_size - 2);

        static std::map<ImGuiID, picker_state> anim;
        picker_state& state = anim[GetID(label)];

        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(picker_pos.x + i * (bar_width / 6) - (i == 5 ? 1 : 0), picker_pos.y + 208), ImVec2(picker_pos.x + (i + 1) * (bar_width / 6) + (i == 0 ? 1 : 0), picker_pos.y + 208 + bar_height), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i], 2.f, i == 0 ? ImDrawFlags_RoundCornersLeft : i == 5 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone);
        float bar0_line_x = IM_ROUND(picker_pos.x + (1.f - H) * bar_width);
        bar0_line_x = ImClamp(bar0_line_x, picker_pos.x + 3.f, picker_pos.x + 186.f);

        state.hue = ImLerp(state.hue, bar0_line_x, g.IO.DeltaTime * 20.f);
        draw_list->AddCircleFilled(ImVec2(state.hue, picker_pos.y + 210), 5, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));

        state.sv = ImLerp(state.sv, sv_cursor_pos, g.IO.DeltaTime * 10.f);

        draw_list->AddShadowCircle(state.sv, 5.f, GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.78f)), 20.f, ImVec2(0, 2));
        draw_list->AddCircle(state.sv, 5.f, col_white, 12, 4.f);
        draw_list->AddCircleFilled(state.sv, 5.f, user_col32_striped_of_alpha, 12);

        if (alpha_bar)
        {
            float alpha = ImSaturate(col[3]);
            draw_list->AddRectFilledMultiColor(picker_pos + ImVec2(0, 231), picker_pos + ImVec2(bar_width, 231 + bar_height), col_black, user_col32_striped_of_alpha, user_col32_striped_of_alpha, col_black, 2.f);
            float bar1_line_x = IM_ROUND(picker_pos.x + alpha * bar_width);
            bar1_line_x = ImClamp(bar1_line_x, picker_pos.x + 3.f, picker_pos.x + 186.f);

            state.alpha = ImLerp(state.alpha, bar1_line_x, g.IO.DeltaTime * 20.f);
            draw_list->AddCircleFilled(ImVec2(state.alpha, picker_pos.y + 233), 5, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));
        }

        PopStyleVar();
        EndGroup();

        if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) value_changed = false;
        if (value_changed && g.LastItemData.ID != 0)  MarkItemEdited(g.LastItemData.ID);

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
        const float width = GetContentRegionMax().x - GetStyle().WindowPadding.x;

        const ImRect rect(pos, pos + ImVec2(width, 18));
        const ImRect clickable(pos + ImVec2(width - 18, 0), pos + ImVec2(width, 18));

        const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(CalcTextSize(desc_id).x + 110, 18));

        ItemSize(total_bb, 0);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

        if (flags & ImGuiColorEditFlags_NoAlpha) flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

        ImVec4 col_rgb = col;
        if (flags & ImGuiColorEditFlags_InputHSV) ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

        RenderTextColor(font::inter_medium_widget, rect.Min, rect.Max, GetColorU32(c::element::text), desc_id, ImVec2(0.0, 0.5f));

        RenderTextColor(font::icomoon, clickable.Min, clickable.Max, GetColorU32(col_rgb), "e", ImVec2(1.0, 0.5f));

        return pressed;
    }

    struct slider_state
    {
        ImVec4 text, circle;
        float slow, hint_opticaly, hint_alpha, circle_pump;
    };

    bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const float y_size = 18;
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, y_size));
        const ImRect slider_bb(window->DC.CursorPos + ImVec2(w - 110, (y_size / 2) - 5), window->DC.CursorPos + ImVec2(w, (y_size / 2) + 5));
        const ImRect slider_active(window->DC.CursorPos + ImVec2(w - 110, 0), window->DC.CursorPos + ImVec2(w, y_size));

        const ImRect total_bb(slider_bb.Min, slider_bb.Max + ImVec2(CalcTextSize(label).x + 110, 8));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

        ItemSize(ImRect(total_bb.Min, total_bb.Max));

        if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

        if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

        bool hovered = ItemHoverable(slider_active, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(slider_active, id, &hovered, &held, NULL);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);

        if (temp_input_is_active)
        {
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
            return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
        }

        static std::map<ImGuiID, slider_state> anim;
        slider_state& state = anim[id];

        state.hint_opticaly = ImLerp(state.hint_opticaly, hovered ? 0.3f : 0.0f, g.IO.DeltaTime * 6.f);
        state.hint_alpha = ImLerp(state.hint_alpha, hovered || IsItemActive() ? 1.f : 0.f, g.IO.DeltaTime * 6.f);
        state.text = ImLerp(state.text, IsItemActive() ? c::element::text_active : hovered ? c::element::text_hov : c::element::text, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(slider_bb.Min + ImVec2(0, 1), slider_bb.Max - ImVec2(0, 1), GetColorU32(c::element::filling), 30.f);

        POINT p;
        ImRect grab_bb;

        const bool value_changed = SliderBehavior(ImRect(slider_bb.Min + ImVec2(4, 0), slider_bb.Max + ImVec2(13, 0)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
        if (value_changed) MarkItemEdited(id);

        if (grab_bb.Max.x > grab_bb.Min.x) {

            state.slow = ImLerp(state.slow, grab_bb.Min.x - frame_bb.Min.x, g.IO.DeltaTime * 15.f);

            GetWindowDrawList()->AddRectFilledMultiColor(slider_bb.Min + ImVec2(0, 1), ImVec2(state.slow + frame_bb.Min.x - 3 - state.circle_pump, grab_bb.Max.y + 1), GetColorU32(c::accent), GetColorU32(c::black), GetColorU32(c::black), GetColorU32(c::accent), 30.f);

            GetWindowDrawList()->AddCircleFilled(ImVec2(frame_bb.Min.x + state.slow - 3, grab_bb.Max.y - 3), 4.0f, GetColorU32(c::element::text_active), 30.f);

            GetWindowDrawList()->AddShadowCircle(ImVec2(frame_bb.Min.x + state.slow - 3, grab_bb.Max.y - 3), 4.0f, GetColorU32(c::element::text_active), 25, ImVec2(0, 0), 30.f);
        }

        RenderTextColor(font::inter_medium_widget, frame_bb.Min + ImVec2(0, 0), frame_bb.Max - ImVec2(120, 1), GetColorU32(c::element::text_active), value_buf, ImVec2(1.0, 0.5));

        GetWindowDrawList()->AddText(ImVec2(frame_bb.Max.x - w, frame_bb.Max.y - ((y_size + CalcTextSize(label).y) / 2)), GetColorU32(state.text), label);

        return value_changed;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    struct begin_state
    {
        ImVec4 background, text, outline;
        float open, alpha, combo_size = 0.f, shadow_opticaly;
        bool opened_combo = false, hovered = false;
        float arrow_roll, total_height;
    };

    static float CalcMaxPopupHeightFromItemCount(int items_count)
    {
        ImGuiContext& g = *GImGui;
        if (items_count <= 0) return FLT_MAX;

        return (25 * items_count + g.Style.WindowPadding.y * 2);
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

    bool BeginCombo(const char* label, const char* preview_value, int val, bool multi, ImGuiComboFlags flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();

        g.NextWindowData.ClearFlags();
        if (window->SkipItems) return false;

        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        static float containerHeight;
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const float w = ((GetContentRegionMax().x - style.WindowPadding.x));
        const float y_size = 18;

        const ImRect frame(window->DC.CursorPos + ImVec2(0, 0), window->DC.CursorPos + ImVec2(w, y_size));
        const ImRect rect(window->DC.CursorPos + ImVec2(w - 130, -5), window->DC.CursorPos + ImVec2(w, y_size + 5));
        const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(CalcTextSize(label).x + 110, y_size));

        ItemSize(ImRect(total_bb.Min, total_bb.Max - ImVec2(0, 0)));

        if (!ItemAdd(rect, id, &rect)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

        static std::map<ImGuiID, begin_state> anim;
        begin_state& state = anim[id];

        if (hovered && g.IO.MouseClicked[0] || state.opened_combo && g.IO.MouseClicked[0] && !state.hovered) state.opened_combo = !state.opened_combo;

        state.arrow_roll = ImLerp(state.arrow_roll, state.opened_combo ? -1.f : 1.f, g.IO.DeltaTime * 6.f);
        state.text = ImLerp(state.text, state.opened_combo ? c::element::text_active : hovered ? c::element::text_hov : c::element::text, g.IO.DeltaTime * 6.f);
        state.combo_size = ImLerp(state.combo_size, state.opened_combo ? CalcMaxPopupHeightFromItemCount(val) : 0.f, g.IO.DeltaTime * 12.f);

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::element::combo::filling), c::element::rounding / 2);

        RenderTextColor(font::inter_medium_widget, frame.Min, frame.Max, GetColorU32(state.text), label, ImVec2(0.0, 0.5f));

        RenderTextColor(font::inter_medium_widget, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(c::element::text_active), preview_value, ImVec2(0.0, 0.49f));

        ImRotateStart();
        RenderTextColor(font::icomoon, rect.Max - ImVec2(29, 29), rect.Max, GetColorU32(c::element::text_active), "l", ImVec2(0.5, 0.52f));
        ImRotateEnd(1.57f * state.arrow_roll);

        if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
        {
            state.opened_combo = false;
            state.combo_size = 0.f;
        }

        if (!state.opened_combo && state.combo_size < 2.f) return false;

        ImGui::SetNextWindowPos(ImVec2(rect.Min.x, rect.Max.y + 5));
        ImGui::SetNextWindowSize(ImVec2(rect.GetWidth(), state.combo_size));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;

        PushStyleColor(ImGuiCol_WindowBg, c::element::combo::filling);
        PushStyleVar(ImGuiStyleVar_WindowRounding, c::element::rounding / 2);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

        bool ret = Begin(label, NULL, window_flags);

        PopStyleVar(3);
        PopStyleColor(1);

        state.hovered = IsWindowHovered();

        if (multi && state.hovered && g.IO.MouseClicked[0]) state.opened_combo = false;

        return true;
    }

    void EndCombo()
    {
        End();
    }

    void MultiCombo(const char* label, bool variable[], const char* labels[], int count)
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

        if (BeginCombo(label, preview.c_str(), count, NULL, NULL))
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

    bool Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
    {
        ImGuiContext& g = *GImGui;

        const char* preview_value = NULL;
        if (*current_item >= 0 && *current_item < items_count)
            preview_value = getter(user_data, *current_item);

        if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
            SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

        if (!BeginCombo(label, preview_value, items_count, ImGuiComboFlags_None, NULL)) return false;

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

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
    {
        const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
        return value_changed;
    }

    bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
    {
        int items_count = 0;
        const char* p = items_separated_by_zeros;
        while (*p)
        {
            p += strlen(p) + 1;
            items_count++;
        }
        bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
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

        state.text = ImLerp(state.text, selected ? c::accent : c::element::text, g.IO.DeltaTime * 6.f);
        state.opticaly = ImLerp(state.opticaly, selected ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max + ImVec2(0, 1), GetColorU32(c::element::selectable, state.opticaly), 2.f);

        PushStyleColor(ImGuiCol_Text, GetColorU32(state.text));
        RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
        PopStyleColor();

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

    bool info_box(const char* icon, const char* label, ImVec2 size)
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

        GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(c::element::info_bar::box), 4.f);
        GetWindowDrawList()->AddRect(rect.Min, rect.Max, GetColorU32(c::element::info_bar::box_outline), 4.f);

        edited::RenderTextColor(font::icomoon, rect.Min, rect.Max - ImVec2(CalcTextSize(label).x + 15, 0), GetColorU32(c::accent), icon, ImVec2(0.5, 0.5));
        edited::RenderTextColor(font::inter_medium_small, rect.Min + ImVec2(CalcTextSize(icon).x + 15, 0), rect.Max, GetColorU32(c::element::text), label, ImVec2(0.5, 0.5));

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
        const ImRect player(pos + ImVec2((width - 125) / 2, 60), pos + ImVec2((width + 125) / 2, 320));
        const ImRect box(pos + ImVec2((width - 180) / 2, 40), pos + ImVec2((width + 180) / 2, 347));

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

        GetWindowDrawList()->AddImage(player_preview, player.Min, player.Max, ImVec2(0, 0), ImVec2(1, 1), GetColorU32(c::element::text_active));

        static std::vector<std::string> partyNames = { "SQUARE.CC", "95%", "Zoomed", "Weapon", "Bomb", "50$", "HIT", "C4" };

        PushFont(font::inter_medium_small);
        PushStyleColor(ImGuiCol_Text, GetColorU32(c::element::text_active));

        for (int n = 0; n < partyNames.size(); n++)
        {
            position = {

            ImVec2((width - CalcTextSize(partyNames[n].c_str()).x) / 2, 15),
            ImVec2(55, 15),
            ImVec2(65, 370),
            ImVec2(250 - CalcTextSize(partyNames[n].c_str()).x, 370),
            ImVec2(260, 45),
            ImVec2(260, 65),
            ImVec2(260, 85),
            ImVec2(260, 105)

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

        //if (*esp_box)  GetWindowDrawList()->AddRect(box.Min, box.Max, GetColorU32(c::accent), 0.f, NULL, 2.f);
        if (*hp_line)  GetWindowDrawList()->AddLine(box.Min + ImVec2(-10, 0), box.Min + ImVec2(-10, 307), GetColorU32(c::element::filling), 3.f);
        if (*hp_line)  GetWindowDrawList()->AddRectFilledMultiColor(box.Min + ImVec2(-10, 307 - hp_result), box.Min + ImVec2(-8, 307), GetColorU32(c::green), GetColorU32(c::green), GetColorU32(c::red), GetColorU32(c::red), 0.f);
    }
}