#include "../settings/functions.h"

void c_widget::text_colored(ImFont* font, const ImU32 col, std::string text)
{
    gui->push_font(font);
    TextColored(ImColor(col), text.data());
    gui->pop_font();
}

bool c_widget::begin_popup(std::string_view name, float size_w, const ImVec2& position)
{
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID(name.data());
    const ImVec2 pos = window->DC.CursorPos;
    ImGuiContext& g = *GImGui;

    ImVec2 content_size;
    float current_position;

    struct popup_state
    {
        bool window_opened, hovered;
        float begin_offset, alpha_popup;
    };

    popup_state* state = gui->anim_container(&state, id);
    if (IsItemHovered() && g.IO.MouseClicked[1] || state->window_opened && (g.IO.MouseClicked[0] || g.IO.MouseClicked[1]) && !state->hovered) state->window_opened = !state->window_opened;

    state->alpha_popup = ImClamp(state->alpha_popup + (gui->fixed_speed(5.f) * (state->window_opened ? 1.f : -1.f)), 0.f, 1.f);

    gui->push_style_var(ImGuiStyleVar_Alpha, state->alpha_popup);
    gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 1.f);
    gui->push_style_var(ImGuiStyleVar_WindowRounding, SCALE(set->c_child.rounding));

    gui->push_style_color(ImGuiCol_WindowBg, gui->get_clr(clr->c_child.layout));
    gui->push_style_color(ImGuiCol_Border, gui->get_clr(clr->c_child.stroke));

    gui->set_next_window_pos(g.LastItemData.Rect.GetBL() + position);
    gui->set_next_window_size(ImVec2(SCALE(size_w), content_size.y));

    gui->begin((std::stringstream{} << id << " - popup").str().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);

    state->hovered = IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize());
    content_size = GetContentRegionAvail();

    return state->window_opened;
}

void c_widget::end_popup()
{
    gui->end();

    gui->pop_style_var(3);
    gui->pop_style_color(2);
}

bool c_widget::set_tooltip(std::string_view tooltip_id, std::string_view tooltip_text)
{
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID(tooltip_text.data());
    const ImVec2 pos = window->DC.CursorPos;
    ImGuiContext& g = *GImGui;

    ImVec2 content_size;

    struct popup_state
    {
        bool show_tooltip, hovered;
        float begin_offset, alpha_popup;
    };

    popup_state* state = gui->anim_container(&state, id);

    if (IsItemHovered())
        state->show_tooltip = true;
    else
        state->show_tooltip = false;

    state->alpha_popup = ImClamp(state->alpha_popup + (gui->fixed_speed(5.f) * (state->show_tooltip ? 1.f : -1.f)), 0.f, 1.f);

    gui->push_style_var(ImGuiStyleVar_Alpha, state->alpha_popup);
    gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 1.f);
    gui->push_style_var(ImGuiStyleVar_WindowRounding, set->c_element.rounding);

    gui->push_style_color(ImGuiCol_WindowBg, gui->get_clr(clr->c_child.layout));
    gui->push_style_color(ImGuiCol_Border, gui->get_clr(clr->c_child.stroke));

    gui->set_next_window_pos(ImGui::GetMousePos() + SCALE(20, 20));
    gui->set_next_window_size(ImVec2(CalcTextSize(tooltip_text.data()).x + SCALE(30), content_size.y));

    gui->begin((std::stringstream{} << id << " - popup").str().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
    {

        state->hovered = IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize());
        content_size = GetContentRegionAvail();

        text_colored(set->c_font.icon[0], gui->get_clr(clr->c_other_clr.white_clr), "L");

        gui->sameline(0, 10);

        text_colored(set->c_font.inter_medium[0], gui->get_clr(clr->c_text.text_active), tooltip_id.data());

        widget->separator();

        text_colored(set->c_font.inter_medium[0], gui->get_clr(clr->c_text.text), tooltip_text.data());

        gui->pop_style_var(3);
        gui->pop_style_color(2);

    }
    gui->end();

    return state->show_tooltip;
}

void item_hande_shortcut(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiInputFlags flags = g.NextItemData.ShortcutFlags;
    IM_ASSERT((flags & ~ImGuiInputFlags_SupportedBySetNextItemShortcut) == 0);

    if (flags & ImGuiInputFlags_Tooltip)
    {
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasShortcut;
        g.LastItemData.Shortcut = g.NextItemData.Shortcut;
    }
    if (!Shortcut(g.NextItemData.Shortcut, flags & ImGuiInputFlags_SupportedByShortcut, id) || g.NavActivateId != 0) return;

    g.NavActivateId = id;
    g.NavActivateFlags = ImGuiActivateFlags_PreferInput | ImGuiActivateFlags_FromShortcut;

    g.NavActivateDownId = g.NavActivatePressedId = id;
    NavHighlightActivated(id);
}

void c_gui::push_font(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    if (!font) font = GetDefaultFont();

    SetCurrentFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void c_gui::pop_font()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();

    SetCurrentFont(g.FontStack.empty() ? GetDefaultFont() : g.FontStack.back());
}

