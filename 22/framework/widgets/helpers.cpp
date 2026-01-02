#include "../headers/functions.h"
#include "../headers/widgets.h"

void c_gui::push_color(style_col idx, ImU32 col)
{
    gui_color_mod backup;
    backup.col = idx;
    backup.backup_value = var->style.colors[idx];
    style_col_stack.push_back(backup);
    var->style.colors[idx] = ColorConvertU32ToFloat4(col);
}

void c_gui::pop_color(int count)
{
    while (count > 0)
    {
        gui_color_mod& backup = style_col_stack.back();
        var->style.colors[backup.col] = backup.backup_value;
        style_col_stack.pop_back();
        count--;
    }
}

void c_gui::push_var(style_var idx, float val)
{
    const data_var_info* var_info = get_style_var_info(idx);
    float* pvar = (float*)var_info->get_var_ptr(&var->style);
    style_var_stack.push_back(gui_style_mod(idx, *pvar));
    *pvar = val;
}

void c_gui::push_var(style_var idx, const ImVec2& val)
{
    const data_var_info* var_info = get_style_var_info(idx);
    ImVec2* pvar = (ImVec2*)var_info->get_var_ptr(&var->style);
    style_var_stack.push_back(gui_style_mod(idx, *pvar));
    *pvar = val;
}

void c_gui::pop_var(int count)
{
    ImGuiContext& g = *GImGui;

    while (count > 0)
    {
        gui_style_mod& backup = style_var_stack.back();
        const data_var_info* info = get_style_var_info(backup.var_idx);
        void* data = info->get_var_ptr(&var->style);
        if (info->count == 1) { ((float*)data)[0] = backup.backup_float[0]; }
        else if (info->count == 2) { ((float*)data)[0] = backup.backup_float[0]; ((float*)data)[1] = backup.backup_float[1]; }
        style_var_stack.pop_back();
        count--;
    }
}

void c_gui::push_font(ImFont* font)
{
    ImGuiContext& g = *GImGui;
    if (font == NULL)
        font = GetDefaultFont();
    g.FontStack.push_back(font);
    SetCurrentFont(font);
    g.CurrentWindow->DrawList->_SetTextureID(font->ContainerAtlas->TexID);
}

void c_gui::pop_font()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.FontStack.Size > 0);
    g.FontStack.pop_back();
    ImFont* font = g.FontStack.Size == 0 ? GetDefaultFont() : g.FontStack.back();
    SetCurrentFont(font);
    g.CurrentWindow->DrawList->_SetTextureID(font->ContainerAtlas->TexID);
}

void c_gui::set_pos(const ImVec2& pos, int type)
{
    ImGuiWindow* window = GetCurrentWindow();

    if (type == pos_all)
        window->DC.CursorPos = window->Pos - window->Scroll + pos;
    else if (type == pos_x)
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + pos.x;
    else if (type == pos_y)
        window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + pos.y;

    window->DC.IsSetPos = true;
}

void c_gui::set_pos(float pos, int type)
{
    set_pos(ImVec2(pos, pos), type);
}

ImVec2 c_gui::get_pos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos + window->Scroll;
}

void c_gui::set_screen_pos(const ImVec2& pos, int type)
{
    ImGuiWindow* window = GetCurrentWindow();

    if (type == pos_all)
        window->DC.CursorPos = pos;
    else if (type == pos_x)
        window->DC.CursorPos.x = pos.x;
    else if (type == pos_y)
        window->DC.CursorPos.y = pos.y;

    window->DC.IsSetPos = true;
}

void c_gui::set_screen_pos(float pos, int type)
{
    set_screen_pos(ImVec2(pos, pos), type);
}

ImVec2 c_gui::get_screen_pos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos;
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
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX;
}

void c_gui::end_group()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(g.GroupStack.Size > 0);

    ImGuiGroupData& group_data = g.GroupStack.back();
    IM_ASSERT(group_data.WindowID == window->ID);

    if (window->DC.IsSetPos)
        ErrorCheckUsingSetCursorPosToExtendParentBoundaries();

    ImRect group_bb(group_data.BackupCursorPos, ImMax(ImMax(window->DC.CursorMaxPos, g.LastItemData.Rect.Max), group_data.BackupCursorPos));
    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorPosPrevLine = group_data.BackupCursorPosPrevLine;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, group_bb.Max);
    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CurrLineSize = group_data.BackupCurrLineSize;
    window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
    window->DC.IsSameLine = group_data.BackupIsSameLine;
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX;

    if (!group_data.EmitItem)
    {
        g.GroupStack.pop_back();
        return;
    }

    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);
    item_size(group_bb.GetSize());
    item_add(group_bb, 0, NULL, ImGuiItemFlags_NoTabStop);

    const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
    const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive == false) && (g.ActiveIdPreviousFrameIsAlive == true);
    if (group_contains_curr_active_id)
        g.LastItemData.ID = g.ActiveId;
    else if (group_contains_prev_active_id)
        g.LastItemData.ID = g.ActiveIdPreviousFrame;
    g.LastItemData.Rect = group_bb;

    const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) && g.HoveredId != 0;
    if (group_contains_curr_hovered_id)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

    if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;

    g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
    if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame)
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Deactivated;

    g.GroupStack.pop_back();
    if (g.DebugShowGroupRects)
        window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255, 0, 255, 255));   // [Debug]
}

