#include "../settings/functions.h"

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
    "Caps",
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
    "Num 0",
    "Num 1",
    "Num 2",
    "Num 3",
    "Num 4",
    "Num 5",
    "Num 6",
    "Num 7",
    "Num 8",
    "Num 9",
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

bool c_widget::keybind(std::string_view label, int* key)
{

    struct c_keybind
    {
        ImVec4 text{ clr->c_text.text };
        float slow{ 0 };
    };

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionAvail().x;
    const float rect_size{ SCALE(30.f) };

    const ImRect decorate_bb(pos, pos + ImVec2(SCALE(30), rect_size));
    const ImRect total_bb(pos + SCALE(40, 0), pos + ImVec2(width, rect_size));

    ItemSize(total_bb, 0.f);
    if (!ItemAdd(total_bb, id, &total_bb)) return false;

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

    bool hovered = ItemHoverable(total_bb, id, 0);

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

        if (g.ActiveId == id) ImGui::ClearActiveID();
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

    c_keybind* state = gui->anim_container(&state, id);
    state->text = ImLerp(state->text, g.ActiveId == id ? clr->c_text.text_active : clr->c_text.text, g.IO.DeltaTime * gui->fixed_speed(8.f));

    draw->add_rect_filled(window->DrawList, decorate_bb.Min, decorate_bb.Max, gui->get_clr(clr->c_element.layout), SCALE(set->c_element.rounding));
    draw->add_rect_filled(window->DrawList, total_bb.Min, total_bb.Max, gui->get_clr(clr->c_element.layout), SCALE(set->c_element.rounding));

    draw->render_text(set->c_font.icon[1], window->DrawList, decorate_bb.Min, decorate_bb.Max, gui->get_clr(clr->c_other_clr.accent_clr), "E", NULL, NULL, { 0.5, 0.5 }, NULL);
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, total_bb.Min, total_bb.Max, gui->get_clr(clr->c_text.text_active), buf_display, NULL, NULL, { 0.5, 0.5 }, NULL);

    return value_changed;
}

bool c_widget::keybind_button(std::string_view name_id, bool* pressing)
{
    struct button_state
    {
        ImVec4 background_toggle;
        ImVec4 background_hold;
    };

    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name_id.data());

    const float label_size = CalcTextSize(name_id.data(), NULL, true).x;
    const float width = GetContentRegionAvail().x;

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + ImVec2(width, SCALE(30)));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (pressed)
         MarkItemEdited(id);
    

    if (IsMouseHoveringRect(rect.Min, rect.Max - ImVec2((width / 2) + SCALE(5), 0), true) && pressed) *pressing = false;
    if (IsMouseHoveringRect(rect.Min + ImVec2((width / 2) + SCALE(5), 0), rect.Max, true) && pressed) *pressing = true;

    button_state* state = gui->anim_container(&state, id);

    state->background_toggle = ImLerp(state->background_toggle, !*pressing ? clr->c_other_clr.accent_clr : clr->c_element.layout, gui->fixed_speed(8.f));
    state->background_hold = ImLerp(state->background_hold, *pressing ? clr->c_other_clr.accent_clr : clr->c_element.layout, gui->fixed_speed(8.f));

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max - ImVec2((width / 2) + SCALE(5), 0), gui->get_clr(state->background_toggle), SCALE(set->c_element.rounding));
    draw->add_rect_filled(window->DrawList, rect.Min + ImVec2((width / 2) + SCALE(5), 0), rect.Max, gui->get_clr(state->background_hold), SCALE(set->c_element.rounding));

    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min, rect.Max - ImVec2((width / 2) + SCALE(5), 0), gui->get_clr(clr->c_text.text_active), "Toggle", NULL, NULL, { 0.5, 0.5 }, NULL);
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min + ImVec2((width / 2) + SCALE(5), 0), rect.Max, gui->get_clr(clr->c_text.text_active), "Hold", NULL, NULL, { 0.5, 0.5 }, NULL);

    return pressed;
}
