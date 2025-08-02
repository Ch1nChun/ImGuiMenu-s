#pragma once

namespace evo {
	class window_t {
	public:
		window_t( std::string window_name, evo::vec2_t* pos, evo::vec2_t size, std::vector<std::string> tabs, int* selected );

		void paint( );
		void input( );

		/* tab and subtab */
		void construct_tabs( );

		/* childs */
		void make_child( child_t* child );
	public:
		int* selected{};
	private:
		std::string window_name{};
		evo::vec2_t* pos{};
		evo::vec2_t size{};
		std::vector<std::string> tabs{};

		bool is_resizing{};
	};
}