void c_gui::begin_content(std::string_view id, const ImVec2& size, const ImVec2& padding, const ImVec2& spacing, window_flags window_flags__, child_flags child_flags__)
{
    gui->push_var(ImGuiStyleVar_WindowPadding, padding);
    gui->begin_def_child(id, size, child_flags__, window_flags__ | window_flags_no_bring_to_front_on_focus | window_flags_always_use_window_padding | window_flags_no_saved_settings | window_flags_no_focus_on_appearing);
    gui->push_var(ImGuiStyleVar_ItemSpacing, spacing);
}

void c_gui::end_content()
{
    gui->pop_var();
    gui->end_def_child();
    gui->pop_var();
}

void c_gui::sameline(float offset_from_start_x, float spacing_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    if (offset_from_start_x != 0.0f)
    {
        if (spacing_w < 0.0f)
            spacing_w = 0.0f;
        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    else
    {
        if (spacing_w < 0.0f)
            spacing_w = var->style.item_spacing.x;
        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
    window->DC.IsSameLine = true;
}

void c_gui::dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    gui->item_size(size);
    gui->item_add(bb, 0);
}

bool begin_def_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, child_flags child_flagss, window_flags window_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    IM_ASSERT(id != 0);

    const child_flags ImGuiChildFlags_SupportedMask_ = child_flags_borders | child_flags_always_use_window_padding | child_flags_resize_x | child_flags_resize_y | child_flags_auto_resize_x | child_flags_auto_resize_y | child_flags_always_auto_resize | child_flags_frame_style | child_flags_nav_flattened;
    IM_UNUSED(ImGuiChildFlags_SupportedMask_);
    IM_ASSERT((child_flagss & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
    IM_ASSERT((window_flags & window_flags_always_auto_resize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
    if (child_flagss & child_flags_always_auto_resize)
    {
        IM_ASSERT((child_flagss & (child_flags_resize_x | child_flags_resize_y)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        IM_ASSERT((child_flagss & (child_flags_auto_resize_x | child_flags_auto_resize_y)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (window_flags & window_flags_always_use_window_padding)
        child_flagss |= child_flags_always_use_window_padding;
    if (window_flags & window_flags_nav_flattened)
        child_flagss |= child_flags_nav_flattened;
#endif
    if (child_flagss & child_flags_auto_resize_x)
        child_flagss &= ~child_flags_resize_x;
    if (child_flagss & child_flags_auto_resize_y)
        child_flagss &= ~child_flags_resize_y;

    window_flags |= window_flags_child_window | window_flags_no_title_bar;
    window_flags |= (parent_window->Flags & window_flags_no_move);
    if (child_flagss & (child_flags_auto_resize_x | child_flags_auto_resize_y | child_flags_always_auto_resize))
        window_flags |= window_flags_always_auto_resize;
    if ((child_flagss & (child_flags_resize_x | child_flags_resize_y)) == 0)
        window_flags |= window_flags_no_resize | window_flags_no_saved_settings;

    if (child_flagss & child_flags_frame_style)
    {
        PushStyleColor(style_col_child_bg, var->style.colors[ImGuiCol_FrameBg]);
        PushStyleVar(style_var_child_rounding, var->style.frame_rounding);
        PushStyleVar(style_var_child_border_size, var->style.frame_border_size);
        PushStyleVar(style_var_window_padding, var->style.frame_padding);
        child_flagss |= child_flags_border | child_flags_always_use_window_padding;
        window_flags |= window_flags_no_move;
    }

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
    g.NextWindowData.ChildFlags = child_flagss;

    const ImVec2 size_avail = GetContentRegionAvail();
    const ImVec2 size_default((child_flagss & child_flags_auto_resize_x) ? 0.0f : size_avail.x, (child_flagss & child_flags_auto_resize_y) ? 0.0f : size_avail.y);
    const ImVec2 size = CalcItemSize(size_arg, size_default.x, size_default.y);
    SetNextWindowSize(size);

    const char* temp_window_name;

    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const float backup_border_size = var->style.child_border_size;
    if ((child_flagss & child_flags_borders) == 0)
        var->style.child_border_size = 0.0f;

    const bool ret = gui->begin(temp_window_name, NULL, window_flags);

    var->style.child_border_size = backup_border_size;
    if (child_flagss & child_flags_frame_style)
    {
        PopStyleVar(3);
        PopStyleColor();
    }

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;

    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
    if (g.ActiveId == temp_id_for_activation)
        ClearActiveID();
    if (g.NavActivateId == id && !(child_flagss & child_flags_nav_flattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(temp_id_for_activation, child_window);
        g.ActiveIdSource = g.NavInputSource;
    }
    return ret;
}

bool c_gui::begin_def_child(std::string_view name, const ImVec2& size_arg, child_flags child_flags, window_flags window_flags)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    return begin_def_child_ex(name.data(), id, size_arg, child_flags, window_flags);
}

void c_gui::end_def_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & window_flags_child_window);

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    gui->end();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
        item_size(child_size);
        const bool nav_flattened = (child_window->ChildFlags & child_flags_nav_flattened) != 0;
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !nav_flattened)
        {
            item_add(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {
            item_add(bb, child_window->ChildId, NULL, ImGuiItemFlags_NoNav);

            if (nav_flattened)
                parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX;
}

void c_gui::set_next_window_pos(const ImVec2& pos, gui_cond cond, const ImVec2& pivot)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond));
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos;
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = pivot;
    g.NextWindowData.PosCond = cond ? cond : gui_cond_always;
}

void c_gui::set_next_window_size(const ImVec2& size, gui_cond cond)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond));
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSize;
    g.NextWindowData.SizeVal = size;
    g.NextWindowData.SizeCond = cond ? cond : gui_cond_always;
}

ImVec2 c_gui::text_size(ImFont* fontm, const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    this->push_font(fontm);

    ImGuiContext& g = *GImGui;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindRenderedTextEnd(text, text_end);
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    const float font_size = g.FontSize;
    if (text == text_display_end)
        return ImVec2(0.0f, font_size);
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);
    text_size.x = IM_TRUNC(text_size.x + 0.99999f);

    this->pop_font();

    return text_size;
}

ImVec2 c_gui::window_size()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size;
}

float c_gui::window_width()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.x;
}

