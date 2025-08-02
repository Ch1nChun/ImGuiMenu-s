#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

#include "variables.h"
#include "elements.h"
#include "settings.h"

#include <vector>
#include <sstream>
#include <string>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <map>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <array>

inline ID3D11Device* g_pd3dDevice = nullptr;
inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline IDXGISwapChain* g_pSwapChain = nullptr;
inline bool                     g_SwapChainOccluded = false;
inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
inline ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

#define SCALE(...) scale_impl(__VA_ARGS__, var->c_dpi.dpi)

inline ImVec2 scale_impl(const ImVec2& vec, float dpi) {
    return ImVec2(roundf(vec.x * dpi), roundf(vec.y * dpi));
}

inline ImVec2 scale_impl(float x, float y, float dpi) {
    return ImVec2(roundf(x * dpi), roundf(y * dpi));
}

inline float scale_impl(float var, float dpi) {
    return roundf(var * dpi);
}

#define PI 3.14159265359f

enum notify_position : int
{
    top_left,
    top_right,
    bottom_left,
    bottom_right,
};

enum fade_direction : int
{
    vertically,
    horizontally,
    diagonally,
    diagonally_reversed,
};

enum watermark_position : int {
    mark_top_left,
    mark_top_right,
    mark_bottom_left,
    mark_bottom_right,
};

enum interpolation_type {
    back,
    elastic,
};

struct lua_data {
    std::string name;
    std::string date;
    bool loaded;
};

struct config_data {
	std::string name;
	std::string date;
};

inline D3DX11_IMAGE_LOAD_INFO info;
inline ID3DX11ThreadPump* pump{ nullptr };

using namespace ImGui;

class c_gui
{
public:

    template <typename T>
    T* anim_container(T** state_ptr, ImGuiID id)
    {
        T* state = static_cast<T*>(GetStateStorage()->GetVoidPtr(id));
        if (!state)
            GetStateStorage()->SetVoidPtr(id, state = new T());

        *state_ptr = state;
        return state;
    }

    ImU32                           get_clr(const ImVec4& col, float alpha = 1.f);

    float                           fixed_speed(float speed) { return speed / ImGui::GetIO().Framerate; };

    bool                            begin(std::string_view name, bool* p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);

    void                            end();

    void                            push_style_color(ImGuiCol idx, ImU32 col);

    void                            pop_style_color(int count = 1);

    void                            push_style_var(ImGuiStyleVar idx, float val);

    void                            push_style_var(ImGuiStyleVar idx, const ImVec2& val);

    void                            pop_style_var(int count = 1);

    void                            push_font(ImFont* font);

    void                            pop_font();

    void                            set_cursor_pos(const ImVec2& local_pos);

    void                            set_cursor_pos_x(float x);
                                    
    void                            set_cursor_pos_y(float y);

    ImVec2                          get_cursor_pos();

    float                           get_cursor_pos_x();

    float                           get_cursor_pos_y();

    void                            spacing();

    void                            sameline(float offset_from_start_x = 0.0f, float spacing_w = -1.0f);

    void                            set_next_window_pos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0));

    void                            set_next_window_size(const ImVec2& size, ImGuiCond cond = 0);

    void                            begin_group();

    void                            end_group();

    bool                            begin_child(std::string_view name, const ImVec2& size_arg = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);

    void                            end_child();

    void                            begin_content(const char* id, const ImVec2& size_arg, const ImVec2& padding, const ImVec2& spacing);

    void                            end_content();

    void                            water_mark(std::string name, std::vector<std::string> function, watermark_position type, bool* visible);

    inline                          ImGuiWindow* get_current_window() { ImGuiContext& g = *GImGui; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; };

    void                            new_frame();

    void                            end_frame();

	std::string						get_current_date();

	void							rotate_start();

	void							rotate_end(float rad, ImVec2 center = ImVec2(0, 0));

	float							deg_to_rad(float deg);

    void                            render();

};

inline c_gui* gui = new c_gui();

