/*
* ImGui Tricks v1.02
* ImTricks is a library that simplifies and increases the functionality of ImGui.
* At the moment, Imtrucks adds functions to speed up the creation of animated menus,
* adds a notification system, and also adds support for ImColor in ColorEdit3/ColorEdit4.
*
* Made with love by pers0na2.
*/

#include <windows.h>
#include <ios>
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#ifndef IMGUI_TRICK_ONCE
#define IMGUI_TRICK_ONCE


/*
// A customized ImColor structure with an additional constructor from the HEX HTML color code
// and a method for obtaining HEX HTML from an already existing color.
*/
struct ImTrickyColor
{
	float values[4];

	ImTrickyColor() { values[0] = 0.f; values[1] = 0.f; values[2] = 0.f; values[3] = 0.f; };

	ImTrickyColor(float r, float g, float b, float a = 1.f) { values[0] = r; values[1] = g; values[2] = b; values[3] = a; };
	ImTrickyColor(int r, int g, int b, int a = 255) { values[0] = r / 255.f; values[1] = g / 255.f; values[2] = b / 255.f; values[3] = a / 255.f; };
	ImTrickyColor(std::string html) {
		auto SplitByCharacters = [](const std::string& str, int splitLength) {

			int NumSubstrings = str.length() / splitLength;
			std::vector<std::string> ret;

			for (int i = 0; i < NumSubstrings; i++) {
				ret.push_back(str.substr(i * splitLength, splitLength));
			}
			if (str.length() % splitLength != 0) {
				ret.push_back(str.substr(splitLength * NumSubstrings));
			}

			return ret;
		};

		if (html.at(0) == '#') {
			html.erase(0, 1);
		}

		while (html.length() != 8) {
			html.append("0");
		}

		std::vector<std::string> colori = SplitByCharacters(html, 2);

		values[0] = stoi(colori[0], nullptr, 16) / 255.f;
		values[1] = stoi(colori[1], nullptr, 16) / 255.f;
		values[2] = stoi(colori[2], nullptr, 16) / 255.f;
		values[3] = stoi(colori[3], nullptr, 16) / 255.f;
	};
	ImTrickyColor(ImColor col) { values[0] = col.Value.x; values[1] = col.Value.y; values[2] = col.Value.z; values[3] = col.Value.w; };

	inline operator ImU32() const { return ImGui::ColorConvertFloat4ToU32(ImVec4(values[0], values[1], values[2], values[3])); }
	inline operator ImVec4() const { return ImVec4(values[0], values[1], values[2], values[3]); }
	inline operator ImColor() const { return ImColor(values[0], values[1], values[2], values[3]); }
	inline float operator[](std::size_t idx) const { return values[idx]; }

	inline void SetHSV(float h, float s, float v, float a = 1.0f) { ImGui::ColorConvertHSVtoRGB(h, s, v, values[0], values[1], values[2]); values[3] = a; }
	inline ImTrickyColor HSV(float h, float s, float v, float a = 1.0f) { float r, g, b; ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImTrickyColor(r, g, b, a); }

	inline std::string HTML() {
		int integers[4] = { values[0] * 255,values[1] * 255, values[2] * 255,values[3] * 255 };
		return "#" + (std::stringstream{} << std::hex << (integers[0] << 24 | integers[1] << 16 | integers[2] << 8 | integers[3])).str();
	};
};

enum NotifyState : int {
	ImTrickNotify_Success = 0,
	ImTrickNotify_Warning = 1,
	ImTrickNotify_Danger = 2,
	ImTrickNotify_Default = 3
};

namespace ImTricks {

	/*
	// The Animations namespace contains everything you need to easily create animations in your ImGui menus.
	*/
	namespace Animations {

		/*
		// Usage:
		//  int trickInt = ImTricks::Animations::FastLerpInt("trickInt", enable_animation, 0, 255, 15);
		//  draw->AddRectFilled(p, p + ImVec2(30, 30), ImColor(255, 255, 255, trickInt));
		*/
		extern int FastIntLerp(const char* identifier, bool state, int min, int max, int speed);

		/*
		// Usage:
		//  float trickFloat = ImTricks::Animations::FastLerpInt("trickFloat", enable_animation, 0.f, 1.f, 0.05f);
		//  draw->AddRectFilled(p, p + ImVec2(30, 30), ImColor(1.f, 1.f, 1.f, trickFloat));
		*/
		extern float FastFloatLerp(std::string identifier, bool state, float min, float max, float speed);