float c_gui::window_height()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Size.y;
}

ImVec2 c_gui::window_pos()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->Pos;
}

ImDrawList* c_gui::window_drawlist()
{
    ImGuiWindow* window = GetCurrentWindow();
    return window->DrawList;
}

static ImDrawList* get_viewport_bg_fg_drawlist(ImGuiViewportP* viewport, size_t drawlist_no, const char* drawlist_name)
{
    ImGuiContext& g = *GImGui;
    // Create the draw list on demand, because they are not frequently used for all viewports
    IM_ASSERT(drawlist_no < IM_ARRAYSIZE(viewport->BgFgDrawLists));
    ImDrawList* draw_list = viewport->BgFgDrawLists[drawlist_no];
    if (draw_list == NULL)
    {
        draw_list = IM_NEW(ImDrawList)(&g.DrawListSharedData);
        draw_list->_OwnerName = drawlist_name;
        viewport->BgFgDrawLists[drawlist_no] = draw_list;
    }

    if (viewport->BgFgDrawListsLastFrame[drawlist_no] != g.FrameCount)
    {
        draw_list->_ResetForNewFrame();
        draw_list->PushTextureID(g.IO.Fonts->TexID);
        draw_list->PushClipRect(viewport->Pos, viewport->Pos + viewport->Size, false);
        viewport->BgFgDrawListsLastFrame[drawlist_no] = g.FrameCount;
    }
    return draw_list;
}

ImDrawList* c_gui::foreground_drawlist()
{
    ImGuiContext& g = *GImGui;
    return get_viewport_bg_fg_drawlist((ImGuiViewportP*)g.Viewports[0], 1, "##Foreground");
}

ImDrawList* c_gui::background_drawlist()
{
    ImGuiContext& g = *GImGui;
    return get_viewport_bg_fg_drawlist((ImGuiViewportP*)g.Viewports[0], 0, "##Background");
}

ImGuiWindow* c_gui::get_window()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->WriteAccessed = true;
    return g.CurrentWindow;
}

void c_gui::push_id(const char* str_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(str_id);
    window->IDStack.push_back(id);
}

void c_gui::push_id(const char* str_id_begin, const char* str_id_end)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(str_id_begin, str_id_end);
    window->IDStack.push_back(id);
}

void c_gui::push_id(const void* ptr_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(ptr_id);
    window->IDStack.push_back(id);
}

void c_gui::push_id(int int_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(int_id);
    window->IDStack.push_back(id);
}

void c_gui::pop_id()
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    IM_ASSERT(window->IDStack.Size > 1);
    window->IDStack.pop_back();
}

