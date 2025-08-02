#pragma once

namespace evo {
	/* child position stuff */
	enum child_pos_t {
		_top_left,
		_top_right,
		_bottom_left,
		_botton_right
	};

	/* child size stuff*/
	enum child_size_t {
		_half,
		_full,
		_custom
	};

	/* child flag */
	enum child_flags_t {
		_custom_x,
		_custom_y
	};

	/* main child class */
	class child_t {
	public:
		child_t( std::string child_title, int child_id, child_pos_t position, child_size_t size, int tab );

		/* child drop */
		child_t( std::vector<std::string> child_title, int child_id, child_pos_t position, child_size_t size, int tab, int* subtab );
	public:
		/*
			* construct the entire child system
			* construct size and position
			* after that init them in construct child
		*/
		void construct_child( );
		void construct_position( );
		void construct_size( );

		/*
			* run painting
		*/
		void paint( );

		/* what tab */
		int tab{};
	public: /* positions */
		void set_position( evo::vec2_t position );
		evo::vec2_t get_position( );
	public: /* visible */
		void set_visible( bool visible );
		bool get_visible( );
	public: /* limit visibility */
		void limit_drawing( );
		void restart_drawing( );	
		bool hovered( );
	public: /* child editors */
		void full_child( );
		void enable_custom_size( child_flags_t flags );
		void set_size( evo::vec2_t size );
	public: /* dep-elements */
		void obj( checkbox_t* checkbox );
		void obj( slider_float_t* slider );
		void obj( slider_int_t* slider );
		void obj( combo_t* combo );
		void obj( multi_dropdown_t* combo );
		void obj( colorpicker_t* colorpicker );
		void obj( keybind_t* keybind );
		void obj( text_t* text );
		void obj( textbox_t* text );
		void obj( listbox_t* listbox );
		void obj( button_t* button );
		void obj( esp_builder_t* esp_builder );
		void obj( popup_t* popup );
	public:
		/* make-elements */
		void make_checkbox( std::string label, bool* value, evo::checkbox_along_t elem = checkbox_along_t::no );
		void make_checkbox_colorpicker( std::string label, bool* value, evo::col_t* color, float* hue );
		void make_slider_float( std::string label, float* value, float min, float max, std::string suffix = "" );
		void make_slider_int( std::string label, int* value, int min, int max, std::string suffix = "" );
		void make_dropdown( std::string label, int* value, std::vector<std::string> items );
		void make_colorpicker( evo::col_t* color, float* hue );
		void make_text( std::string label );
		void make_textbox( std::string label, std::string* value );
		void make_listbox( std::string label, int* value, std::vector<std::string>& items, std::string& filter );
		void make_multibox( std::string label, std::vector<evo::multi_dropdown_system> items );
		void make_keybind( int* key, int* type );
		void make_button( std::string label, std::function<void()> func );
	public: /* scrolling */
		void enable_scrolling( );
	private:
		/* child pos and size */
		child_pos_t position{};
		child_size_t size{};

		/* child name and id */
		std::string child_title{};
		std::vector<std::string> child_title_vec{};
		int child_id{};
		int* selected_subtab;

		/* base window position */
		evo::vec2_t base_window{};
		evo::vec2_t base_size{};
		evo::vec2_t base_id_size[ 1000 ]{};

		bool focused_child{};
		float anim_child_1{};

		/* element position */
		struct {
			float x;
			float y[ 1000 ];
		} base_element;

		/* is visible */
		bool base_visible{}, base_hovered{}, base_full[ 1000 ]{ false }, base_custom_size_x[ 1000 ]{ false }, base_custom_size_y[ 1000 ]{ false };
	};
}