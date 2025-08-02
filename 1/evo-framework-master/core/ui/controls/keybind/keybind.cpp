#include "../../../inc.hpp"

extern const char* stable_keys[ 254 ];
std::vector<std::string> bind_list = { "Always", "Hold on", "Toggle", "Hold off" };

static short int str_idx_bind = -1;
static bool state_bind = false;
static int key_bind = -1;
char buffer_bind[ 128 ];
static short int stored_index_list = -1;
static bool state_list = false;
static int time_list;
static bool is_input_good = false;
std::string bind_name[ 1000 ];

evo::keybind_t::keybind_t( int* value, int* bind_type ) {
	this->value = value;
	this->bind_type = bind_type;
}

void evo::keybind_t::paint( ) { 
	/* update name */
	if ( bind_name[ _container->get_id( ) ] == "" )
		bind_name[ _container->get_id( ) ] = "-";

	auto text_s = _render->text_size( bind_name[ _container->get_id( ) ].c_str( ), evo::fonts_t::_default2 );

	int main_width = 14;
	int added_width = text_s.x;

	main_width = main_width + added_width;

	static int stored_width[ 1000 ]{ main_width };
	static bool update_bind[ 1000 ]{ false };
	if ( stored_width[ _container->get_id( ) ] != main_width ) {
		stored_width[ _container->get_id( ) ] = main_width;
		update_bind[ _container->get_id( ) ] = true;
	}

	/* animation */
	auto animation = animation_controller.get( "#keybind" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.1f ) * ( update_bind[ _container->get_id( ) ] ? 1.f : -1.f ) );

	if ( animation.value >= 0.99f ) {
		update_bind[ _container->get_id( ) ] = false;
	}

	_render->add_rect_filled_shadowed( this->base_window.x - 2, this->base_window.y, main_width, 17, _container->window_outline.blend(
		_container->window_accent, animation.value
	), 2, 10 );
	_render->add_rect_filled( this->base_window.x - 2, this->base_window.y, main_width, 17, _container->window_backround, 2 );

	_render->add_text( this->base_window.x + 5, this->base_window.y - 1, _container->window_text,
					   evo::fonts_t::_default2, bind_name[ _container->get_id( ) ].c_str( ) );
}

