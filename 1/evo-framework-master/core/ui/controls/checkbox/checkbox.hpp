#pragma once

namespace evo {
	enum checkbox_along_t {
		no,
		has_element
	};

	class checkbox_t {
	public:
		/* constructor */
		checkbox_t( std::string label, bool* value, checkbox_along_t mode = checkbox_along_t::no );

		void paint( );
		void input( );
	private:
		std::string label{};
		
		checkbox_along_t type;

		/* value has to be a ptr */
		bool* value{};
		bool hovered{};
	public:
		//__forceinline void bind_colorpicker( evo::child_t* cur_child, evo::col_t* color, float* colorh ) {
		//	cur_child->make_colorpicker( color, colorh )
		//}

		/* base window - element position */
		evo::vec2_t base_window{};
	};
}