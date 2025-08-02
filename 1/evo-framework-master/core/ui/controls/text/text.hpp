#pragma once

namespace evo {
	class text_t {
	public:
		text_t( std::string label );
	public:
		void paint( );

		vec2_t base_window{};
	private:
		std::string label{};
	};
}