void c_gui::set_next_window_pos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
{
    ImGuiContext& g = *GImGui;

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos;
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = ImVec2(ImClamp(pivot.x, 0.0f, 1.0f), ImClamp(pivot.y, 0.0f, 1.0f));
    g.NextWindowData.PosCond = (cond != 0 && ImIsPowerOfTwo(cond)) ? cond : ImGuiCond_Always;
}
void c_gui::set_next_window_size(const ImVec2& size, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(size.x >= 0.0f && size.y >= 0.0f);
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond));

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSize;
    g.NextWindowData.SizeVal = size;
    g.NextWindowData.SizeCond = cond != 0 ? cond : ImGuiCond_Always;
}

ImVec2 c_gui::get_cursor_pos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos + window->Scroll;
}

float c_gui::get_cursor_pos_x()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.x - window->Pos.x + window->Scroll.x;
}

float c_gui::get_cursor_pos_y()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.y - window->Pos.y + window->Scroll.y;
}


void c_gui::set_cursor_pos(const ImVec2& local_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
    window->DC.IsSetPos = true;
}

void c_gui::set_cursor_pos_x(float x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + x;
    window->DC.IsSetPos = true;
}

void c_gui::set_cursor_pos_y(float y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + y;
    window->DC.IsSetPos = true;
}

void c_gui::new_frame()
{
    NewFrame();
}

void c_gui::end_frame()
{
    Render();
}

ImU32 c_gui::get_clr(const ImVec4& col, float alpha)
{
    ImVec4 c = col;
    c.w *= GetStyle().Alpha * alpha;
    return ColorConvertFloat4ToU32(c);
}

void c_gui::push_style_color(ImGuiCol idx, ImU32 col)
{
    ImGuiContext& g = *GImGui;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorStack.push_back(backup);
    if (g.DebugFlashStyleColorIdx != idx) g.Style.Colors[idx] = ColorConvertU32ToFloat4(col);
}

void c_gui::pop_style_color(int count)
{
    ImGuiContext& g = *GImGui;
    if (g.ColorStack.Size < count)
    {
        count = g.ColorStack.Size;
    }
    while (count > 0)
    {
        ImGuiColorMod& backup = g.ColorStack.back();
        g.Style.Colors[backup.Col] = backup.BackupValue;
        g.ColorStack.pop_back();
        count--;
    }
}

void c_gui::push_style_var(ImGuiStyleVar idx, float val)
{
    ImGuiContext& g = *GImGui;
    const ImGuiDataVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1)
    {
        float* pvar = (float*)var_info->GetVarPtr(&g.Style);
        g.StyleVarStack.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
}

void c_gui::push_style_var(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGuiContext& g = *GImGui;
    const ImGuiDataVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 2)
    {
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&g.Style);
        g.StyleVarStack.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
}

void c_gui::pop_style_var(int count)
{
    ImGuiContext& g = *GImGui;
    if (g.StyleVarStack.Size < count)
    {
        count = g.StyleVarStack.Size;
    }
    while (count > 0)
    {
        ImGuiStyleMod& backup = g.StyleVarStack.back();
        const ImGuiDataVarInfo* info = GetStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&g.Style);
        if (info->Type == ImGuiDataType_Float && info->Count == 1) { ((float*)data)[0] = backup.BackupFloat[0]; }
        else if (info->Type == ImGuiDataType_Float && info->Count == 2) { ((float*)data)[0] = backup.BackupFloat[0]; ((float*)data)[1] = backup.BackupFloat[1]; }
        g.StyleVarStack.pop_back();
        count--;
    }
}

void c_gui::spacing()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return;
    ImGui::ItemSize(ImVec2(0, 0));
}

void c_gui::sameline(float offset_from_start_x, float spacing_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems) return;

    if (offset_from_start_x != 0.0f)
    {
        if (spacing_w < 0.0f) spacing_w = 0.0f;
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    else
    {
        if (spacing_w < 0.0f) spacing_w = GetStyle().ItemSpacing.x;

        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
    window->DC.IsSameLine = true;
}

void c_gui::begin_group()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    g.GroupStack.resize(g.GroupStack.Size + 1);
    ImGuiGroupData& group_data = g.GroupStack.back();
    group_data.WindowID = window->ID;
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorPosPrevLine = window->DC.CursorPosPrevLine;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndent = window->DC.Indent;
    group_data.BackupGroupOffset = window->DC.GroupOffset;
    group_data.BackupCurrLineSize = window->DC.CurrLineSize;
    group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
    group_data.BackupHoveredIdIsAlive = g.HoveredId != 0;
    group_data.BackupIsSameLine = window->DC.IsSameLine;
    group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
    group_data.EmitItem = true;

    window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
    window->DC.Indent = window->DC.GroupOffset;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
    if (g.LogEnabled) g.LogLinePosY = -FLT_MAX;
}