void evo::keybind_t::paint_list( ) { 
	/* animation */
	auto animation = animation_controller.get( "#keybind-open" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.5f ) * ( this->focused_list && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	if ( animation.value > 0.f ) {
		_ext->make_rect_filled( this->base_window.x, this->base_window.y + 20, 100, ( ( bind_list.size( ) * 22 ) + 6 ) * animation.value, _container->window_backround, 3 );
		//_ext->make_rect_filled_shadow( this->base_window.x, this->base_window.y + 20, 100, ( ( bind_list.size( ) * 23 ) + 3 ) * animation.value, _container->window_outline, 3, 10 );

		auto text_size = _render->text_size( bind_name[ _container->get_id( ) ].c_str( ), evo::fonts_t::_default2 ).x;

		int main_width = 14;
		int added_width = text_size;

		main_width = main_width + added_width;

		for ( int i = 0; i < bind_list.size( ); i++ ) {
			if ( _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( { this->base_window.x, this->base_window.y + 20 + ( i * 22 ) }, { 100, 20 } ) ) {
				*bind_type = i;
				state_list = !state_list;
				stored_index_list = -1;
			}

			/* animation */
			auto animation_2 = animation_controller.get( std::to_string( i ) + "#chltext" + animation_controller.current_child );
			animation_2.adjust( animation_2.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( *this->bind_type == i && ( animation.value ) ? 1.f : -1.f ) );

			evo::_ext->make_text( this->base_window.x + 5, this->base_window.y + 25 + ( i * 22 ), _container->window_text.blend( _container->window_accent, animation_2.value ).modify_alpha( 170 * animation.value ), evo::fonts_t::_default2,
								  bind_list[ i ].c_str( ) );
		}
		
	}
	_container->base_handler[ 3 ] = state_list;
}

void evo::keybind_t::input( ) {
	auto text_size = _render->text_size( bind_name[ _container->get_id( ) ].c_str( ), evo::fonts_t::_default2 ).x;

	int main_width = 14;
	int added_width = text_size;

	main_width = main_width + added_width;

	/* handling */
	if ( !theme::colorpicker_is_opened && _container->can_interact( ) && GetAsyncKeyState( VK_LBUTTON )
		 && _input->mouse_in_box( { this->base_window.x - 2, this->base_window.y }, 
								  { (float)main_width, float( 17 ) } ) ) { /* not sure yet */
		if ( !state_bind )
			state_bind = true;


		str_idx_bind = _container->get_id( );
	}

	if ( str_idx_bind == _container->get_id( ) )
		focused = state_bind;

	if ( focused ) {
		for ( int i = 0; i < 255; i++ ) {
			if ( _input->key_pressed( i ) ) {
				if ( i == VK_ESCAPE ) { /* see this it might be broken*/
					*value = -1;
					str_idx_bind = -1;
					return;
				}

				*value = i;
				str_idx_bind = -1;
				return;
			}
		}
	}

	if ( focused ) {
		bind_name[ _container->get_id( ) ] = "...";
	} else {
		if ( *value >= 0 ) {
			bind_name[ _container->get_id( ) ] = stable_keys[ *value ];

			if ( bind_name[ _container->get_id( ) ].c_str( ) ) {
				is_input_good = true;
			} else {
				if ( GetKeyNameText( *value << 16, buffer_bind, 127 ) ) {
					bind_name[ _container->get_id( ) ] = buffer_bind;
					is_input_good = true;
				}
			}
		}

		if ( !is_input_good ) {
			bind_name[ _container->get_id( ) ] = "-";
		}
	}
}

void evo::keybind_t::input_list( ) { 
	auto text_size = _render->text_size( bind_name[ _container->get_id( ) ].c_str( ), evo::fonts_t::_default2 ).x;

	int main_width = 14;
	int added_width = text_size;

	main_width = main_width + added_width;

	/* handling */
	if ( state_list && stored_index_list == _container->get_id( ) && _input->key_pressed( VK_LBUTTON )
		 && !_input->mouse_in_box( { this->base_window.x, this->base_window.y + 15 }, { 80, ( float )( bind_list.size( ) * 23 ) } ) ) {
		state_list = !state_list;
		stored_index_list = -1;
	}

	/* input */
	if ( !_container->base_handler[ 1 ] && !_container->base_handler[ 0 ] &&
		 !_container->base_handler[ 2 ] && _container->can_interact( ) && time_list == -1 &&
		 _input->mouse_in_box( { this->base_window.x - 2, this->base_window.y }, { (float)main_width, 17 } ) && _input->key_pressed( VK_RBUTTON ) ) {
		state_list = !state_list;
		stored_index_list = _container->get_id( );
	}

	if ( time_list >= 12 )
		time_list = 12;

	else if ( time_list <= 0 )
		time_list = 0;

	if ( state_list )
		time_list++;
	else
		time_list--;

	_container->base_opened[ _container->base_handler_t::bind ] = time_list;
	if ( stored_index_list == _container->get_id( ) )
		this->focused_list = state_list;

	_container->base_opened_state[ _container->base_handler_t::bind ][ _container->get_id( ) ] = this->focused_list;
}

const char* stable_keys[ 254 ] = {
		"-", "m1", "m2", "brk", "m3", "m4", "m5",
		"-", "bspc", "tab", "-", "-", "-", "enter", "-", "-", "shift",
		"ctrl", "alt", "pau", "caps", "-", "-", "-", "-", "-", "-",
		"esc", "-", "-", "-", "-", "space", "pgup", "pgdown", "end", "home", "left",
		"up", "right", "down", "-", "prnt", "-", "prtscr", "ins", "del", "-", "0", "1",
		"2", "3", "4", "5", "6", "7", "8", "9", "-", "-", "-", "-", "-", "-",
		"-", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
		"v", "w", "x", "y", "z", "lftwin", "rghtwin", "-", "-", "-", "num0", "num1",
		"num2", "num3", "num4", "num5", "num6", "num7", "num8", "num9", "*", "+", "_", "-", ".", "/", "f1", "f2", "f3",
		"f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20",
		"f21",
		"f22", "f23", "f24", "-", "-", "-", "-", "-", "-", "-", "-",
		"num lock", "scroll lock", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "-", "-", "-", "-", "lshft", "rshft", "lctrl",
		"rctrl", "lmenu", "rmenu", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "ntrk", "ptrk", "stop", "play", "-", "-",
		"-", "-", "-", "-", ";", "+", ",", "-", ".", "/?", "~", "-", "-",
		"-", "-", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "-", "-", "-", "{", "\\|", "}", "'\"", "-",
		"-", "-", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "-", "-", "-", "-", "-", "-",
		"-", "-", "-", "-", "-", "-", "-", "-", "-",
		"-", "-"
};
const char* lower_case[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};
const char* upper_case[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "_", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};