ImGuiID c_gui::get_id(const char* str, const char* str_end)
{
    ImGuiID seed = get_window()->IDStack.back();
    ImGuiID id = ImHashStr(str, str_end ? (str_end - str) : 0, seed);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    ImGuiContext& g = *get_window()->Ctx;
    if (g.DebugHookIdInfo == id)
        ImGui::DebugHookIdInfo(id, ImGuiDataType_String, str, str_end);
#endif
    return id;
}

ImGuiID c_gui::get_id(const void* ptr)
{
    ImGuiID seed = get_window()->IDStack.back();
    ImGuiID id = ImHashData(&ptr, sizeof(void*), seed);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    ImGuiContext& g = *get_window()->Ctx;
    if (g.DebugHookIdInfo == id)
        ImGui::DebugHookIdInfo(id, ImGuiDataType_Pointer, ptr, NULL);
#endif
    return id;
}

ImGuiID c_gui::get_id(int n)
{
    ImGuiID seed = get_window()->IDStack.back();
    ImGuiID id = ImHashData(&n, sizeof(n), seed);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    ImGuiContext& g = *get_window()->Ctx;
    if (g.DebugHookIdInfo == id)
        ImGui::DebugHookIdInfo(id, ImGuiDataType_S32, (void*)(intptr_t)n, NULL);
#endif
    return id;
}

void c_gui::item_size(const ImVec2& size, float text_baseline_y)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    const float offset_to_match_baseline_y = (text_baseline_y >= 0) ? ImMax(0.0f, window->DC.CurrLineTextBaseOffset - text_baseline_y) : 0.0f;

    const float line_y1 = window->DC.IsSameLine ? window->DC.CursorPosPrevLine.y : window->DC.CursorPos.y;
    const float line_height = ImMax(window->DC.CurrLineSize.y, window->DC.CursorPos.y - line_y1 + size.y + offset_to_match_baseline_y);

    window->DC.CursorPosPrevLine.x = window->DC.CursorPos.x + size.x;
    window->DC.CursorPosPrevLine.y = line_y1;
    window->DC.CursorPos.x = IM_TRUNC(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
    window->DC.CursorPos.y = IM_TRUNC(line_y1 + line_height + var->style.item_spacing.y);
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y - var->style.item_spacing.y);

    window->DC.PrevLineSize.y = line_height;
    window->DC.CurrLineSize.y = 0.0f;
    window->DC.PrevLineTextBaseOffset = ImMax(window->DC.CurrLineTextBaseOffset, text_baseline_y);
    window->DC.CurrLineTextBaseOffset = 0.0f;
    window->DC.IsSameLine = window->DC.IsSetPos = false;

    if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
        sameline();
}

void c_gui::item_size(const ImRect& bb, float text_baseline_y)
{
    item_size(bb.GetSize(), text_baseline_y);
}

bool c_gui::item_add(const ImRect& bb, ImGuiID id, const ImRect* nav_bb_arg, ImGuiItemFlags extra_flags)
{
    return ItemAdd(bb, id, nav_bb_arg, extra_flags);
}

static ImGuiHoveredFlags apply_hover_flags_for_tooltip(ImGuiHoveredFlags user_flags, ImGuiHoveredFlags shared_flags)
{
    if (user_flags & (ImGuiHoveredFlags_DelayNone | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_DelayNormal))
        shared_flags &= ~(ImGuiHoveredFlags_DelayNone | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_DelayNormal);
    return user_flags | shared_flags;
}

static ImGuiWindow* get_combined_root_window(ImGuiWindow* window, bool popup_hierarchy)
{
    ImGuiWindow* last_window = NULL;
    while (last_window != window)
    {
        last_window = window;
        window = window->RootWindow;
        if (popup_hierarchy)
            window = window->RootWindowPopupTree;
    }
    return window;
}

