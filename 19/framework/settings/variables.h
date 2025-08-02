#include "imgui.h"
#include <vector>
#include <string>

struct lua_data;
struct config_data;

class c_variable
{
public:

	struct
	{
		ImGuiWindowFlags preview_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		int box_c = 1;
		const char* box_type[2] = { "Default", "Corner" };

		ImVec4 corner_color = ImColor(78, 78, 91);

		bool flags = true;
		bool bars = true;
		bool esp_preview = true;

		float texts_spacing = 15.f;
		float bars_spacing = 10.f;
		float box_padding = 10.f;

		float anim_speed = 25.f;

	} c_preview;

	struct
	{

		std::vector<std::string> selection_icon = { "K", "F", "H", "N", "V", "J", "E" };

		int selection = 0, selection_active = 0;
		float selection_alpha = 1, selection_add;

	} c_selection;

	struct
	{
		std::vector<std::string> watermark_content = { "SYNTHETIC", "Server", "144FPS", "64PING", "12:15PM" };

		bool watermark = true;
		int watermark_position = 1;

	} c_watermark;

	struct
	{
		int notify_position = 0;

	} c_notify;

	struct
	{
		float size = 11.f;
		float rounding = 2.f;
		ImVec2 padding = ImVec2(10, 10);
		ImVec2 picker_size = ImVec2(150, 150);
		ImVec2 bar_size = ImVec2(picker_size.x, 8);

	} c_colorpicker;

	struct
	{

		float dpi = 1.f;
		int dpi_saved = 100;

		bool dpi_changed = false;

	} c_dpi;

	struct
	{
		bool silent_aimbot = false;
		bool hit_chance = false;

		bool ragebot = true;
		bool rage_holding = false;
		bool rage_value = false;
		bool rage_show_binds = false;
		int rage_key = 0;

		int fov = 90;

	} c_ragebot;

	struct
	{
		int smoothness = 50;

		bool enable_recoil = true;
		bool recoil_holding = false;
		bool recoil_value = false;
		bool recoil_show_binds = false;

		int recoil_key = 0;

	} c_recoil;

	struct
	{
		int body_selection = 0;
		std::vector<std::string> body_list = { "Default", "Body" };

		int points_selection = 0;
		std::vector<std::string> points_list = { "On limbs", "None" };

	} c_other;

	struct
	{
		bool head_safety = true;

		int hit_chance = 10;
		int max_misses = 50;

		bool point_scale = true;
		bool point_holding = false;
		bool point_value = false;
		bool point_show_binds = false;
		int point_key = 0;

	} c_move;

	struct
	{
		bool trigger_in_smoke = false;

		bool enable_trigger = true;
		bool trigger_holding = false;
		bool trigger_value = false;
		bool trigger_show_binds = false;
		int trigger_key = 0;

	} c_trigger;

	struct
	{
		float pitch = 0.5f;
		float yaw = 0.5f;
		
		bool head_safety = false;

		bool point_scale = false;
		bool point_holding = false;
		bool point_value = false;
		bool point_show_binds = false;
		int point_key = 0;

	} c_settings;

	struct
	{
		bool esp = true;
		bool esp_holding = false;
		bool esp_value = false;
		bool esp_show_binds = false;
		int esp_key = 0;

		bool through_walls = true;

		int tracers_selection = 0;
		std::vector<std::string> tracers_list = { "Disabled", "In the field" };

		bool dynamic_boxes = false;

		bool ingame_radar = true;
		float inagame_color[4] = { 0.87f, 0.46f, 0.46f, 1.f };
	} c_esp;

	struct
	{
		bool glow = true;
		bool glow_holding = false;
		bool glow_value = false;
		bool glow_show_binds = false;
		int glow_key = 0;

		int power = 30;
	} c_glow;

	struct
	{
		bool attachments = true;
		float attachments_color[4] = { 0.46f, 0.52f, 0.87f, 1.f };

		bool teammates = true;
		bool teammates_holding = false;
		bool teammates_value = false;
		bool teammates_show_binds = false;
		int teammates_key = 0;

	} c_attachments;

	struct
	{
		bool chams = false;
		bool chams_holding = false;
		bool chams_value = false;
		bool chams_show_binds = false;
		int chams_key = 0;

		bool backtrack = true;
		float backtrack_color[4] = { 0.46f, 0.52f, 0.87f, 1.f };

		bool onshot = false;
		float onshot_color[4] = { 0.46f, 0.87f, 0.65f, 1.f };

		bool ragdolls = true;
		float ragdolls_color[4] = { 0.87f, 0.65f, 0.46f, 1.f };

	} c_chams;

	struct
	{
		bool skeleton = true;

		int snaplines_selection = 1;
		std::vector<std::string> snaplines_list = { "Top", "Bottom", "Left", "Right" };

		bool weapon = true;
		float weapon_color[4] = { 1.f, 1.f, 1.f, 1.f };

		bool nickname = true;
		float nickname_color[4] = { 1.f, 1.f, 1.f, 1.f };

	} c_skeleton;

	struct
	{
		std::vector<lua_data> data;

		bool opened = false;
		bool create = false;

		char name[128];
		std::string editable;

		int sort_selection = 0;
		std::vector<std::string> sort_list = { "Newest", "Oldest" };
	} c_lua;

	struct
	{
		std::vector<config_data> data;

		bool create = false;
		int active = -1;

		char name[128];

		int sort_selection = 0;
		std::vector<std::string> sort_list = { "Newest", "Oldest" };

	} c_config;
};

inline c_variable* var = new c_variable();