class c_widget
{
public:

    bool                            checkbox(std::string_view label, bool* callback);

    bool                            checkbox_with_key(std::string_view label, bool* callback, int* key, bool* mode, bool* value, bool* show_in_binds);

	bool                            checkbox_with_color(std::string_view label, bool* callback, float col[4], bool alpha = true);

    bool                            slider_float(std::string_view label, float* v, float v_min, float v_max, float power = 0.1f, const char* format = "");

    bool                            slider_int(std::string_view label, int* v, int v_min, int v_max, int power = 1, const char* format = "");

    bool                            dropdown(std::string_view label, int* current_item, std::vector<std::string>& items, int max_count);

    bool                            tool_dropdown(std::string_view label, int* current_item, std::vector<std::string>& items, int max_count);
    
    void                            multi_dropdown(std::string_view label, bool variable[], std::vector<std::string>& labels, int max_count);

    bool                            keybind(std::string_view label, int* key);

    bool                            keybind_button(std::string_view name_id, bool* pressing);

    bool                            begin_popup(std::string_view name, float size_w, const ImVec2& position = {0, 0});

    void                            end_popup();

    bool                            set_tooltip(std::string_view tooltip_id, std::string_view tooltip_text);

    void                            text_colored(ImFont* font, const ImU32 col, std::string text);

	bool                            color_edit(std::string_view label, float col[4], ImGuiColorEditFlags flags = 0);

    bool                            button(std::string_view label, const ImVec2& size);
    
    bool                            tool_button(std::string_view label, std::string_view icon, const ImVec2& size);

	bool                            lua_tool_button(std::string_view icon, std::string_view aid);

    bool                            text_field(std::string_view hint, std::string_view label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = 0, void* user_data = 0);

    bool                            selection(std::string_view label, const ImVec2& size, int selection_id, int& selection_variable);

    bool                            lua_selectable(lua_data* data, int lua_id);
    
	bool                            config_selectable(config_data* data, int selection_id, int& selection_variable);

    void                            separator();

};

inline c_widget* widget = new c_widget();

class c_draw
{
public:

    void                            push_clip_rect(ImDrawList* draw, const ImVec2& cr_min, const ImVec2& cr_max, bool intersect_with_current_clip_rect);
        
    void                            pop_clip_rect(ImDrawList* draw);

    void                            add_text(ImDrawList* draw_list, const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = NULL, const ImVec4* cpu_fine_clip_rect = NULL);

    void                            render_text(ImFont* font, ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 color, const char* text, const char* text_display_end = NULL, const ImVec2* text_size_if_known = NULL, const ImVec2& align = { 0.5, 0.5 }, const ImRect* clip_rect = NULL);

	void							render_text_with_spacing(const char* text, float spacing, ImVec2 position1, ImVec2 position2, ImU32 color, bool centered);

    void                            rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding = 0.f, ImDrawFlags flags = 0);

    void                            add_rect_filled(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0, ImDrawFlags flags = 0);

    void                            add_circle_filled(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0);

    void                            add_circle(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 0);

    void                            add_rect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags = 0, float tickness = 0);

    void                            fade_rect_filled(ImDrawList* draw, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 col_one, ImU32 col_two, fade_direction direction, float rounding = 0.f, ImDrawFlags flags = 0);

    void                            render_text(ImDrawList* draw_list, ImFont* font, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 color, const char* text, const char* text_display_end = NULL, const ImVec2* text_size_if_known = NULL, const ImVec2& align = ImVec2(0.f, 0.f), const ImRect* clip_rect = NULL);

    void                            radial_gradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);

    void                            set_linear_color_alpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);

    void                            add_image(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col);

    void                            add_image_rounded(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding = 0, ImDrawFlags flags = 0);

    void                            add_line(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0);

};

inline c_draw* draw = new c_draw();

struct notify_state
{
    int notify_id;

    std::string_view text;

    float notify_delay;