bool c_gui::is_window_hovered(ImGuiHoveredFlags flags)
{
    IM_ASSERT((flags & ~ImGuiHoveredFlags_AllowedMaskForIsWindowHovered) == 0 && "Invalid flags for IsWindowHovered()!");

    ImGuiContext& g = *GImGui;
    ImGuiWindow* ref_window = g.HoveredWindow;
    ImGuiWindow* cur_window = g.CurrentWindow;
    if (ref_window == NULL)
        return false;

    if ((flags & ImGuiHoveredFlags_AnyWindow) == 0)
    {
        IM_ASSERT(cur_window);
        const bool popup_hierarchy = (flags & ImGuiHoveredFlags_NoPopupHierarchy) == 0;
        if (flags & ImGuiHoveredFlags_RootWindow)
            cur_window = get_combined_root_window(cur_window, popup_hierarchy);

        bool result;
        if (flags & ImGuiHoveredFlags_ChildWindows)
            result = IsWindowChildOf(ref_window, cur_window, popup_hierarchy);
        else
            result = (ref_window == cur_window);
        if (!result)
            return false;
    }

    if (!IsWindowContentHoverable(ref_window, flags))
        return false;
    if (!(flags & ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        if (g.ActiveId != 0 && !g.ActiveIdAllowOverlap && g.ActiveId != ref_window->MoveId)
            return false;

    if (flags & ImGuiHoveredFlags_ForTooltip)
        flags = apply_hover_flags_for_tooltip(flags, var->style.hover_flags_for_tooltip_mouse);
    if ((flags & ImGuiHoveredFlags_Stationary) != 0 && g.HoverWindowUnlockedStationaryId != ref_window->ID)
        return false;

    return true;
}

bool c_gui::is_window_focused(ImGuiFocusedFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* ref_window = g.NavWindow;
    ImGuiWindow* cur_window = g.CurrentWindow;

    if (ref_window == NULL)
        return false;
    if (flags & ImGuiFocusedFlags_AnyWindow)
        return true;

    IM_ASSERT(cur_window);
    const bool popup_hierarchy = (flags & ImGuiFocusedFlags_NoPopupHierarchy) == 0;
    if (flags & ImGuiHoveredFlags_RootWindow)
        cur_window = get_combined_root_window(cur_window, popup_hierarchy);

    if (flags & ImGuiHoveredFlags_ChildWindows)
        return IsWindowChildOf(ref_window, cur_window, popup_hierarchy);
    else
        return (ref_window == cur_window);
}

void c_gui::set_window_focus()
{
    FocusWindow(GImGui->CurrentWindow);
}

void c_gui::set_window_focus(const char* name)
{
    if (name)
    {
        if (ImGuiWindow* window = FindWindowByName(name))
            FocusWindow(window);
    }
    else
    {
        FocusWindow(NULL);
    }
}

bool c_gui::is_rect_visible(const ImVec2& size)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ImRect(window->DC.CursorPos, window->DC.CursorPos + size));
}

bool c_gui::is_rect_visible(const ImRect& rect)
{
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(rect);
}

ImVec2 c_gui::adjust_window_pos(const ImVec2& rect, const ImVec2& window_size)
{
    ImVec2 wpos = rect;
    ImGuiIO& io = ImGui::GetIO();
    wpos = ImClamp(wpos, ImVec2(0, 0), io.DisplaySize - window_size);

    return wpos;
}

