#pragma once

namespace evo {
	class combo_t {
	public:
		combo_t( std::string label, int* value, std::vector<std::string> items );
	public:
		void paint( );
		void input( );
	public:
		evo::vec2_t base_window{};
	private:
		std::string label{};
		std::vector<std::string> items{};

		int* value{};
		bool hovered{}, focused{};
	};
}