    float notify_alpha{ 0.f }, notify_offset{ 0.f }, notify_timer{ 0.f };

    bool active_notify{ true };

    notify_position type;
};

class c_notify
{
public:

    void setup_notify();

    void add_notify(std::string_view text, float notify_delay, notify_position type);

    void render_notify(int cur_notify_value, float notify_alpha, float notify_offset, float notify_percentage, float notify_delay, std::string_view text, notify_position type);

    int notify_count{ 0 };

    std::vector<notify_state> notifications;
};

inline c_notify* notify = new c_notify();

struct add_item_state
{
    bool window_opened = false;
    float window_alpha = 0.f;
    bool window_hovered = false;
};

struct preview_box
{
    ImVec2 position;
    ImVec2 size;
};

enum preview_area
{
    area_top,
    area_bottom,
    area_left,
    area_right,
    area_none
};

enum item_position
{
    position_top,
    position_bottom,
    position_left,
    position_right
};

struct item_state
{
    std::string name;
    ImVec4 color;
    int position;
    ImVec2 location;
    bool swapped;
    bool active;
    ImRect rect = ImRect(ImVec2(0, 0), ImVec2(0, 0));
};

class esp_preview
{
public:

    std::vector<item_state> text =
    {
        { "NICKNAME", ImVec4(1.f, 1.f, 1.f, 1.f), position_top },

        { "MONEY", ImVec4(1.f, 1.f, 1.f, 1.f), position_right },
        { "SCOPE", ImVec4(1.f, 1.f, 1.f, 1.f), position_right },
        { "PING", ImVec4(1.f, 1.f, 1.f, 1.f), position_right },
        { "KD", ImVec4(1.f, 1.f, 1.f, 1.f), position_right },

        { "DISTANCE", ImVec4(1.f, 1.f, 1.f, 1.f), position_left },
        { "FLASH", ImVec4(1.f, 1.f, 1.f, 1.f), position_left },
        { "BLIND", ImVec4(1.f, 1.f, 1.f, 1.f), position_left },
        { "LC", ImVec4(1.f, 1.f, 1.f, 1.f), position_left },

        { "WEAPON", ImVec4(1.f, 1.f, 1.f, 1.f), position_bottom },
    };

    std::vector<item_state> bar =
    {
        { "HEALTH", ImVec4(132 / 255.f, 255 / 255.f, 152 / 255.f, 1.f), position_left },
        { "ARMOR", ImVec4(130 / 255.f, 150 / 255.f, 255 / 255.f, 1.f), position_right },
    };

    void initialize_preview(const ImVec2& pos, const ImVec2& size, bool render_text, bool render_bar);
    void render_box(int type);
    void render_text(item_state& state);
    void render_bar(item_state& state);

private:
    preview_box box;
    ImRect area_rect[4];

    void fill_box(const ImVec2& pos, const ImVec2& size);
    void fill_areas();
    void fill_buffers();

    int texts_buffer[4] = { 0, 0, 0, 0 };
    int bars_buffer[4] = { 0, 0, 0, 0 };
    int hovered_area = area_none;

    float area_size = 80.f;
};

inline esp_preview* esp = new esp_preview();


class c_easing
{

public:

    float easing_value;

    float ease_in_elastic(float t) { const float c4 = (2 * PI) / 2;  return (t <= 0.01f) ? 0.0f : (t >= 0.60f) ? 1.0f : pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1; }

