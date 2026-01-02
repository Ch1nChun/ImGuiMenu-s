#include "../headers/includes.h"

namespace ImStb
{
#include "imstb_textedit.h"
}

static int input_text_calc_text_len_and_line_count(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (true)
    {
        const char* s_eol = strchr(s, '\n');
        line_count++;
        if (s_eol == NULL)
        {
            s = s + strlen(s);
            break;
        }
        s = s_eol + 1;
    }
    *out_text_end = s;
    return line_count;
}

static ImVec2 input_text_calc_text_size(ImGuiContext* ctx, const char* text_begin, const char* text_end, const char** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false)
{
    ImGuiContext& g = *ctx;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const char* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
            s += 1;
        else
            s += ImTextCharFromUtf8(&c, s, text_end);

        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX.Data[c] : font->FallbackAdvanceX) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        *out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

    if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

namespace ImStb
{
    static int     STB_TEXTEDIT_STRINGLEN(const ImGuiInputTextState* obj) { return obj->CurLenA; }
    static char    STB_TEXTEDIT_GETCHAR(const ImGuiInputTextState* obj, int idx) { IM_ASSERT(idx <= obj->CurLenA); return obj->TextA[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(ImGuiInputTextState* obj, int line_start_idx, int char_idx) { unsigned int c; ImTextCharFromUtf8(&c, obj->TextA.Data + line_start_idx + char_idx, obj->TextA.Data + obj->TextA.Size); if ((ImWchar)c == '\n') return IMSTB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *obj->Ctx; return g.Font->GetCharAdvance((ImWchar)c) * g.FontScale; }
    static char    STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, ImGuiInputTextState* obj, int line_start_idx)
    {
        const char* text = obj->TextA.Data;
        const char* text_remaining = NULL;
        const ImVec2 size = input_text_calc_text_size(obj->Ctx, text + line_start_idx, text + obj->CurLenA, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

#define IMSTB_TEXTEDIT_GETNEXTCHARINDEX  IMSTB_TEXTEDIT_GETNEXTCHARINDEX_IMPL
#define IMSTB_TEXTEDIT_GETPREVCHARINDEX  IMSTB_TEXTEDIT_GETPREVCHARINDEX_IMPL

    static int IMSTB_TEXTEDIT_GETNEXTCHARINDEX_IMPL(ImGuiInputTextState* obj, int idx)
    {
        if (idx >= obj->CurLenA)
            return obj->CurLenA + 1;
        unsigned int c;
        return idx + ImTextCharFromUtf8(&c, obj->TextA.Data + idx, obj->TextA.Data + obj->TextA.Size);
    }

    static int IMSTB_TEXTEDIT_GETPREVCHARINDEX_IMPL(ImGuiInputTextState* obj, int idx)
    {
        if (idx <= 0)
            return -1;
        const char* p = ImTextFindPreviousUtf8Codepoint(obj->TextA.Data, obj->TextA.Data + idx);
        return (int)(p - obj->TextA.Data);
    }

    static bool ImCharIsSeparatorW(unsigned int c)
    {
        static const unsigned int separator_list[] =
        {
            ',', 0x3001, '.', 0x3002, ';', 0xFF1B, '(', 0xFF08, ')', 0xFF09, '{', 0xFF5B, '}', 0xFF5D,
            '[', 0x300C, ']', 0x300D, '|', 0xFF5C, '!', 0xFF01, '\\', 0xFFE5, '/', 0x30FB, 0xFF0F,
            '\n', '\r',
        };
        for (unsigned int separator : separator_list)
            if (c == separator)
                return true;
        return false;
    }

    static int is_word_boundary_from_right(ImGuiInputTextState* obj, int idx)
    {
        // When ImGuiInputTextFlags_Password is set, we don't want actions such as CTRL+Arrow to leak the fact that underlying data are blanks or separators.
        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        const char* curr_p = obj->TextA.Data + idx;
        const char* prev_p = ImTextFindPreviousUtf8Codepoint(obj->TextA.Data, curr_p);
        unsigned int curr_c; ImTextCharFromUtf8(&curr_c, curr_p, obj->TextA.Data + obj->TextA.Size);
        unsigned int prev_c; ImTextCharFromUtf8(&prev_c, prev_p, obj->TextA.Data + obj->TextA.Size);

        bool prev_white = ImCharIsBlankW(prev_c);
        bool prev_separ = ImCharIsSeparatorW(prev_c);
        bool curr_white = ImCharIsBlankW(curr_c);
        bool curr_separ = ImCharIsSeparatorW(curr_c);
        return ((prev_white || prev_separ) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int is_word_boundary_from_left(ImGuiInputTextState* obj, int idx)
    {
        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        const char* curr_p = obj->TextA.Data + idx;
        const char* prev_p = ImTextFindPreviousUtf8Codepoint(obj->TextA.Data, curr_p);
        unsigned int prev_c; ImTextCharFromUtf8(&prev_c, curr_p, obj->TextA.Data + obj->TextA.Size);
        unsigned int curr_c; ImTextCharFromUtf8(&curr_c, prev_p, obj->TextA.Data + obj->TextA.Size);

        bool prev_white = ImCharIsBlankW(prev_c);
        bool prev_separ = ImCharIsSeparatorW(prev_c);
        bool curr_white = ImCharIsBlankW(curr_c);
        bool curr_separ = ImCharIsSeparatorW(curr_c);
        return ((prev_white) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(ImGuiInputTextState* obj, int idx)
    {
        idx = IMSTB_TEXTEDIT_GETPREVCHARINDEX(obj, idx);
        while (idx >= 0 && !is_word_boundary_from_right(obj, idx))
            idx = IMSTB_TEXTEDIT_GETPREVCHARINDEX(obj, idx);
        return idx < 0 ? 0 : idx;
    }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_MAC(ImGuiInputTextState* obj, int idx)
    {
        int len = obj->CurLenA;
        idx = IMSTB_TEXTEDIT_GETNEXTCHARINDEX(obj, idx);
        while (idx < len && !is_word_boundary_from_left(obj, idx))
            idx = IMSTB_TEXTEDIT_GETNEXTCHARINDEX(obj, idx);
        return idx > len ? len : idx;
    }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_WIN(ImGuiInputTextState* obj, int idx)
    {
        idx = IMSTB_TEXTEDIT_GETNEXTCHARINDEX(obj, idx);
        int len = obj->CurLenA;
        while (idx < len && !is_word_boundary_from_right(obj, idx))
            idx = IMSTB_TEXTEDIT_GETNEXTCHARINDEX(obj, idx);
        return idx > len ? len : idx;
    }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(ImGuiInputTextState* obj, int idx) { ImGuiContext& g = *obj->Ctx; if (g.IO.ConfigMacOSXBehaviors) return STB_TEXTEDIT_MOVEWORDRIGHT_MAC(obj, idx); else return STB_TEXTEDIT_MOVEWORDRIGHT_WIN(obj, idx); }
#define STB_TEXTEDIT_MOVEWORDLEFT       STB_TEXTEDIT_MOVEWORDLEFT_IMPL  // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT      STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(ImGuiInputTextState* obj, int pos, int n)
    {
        char* dst = obj->TextA.Data + pos;

        obj->Edited = true;
        obj->CurLenA -= n;

        // Offset remaining text (FIXME-OPT: Use memmove)
        const char* src = obj->TextA.Data + pos + n;
        while (char c = *src++)
            *dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(ImGuiInputTextState* obj, int pos, const char* new_text, int new_text_len)
    {
        const bool is_resizable = (obj->Flags & ImGuiInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenA;
        IM_ASSERT(pos <= text_len);

        if (!is_resizable && (new_text_len + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        // Grow internal buffer if needed
        if (new_text_len + text_len + 1 > obj->TextA.Size)
        {
            if (!is_resizable)
                return false;
            obj->TextA.resize(text_len + ImClamp(new_text_len, 32, ImMax(256, new_text_len)) + 1);
        }

        char* text = obj->TextA.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos));
        memcpy(text + pos, new_text, (size_t)new_text_len);

        obj->Edited = true;
        obj->CurLenA += new_text_len;
        obj->TextA[obj->CurLenA] = '\0';

        return true;
    }

    // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define STB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define STB_TEXTEDIT_K_SHIFT        0x400000

#define IMSTB_TEXTEDIT_IMPLEMENTATION
#define IMSTB_TEXTEDIT_memmove memmove
#include "imstb_textedit.h"

// stb_textedit internally allows for a single undo record to do addition and deletion, but somehow, calling
// the stb_textedit_paste() function creates two separate records, so we perform it manually. (FIXME: Report to nothings/stb?)
    static void stb_textedit_replace(ImGuiInputTextState* str, STB_TexteditState* state, const IMSTB_TEXTEDIT_CHARTYPE* text, int text_len)
    {
        stb_text_makeundo_replace(str, state, 0, str->CurLenA, text_len);
        ImStb::STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenA);
        state->cursor = state->select_start = state->select_end = 0;
        if (text_len <= 0)
            return;
        if (ImStb::STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
        {
            state->cursor = state->select_start = state->select_end = text_len;
            state->has_preferred_x = 0;
            return;
        }
        IM_ASSERT(0); // Failed to insert character, normally shouldn't happen because of how we currently use stb_textedit_replace()
    }

} // namespace ImStb

static bool input_text_filter_character(ImGuiContext* ctx, unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data, bool input_source_is_clipboard = false)
{
    unsigned int c = *p_char;

    // Filter non-printable (NB: isprint is unreliable! see #2467)
    bool apply_named_filters = true;
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n') && (flags & ImGuiInputTextFlags_Multiline) != 0; // Note that an Enter KEY will emit \r and be ignored (we poll for KEY in InputText() code)
        pass |= (c == '\t') && (flags & ImGuiInputTextFlags_AllowTabInput) != 0;
        if (!pass)
            return false;
        apply_named_filters = false; // Override named filters below so newline and tabs can still be inserted.
    }

    if (input_source_is_clipboard == false)
    {
        // We ignore Ascii representation of delete (emitted from Backspace on OSX, see #2578, #2817)
        if (c == 127)
            return false;

        // Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
        if (c >= 0xE000 && c <= 0xF8FF)
            return false;
    }

    // Filter Unicode ranges we are not handling in this build
    if (c > IM_UNICODE_CODEPOINT_MAX)
        return false;

    // Generic named filters
    if (apply_named_filters && (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsScientific | (ImGuiInputTextFlags)ImGuiInputTextFlags_LocalizeDecimalPoint)))
    {
        // The libc allows overriding locale, with e.g. 'setlocale(LC_NUMERIC, "de_DE.UTF-8");' which affect the output/input of printf/scanf to use e.g. ',' instead of '.'.
        // The standard mandate that programs starts in the "C" locale where the decimal point is '.'.
        // We don't really intend to provide widespread support for it, but out of empathy for people stuck with using odd API, we support the bare minimum aka overriding the decimal point.
        // Change the default decimal_point with:
        //   ImGui::GetPlatformIO()->Platform_LocaleDecimalPoint = *localeconv()->decimal_point;
        // Users of non-default decimal point (in particular ',') may be affected by word-selection logic (is_word_boundary_from_right/is_word_boundary_from_left) functions.
        ImGuiContext& g = *ctx;
        const unsigned c_decimal_point = (unsigned int)g.PlatformIO.Platform_LocaleDecimalPoint;
        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific | (ImGuiInputTextFlags)ImGuiInputTextFlags_LocalizeDecimalPoint))
            if (c == '.' || c == ',')
                c = c_decimal_point;

        // Full-width -> half-width conversion for numeric fields (https://en.wikipedia.org/wiki/Halfwidth_and_Fullwidth_Forms_(Unicode_block)
        // While this is mostly convenient, this has the side-effect for uninformed users accidentally inputting full-width characters that they may
        // scratch their head as to why it works in numerical fields vs in generic text fields it would require support in the font.
        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific | ImGuiInputTextFlags_CharsHexadecimal))
            if (c >= 0xFF01 && c <= 0xFF5E)
                c = c - 0xFF01 + 0x21;

        // Allow 0-9 . - + * /
        if (flags & ImGuiInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        // Allow 0-9 . - + * / e E
        if (flags & ImGuiInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        // Allow 0-9 a-F A-F
        if (flags & ImGuiInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        // Turn a-z into A-Z
        if (flags & ImGuiInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                c += (unsigned int)('A' - 'a');

        if (flags & ImGuiInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;

        *p_char = c;
    }

    // Custom callback filter
    if (flags & ImGuiInputTextFlags_CallbackCharFilter)
    {
        ImGuiContext& g = *GImGui;
        ImGuiInputTextCallbackData callback_data;
        callback_data.Ctx = &g;
        callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

static void input_text_reconcile_undo_state_after_user_callback(ImGuiInputTextState* state, const char* new_buf_a, int new_length_a)
{
    const char* old_buf = state->CallbackTextBackup.Data;
    const int old_length = state->CallbackTextBackup.Size - 1;

    const int shorter_length = ImMin(old_length, new_length_a);
    int first_diff;
    for (first_diff = 0; first_diff < shorter_length; first_diff++)
        if (old_buf[first_diff] != new_buf_a[first_diff])
            break;
    if (first_diff == old_length && first_diff == new_length_a)
        return;

    int old_last_diff = old_length - 1;
    int new_last_diff = new_length_a - 1;
    for (; old_last_diff >= first_diff && new_last_diff >= first_diff; old_last_diff--, new_last_diff--)
        if (old_buf[old_last_diff] != new_buf_a[new_last_diff])
            break;

    const int insert_len = new_last_diff - first_diff + 1;
    const int delete_len = old_last_diff - first_diff + 1;
    if (insert_len > 0 || delete_len > 0)
        if (IMSTB_TEXTEDIT_CHARTYPE* p = stb_text_createundo(&state->Stb->undostate, first_diff, delete_len, insert_len))
            for (int i = 0; i < delete_len; i++)
                p[i] = old_buf[first_diff + i];
}

bool text_field_ex(std::string_view label, std::string_view hint, char* buf, int buf_size, float text_padding, float line_padding, ImU32 text_col, bool centered, const ImRect& rect, bool* active, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* callback_user_data, bool user_clicked)
{
    struct text_field_state
    {
        float offset{ 0 };
        float alpha{ 0 };
        float buf_size{ 0 };
        float cursor_alpha{ 0 };
        ImVec2 get_draw_scroll{ 0, 0 };
        ImVec2 get_cursor_offset{ 0, 0 };
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(buf != NULL && buf_size >= 0);
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;

    if (is_multiline)
        gui->begin_group();
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;

    text_field_state* animstate = gui->anim_container<text_field_state>(id);

    ImGuiWindow* draw_window = window;
    ImVec2 inner_size = rect.GetSize();
    ImGuiLastItemData item_data_backup;
    if (is_multiline)
    {
        ImVec2 backup_pos = window->DC.CursorPos;
        gui->item_size(rect, style.FramePadding.y);
        if (!gui->item_add(rect, id, &rect, ImGuiItemFlags_Inputable))
        {
            gui->end_group();
            return false;
        }
        item_data_backup = g.LastItemData;
        window->DC.CursorPos = backup_pos;

        // Prevent NavActivation from Tabbing when our widget accepts Tab inputs: this allows cycling through widgets without stopping.
        if (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_FromTabbing) && (flags & ImGuiInputTextFlags_AllowTabInput))
            g.NavActivateId = 0;

        // Prevent NavActivate reactivating in BeginChild() when we are already active.
        const ImGuiID backup_activate_id = g.NavActivateId;
        if (g.ActiveId == id) // Prevent reactivation
            g.NavActivateId = 0;

        // We reproduce the contents of BeginChildFrame() in order to provide 'label' so our window internal data are easier to read/debug.
        bool child_visible = gui->begin_def_child(label.data(), rect.GetSize(), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove);
        g.NavActivateId = backup_activate_id;
        if (!child_visible)
        {
            gui->end_def_child();
            gui->end_group();
            return false;
        }
        draw_window = g.CurrentWindow; // Child window
        draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent); // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
        draw_window->DC.CursorPos += style.FramePadding;
        inner_size.x -= draw_window->ScrollbarSizes.x;
    }
    else
    {
        // Support for internal ImGuiInputTextFlags_MergedItem flag, which could be redesigned as an ItemFlags if needed (with test performed in ItemAdd)
        /*gui->item_size(rect, style.FramePadding.y);
        if (!(flags & ImGuiInputTextFlags_MergedItem))
            if (!gui->item_add(rect, id, &rect, ImGuiItemFlags_Inputable))
                return false;*/
    }
    const bool hovered = gui->item_hoverable(rect, id, g.LastItemData.InFlags);
    gui->push_font(g.Font);

    // We are only allowed to access the state if we are already the active widget.
    ImGuiInputTextState* state = GetInputTextState(id);

    if (g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly)
        flags |= ImGuiInputTextFlags_ReadOnly;
    const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & ImGuiInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL); // Must provide a callback if you set the ImGuiInputTextFlags_CallbackResize flag!

    const bool input_requested_by_nav = (g.ActiveId != id) && ((g.NavActivateId == id) && ((g.NavActivateFlags & ImGuiActivateFlags_PreferInput) || (g.NavInputSource == ImGuiInputSource_Keyboard)));

    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    bool clear_active_id = false;
    bool select_all = false;

    float scroll_y = is_multiline ? draw_window->Scroll.y : FLT_MAX;

    const bool init_reload_from_user_buf = (state != NULL && state->ReloadUserBuf);
    const bool init_changed_specs = (state != NULL && state->Stb->single_line != !is_multiline); // state != NULL means its our state.
    const bool init_make_active = (user_clicked || user_scroll_finish || input_requested_by_nav);
    const bool init_state = (init_make_active || user_scroll_active);
    if ((init_state && g.ActiveId != id) || init_changed_specs || init_reload_from_user_buf)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();
        state->ReloadUserBuf = false;

        // Backup state of deactivating item so they'll have a chance to do a write to output buffer on the same frame they report IsItemDeactivatedAfterEdit (#4714)
        InputTextDeactivateHook(state->ID);

        // From the moment we focused we are normally ignoring the content of 'buf' (unless we are in read-only mode)
        const int buf_len = (int)strlen(buf);
        if (!init_reload_from_user_buf)
        {
            // Take a copy of the initial buffer value.
            state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
            memcpy(state->InitialTextA.Data, buf, buf_len + 1);
        }

        // Preserve cursor position and undo/redo stack if we come back to same widget
        // FIXME: Since we reworked this on 2022/06, may want to differentiate recycle_cursor vs recycle_undostate?
        bool recycle_state = (state->ID == id && !init_changed_specs && !init_reload_from_user_buf);
        if (recycle_state && (state->CurLenA != buf_len || (strncmp(state->TextA.Data, buf, buf_len) != 0)))
            recycle_state = false;

        // Start edition
        state->ID = id;
        state->TextA.resize(buf_size + 1);          // we use +1 to make sure that .Data is always pointing to at least an empty string.
        state->CurLenA = (int)strlen(buf);
        memcpy(state->TextA.Data, buf, state->CurLenA + 1);

        if (recycle_state)
        {
            // Recycle existing cursor/selection/undo stack but clamp position
            // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
            state->CursorClamp();
        }
        else
        {
            state->Scroll = ImVec2(0.0f, 0.0f);
            stb_textedit_initialize_state(state->Stb, !is_multiline);
        }

        if (init_reload_from_user_buf)
        {
            state->Stb->select_start = state->ReloadSelectionStart;
            state->Stb->cursor = state->Stb->select_end = state->ReloadSelectionEnd;
            state->CursorClamp();
        }
        else if (!is_multiline)
        {
            if (flags & ImGuiInputTextFlags_AutoSelectAll)
                select_all = true;
            if (input_requested_by_nav && (!recycle_state || !(g.NavActivateFlags & ImGuiActivateFlags_TryToPreserveState)))
                select_all = true;
            if (user_clicked && io.KeyCtrl)
                select_all = true;
        }

        if (flags & ImGuiInputTextFlags_AlwaysOverwrite)
            state->Stb->insert_mode = 1; // stb field name is indeed incorrect (see #2863)
    }

    const bool is_osx = io.ConfigMacOSXBehaviors;
    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
    }
    if (g.ActiveId == id)
    {
        // Declare some inputs, the other are registered and polled via Shortcut() routing system.
        if (user_clicked)
            SetKeyOwner(ImGuiKey_MouseLeft, id);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        if (is_multiline || (flags & ImGuiInputTextFlags_CallbackHistory))
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        SetKeyOwner(ImGuiKey_Enter, id);
        SetKeyOwner(ImGuiKey_KeypadEnter, id);
        SetKeyOwner(ImGuiKey_Home, id);
        SetKeyOwner(ImGuiKey_End, id);
        if (is_multiline)
        {
            SetKeyOwner(ImGuiKey_PageUp, id);
            SetKeyOwner(ImGuiKey_PageDown, id);
        }
        // FIXME: May be a problem to always steal Alt on OSX, would ideally still allow an uninterrupted Alt down-up to toggle menu
        if (is_osx)
            SetKeyOwner(ImGuiMod_Alt, id);

        // Expose scroll in a manner that is agnostic to us using a child window
        if (is_multiline && state != NULL)
            state->Scroll.y = draw_window->Scroll.y;
    }

    // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    // Release focus when we click outside
    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
        clear_active_id = true;

    // Lock the decision of whether we are going to take the path displaying the cursor or selection
    bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && (state->HasSelection() || select_all) && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool validated = false;

    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state;
    const bool is_displaying_hint = false;

    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('D');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        state->Edited = false;
        state->BufCapacityA = buf_size;
        state->Flags = flags;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];

        const float text_width = animstate->buf_size;

        // Edit in progress
        const float mouse_x = centered ? (io.MousePos.x - rect.Min.x - style.FramePadding.x) + state->Scroll.x - ((rect.GetWidth() - text_width) / 2.0f) : (io.MousePos.x - rect.Min.x - text_padding) + state->Scroll.x;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y) : (g.FontSize * 0.5f));

        if (select_all)
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        }
        else if (hovered && io.MouseClickedCount[0] >= 2 && !io.KeyShift)
        {
            stb_textedit_click(state, state->Stb, mouse_x, mouse_y);
            const int multiclick_count = (io.MouseClickedCount[0] - 2);
            if ((multiclick_count % 2) == 0)
            {
                // Double-click: Select word
                // We always use the "Mac" word advance for double-click select vs CTRL+Right which use the platform dependent variant:
                // FIXME: There are likely many ways to improve this behavior, but there's no "right" behavior (depends on use-case, software, OS)
                const bool is_bol = (state->Stb->cursor == 0) || ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb->cursor - 1) == '\n';
                if (STB_TEXT_HAS_SELECTION(state->Stb) || !is_bol)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
                //state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
                if (!STB_TEXT_HAS_SELECTION(state->Stb))
                    ImStb::stb_textedit_prep_selection_at_cursor(state->Stb);
                state->Stb->cursor = ImStb::STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb->cursor);
                state->Stb->select_end = state->Stb->cursor;
                ImStb::stb_textedit_clamp(state, state->Stb);
            }
            else
            {
                // Triple-click: Select line
                const bool is_eol = ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb->cursor) == '\n';
                state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART);
                state->OnKeyPressed(STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_SHIFT);
                state->OnKeyPressed(STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_SHIFT);
                if (!is_eol && is_multiline)
                {
                    ImSwap(state->Stb->select_start, state->Stb->select_end);
                    state->Stb->cursor = state->Stb->select_end;
                }
                state->CursorFollow = false;
            }
            state->CursorAnimReset();
        }
        else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                if (io.KeyShift)
                    stb_textedit_drag(state, state->Stb, mouse_x, mouse_y);
                else
                    stb_textedit_click(state, state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        }
        else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        // We expect backends to emit a Tab key but some also emit a Tab character which we ignore (#2467, #1336)
        // (For Tab and Enter: Win32/SFML/Allegro are sending both keys and chars, GLFW and SDL are only sending keys. For Space they all send all threes)
        if ((flags & ImGuiInputTextFlags_AllowTabInput) && !is_readonly)
        {
            if (Shortcut(ImGuiKey_Tab, ImGuiInputFlags_Repeat, id))
            {
                unsigned int c = '\t'; // Insert TAB
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data))
                    state->OnCharPressed(c);
            }
            // FIXME: Implement Shift+Tab
            /*
            if (Shortcut(ImGuiKey_Tab | ImGuiMod_Shift, ImGuiInputFlags_Repeat, id))
            {
            }
            */
        }

        // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
        // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeyCtrl);
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !input_requested_by_nav)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {
                    // Insert character if they pass filtering
                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t') // Skip Tab, see above.
                        continue;
                    if (input_text_filter_character(&g, &c, flags, callback, callback_user_data))
                        state->OnCharPressed(c);
                }

            // Consume characters
            io.InputQueueCharacters.resize(0);
        }
    }

    // Process other shortcuts/key-presses
    bool revert_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);

        const int row_count_per_page = ImMax((int)((inner_size.y - style.FramePadding.y) / g.FontSize), 1);
        state->Stb->row_count_per_page = row_count_per_page;

        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = is_osx && io.KeyCtrl && !io.KeySuper && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End

        // Using Shortcut() with ImGuiInputFlags_RouteFocused (default policy) to allow routing operations for other code (e.g. calling window trying to use CTRL+A and CTRL+B: formet would be handled by InputText)
        // Otherwise we could simply assume that we own the keys as we are active.
        const ImGuiInputFlags f_repeat = ImGuiInputFlags_Repeat;
        const bool is_cut = (Shortcut(ImGuiMod_Ctrl | ImGuiKey_X, f_repeat, id) || Shortcut(ImGuiMod_Shift | ImGuiKey_Delete, f_repeat, id)) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = (Shortcut(ImGuiMod_Ctrl | ImGuiKey_C, 0, id) || Shortcut(ImGuiMod_Ctrl | ImGuiKey_Insert, 0, id)) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = (Shortcut(ImGuiMod_Ctrl | ImGuiKey_V, f_repeat, id) || Shortcut(ImGuiMod_Shift | ImGuiKey_Insert, f_repeat, id)) && !is_readonly;
        const bool is_undo = (Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, f_repeat, id)) && !is_readonly && is_undoable;
        const bool is_redo = (Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, f_repeat, id) || (is_osx && Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z, f_repeat, id))) && !is_readonly && is_undoable;
        const bool is_select_all = Shortcut(ImGuiMod_Ctrl | ImGuiKey_A, 0, id);

        // We allow validate/cancel with Nav source (gamepad) to makes it easier to undo an accidental NavInput press with no keyboard wired, but otherwise it isn't very useful.
        const bool nav_gamepad_active = (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (io.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
        const bool is_enter_pressed = IsKeyPressed(ImGuiKey_Enter, true) || IsKeyPressed(ImGuiKey_KeypadEnter, true);
        const bool is_gamepad_validate = nav_gamepad_active && (IsKeyPressed(ImGuiKey_NavGamepadActivate, false) || IsKeyPressed(ImGuiKey_NavGamepadInput, false));
        const bool is_cancel = Shortcut(ImGuiKey_Escape, f_repeat, id) || (nav_gamepad_active && Shortcut(ImGuiKey_NavGamepadCancel, f_repeat, id));

        // FIXME: Should use more Shortcut() and reduce IsKeyPressed()+SetKeyOwner(), but requires modifiers combination to be taken account of.
        // FIXME-OSX: Missing support for Alt(option)+Right/Left = go to end of line, or next line if already in end of line.
        if (IsKeyPressed(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_PageUp) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGUP | k_mask); scroll_y -= row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_PageDown) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGDOWN | k_mask); scroll_y += row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
        else if (IsKeyPressed(ImGuiKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
        else if (IsKeyPressed(ImGuiKey_Delete) && !is_readonly && !is_cut)
        {
            if (!state->HasSelection())
            {
                // OSX doesn't seem to have Super+Delete to delete until end-of-line, so we don't emulate that (as opposed to Super+Backspace)
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
        }
        else if (IsKeyPressed(ImGuiKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeyCtrl && !io.KeyAlt && !io.KeySuper)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        }
        else if (is_enter_pressed || is_gamepad_validate)
        {
            // Determine if we turn Enter into a \n character
            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || is_gamepad_validate || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                validated = true;
                if (io.ConfigInputTextEnterKeepActive && !is_multiline)
                    state->SelectAll(); // No need to scroll
                else
                    clear_active_id = true;
            }
            else if (!is_readonly)
            {
                unsigned int c = '\n'; // Insert new line
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data))
                    state->OnCharPressed(c);
            }
        }
        else if (is_cancel)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {
                if (buf[0] != 0)
                {
                    revert_edit = true;
                }
                else
                {
                    render_cursor = render_selection = false;
                    clear_active_id = true;
                }
            }
            else
            {
                clear_active_id = revert_edit = true;
                render_cursor = render_selection = false;
            }
        }
        else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        }
        else if (is_select_all)
        {
            state->SelectAll();
            state->CursorFollow = true;
        }
        else if (is_cut || is_copy)
        {
            // Cut, Copy
            if (g.PlatformIO.Platform_SetClipboardTextFn != NULL)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb->select_start, state->Stb->select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb->select_start, state->Stb->select_end) : state->CurLenA;

                char backup = state->TextA.Data[ie];
                state->TextA.Data[ie] = 0; // A bit of a hack since SetClipboardText only takes null terminated strings
                SetClipboardText(state->TextA.Data + ib);
                state->TextA.Data[ie] = backup;
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, state->Stb);
            }
        }
        else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                char* clipboard_filtered = (char*)IM_ALLOC(clipboard_len + 1);
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s != 0; )
                {
                    unsigned int c;
                    int len = ImTextCharFromUtf8(&c, s, NULL);
                    s += len;
                    if (!input_text_filter_character(&g, &c, flags, callback, callback_user_data, true))
                        continue;
                    memcpy(clipboard_filtered + clipboard_filtered_len, s - len, len);
                    clipboard_filtered_len += len;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(state, state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    // Process callbacks and apply result back to user's buffer.
    const char* apply_new_text = NULL;
    int apply_new_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        if (revert_edit && !is_readonly)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {
                // Clear input
                IM_ASSERT(buf[0] != 0);
                apply_new_text = "";
                apply_new_text_length = 0;
                value_changed = true;
                IMSTB_TEXTEDIT_CHARTYPE empty_string;
                stb_textedit_replace(state, state->Stb, &empty_string, 0);
            }
            else if (strcmp(buf, state->InitialTextA.Data) != 0)
            {
                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;

                // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
                // Push records into the undo stack so we can CTRL+Z the revert operation itself
                value_changed = true;
                stb_textedit_replace(state, state->Stb, state->InitialTextA.Data, state->InitialTextA.Size - 1);
            }
        }

        // When using 'ImGuiInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer
        // before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
        // If we didn't do that, code like InputInt() with ImGuiInputTextFlags_EnterReturnsTrue would fail.
        // This also allows the user to use InputText() with ImGuiInputTextFlags_EnterReturnsTrue without maintaining any user-side storage
        // (please note that if you use this property along ImGuiInputTextFlags_CallbackResize you can end up with your temporary string object
        // unnecessarily allocating once a frame, either store your string data, either if you don't then don't use ImGuiInputTextFlags_CallbackResize).
        const bool apply_edit_back_to_user_buffer = !revert_edit || (validated && (flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.

            // User callback
            if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                ImGuiInputTextFlags event_flag = 0;
                ImGuiKey event_key = ImGuiKey_None;
                if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && Shortcut(ImGuiKey_Tab, 0, id))
                {
                    event_flag = ImGuiInputTextFlags_CallbackCompletion;
                    event_key = ImGuiKey_Tab;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_UpArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_DownArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackEdit) && state->Edited)
                {
                    event_flag = ImGuiInputTextFlags_CallbackEdit;
                }
                else if (flags & ImGuiInputTextFlags_CallbackAlways)
                {
                    event_flag = ImGuiInputTextFlags_CallbackAlways;
                }

                if (event_flag)
                {
                    ImGuiInputTextCallbackData callback_data;
                    callback_data.Ctx = &g;
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    // FIXME-OPT: Undo stack reconcile needs a backup of the data until we rework API, see #7925
                    state->CallbackTextBackup.resize(state->CurLenA + 1);
                    memcpy(state->CallbackTextBackup.Data, state->TextA.Data, state->CurLenA + 1);

                    char* callback_buf = is_readonly ? buf : state->TextA.Data;
                    callback_data.EventKey = event_key;
                    callback_data.Buf = callback_buf;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    const int utf8_cursor_pos = callback_data.CursorPos = state->Stb->cursor;
                    const int utf8_selection_start = callback_data.SelectionStart = state->Stb->select_start;
                    const int utf8_selection_end = callback_data.SelectionEnd = state->Stb->select_end;

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    callback_buf = is_readonly ? buf : state->TextA.Data; // Pointer may have been invalidated by a resize callback
                    IM_ASSERT(callback_data.Buf == callback_buf);         // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    const bool buf_dirty = callback_data.BufDirty;
                    if (callback_data.CursorPos != utf8_cursor_pos || buf_dirty) { state->Stb->cursor = callback_data.CursorPos; state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start || buf_dirty) { state->Stb->select_start = (callback_data.SelectionStart == callback_data.CursorPos) ? state->Stb->cursor : callback_data.SelectionStart; }
                    if (callback_data.SelectionEnd != utf8_selection_end || buf_dirty) { state->Stb->select_end = (callback_data.SelectionEnd == callback_data.SelectionStart) ? state->Stb->select_start : callback_data.SelectionEnd; }
                    if (buf_dirty)
                    {
                        // Callback may update buffer and thus set buf_dirty even in read-only mode.
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        input_text_reconcile_undo_state_after_user_callback(state, callback_data.Buf, callback_data.BufTextLen); // FIXME: Move the rest of this block inside function and rename to InputTextReconcileStateAfterUserCallback() ?
                        state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        state->TextA.Size = state->CurLenA + 1;
                        state->CursorAnimReset();
                    }
                }
            }

            // Will copy result string if modified
            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
                value_changed = true;
            }
        }
    }

    // Handle reapplying final data on deactivation (see InputTextDeactivateHook() for details)
    if (g.InputTextDeactivatedState.ID == id)
    {
        if (g.ActiveId != id && IsItemDeactivatedAfterEdit() && !is_readonly && strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0)
        {
            apply_new_text = g.InputTextDeactivatedState.TextA.Data;
            apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
            value_changed = true;
            //IMGUI_DEBUG_LOG("InputText(): apply Deactivated data for 0x%08X: \"%.*s\".\n", id, apply_new_text_length, apply_new_text);
        }
        g.InputTextDeactivatedState.ID = 0;
    }

    // Copy result to user buffer. This can currently only happen when (g.ActiveId == id)
    if (apply_new_text != NULL)
    {
        //// We cannot test for 'backup_current_text_length != apply_new_text_length' here because we have no guarantee that the size
        //// of our owned buffer matches the size of the string object held by the user, and by design we allow InputText() to be used
        //// without any storage on user's side.
        IM_ASSERT(apply_new_text_length >= 0);
        if (is_resizable)
        {
            ImGuiInputTextCallbackData callback_data;
            callback_data.Ctx = &g;
            callback_data.EventFlag = ImGuiInputTextFlags_CallbackResize;
            callback_data.Flags = flags;
            callback_data.Buf = buf;
            callback_data.BufTextLen = apply_new_text_length;
            callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
            callback_data.UserData = callback_user_data;
            callback(&callback_data);
            buf = callback_data.Buf;
            buf_size = callback_data.BufSize;
            apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
            IM_ASSERT(apply_new_text_length <= buf_size);
        }
        //IMGUI_DEBUG_PRINT("InputText(\"%s\"): apply_new_text length %d\n", label, apply_new_text_length);

        // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
        ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
    }

    // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
    // Otherwise request text input ahead for next frame.
    if (g.ActiveId == id && clear_active_id)
        ClearActiveID();
    else if (g.ActiveId == id)
        g.WantTextInputNextFrame = 1;

    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
    const char* buf_display_end = NULL; // We have specialized paths below for setting the length

    gui->easing(animstate->alpha, strlen(buf_display) > 0 ? 1.f : 0.f, 8.f, static_easing);

    *active = render_cursor;

    // Render frame
    if (!is_multiline)
    {

    }

    const ImVec2 buf_text_size = gui->text_size(g.Font, buf_display);

    animstate->buf_size = buf_text_size.x;

    const ImVec4 clip_rect(rect.Min.x, rect.Min.y, rect.Min.x + inner_size.x, rect.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 draw_pos = centered ? ImVec2(rect.Min.x + (rect.GetWidth() - buf_text_size.x) / 2, rect.GetCenter().y - gui->text_size(g.Font, "A").y / 2) : ImVec2(rect.Min.x + text_padding, rect.GetCenter().y - gui->text_size(g.Font, "A").y / 2 - 1);
    ImVec2 text_size(0.0f, 0.0f);

    // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
    // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
    // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
    if (is_displaying_hint)
    {
        buf_display = hint.data();
        buf_display_end = hint.data() + strlen(hint.data());
    }

    // Render text. We currently only render selection when the widget is active or while scrolling.
    // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        // Render text (with cursor and selection)
        // This is going to be messy. We need to:
        // - Display the text (this alone can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
        const char* text_begin = state->TextA.Data;
        const char* text_end = text_begin + state->CurLenA;
        ImVec2 cursor_offset, select_start_offset;

        {
            // Find lines numbers straddling cursor and selection min position
            int cursor_line_no = render_cursor ? -1 : -1000;
            int selmin_line_no = render_selection ? -1 : -1000;
            const char* cursor_ptr = render_cursor ? text_begin + state->Stb->cursor : NULL;
            const char* selmin_ptr = render_selection ? text_begin + ImMin(state->Stb->select_start, state->Stb->select_end) : NULL;

            // Count lines and find line number for cursor and selection ends
            int line_count = 1;
            if (is_multiline)
            {
                for (const char* s = text_begin; (s = (const char*)memchr(s, '\n', (size_t)(text_end - s))) != NULL; s++)
                {
                    if (cursor_line_no == -1 && s >= cursor_ptr) { cursor_line_no = line_count; }
                    if (selmin_line_no == -1 && s >= selmin_ptr) { selmin_line_no = line_count; }
                    line_count++;
                }
            }
            if (cursor_line_no == -1)
                cursor_line_no = line_count;
            if (selmin_line_no == -1)
                selmin_line_no = line_count;

            // Calculate 2d position by finding the beginning of the line and measuring distance
            cursor_offset.x = input_text_calc_text_size(&g, ImStrbol(cursor_ptr, text_begin), cursor_ptr).x;
            cursor_offset.y = cursor_line_no * g.FontSize;
            if (selmin_line_no >= 0)
            {
                select_start_offset.x = input_text_calc_text_size(&g, ImStrbol(selmin_ptr, text_begin), selmin_ptr).x;
                select_start_offset.y = selmin_line_no * g.FontSize;
            }

            // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
            if (is_multiline)
                text_size = ImVec2(inner_size.x, line_count * g.FontSize);
        }

        // Scroll
        if (render_cursor && state->CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = inner_size.x * 0.25f;
                const float visible_width = inner_size.x - text_padding;

                if (!centered)
                {
                    if (cursor_offset.x < state->Scroll.x)
                        state->Scroll.x = IM_TRUNC(ImMax(0.0f, cursor_offset.x - scroll_increment_x));
                    else if (cursor_offset.x - visible_width >= state->Scroll.x)
                        state->Scroll.x = IM_TRUNC(cursor_offset.x - visible_width + scroll_increment_x);
                }

            }
            else
            {
                state->Scroll.y = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                // Test if cursor is vertically visible
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - (inner_size.y - style.FramePadding.y * 2.0f) >= scroll_y)
                    scroll_y = cursor_offset.y - inner_size.y + style.FramePadding.y * 2.0f;
                const float scroll_max_y = ImMax((text_size.y + style.FramePadding.y * 2.0f) - inner_size.y, 0.0f);
                scroll_y = ImClamp(scroll_y, 0.0f, scroll_max_y);
                draw_pos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                draw_window->Scroll.y = scroll_y;
            }

            state->CursorFollow = false;
        }

        // Draw selection
        const ImVec2 draw_scroll = ImVec2(state->Scroll.x, 0.0f);
        if (render_selection)
        {
            const char* text_selected_begin = text_begin + ImMin(state->Stb->select_start, state->Stb->select_end);
            const char* text_selected_end = text_begin + ImMax(state->Stb->select_start, state->Stb->select_end);

            ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const char* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    p = (const char*)memchr((void*)p, '\n', text_selected_end - p);
                    p = p ? p + 1 : text_selected_end;
                }
                else
                {
                    ImVec2 rect_size = input_text_calc_text_size(&g, p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = IM_TRUNC(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                    ImRect d(ImVec2(rect_pos.x, rect.Min.y + line_padding), ImVec2(rect_pos.x + rect_size.x, rect.Max.y - line_padding));
                    d.ClipWith(clip_rect);
                    if (d.Overlaps(clip_rect))
                        window->DrawList->AddRectFilled(d.Min, d.Max, draw->get_clr(clr->white, 0.1f), var->style.frame_rounding);
                    rect_pos.x = draw_pos.x - draw_scroll.x;
                }
                rect_pos.y += g.FontSize;
            }
        }

        // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
        // FIXME-OPT: Multiline could submit a smaller amount of contents to AddText() since we already iterated through it.
        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
            window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, text_col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }

        animstate->get_cursor_offset = cursor_offset;
        animstate->get_draw_scroll = draw_scroll;
    }
    else
    {
        // Render text only (no selection, no cursor)
        if (is_multiline)
            text_size = ImVec2(inner_size.x, input_text_calc_text_len_and_line_count(buf_display, &buf_display_end) * g.FontSize); // We don't need width
        else if (!is_displaying_hint && g.ActiveId == id)
            buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint)
            buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? ImGuiCol_TextDisabled : ImGuiCol_Text);
            window->DrawList->AddText(g.Font, g.FontSize, draw_pos, text_col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }
    }

    gui->easing(animstate->cursor_alpha, g.ActiveId == id ? 1.f : 0.f, 7.f, static_easing);

    if (animstate->cursor_alpha > 0.01f)
    {
        ImVec2 cursor_screen_pos = ImTrunc(draw_pos + animstate->get_cursor_offset - animstate->get_draw_scroll);
        ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
        gui->easing(animstate->offset, round(cursor_screen_rect.Min.x - rect.Min.x), 24.f, dynamic_easing);

        if (cursor_screen_rect.Overlaps(clip_rect))
            draw->line(window->DrawList, rect.Min + ImVec2(animstate->offset, line_padding), ImVec2(rect.Min.x + animstate->offset, rect.Max.y - line_padding), draw->get_clr(clr->white, animstate->cursor_alpha), 1);

        if (!is_readonly)
        {
            g.PlatformImeData.WantVisible = true;
            g.PlatformImeData.InputPos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
            g.PlatformImeData.InputLineHeight = g.FontSize;
        }
    }

    if (is_password && !is_displaying_hint)
        PopFont();

    if (is_multiline)
    {
        // For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (see #4761, #7870)...
        Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
        g.NextItemData.ItemFlags |= (ImGuiItemFlags)ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
        gui->end_def_child();
        item_data_backup.StatusFlags |= (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredWindow);

        // ...and then we need to undo the group overriding last item data, which gets a bit messy as EndGroup() tries to forward scrollbar being active...
        // FIXME: This quite messy/tricky, should attempt to get rid of the child window.
        gui->end_group();
        if (g.LastItemData.ID == 0 || g.LastItemData.ID != GetWindowScrollbarID(draw_window, ImGuiAxis_Y))
        {
            g.LastItemData.ID = id;
            g.LastItemData.InFlags = item_data_backup.InFlags;
            g.LastItemData.StatusFlags = item_data_backup.StatusFlags;
        }
    }

    // Log as text
    if (g.LogEnabled && (!is_password || is_displaying_hint))
    {
        LogSetNextTextDecoration("{", "}");
        LogRenderedText(&draw_pos, buf_display, buf_display_end);
    }

    if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
        MarkItemEdited(id);

    gui->pop_font();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
    if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
        return validated;
    else
        return value_changed;
}

