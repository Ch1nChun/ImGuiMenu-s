#include "imgui_tricks.hpp"

namespace ImTricks {

	namespace Animations {

		int FastIntLerp(const char* identifier, bool state, int min, int max, int speed) {

			static std::unordered_map<const char*, int> valuesMapInt;
			auto value = valuesMapInt.find(identifier);

			if (value == valuesMapInt.end()) {
				valuesMapInt.insert({ identifier,  min });
				value = valuesMapInt.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		float FastFloatLerp(std::string identifier, bool state, float min, float max, float speed) {

			static std::unordered_map<std::string, float> valuesMapFloat;
			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		ImVec2 FastImVec2Lerp(const char* identifier, bool state, ImVec2 min, ImVec2 max, float speed) {

			static std::unordered_map<const char*, ImVec2> valuesMapFloat;
			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second.x < max.x)
					value->second.x += frameRateSpeed;

				if (value->second.y < max.y)
					value->second.y += frameRateSpeed;
			}
			else {
				if (value->second.x > min.x)
					value->second.x -= frameRateSpeed;

				if (value->second.y > min.y)
					value->second.y -= frameRateSpeed;
			}

			value->second.x = std::clamp(value->second.x, min.x, max.x);
			value->second.y = std::clamp(value->second.y, min.y, max.y);

			return value->second;
		}

		ImColor FastColorLerp(ImColor start, ImColor end, float stage)
		{
			ImVec4 lerp = ImLerp(
				ImVec4(start.Value.x, start.Value.y, start.Value.z, start.Value.w),
				ImVec4(end.Value.x, end.Value.y, end.Value.z, end.Value.w),
				stage);

			return ImGui::ColorConvertFloat4ToU32(lerp);
		}

		ImTrickyColor FastColorLerp(ImTrickyColor start, ImTrickyColor end, float stage)
		{
			ImVec4 lerp = ImLerp(
				ImVec4(start[0], start[1], start[2], start[3]),
				ImVec4(end[0], end[1], end[2], end[3]),
				stage);

			return ImTrickyColor(lerp.x, lerp.y, lerp.z, lerp.w);
		}
	}

	namespace NotifyManager {

		std::vector<NotifyStruct> notifies;

		void AddNotify(const char* message, NotifyState state) {
			notifies.push_back({ message, state, GetTickCount64() + 3000 });
		}

		void HandleNotifies(ImDrawList* draw) {

			if (notifies.empty())
				return;

			const auto ScreenSize = ImGui::GetIO().DisplaySize;
			ImVec2 NotifyPos = ScreenSize - ImVec2(320.f, 50.f);

			auto DrawNotify = [&draw, &NotifyPos](NotifyStruct notify) {

				const auto NotifyEndPos = NotifyPos + ImVec2(300, 30);

				draw->AddRectFilled(NotifyPos, NotifyEndPos, ImGui::GetColorU32(ImGuiCol_PopupBg), ImGui::GetStyle().PopupRounding);

				auto StateColor = ImColor(45, 45, 45);

				switch (notify.state) {
				case ImTrickNotify_Success: StateColor = ImColor(0, 255, 0); break;
				case ImTrickNotify_Warning: StateColor = ImColor(130, 255, 0); break;
				case ImTrickNotify_Danger: StateColor = ImColor(255, 0, 0); break;
				case ImTrickNotify_Default:
				default:
					StateColor = ImColor(45, 45, 45);
					break;
				}

				draw->AddRectFilled(NotifyPos, ImVec2(NotifyPos.x + 5.f, NotifyPos.y + 30), StateColor, 1.f);

				const auto TextSize = ImGui::CalcTextSize(notify.message);
				const auto TextPos = NotifyPos + ImVec2(15.f, 15.f - TextSize.y / 2.f);

				draw->AddText(TextPos, ImGui::GetColorU32(ImGuiCol_Text), notify.message);

				NotifyPos.y -= 40.f;
			};

			for (int i = 0; i < notifies.size(); i++)
			{
				auto current_notify = notifies.at(i);
				if (current_notify.time < GetTickCount64())
				{
					notifies.erase(notifies.begin() + i);
					continue;
				}
				DrawNotify(current_notify);            
			}
		}
	}

	namespace Widgets {
		void ColorEdit3(const char* label, ImColor& color, ImGuiColorEditFlags flags) {
			float col[3] = { color.Value.x, color.Value.y, color.Value.z };

			if (ImGui::ColorEdit3(label, col, flags))
				color = ImColor(col[0], col[1], col[2]);
		}

		void ColorEdit4(const char* label, ImColor& color, ImGuiColorEditFlags flags) {
			float col[4] = { color.Value.x, color.Value.y, color.Value.z , color.Value.w };

			if (ImGui::ColorEdit4(label, col, flags))
				color = ImColor(col[0], col[1], col[2], col[3]);
		}