    float ease_in_back(float t) { const float c1 = 1.70158; const float c3 = c1 + 1; return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2); }

    struct easing_state {
        float animTime = 0.0f;
        bool reverse = false;
    };

    template <typename T>
    T im_ease(int animation_id, bool callback, T min, T max, float speed, interpolation_type type) {
        easing_state* state = gui->anim_container(&state, GetCurrentWindow()->GetID(animation_id));

        state->animTime = (callback != state->reverse) ? 0.0f : min(state->animTime + 0.1f * speed, 1.0f);
        state->reverse = callback;

        easing_value = (type == elastic) ? ease_in_elastic(state->animTime) : (type == back) ? ease_in_back(state->animTime) : 0;

        return callback ? easing_value * (max - min) + min : easing_value * (min - max) + max;
    }

    template <>
    ImVec2 im_ease<ImVec2>(int animation_id, bool callback, ImVec2 min, ImVec2 max, float speed, interpolation_type type) {
        easing_state* state = gui->anim_container(&state, GetCurrentWindow()->GetID(animation_id));

        state->animTime = (callback != state->reverse) ? 0.0f : min(state->animTime + 0.1f * speed, 1.0f);
        state->reverse = callback;

        easing_value = (type == elastic) ? ease_in_elastic(state->animTime) : (type == back) ? ease_in_back(state->animTime) : 0;

        ImVec2 result;
        result.x = callback ? easing_value * (max.x - min.x) + min.x : easing_value * (min.x - max.x) + max.x;
        result.y = callback ? easing_value * (max.y - min.y) + min.y : easing_value * (min.y - max.y) + max.y;
        return result;
    }
};

inline std::unique_ptr<c_easing> easing = std::make_unique<c_easing>();

class c_text_editor
{
public:
	enum class pallete_index
	{
		def,
		keyword,
		number,
		string,
		char_literal,
		punctuation,
		preprocessor,
		identifier,
		known_identifier,
		preproc_identifier,
		comment,
		multi_line_comment,
		background,
		cursor,
		selection,
		error_marker,
		breakpoint,
		line_number,
		current_line_fill,
		current_line_fill_inactive,
		current_line_edge,
		max
	};

	enum class selection_mode
	{
		normal,
		word,
		line
	};

	struct breakpoint
	{
		int m_line;
		bool m_enabled;
		std::string m_condition;

		breakpoint()
			: m_line(-1)
			, m_enabled(false)
		{}
	};

	struct coordinates
	{
		int m_line, m_column;
		coordinates() : m_line(0), m_column(0) {}
		coordinates(int a_line, int a_column) : m_line(a_line), m_column(a_column)
		{

		}

		static coordinates Invalid() { static coordinates invalid(-1, -1); return invalid; }

		bool operator ==(const coordinates& o) const
		{
			return
				m_line == o.m_line &&
				m_column == o.m_column;
		}

		bool operator !=(const coordinates& o) const
		{
			return
				m_line != o.m_line ||
				m_column != o.m_column;
		}

		bool operator <(const coordinates& o) const
		{
			if (m_line != o.m_line)
				return m_line < o.m_line;
			return m_column < o.m_column;
		}

		bool operator >(const coordinates& o) const
		{
			if (m_line != o.m_line)
				return m_line > o.m_line;
			return m_column > o.m_column;
		}

		bool operator <=(const coordinates& o) const
		{
			if (m_line != o.m_line)
				return m_line < o.m_line;
			return m_column <= o.m_column;
		}

		bool operator >=(const coordinates& o) const
		{
			if (m_line != o.m_line)
				return m_line > o.m_line;
			return m_column >= o.m_column;
		}
	};

	struct identifier
	{
		coordinates m_location;
		std::string m_declaration;
	};

	typedef std::string string;
	typedef std::unordered_map<std::string, identifier> identifiers;
	typedef std::unordered_set<std::string> keywords;
	typedef std::map<int, std::string> error_markers;
	typedef std::unordered_set<int> breakpoints;
	typedef std::array<ImU32, (unsigned)pallete_index::max> palette;
	typedef uint8_t Char;

	struct glyph
	{
		Char m_char;
		pallete_index m_color_idex = pallete_index::def;
		bool m_comment : 1;
		bool m_multiline_comment : 1;
		bool m_preprocessor : 1;

		glyph(Char a_char, pallete_index a_color_index) : m_char(a_char), m_color_idex(a_color_index),
			m_comment(false), m_multiline_comment(false), m_preprocessor(false) {}
	};