struct text_field_state
{
    bool active;
    float alpha;
    ImVec4 text;
};

bool c_widgets::text_field(std::string name, char* buf, int buf_size) 
{
    c_window* window = gui->get_window();
    if (window->SkipItems)
        return false;

    c_id id = window->GetID(name.data());
    text_field_state* state = gui->anim_container<text_field_state>(window->GetID((std::stringstream{} << id << "text_field_state").str().data()));

    c_vec2 pos = window->DC.CursorPos;
    c_vec2 size = c_vec2(gui->content_avail().x, s_(70));
    c_rect rect(pos, pos + size);
    c_rect inner(rect.Min + s_(0, 10), rect.Max - s_(0, 10));
    c_rect button(inner.GetBL() - s_(0, 29), inner.GetBR());

    gui->item_size(rect);
    if (!gui->item_add(rect, id))
        return false;

    bool user_clicked = rect.Contains(GetMousePos()) && gui->mouse_clicked(0) && gui->is_window_hovered(0);
    gui->easing(state->text, state->active ? clr->white.Value : clr->text.Value, 24.f, dynamic_easing);
    gui->easing(state->alpha, strlen(buf) > 0 ? 0.f : 1.f, 24.f, dynamic_easing);

    draw->rect_filled(window->DrawList, button.Min, button.Max, draw->get_clr(clr->widget), s_(2));

    draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(clr->white),
        name.data(), 0, 0, { 0, 0 });

    draw->text_clipped(window->DrawList, font->get(inter_medium, 11), button.Min + s_(elements->padding.x, 0) + ImVec2((button.GetWidth() / 2) * (1.f - state->alpha), 0), button.Max, draw->get_clr(clr->text, state->alpha), name.data(), 0, 0, { 0, 0.5 });

    gui->push_var(style_var_frame_rounding, 0);
    gui->push_font(font->get(inter_medium, 11));

    bool value_changed = text_field_ex(name, "", buf, buf_size, s_(elements->padding.x), s_(8), draw->get_clr(state->text), false, button, &state->active, 0, 0, 0, user_clicked);

    gui->pop_font();
    gui->pop_var();

    if (gui->content_avail().y > 0)
    {
        draw->line(window->DrawList, rect.GetBL(), rect.GetBR(), draw->get_clr(clr->border));
    }

    return value_changed;
}