bool c_gui::button_behavior(const ImRect& bb, ImGuiID id, bool* out_hovered, bool* out_held, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if ((flags & ImGuiButtonFlags_MouseButtonMask_) == 0)
        flags |= ImGuiButtonFlags_MouseButtonLeft;

    if ((flags & ImGuiButtonFlags_PressedOnMask_) == 0)
        flags |= ImGuiButtonFlags_PressedOnDefault_;

    ImGuiItemFlags item_flags = (g.LastItemData.ID == id ? g.LastItemData.InFlags : g.CurrentItemFlags);
    if (flags & ImGuiButtonFlags_AllowOverlap)
        item_flags |= ImGuiItemFlags_AllowOverlap;
    if (flags & ImGuiButtonFlags_Repeat)
        item_flags |= ImGuiItemFlags_ButtonRepeat;

    ImGuiWindow* backup_hovered_window = g.HoveredWindow;
    const bool flatten_hovered_children = (flags & ImGuiButtonFlags_FlattenChildren) && g.HoveredWindow && g.HoveredWindow->RootWindow == window;
    if (flatten_hovered_children)
        g.HoveredWindow = window;

#ifdef IMGUI_ENABLE_TEST_ENGINE
    if (g.LastItemData.ID != id)
        IMGUI_TEST_ENGINE_ITEM_ADD(id, bb, NULL);
#endif

    bool pressed = false;
    bool hovered = ItemHoverable(bb, id, item_flags);

    if (g.DragDropActive && (flags & ImGuiButtonFlags_PressedOnDragDropHold) && !(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
        if (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            hovered = true;
            SetHoveredID(id);
            if (g.HoveredIdTimer - g.IO.DeltaTime <= 0.7f && g.HoveredIdTimer >= 0.7f)
            {
                pressed = true;
                g.DragDropHoldJustPressedId = id;
                FocusWindow(window);
            }
        }

    if (flatten_hovered_children)
        g.HoveredWindow = backup_hovered_window;

    const ImGuiID test_owner_id = (flags & ImGuiButtonFlags_NoTestKeyOwner) ? ImGuiKeyOwner_Any : id;
    if (hovered)
    {
        IM_ASSERT(id != 0);

        int mouse_button_clicked = -1;
        int mouse_button_released = -1;
        for (int button = 0; button < 3; button++)
            if (flags & (ImGuiButtonFlags_MouseButtonLeft << button))
            {
                if (IsMouseClicked(button, ImGuiInputFlags_None, test_owner_id) && mouse_button_clicked == -1) { mouse_button_clicked = button; }
                if (IsMouseReleased(button, test_owner_id) && mouse_button_released == -1) { mouse_button_released = button; }
            }

        if (!(flags & ImGuiButtonFlags_NoKeyModifiers) || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
        {
            if (mouse_button_clicked != -1 && g.ActiveId != id)
            {
                if (!(flags & ImGuiButtonFlags_NoSetKeyOwner))
                    SetKeyOwner(MouseButtonToKey(mouse_button_clicked), id);
                if (flags & (ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnClickReleaseAnywhere))
                {
                    SetActiveID(id, window);
                    g.ActiveIdMouseButton = mouse_button_clicked;
                    if (!(flags & ImGuiButtonFlags_NoNavFocus))
                    {
                        SetFocusID(id, window);
                        FocusWindow(window);
                    }
                    else
                    {
                        FocusWindow(window, ImGuiFocusRequestFlags_RestoreFocusedChild);
                    }
                }
                if ((flags & ImGuiButtonFlags_PressedOnClick) || ((flags & ImGuiButtonFlags_PressedOnDoubleClick) && g.IO.MouseClickedCount[mouse_button_clicked] == 2))
                {
                    pressed = true;
                    if (flags & ImGuiButtonFlags_NoHoldingActiveId)
                        ClearActiveID();
                    else
                        SetActiveID(id, window);
                    g.ActiveIdMouseButton = mouse_button_clicked;
                    if (!(flags & ImGuiButtonFlags_NoNavFocus))
                    {
                        SetFocusID(id, window);
                        FocusWindow(window);
                    }
                    else
                    {
                        FocusWindow(window, ImGuiFocusRequestFlags_RestoreFocusedChild);
                    }
                }
            }
            if (flags & ImGuiButtonFlags_PressedOnRelease)
            {
                if (mouse_button_released != -1)
                {
                    const bool has_repeated_at_least_once = (item_flags & ImGuiItemFlags_ButtonRepeat) && g.IO.MouseDownDurationPrev[mouse_button_released] >= g.IO.KeyRepeatDelay; // Repeat mode trumps on release behavior
                    if (!has_repeated_at_least_once)
                        pressed = true;
                    if (!(flags & ImGuiButtonFlags_NoNavFocus))
                        SetFocusID(id, window);
                    ClearActiveID();
                }
            }

            if (g.ActiveId == id && (item_flags & ImGuiItemFlags_ButtonRepeat))
                if (g.IO.MouseDownDuration[g.ActiveIdMouseButton] > 0.0f && IsMouseClicked(g.ActiveIdMouseButton, ImGuiInputFlags_Repeat, test_owner_id))
                    pressed = true;
        }

        if (pressed)
            g.NavDisableHighlight = true;
    }

    if (g.NavId == id && !g.NavDisableHighlight && g.NavDisableMouseHover)
        if (!(flags & ImGuiButtonFlags_NoHoveredOnFocus))
            hovered = true;
    if (g.NavActivateDownId == id)
    {
        bool nav_activated_by_code = (g.NavActivateId == id);
        bool nav_activated_by_inputs = (g.NavActivatePressedId == id);
        if (!nav_activated_by_inputs && (item_flags & ImGuiItemFlags_ButtonRepeat))
        {
            const ImGuiKeyData* key1 = GetKeyData(ImGuiKey_Space);
            const ImGuiKeyData* key2 = GetKeyData(ImGuiKey_Enter);
            const ImGuiKeyData* key3 = GetKeyData(ImGuiKey_NavGamepadActivate);
            const float t1 = ImMax(ImMax(key1->DownDuration, key2->DownDuration), key3->DownDuration);
            nav_activated_by_inputs = CalcTypematicRepeatAmount(t1 - g.IO.DeltaTime, t1, g.IO.KeyRepeatDelay, g.IO.KeyRepeatRate) > 0;
        }
        if (nav_activated_by_code || nav_activated_by_inputs)
        {
            pressed = true;
            SetActiveID(id, window);
            g.ActiveIdSource = g.NavInputSource;
            if (!(flags & ImGuiButtonFlags_NoNavFocus) && !(g.NavActivateFlags & ImGuiActivateFlags_FromShortcut))
                SetFocusID(id, window);
            if (g.NavActivateFlags & ImGuiActivateFlags_FromShortcut)
                g.ActiveIdFromShortcut = true;
        }
    }

    bool held = false;
    if (g.ActiveId == id)
    {
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (g.ActiveIdIsJustActivated)
                g.ActiveIdClickOffset = g.IO.MousePos - bb.Min;

            const int mouse_button = g.ActiveIdMouseButton;
            if (mouse_button == -1)
            {
                ClearActiveID();
            }
            else if (IsMouseDown(mouse_button, test_owner_id))
            {
                held = true;
            }
            else
            {
                bool release_in = hovered && (flags & ImGuiButtonFlags_PressedOnClickRelease) != 0;
                bool release_anywhere = (flags & ImGuiButtonFlags_PressedOnClickReleaseAnywhere) != 0;
                if ((release_in || release_anywhere) && !g.DragDropActive)
                {
                    bool is_double_click_release = (flags & ImGuiButtonFlags_PressedOnDoubleClick) && g.IO.MouseReleased[mouse_button] && g.IO.MouseClickedLastCount[mouse_button] == 2;
                    bool is_repeating_already = (item_flags & ImGuiItemFlags_ButtonRepeat) && g.IO.MouseDownDurationPrev[mouse_button] >= g.IO.KeyRepeatDelay;
                    bool is_button_avail_or_owned = TestKeyOwner(MouseButtonToKey(mouse_button), test_owner_id);
                    if (!is_double_click_release && !is_repeating_already && is_button_avail_or_owned)
                        pressed = true;
                }
                ClearActiveID();
            }
            if (!(flags & ImGuiButtonFlags_NoNavFocus))
                g.NavDisableHighlight = true;
        }
        else if (g.ActiveIdSource == ImGuiInputSource_Keyboard || g.ActiveIdSource == ImGuiInputSource_Gamepad)
        {
            if (g.NavActivateDownId == id)
                held = true;
            else
                ClearActiveID();
        }
        if (pressed)
            g.ActiveIdHasBeenPressedBefore = true;
    }

    if (g.NavHighlightActivatedId == id)
        hovered = true;

    if (out_hovered) *out_hovered = hovered;
    if (out_held) *out_held = held;

    return pressed;
}

bool c_gui::invisible_button(const char* str_id, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

    const ImGuiID id = window->GetID(str_id);
    ImVec2 size = CalcItemSize(size_arg, 0.0f, 0.0f);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    item_size(size);
    if (!item_add(bb, id))
        return false;

    bool hovered, held;
    bool pressed = button_behavior(bb, id, &hovered, &held, flags);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

ImVec4 c_gui::u32_to_float4(ImU32 in)
{
    float s = 1.0f / 255.0f;
    return ImVec4(
        ((in >> IM_COL32_R_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_G_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_B_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_A_SHIFT) & 0xFF) * s);
}

ImU32 c_gui::float4_to_u32(const ImVec4& in)
{
    ImU32 out;
    out = ((ImU32)IM_F32_TO_INT8_SAT(in.x)) << IM_COL32_R_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.y)) << IM_COL32_G_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.z)) << IM_COL32_B_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.w)) << IM_COL32_A_SHIFT;
    return out;
}

