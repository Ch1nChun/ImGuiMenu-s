#pragma once
#include <map>
#include <variant>
#include <array>
#include <vector>
#include <string>
#include <mutex>
#include <memory>

struct checkbox_t
{
    std::string name;
    bool enabled;
};

struct slider_int_t
{
    std::string name;
    int callback;
    int min;
    int max;
    std::string format;
};

struct slider_float_t
{
    std::string name;
    float callback;
    float min;
    float max;
    std::string format;
};

enum config_type
{
    checkbox_type,
    slider_int_type,
    slider_float_type
};

using config_variant = std::variant<checkbox_t, slider_int_t, slider_float_t>;

class c_config
{
public:

    void init_config();

    template <typename T>
    T& get(const std::string& name) { return std::get<T>(options[name]); }

    template <typename T>
    T* fill(const std::string& name)
    {
        auto& option = options[name];

        return std::get_if<T>(&option);
    }

    std::vector<std::pair<std::string, int>> order;

private:

    template <typename T, typename... Args>
    void add_option(const std::string& name, Args&&... args)
    {
        T option{ name, std::forward<Args>(args)... };
        options[name] = option;
        order.push_back({ name, get_type<T>() });
    }

    template <typename T>
    int get_type() const
    {
        if constexpr (std::is_same_v<T, checkbox_t>) return checkbox_type;
        if constexpr (std::is_same_v<T, slider_int_t>) return slider_int_type;
        if constexpr (std::is_same_v<T, slider_float_t>) return slider_float_type;
    }

    std::map<std::string, config_variant> options;
};

inline std::unique_ptr<c_config> cfg = std::make_unique<c_config>();