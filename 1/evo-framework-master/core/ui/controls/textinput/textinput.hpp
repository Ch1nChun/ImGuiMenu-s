#pragma once

namespace evo {
	class textbox_t {
	public:
		textbox_t( std::string label, std::string* value );
	public:
		void input( );
		void paint( );
	public:
		vec2_t base_window{};
	private:
		std::string label{};
		std::string* value{};

		bool focused{};
	public:
		float grab_animation_data[ 1000 ];
	};
}