void c_gui::rgb_to_hsv(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
    float K = 0.f;
    if (g < b)
    {
        ImSwap(g, b);
        K = -1.f;
    }
    if (r < g)
    {
        ImSwap(r, g);
        K = -2.f / 6.f - K;
    }

    const float chroma = r - (g < b ? g : b);
    out_h = ImFabs(K + (g - b) / (6.f * chroma + 1e-20f));
    out_s = chroma / (r + 1e-20f);
    out_v = r;
}

void c_gui::hsv_to_rgb(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
    if (s == 0.0f)
    {
        out_r = out_g = out_b = v;
        return;
    }

    h = ImFmod(h, 1.0f) / (60.0f / 360.0f);
    int   i = (int)h;
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
    case 0: out_r = v; out_g = t; out_b = p; break;
    case 1: out_r = q; out_g = v; out_b = p; break;
    case 2: out_r = p; out_g = v; out_b = t; break;
    case 3: out_r = p; out_g = q; out_b = v; break;
    case 4: out_r = t; out_g = p; out_b = v; break;
    case 5: default: out_r = v; out_g = p; out_b = q; break;
    }
}

bool c_gui::item_hoverable(const ImRect& bb, ImGuiID id, ImGuiItemFlags item_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    if (id != 0 && g.HoveredIdPreviousFrame == id && (item_flags & ImGuiItemFlags_AllowDuplicateId) == 0)
    {
        g.HoveredIdPreviousFrameItemCount++;
        if (g.DebugDrawIdConflicts == id)
            window->DrawList->AddRect(bb.Min - ImVec2(1, 1), bb.Max + ImVec2(1, 1), IM_COL32(255, 0, 0, 255), 0.0f, ImDrawFlags_None, 2.0f);
    }
#endif

    if (g.HoveredWindow != window)
        return false;
    if (!IsMouseHoveringRect(bb.Min, bb.Max))
        return false;

    if (g.HoveredId != 0 && g.HoveredId != id && !g.HoveredIdAllowOverlap)
        return false;
    if (g.ActiveId != 0 && g.ActiveId != id && !g.ActiveIdAllowOverlap)
        if (!g.ActiveIdFromShortcut)
            return false;

    if (!(item_flags & ImGuiItemFlags_NoWindowHoverableCheck) && !IsWindowContentHoverable(window, ImGuiHoveredFlags_None))
    {
        g.HoveredIdIsDisabled = true;
        return false;
    }

    if (id != 0)
    {
        if (g.DragDropActive && g.DragDropPayload.SourceId == id && !(g.DragDropSourceFlags & ImGuiDragDropFlags_SourceNoDisableHover))
            return false;

        SetHoveredID(id);

        if (item_flags & ImGuiItemFlags_AllowOverlap)
        {
            g.HoveredIdAllowOverlap = true;
            if (g.HoveredIdPreviousFrame != id)
                return false;
        }

        if (id == g.LastItemData.ID && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HasShortcut) && g.ActiveId != id)
            if (IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_DelayNormal))
                SetTooltip("%s", GetKeyChordName(g.LastItemData.Shortcut));
    }

    if (item_flags & ImGuiItemFlags_Disabled)
    {
        if (g.ActiveId == id && id != 0)
            ClearActiveID();
        g.HoveredIdIsDisabled = true;
        return false;
    }