void c_gui::end_group()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(g.GroupStack.Size > 0);

    ImGuiGroupData& group_data = g.GroupStack.back();
    IM_ASSERT(group_data.WindowID == window->ID);

    if (window->DC.IsSetPos) ErrorCheckUsingSetCursorPosToExtendParentBoundaries();

    ImRect group_bb(group_data.BackupCursorPos, ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));

    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorPosPrevLine = group_data.BackupCursorPosPrevLine;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CurrLineSize = group_data.BackupCurrLineSize;
    window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
    window->DC.IsSameLine = group_data.BackupIsSameLine;
    if (g.LogEnabled) g.LogLinePosY = -FLT_MAX;

    if (!group_data.EmitItem)
    {
        g.GroupStack.pop_back();
        return;
    }

    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);
    ImGui::ItemSize(group_bb.GetSize());
    ItemAdd(group_bb, 0, NULL, ImGuiItemFlags_NoTabStop);

    const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
    const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive == false) && (g.ActiveIdPreviousFrameIsAlive == true);
    if (group_contains_curr_active_id) g.LastItemData.ID = g.ActiveId;
    else if (group_contains_prev_active_id) g.LastItemData.ID = g.ActiveIdPreviousFrame;
    g.LastItemData.Rect = group_bb;

    const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) && g.HoveredId != 0;
    if (group_contains_curr_hovered_id) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

    if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)  g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;

    g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
    if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Deactivated;

    g.GroupStack.pop_back();
    if (g.DebugShowGroupRects) window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255, 0, 255, 255));
}

void c_widget::separator()
{
    draw->add_line(GetWindowDrawList(), GetCursorScreenPos(), GetCursorScreenPos() + ImVec2(GetContentRegionAvail().x, 0), gui->get_clr(clr->c_element.separator), 1.f);
    gui->spacing();
}

void c_gui::water_mark(std::string name, std::vector<std::string> function, watermark_position type, bool* visible)
{
    static ImVec2 content_size, pos, current_pos;

    struct spectator_state
    {
        float alpha;
    };

    spectator_state* state = gui->anim_container(&state, ImGui::GetID(name.c_str()));

    state->alpha = ImLerp(state->alpha, *visible ? 1.f : 0.f, fixed_speed(12.f));

    push_style_var(ImGuiStyleVar_Alpha, state->alpha);

    if (state->alpha >= 0.01f) {

        switch (type)
        {
        case mark_top_left:
            pos = SCALE(10, 10);
            break;
        case mark_top_right:
            pos = ImVec2(ImGui::GetIO().DisplaySize.x - content_size.x, SCALE(10));
            break;
        case mark_bottom_left:
            pos = ImVec2(SCALE(10), ImGui::GetIO().DisplaySize.y - content_size.y);
            break;
        case mark_bottom_right:
            pos = ImVec2(ImGui::GetIO().DisplaySize.x - content_size.x, ImGui::GetIO().DisplaySize.y - content_size.y);
            break;
        }

        current_pos = ImLerp(current_pos, pos, fixed_speed(25.f));

        gui->push_style_color(ImGuiCol_WindowBg, gui->get_clr(clr->c_window.layout));
        gui->push_style_var(ImGuiStyleVar_WindowRounding, SCALE(4.f));
        gui->push_style_var(ImGuiStyleVar_WindowPadding, SCALE(10, 10));
        gui->push_style_var(ImGuiStyleVar_ItemSpacing, SCALE(20, 10));

        SetNextWindowPos(current_pos);
        gui->begin("watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            for (int i = 0; i < function.size(); i++)
            {
                widget->text_colored(set->c_font.inter_medium[0], gui->get_clr(i == 0 ? clr->c_text.text_active : clr->c_text.text), function[i]);
                gui->sameline();
                draw->add_rect_filled(draw_list, ImGui::GetCursorScreenPos() - SCALE(12, 0), ImGui::GetCursorScreenPos() + SCALE(-9, 14), gui->get_clr(clr->c_child.stroke), 10.f);
                gui->sameline();
            }

            content_size = ImGui::GetContentRegionMax() + SCALE(20, 20);
        }
        gui->end();

        gui->pop_style_var(3);
        gui->pop_style_color(1);

    }
    pop_style_var();
}

#include <chrono>

std::string c_gui::get_current_date()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm* now_tm = std::localtime(&now_time);

    std::ostringstream date_stream;
    date_stream << std::setfill('0') << std::setw(2) << now_tm->tm_mday << '/'
        << std::setfill('0') << std::setw(2) << (now_tm->tm_mon + 1) << '/'
        << std::setfill('0') << std::setw(2) << (now_tm->tm_year % 100);

    return date_stream.str();
}

int rotation_start_index;
void c_gui::rotate_start()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 rotate_center()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

void c_gui::rotate_end(float rad, ImVec2 center)
{
    if (center.x == 0 && center.y == 0) center = rotate_center();

    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

float c_gui::deg_to_rad(float deg)
{
    return deg / 57.3f;
}
