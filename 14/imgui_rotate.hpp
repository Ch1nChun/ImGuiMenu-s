#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"

//int rotation_start_index;
//
//void ImRotateStart()
//{
//	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
//}
//
//ImVec2 ImRotationCenter()
//{
//	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);
//
//	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
//	for (int i = rotation_start_index; i < buf.Size; i++)
//		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
//
//	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
//}
//
//ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }
//
//void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
//{
//	float s = sin(rad), c = cos(rad);
//	center = ImRotate(center, s, c) - center;
//
//	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
//	for (int i = rotation_start_index; i < buf.Size; i++)
//		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
//}

/*
ImRotateStart();
ImGui::Text("Text rotate");
ImRotateEnd(0.0005f * ::GetTickCount());
*/