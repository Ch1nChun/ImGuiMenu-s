#include "custom.h"

#pragma warning (disable : 4244) // O_o

struct tab_anim
{
    float label_anim;
    float hovered_anim;
    float active_anim;
};

bool custom::selected(const char* label, bool tab)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + label_size.x, pos.y + label_size.y));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y), style.FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, tab_anim> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, {0.6f, 0.f, 0.f} });
        it_anim = anim.find(id);
    }

    it_anim->second.label_anim = ImLerp(it_anim->second.label_anim, tab ? 0.0f : 0.6f, 0.05f * (1.0f - ImGui::GetIO().DeltaTime));
    it_anim->second.hovered_anim = ImLerp(it_anim->second.hovered_anim, hovered ? 0.2f : 0.0f, 0.03f * (1.0f - ImGui::GetIO().DeltaTime));
    it_anim->second.active_anim = ImLerp(it_anim->second.active_anim, tab ? 1.0f : 0.0f, 0.08f * (1.0f - ImGui::GetIO().DeltaTime));

    window->DrawList->AddText(ImVec2((rect.Min.x + rect.Max.x) / 2.f - (label_size.x / 2.f), (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f)), ImColor(1.0f, 1.0f, 1.0f, it_anim->second.label_anim + it_anim->second.hovered_anim), label);
    window->DrawList->AddText(ImVec2((rect.Min.x + rect.Max.x) / 2.f - (label_size.x / 2.f), (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f)), ImColor(accent_colour[0], accent_colour[1], accent_colour[2], it_anim->second.active_anim), label);

    return pressed;
}