		void ColorEdit3(const char* label, ImTrickyColor& color, ImGuiColorEditFlags flags) {
			float col[3] = { color[0], color[1], color[2] };

			if (ImGui::ColorEdit3(label, col, flags))
				color = ImTrickyColor(col[0], col[1], col[2]);
		}

		void ColorEdit4(const char* label, ImTrickyColor& color, ImGuiColorEditFlags flags) {
			float col[4] = { color[0], color[1], color[2] , color[3] };

			if (ImGui::ColorEdit4(label, col, flags))
				color = ImTrickyColor(col[0], col[1], col[2], col[3]);
		}
	}

	namespace DrawList {
		/*
		// The original author of the code is nathanvoglsam
		// https://github.com/nathanvoglsam
		*/
		void AddConvexPolyFilledGradient(ImDrawList* draw, const ImVec2* points, const ImTrickyColor* col, const int points_count)
		{
			const ImVec2 uv = draw->_Data->TexUvWhitePixel;

			if (draw->Flags & ImDrawListFlags_AntiAliasedFill)
			{
				// Anti-aliased Fill
				const float AA_SIZE = 1.0f;
				const int idx_count = (points_count - 2) * 3 + points_count * 6;
				const int vtx_count = (points_count * 2);
				draw->PrimReserve(idx_count, vtx_count);

				// Add indexes for fill
				unsigned int vtx_inner_idx = draw->_VtxCurrentIdx;
				unsigned int vtx_outer_idx = draw->_VtxCurrentIdx + 1;
				for (int i = 2; i < points_count; i++)
				{
					draw->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); draw->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); draw->_IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
					draw->_IdxWritePtr += 3;
				}

				// Compute normals
				ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
				for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
				{
					const ImVec2& p0 = points[i0];
					const ImVec2& p1 = points[i1];
					ImVec2 diff = p1 - p0;
					diff *= ImInvLength(diff, 1.0f);
					temp_normals[i0].x = diff.y;
					temp_normals[i0].y = -diff.x;
				}

				for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
				{
					// Average normals
					const ImVec2& n0 = temp_normals[i0];
					const ImVec2& n1 = temp_normals[i1];
					ImVec2 dm = (n0 + n1) * 0.5f;
					float dmr2 = dm.x * dm.x + dm.y * dm.y;
					if (dmr2 > 0.000001f)
					{
						float scale = 1.0f / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					dm *= AA_SIZE * 0.5f;

					// Add vertices
					draw->_VtxWritePtr[0].pos = (points[i1] - dm); draw->_VtxWritePtr[0].uv = uv; draw->_VtxWritePtr[0].col = col[i1];        // Inner
					draw->_VtxWritePtr[1].pos = (points[i1] + dm); draw->_VtxWritePtr[1].uv = uv; draw->_VtxWritePtr[1].col = col[i1] & ~IM_COL32_A_MASK;  // Outer
					draw->_VtxWritePtr += 2;

					// Add indexes for fringes
					draw->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); draw->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); draw->_IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
					draw->_IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); draw->_IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); draw->_IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
					draw->_IdxWritePtr += 6;
				}
				draw->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
			}
			else
			{
				// Non Anti-aliased Fill
				const int idx_count = (points_count - 2) * 3;
				const int vtx_count = points_count;
				draw->PrimReserve(idx_count, vtx_count);
				for (int i = 0; i < vtx_count; i++)
				{
					draw->_VtxWritePtr[0].pos = points[i]; draw->_VtxWritePtr[0].uv = uv; draw->_VtxWritePtr[0].col = col[i];
					draw->_VtxWritePtr++;
				}
				for (int i = 2; i < points_count; i++)
				{
					draw->_IdxWritePtr[0] = (ImDrawIdx)(draw->_VtxCurrentIdx); draw->_IdxWritePtr[1] = (ImDrawIdx)(draw->_VtxCurrentIdx + i - 1); draw->_IdxWritePtr[2] = (ImDrawIdx)(draw->_VtxCurrentIdx + i);
					draw->_IdxWritePtr += 3;
				}
				draw->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
			}
		}

		/*
		// The original author of the code is nathanvoglsam
		// https://github.com/nathanvoglsam
		*/
		void AddTriangleFilledMultiColor(ImDrawList* draw, const ImVec2& a, const ImVec2& b, const ImVec2& c, ImTrickyColor col_a, ImTrickyColor col_b, ImTrickyColor col_c)
		{
			ImVec2 verts[] = { a, b, c };
			ImTrickyColor colors[] = { col_a, col_b, col_c };
			AddConvexPolyFilledGradient(draw, verts, colors, 3);
		}

		void AddConvexPolyFilledMultiColor(ImDrawList* draw, const ImVec2* verts, ImTrickyColor* colors, int points)
		{
			AddConvexPolyFilledGradient(draw, verts, colors, points);
		}
	}

}
