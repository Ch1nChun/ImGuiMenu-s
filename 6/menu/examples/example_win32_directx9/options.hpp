#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#define FLOAT_TO_CLR(clr) clr[0] * 255.0f, clr[1] * 255.0f, clr[2] * 255.0f, clr[3] * 255.0f

/*					const chars					*/
static const char* prioritys[] = { "Damage", "Accuracy", "Fov" };
static const char* hitboxes[] = { "Head", "Neck", "Upper Chest", "Chest", "Arms", "Pelvis", "Stomach", "Legs", "Foots" };
/*					const chars					*/

struct ragebot_settings {
	bool enabled = true;
	bool hpbar = false;
	bool silent = false;
	float fov = 0;
	int priority = 0;
	int hitbox = 0;
	bool hitboxes[9];
	int double_tap = 0;
	int hide_shots = 0;
};

extern std::map<int, const char*> weapon_names;
extern const std::map<int, const char*> config_names;
extern const char* const KeyNames[];
struct legitbot_settings {
	bool enabled = false;
	bool deathmatch = false;
	bool autopistol = false;
	bool smoke_check = false;
	bool flash_check = false;
	bool jump_check = false;
	bool autowall = false;
	bool silent = false;
	bool autofire = false;
	bool bind = true;
	bool rcs = false;
	bool rcs_fov_enabled = false;
	bool rcs_smooth_enabled = false;
	bool hitboxes[8];
	struct {
		bool enabled = false;
		int ticks = 6;
	} backtrack;
	bool only_in_zoom = true;
	int autofire_key = 1;
	int key = 1;
	int aim_type = 1;
	int priority = 0;
	int fov_type = 0;
	int rcs_type = 0;
	int hitbox = 0;
	float fov = 0.f;
	float silent_fov = 0.f;
	float rcs_fov = 0.f;
	float smooth = 1;
	float rcs_smooth = 1;
	int shot_delay = 0;
	int kill_delay = 0;
	int rcs_x = 100;
	int rcs_y = 100;
	int rcs_start = 1;
	int min_damage = 1;
};

struct skins_s
{
	struct sides
	{
		bool enabled = false;
		int definitionIndex = 0;
		int paintKit = 0;
		int seed = 0;
		int stattrak = -1;
		int quality = 4;
		float wear = FLT_MIN;
	};

	sides t;
	sides ct;
};

struct weapons
{
	skins_s skins;
	legitbot_settings legitbot;
};

struct skinInfo
{
	std::string name;
	std::string cdnName;
	std::vector<std::string> weaponName;
	int rarity;
};

class Options
{
public:
	/*			AimBot			*/
	std::map<int, ragebot_settings> ragebot = { };

	/*			AimBot			*/
	struct {
		bool active = false;
	} abot;

	/*			Weapons			*/
	std::map<short, weapons> weapons;

	/*			Visuals			*/
	struct {
		bool enabled = false;
		bool enemy_only = false;
		int boxes = 0;
		bool names = false;
		bool health = false;
		bool armour = false;
		bool weapons = false;
		bool dropped_weapons = false;
		bool defuse_kit = false;
		bool planted_c4 = false;
		bool items = false;
		bool drawfov = false;
		bool no_hands = false;
		bool no_flash = false;
		bool no_smoke = false;
	} esp;

	struct {
		bool enabled = false;
		bool enemy_only = false;
		bool players = false;
		bool chickens = false;
		bool c4_carrier = false;
		bool planted_c4 = false;
		bool defuse_kits = false;
		bool weapons = false;
	} glow;

	struct {
		bool enabled = false;
		bool enemy_only = false;
		bool visible_only = false;
		int type = 0;

		bool arms_enabled = false;
		int type_arms = 0;

		bool sleeves_enabled = false;
		int type_sleeves = 0;

		bool weapon_enabled = false;
		int type_weapon = 0;
	} chams;

	/*			Misc			*/
	struct {
		bool bhop = false;
		bool thirdperson = false;
		bool autoaccept = false;
		bool autodefuse = false;
		bool showranks = false;
		bool showmoney = false;
		bool clantag = false;
		bool antiobs = false;
		bool background = true;
		bool backdrop = true;
		bool watermark = true;
		int thirdperson_bind = 0;
		float thirdperson_dist = 50.f;
		int override_fov = 90;
		int viewmodel_fov = 68;
	} misc;
	
	/*			Changers			*/
	struct
	{
		std::unordered_map<int, skinInfo> skinInfo;
		int itemCount = 0;
		bool medal = false;
		bool musickits = false; 
		int medalIDEquip;
		int medalID; 
		std::vector<uint32_t> packets_medals = { 1372, 958, 957, 956, 955 };
	} inventory;

	struct
	{
		bool enabled = false;
		bool prime = false;

		int friendly = -1;
		int teaching = -1;
		int leader = -1;

		int rank_mm = 0;
		int wins_mm = -1;
		int rank_wm = 0;
		int wins_wm = -1;
		int rank_dz = 0;
		int wins_dz = -1;

		int player_level = -1;
		int player_xp = -1;
	} profile;

	/*			Colors			*/
	struct {
		struct {
			float visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float crosshair[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float weapons[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float defuse[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float c4[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float item[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		} esp;

		struct {
			float enemy[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float chickens[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float c4_carrier[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float planted_c4[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float defuse[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float weapons[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		} glow;

		struct {
			float visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float arms[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float sleeves[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			float weapon[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		} chams;
	} color;

	/*			cheatname			*/
	struct {
		char* version = "v 0.2 | ";
		bool inject = false;
	} cheatname;

	/*			WaterMark			*/
	struct {
		std::string time = "unknown";
		int ping = 0;
	} watermark;
};

extern Options opt;
extern bool   g_Unload;