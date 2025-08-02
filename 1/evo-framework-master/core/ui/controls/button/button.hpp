#pragma once

namespace evo {
	class button_t {
	public:
		button_t( std::string name, std::function< void( ) > function );
	public:
		void input( );
		void paint( );
	public:
		vec2_t pos{};
	private:
		bool focused_element{};
		std::string name{};
		std::function< void( ) > function = { };
	};
}