#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    if (id != 0)
    {
        if (g.DebugItemPickerActive && g.HoveredIdPreviousFrame == id)
            GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(255, 255, 0, 255));
        if (g.DebugItemPickerBreakId == id)
            IM_DEBUG_BREAK();
    }
#endif

    if (g.NavDisableMouseHover)
        return false;

    return true;
}

bool c_gui::is_item_hovered(ImGuiHoveredFlags flags)
{
    return IsItemHovered(flags);
}

bool c_gui::is_item_active()
{
    ImGuiContext& g = *GImGui;
    if (g.ActiveId)
        return g.ActiveId == g.LastItemData.ID;
    return false;
}

bool c_gui::is_item_clicked(mouse_button mouse_button)
{
    return mouse_clicked(mouse_button) && is_item_hovered(ImGuiHoveredFlags_None);
}

bool c_gui::mouse_down(mouse_button button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDown[button] && TestKeyOwner(MouseButtonToKey(button), ImGuiKeyOwner_Any);
}

bool c_gui::mouse_clicked(mouse_button button, bool repeat)
{
    return IsMouseClicked(button, repeat ? ImGuiInputFlags_Repeat : ImGuiInputFlags_None, ImGuiKeyOwner_Any);
}

bool c_gui::mouse_released(mouse_button button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseReleased[button] && TestKeyOwner(MouseButtonToKey(button), ImGuiKeyOwner_Any);
}

bool c_gui::mouse_double_clicked(mouse_button button)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseClickedCount[button] == 2 && TestKeyOwner(MouseButtonToKey(button), ImGuiKeyOwner_Any);
}

ImVec2 c_gui::content_avail()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = (window->DC.CurrentColumns || g.CurrentTable) ? window->WorkRect.Max : window->ContentRegionRect.Max;
    return mx - window->DC.CursorPos;
}

ImVec2 c_gui::content_max()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImVec2 mx = (window->DC.CurrentColumns || g.CurrentTable) ? window->WorkRect.Max : window->ContentRegionRect.Max;
    return mx - window->Pos;
}

const char* c_gui::text_end(const char* text)
{
    const char* text_display_end = text;
    const char* text_end = text + strlen(text);

    while (text_display_end < text_end && *text_display_end != '\0' && (text_display_end[0] != '#' || text_display_end[1] != '#'))
        text_display_end++;
    return text_display_end;
}

bool c_gui::is_window_cond(ImGuiWindow* window, const std::vector<std::string>& names)
{
    if (window)
    {
        for (const auto& name : names)
        {
            if (strstr(window->Name, name.c_str()) != nullptr)
            {
                return true;
            }
        }
    }

    return false;
}

ImVec2 c_gui::mouse_pos()
{
    ImGuiContext& g = *GImGui;
    return g.IO.MousePos;
}

void c_gui::set_style()
{
    auto* style = &var->style;

    style->window_padding = s_(elements->padding);
    style->item_spacing = s_(elements->padding);
    style->window_border_size = 0.f;
    style->window_shadow_offset = s_(0, 0);
    style->window_shadow_size = 0.f;
    style->scrollbar_size = s_(10.f);
    style->scrollbar_content_padding = s_(10.f);
    style->scrollbar_border_padding = s_(0, 0);
    
}

void c_gui::draw_decorations()
{
    c_window* window = gui->get_window();

    c_vec2 pos = window->Pos;
    c_vec2 size = window->Size;
    c_rect rect = c_rect(pos, pos + size);

    c_draw_list* draw_list = gui->background_drawlist();

    draw->rect_filled(draw_list, rect.Min, rect.Max, draw->get_clr(clr->layout), s_(elements->window.rounding));
}

void c_gui::initialize()
{
    notify->setup_notify();
}

ImRect c_gui::get_item_rect()
{
    return ImRect(GetItemRectMin(), GetItemRectMax());
}