	typedef std::vector<glyph> line;
	typedef std::vector<line> lines;

	struct language_definition
	{
		typedef std::pair<std::string, pallete_index> token_regex_string;
		typedef std::vector<token_regex_string> token_regex_strings;
		typedef bool(*tokenize_callback)(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, pallete_index& m_pallete_index);

		std::string m_name;
		keywords m_keywords;
		identifiers m_identifiers;
		identifiers m_preproc_identifiers;
		std::string m_comment_start, m_comment_end, m_single_line_comment;
		char m_preproc_char;
		bool m_auto_indentation;

		tokenize_callback m_tokenize;

		token_regex_strings m_token_regex_strings;

		bool m_case_sensitive;

		language_definition()
			: m_preproc_char('#'), m_auto_indentation(true), m_tokenize(nullptr), m_case_sensitive(true)
		{
		}
	};

	c_text_editor();
	~c_text_editor();

	void set_error_markers(const error_markers& aMarkers) { m_error_markers = aMarkers; }
	void set_break_points(const breakpoints& aMarkers) { m_breakpoints = aMarkers; }

	void render(const char* aTitle, const ImVec2& aSize = ImVec2(), bool aBorder = false);
	void set_text(const std::string& aText);
	std::string get_text() const;

	void set_text_lines(const std::vector<std::string>& aLines);
	std::vector<std::string> get_text_lines() const;

	std::string get_selected_text() const;
	std::string get_current_line_text()const;

	int get_total_lines() const { return (int)m_lines.size(); }
	bool is_overwrite() const { return m_overwrite; }

	void set_read_only(bool aValue);
	bool is_read_only() const { return m_read_only; }
	bool it_text_changed() const { return m_text_changed; }
	bool is_cursor_position_changed() const { return m_cursor_position_changed; }

	bool is_colorizer_enabled() const { return m_colorizer_enabled; }
	void set_colorizer_enable(bool aValue);

	coordinates get_cursor_position() const { return get_actual_cursor_coordinates(); }
	void set_cursor_position(const coordinates& aPosition);

	inline void set_handle_mouse_inputs(bool aValue) { m_handle_mouse_inputs = aValue; }
	inline bool is_handle_mouse_inputs_enabled() const { return m_handle_keyboard_inputs; }

	inline void set_handle_keyboard_inputs(bool aValue) { m_handle_keyboard_inputs = aValue; }
	inline bool is_handle_keyboard_inputs_enabled() const { return m_handle_keyboard_inputs; }

	inline void set_imgui_child_ignored(bool aValue) { m_ignore_imgui_child = aValue; }
	inline bool is_imgui_child_ignored() const { return m_ignore_imgui_child; }

	inline void set_show_whitespaces(bool aValue) { m_show_whitespaces = aValue; }
	inline bool is_showing_whitespaces() const { return m_show_whitespaces; }

	void set_tab_size(int aValue);
	inline int get_tab_size() const { return m_tab_size; }

	void insert_text(const std::string& aValue);
	void insert_text(const char* aValue);

