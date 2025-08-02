#pragma once

extern std::string bind_name[ 1000 ];
extern const char* lower_case[ 254 ];
extern const char* upper_case[ 254 ];

namespace evo {
	class keybind_t {
	public:
		keybind_t( int* value, int* bind_type );
	public:
		void paint( );
		void paint_list( );

		void input( );
		void input_list( );
	public:
		vec2_t base_window{};

		int stored_with{};
	private:
		int* value{}, * bind_type{};
		bool focused{}, focused_list{};
	};
}