		/*
		// Usage:
		//  float trickFloat = ImTricks::Animations::FastFloatLerp("header", enable_animation, 0.f, 1.f, 0.05f);
		//  ImVec2 pos1 = ImTricks::Animations::FastImVec2Lerp("pos1", enable_animation, ImVec2(0, 50), ImVec2(100, 50), 5);
		//  ImVec2 pos2 = ImTricks::Animations::FastImVec2Lerp("pos2", enable_animation, ImVec2(413, 80), ImVec2(513, 80), 5);
		//  draw->AddRectFilled(p + pos1, p + pos2, ImTricks::Animations::FastColorLerp(ImColor(255, 0, 0), ImColor(0, 255, 0), trickFloat));
		*/
		extern ImVec2 FastImVec2Lerp(const char* identifier, bool state, ImVec2 min, ImVec2 max, float speed);

		/*
		// Usage:
		//	float trickFloat = ImTricks::Animations::FastLerpFloat("header", check, 0.f, 1.f, 0.05f);
		//	draw->AddRectFilled(p, p + ImVec2(513, 30), ImTricks::Animations::FastColorLerp(ImColor(255, 0, 0), ImColor(0, 255, 0), trickFloat));
		*/
		extern ImColor FastColorLerp(ImColor start, ImColor end, float stage);
		extern ImTrickyColor FastColorLerp(ImTrickyColor start, ImTrickyColor end, float stage);
	}

	/*
	// The NotifyManager namespace contains everything you need to easily create notifications in your interface.
	*/
	namespace NotifyManager {

		struct NotifyStruct {
			const char* message;
			NotifyState state;
			ULONGLONG time;
		};

		extern std::vector<NotifyStruct> notifies;

		/*
		// Description:
		// Responsible for adding notifications to the display queue.
		//
		// Usage:
		//	if (ImGui::Button("Create Notify", { 120, 25 }))
		//		ImTricks::NotifyManager::AddNotify("The notification was displayed successfully.", ImTrickNotify_Success);
		*/
		extern void AddNotify(const char* message, NotifyState state);

		/*
		// Description:
		// Call a function in your render. This function is responsible for displaying and monitoring notifications.
		//
		// Usage:
		//  ImTricks::NotifyManager::HandleNotifies(ImGui::GetOverlayDrawList());
		*/
		extern void HandleNotifies(ImDrawList* draw = ImGui::GetBackgroundDrawList());

	}


	namespace Widgets {

		// I don't understand why ocornut didn't add ImColor support to ColorEdit.
		extern void ColorEdit3(const char* label, ImColor& color, ImGuiColorEditFlags flags = NULL);
		extern void ColorEdit4(const char* label, ImColor& color, ImGuiColorEditFlags flags = NULL);
		extern void ColorEdit3(const char* label, ImTrickyColor& color, ImGuiColorEditFlags flags = NULL);
		extern void ColorEdit4(const char* label, ImTrickyColor& color, ImGuiColorEditFlags flags = NULL);

	}

	namespace DrawList {

		/*
		// The original author of the code is nathanvoglsam
		// https://github.com/nathanvoglsam
		//
		// Usage:
		//  ImTricks::DrawList::AddTriangleFilledMultiColor(draw, p + ImVec2(0, 0), p + ImVec2(150, 150), p + ImVec2(300, 0), ImTrickyColor("#32a877ff"), ImTrickyColor("#3246a8ff"), ImTrickyColor("#a83232ff"));
		*/
		extern void AddTriangleFilledMultiColor(ImDrawList* draw, const ImVec2& a, const ImVec2& b, const ImVec2& c, ImTrickyColor col_a, ImTrickyColor col_b, ImTrickyColor col_c);

		/*
		// Usage:
		//	ImVec2 points[] = { p + ImVec2(0, 75),p + ImVec2(75, 0), p + ImVec2(150, 25), p + ImVec2(150, 150), p + ImVec2(75, 200), p + ImVec2(0, 150) };
		//	ImTrickyColor colors[] = { ImTrickyColor("#00ffffff"), ImTrickyColor("#00ffffff"),ImTrickyColor("#00ffffff"), ImTrickyColor("#e600ffff"),ImTrickyColor("#e600ffff"), ImTrickyColor("#e600ffff") };
		//	ImTricks::DrawList::AddConvexPolyFilledMultiColor(draw, points, colors, 6);
		*/
		extern void AddConvexPolyFilledMultiColor(ImDrawList* draw, const ImVec2* verts, ImTrickyColor* colors, int points);
	}
}

static inline ImVec2 operator+(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }
static inline ImVec2 operator-(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }
static inline ImVec4 operator+(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }
static inline ImVec4 operator-(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }


#endif