	void move_up(int aAmount = 1, bool aSelect = false);
	void move_down(int aAmount = 1, bool aSelect = false);
	void move_left(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void move_right(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void move_top(bool aSelect = false);
	void move_bottom(bool aSelect = false);
	void move_home(bool aSelect = false);
	void move_end(bool aSelect = false);

	void set_selection_start(const coordinates& aPosition);
	void set_selection_end(const coordinates& aPosition);
	void set_selection(const coordinates& aStart, const coordinates& aEnd, selection_mode aMode = selection_mode::normal);
	void select_word_under_cursor();
	void select_all();
	bool has_selection() const;

	void copy();
	void cut();
	void paste();
	void mdelete();

	bool can_undo() const;
	bool can_redo() const;
	void undo(int aSteps = 1);
	void redo(int aSteps = 1);

private:
	typedef std::vector<std::pair<std::regex, pallete_index>> regex_list;

	struct editor_state
	{
		coordinates m_selection_start;
		coordinates m_selection_end;
		coordinates m_cursor_position;
	};

	class undo_record
	{
	public:
		undo_record() {}
		~undo_record() {}

		undo_record(
			const std::string& aAdded,
			const c_text_editor::coordinates aAddedStart,
			const c_text_editor::coordinates aAddedEnd,

			const std::string& aRemoved,
			const c_text_editor::coordinates aRemovedStart,
			const c_text_editor::coordinates aRemovedEnd,

			c_text_editor::editor_state& aBefore,
			c_text_editor::editor_state& aAfter);

		void undo(c_text_editor* aEditor);
		void redo(c_text_editor* aEditor);

		std::string m_added;
		coordinates m_added_start;
		coordinates m_added_end;

		std::string m_removed;
		coordinates m_removed_start;
		coordinates m_removed_end;

		editor_state m_before;
		editor_state m_after;
	};

	typedef std::vector<undo_record> undo_buffer;

	void proces_inputs();
	void colorize(int aFromLine = 0, int aCount = -1);
	void colorize_range(int aFromLine = 0, int aToLine = 0);
	void colorize_internal();
	float text_distance_to_line_start(const coordinates& aFrom) const;
	void ensure_cursor_visible();
	int get_page_size() const;
	std::string get_text(const coordinates& aStart, const coordinates& aEnd) const;
	coordinates get_actual_cursor_coordinates() const;
	coordinates sanitize_coordinates(const coordinates& aValue) const;
	void advance(coordinates& aCoordinates) const;
	void delete_range(const coordinates& aStart, const coordinates& aEnd);
	int insert_text_at(coordinates& aWhere, const char* aValue);
	void add_undo(undo_record& aValue);
	coordinates screen_pos_to_coordinates(const ImVec2& aPosition) const;
	coordinates find_word_start(const coordinates& aFrom) const;
	coordinates find_word_end(const coordinates& aFrom) const;
	coordinates find_next_word(const coordinates& aFrom) const;
	int get_character_index(const coordinates& aCoordinates) const;
	int get_character_column(int a_line, int aIndex) const;
	int get_line_character_count(int a_line) const;
	int get_line_max_column(int a_line) const;
	bool is_on_word_boundary(const coordinates& aAt) const;
	void remove_line(int aStart, int aEnd);
	void remove_line(int aIndex);
	line& insert_line(int aIndex);
	void enter_character(ImWchar a_char, bool aShift);
	void backspace();
	void delete_selection();
	std::string get_word_under_cursor() const;
	std::string get_word_at(const coordinates& aCoords) const;
	ImU32 get_glyph_color(const glyph& aGlyph) const;

	void handle_keyboard_inputs();
	void handle_mouse_inputs();
	void render();

	float m_line_spacing;
	lines m_lines;
	editor_state m_state;
	undo_buffer m_undo_buffer;
	int m_undo_index;

	int m_tab_size;
	bool m_overwrite;
	bool m_read_only;
	bool m_within_render;
	bool m_scroll_to_cursor;
	bool m_scroll_to_top;
	bool m_text_changed;
	bool m_colorizer_enabled;
	float m_text_start;
	int  m_left_margin;
	bool m_cursor_position_changed;
	int m_color_range_min, m_color_range_max;
	selection_mode m_selection_mode;
	bool m_handle_keyboard_inputs;
	bool m_handle_mouse_inputs;
	bool m_ignore_imgui_child;
	bool m_show_whitespaces;

	palette m_pallete_base;
	palette m_pallete;
	language_definition m_language_definition;
	regex_list m_regex_list;

	bool m_check_comments;
	breakpoints m_breakpoints;
	error_markers m_error_markers;
	ImVec2 m_char_advance;
	coordinates m_interactive_start, m_interactive_end;
	std::string m_line_buffer;
	uint64_t m_start_time;

	float m_last_click;
};

inline c_text_editor* text_editor = new c_text_editor();
