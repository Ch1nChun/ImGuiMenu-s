#include "../../../inc.hpp"

static auto str_idx_color = -1;
static bool state_color = false;
inline static auto str_index_dropdown = -1;
inline static bool state_dropdown = false;
inline static int time_dropdown;

int elem_height[1000];

evo::popup_t::popup_t( int pop_id, std::string popup ) { 
	this->popup = popup;
	this->pop_id = pop_id;
}

void evo::popup_t::handle_checkbox( ) {
	for ( int check = 0; check < this->checkbox_elems.size( ); check++ ) {
		auto animation = animation_controller.get( this->checkbox_elems[ check ].label + "#active" + std::to_string( this->pop_id ) + animation_controller.current_child );
		animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( *this->checkbox_elems[ check ].value ? 1.f : -1.f ) );

		if ( evo::_input->mouse_in_box( evo::vec2_t( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ] ),
										evo::vec2_t( _container->group_width - 30, 30 ) ) && evo::_input->key_pressed( VK_LBUTTON ) ) {
			*this->checkbox_elems[ check ].value = !*this->checkbox_elems[ check ].value;
		}

		_ext->make_rect( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ], _container->group_width - 30,
						 30, _container->window_outline.modify_alpha( 80 * this->track_animation[ this->pop_id ] ), 2, 1 );

		_ext->make_rect_filled( this->base_window.x + 15 + _container->group_width - 60, this->base_window.y + 43 + elem_height[ this->pop_id ],
								15, 15, _container->window_backround.darker( 5 ).modify_alpha( 255 * this->track_animation[ this->pop_id ] )/*.blend( _container->window_accent, animation.value ).modify_alpha( 255 * this->track_animation[ _container->get_id( ) ] )*/, 2 );

		_ext->make_rect_filled( this->base_window.x + 15 + _container->group_width - 60, this->base_window.y + 43 + elem_height[ this->pop_id ],
								15 * animation.value, 15 * animation.value, _container->window_accent.darker( 5 ).modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 2, true );

		_ext->make_rect_filled_shadow( this->base_window.x + 15 + _container->group_width - 60, this->base_window.y + 43 + elem_height[ this->pop_id ],
									   15, 15, _container->window_backround.darker( 5 ).blend( _container->window_accent, animation.value ).modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 2, 15 );

		_ext->make_text( this->base_window.x + 25, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 155 * this->track_animation[ this->pop_id ] ), evo::fonts_t::_default2,
						 this->checkbox_elems[ check ].label.c_str( ) );

		elem_height[this->pop_id ] += 35;
		_container->set_id( _container->get_id( ) + 1 );
	}
}

void evo::popup_t::handle_slider_int( ) {
	for ( auto& slider : this->elem_sliderint ) {
		_ext->make_rect( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ], _container->group_width - 30,
						 30, _container->window_outline.modify_alpha( 80 * this->track_animation[ this->pop_id ] ), 2, 1 );

		_ext->make_text( this->base_window.x + 25, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 155 * this->track_animation[ this->pop_id ] ),
						 evo::fonts_t::_default2, slider.label.c_str( ), evo::font_flags_t::dropshadow_low );

		int _height{ 2 };
		int _added_height{};

		/* slider value */
		auto min_delta = *slider.value - slider.min;
		auto delta = static_cast< float >( slider.max - slider.min );
		auto total = ( min_delta / delta ) * ( _container->group_width - 30 );
		total = std::fmin( total, ( _container->group_width - 30 ) );

		if ( total > slider.min ) {
			_ext->make_rect_filled_shadow( this->base_window.x + 17, this->base_window.y + 56 + elem_height[ this->pop_id ] + 6, total - 2, 2, _container->window_accent.modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 5, 10 );
			_ext->make_rect_filledex( this->base_window.x + 17, this->base_window.y + 56 + elem_height[ this->pop_id ] + 6, total - 2, 2, _container->window_accent.modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 5, true );
		}

		/* input */
		{
			auto x = _container->group_width - 30;
			auto delta = slider.max - slider.min;
			static auto str_index = -1;

			int text_size = evo::_render->text_size( slider.label.c_str( ), evo::fonts_t::_default2 ).y;
			//this->hovered = evo::_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y + text_size + 4 ), vec2_t( _container->group_width, 7 ) );

			if ( _container->can_interact( ) && !theme::colorpicker_is_opened ) { // + 15 + elem_height
				if ( _input->key_pressed( VK_LBUTTON ) & 1 && evo::_input->mouse_in_box( vec2_t( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ] ), vec2_t( _container->group_width - 60, 30 ) ) ) {
					str_index = _container->get_id( );
				}
			}

			if ( GetAsyncKeyState( VK_LBUTTON ) && str_index == _container->get_id( ) ) {
				//t//his->in_use = true;

				float normalized_pos = ( _input->get_mouse_position( ).x - this->base_window.x ) / ( _container->group_width - 30 );
				float target_value = slider.min + delta * normalized_pos;

				*slider.value = animation_controller.lerp_single( *slider.value, target_value, slider.max < 30 ? 1.f : 0.2f );

				//	std::cout << std::to_string( *slider.value ) + " v\n";

					/* gheto clamping */
				if ( *slider.value < slider.min )
					*slider.value = slider.min;

				if ( *slider.value > slider.max )
					*slider.value = slider.max;
			}

			/* update index */
			if ( !GetAsyncKeyState( VK_LBUTTON ) ) {
				//this->in_use = false;
				str_index = -1;
			}
		}

		std::string full_txt{};
		full_txt = std::to_string( *slider.value ) + slider.sufix;

		_ext->make_text( this->base_window.x + _container->group_width - 25 - _render->text_size( full_txt.c_str( ), 1 ).x, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 155 * this->track_animation[ this->pop_id ] ),
								evo::fonts_t::_default2, full_txt.c_str( ) );

		elem_height[ this->pop_id ] += 35;
		_container->set_id( _container->get_id( ) + 1 );
	}

}

void evo::popup_t::handle_slider_float( ) { 
	for ( auto& slider : this->elem_sliderfloat ) {
		_ext->make_rect( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ], _container->group_width - 30,
						 30, _container->window_outline.modify_alpha( 80 * this->track_animation[ this->pop_id ] ), 2, 1 );

		_ext->make_text( this->base_window.x + 25, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 155 * this->track_animation[ this->pop_id ] ),
						 evo::fonts_t::_default2, slider.label.c_str( ), evo::font_flags_t::dropshadow_low );

		int _height{ 2 };
		int _added_height{};

		/* slider value */
		auto min_delta = *slider.value - slider.min;
		auto delta = static_cast< float >( slider.max - slider.min );
		auto total = ( min_delta / delta ) * ( 200 - 30 );
		total = std::fmin( total, ( 200 - 30 ) );

		if ( total > slider.min ) {
			_ext->make_rect_filled_shadow( this->base_window.x + 17, this->base_window.y + 56 + elem_height[ this->pop_id ] + 6, total - 2, 2, _container->window_accent.modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 5, 10 );
			_ext->make_rect_filledex( this->base_window.x + 17, this->base_window.y + 56 + elem_height[ this->pop_id ] + 6, total - 2, 2, _container->window_accent.modify_alpha( 255 * this->track_animation[ this->pop_id ] ), 5, true );
		}

		/* input */
		{
			auto x = _container->group_width - 30;
			auto delta = slider.max - slider.min;
			static auto str_index = -1;

			int text_size = evo::_render->text_size( slider.label.c_str( ), evo::fonts_t::_default2 ).y;
			//this->hovered = evo::_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y + text_size + 4 ), vec2_t( _container->group_width, 7 ) );

			if ( _container->can_interact( ) && !theme::colorpicker_is_opened ) { // + 15 + elem_height
				if ( _input->key_pressed( VK_LBUTTON ) & 1 && evo::_input->mouse_in_box( vec2_t( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ] ), vec2_t( _container->group_width - 60, 30 ) ) ) {
					str_index = _container->get_id( );
				}
			}

			if ( GetAsyncKeyState( VK_LBUTTON ) && str_index == _container->get_id( ) ) {
				//t//his->in_use = true;

				float normalized_pos = ( _input->get_mouse_position( ).x - this->base_window.x ) / ( _container->group_width - 30 );
				float target_value = slider.min + delta * normalized_pos;

				*slider.value = animation_controller.lerp_single( *slider.value, target_value, slider.max < 30 ? 1.f : 0.2f );

				//	std::cout << std::to_string( *slider.value ) + " v\n";

					/* gheto clamping */
				if ( *slider.value < slider.min )
					*slider.value = slider.min;

				if ( *slider.value > slider.max )
					*slider.value = slider.max;
			}

			/* update index */
			if ( !GetAsyncKeyState( VK_LBUTTON ) ) {
				//this->in_use = false;
				str_index = -1;
			}
		}

		std::string full_txt{};
		full_txt = precision( *slider.value, 1 ) + slider.sufix;

		_ext->make_text( this->base_window.x + _container->group_width - 25 - _render->text_size( full_txt.c_str( ), 1 ).x, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 155 * this->track_animation[ this->pop_id ] ),
						 evo::fonts_t::_default2, full_txt.c_str( ) );

		elem_height[ this->pop_id ] += 35;
		_container->set_id( _container->get_id( ) + 1 );
	}
}

void evo::popup_t::handle_text( ) { 
	for ( auto& text : this->elem_texct ) {
		_ext->make_rect( this->base_window.x + 15, this->base_window.y + 35 + elem_height[ this->pop_id ], _container->group_width - 30,
						 30, _container->window_outline.modify_alpha( 80 * this->track_animation[ this->pop_id ] ), 2, 1 );

		_ext->make_text( this->base_window.x + 15, this->base_window.y + 41 + elem_height[ this->pop_id ], _container->window_text.modify_alpha( 150 ), evo::fonts_t::_default2, text.label.c_str( ), evo::font_flags_t::dropshadow_low );
		
		elem_height[ this->pop_id ] += 35;
		_container->set_id( _container->get_id( ) + 1 );
	}
}

void evo::popup_t::handle_dropdown( ) { }

void evo::popup_t::paint( ) { 
	elem_height[ this->pop_id ] = 0;

	auto anim_2 = animation_controller.get( std::to_string( _container->get_id( ) ) + "#color22picker_focused" + animation_controller.current_child );
	anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.5f ) * ( ( _container->anim_controler > 0.f ) && this->focused_element ? 1.f : -1.f ) );


	this->track_animation[ this->pop_id ] = anim_2.value;

	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha( 255 * _container->anim_controler ).blend(
								  _container->window_backround.darker( -5 ), anim_2.value
							  ), 2 );

	this->handle_checkbox( );
	this->handle_slider_int( );
	this->handle_slider_float( );
	this->handle_text( );
	
	// store.
	this->track_size[ this->pop_id ] = elem_height[ this->pop_id ];

	// element and stuff
	_render->add_rect_filled( this->base_window.x, this->base_window.y + 30, _container->group_width,
							  5 + elem_height[ this->pop_id ] * anim_2.value, _container->window_backround.modify_alpha( 255 * anim_2.value ), 2 );

	_render->add_rect_filled( this->base_window.x, this->base_window.y + 30, _container->group_width,
		1, _container->window_outline.modify_alpha( 255 * anim_2.value ), 2 );

	// other shit 
	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30 + ( elem_height[ this->pop_id ] + 5) *anim_2.value, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );

	_render->add_text( this->base_window.x + 10, this->base_window.y + 5,
					   _container->window_text.modify_alpha( 155 * _container->anim_controler ), 1, this->popup.c_str( ), 0 );

	_render->add_text( this->base_window.x + _container->group_width - 30, this->base_window.y + 6,
					   _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_arrows, this->focused_element ? "a" : "d", 0 );


}

void evo::popup_t::input( ) { 
	if ( _container->can_interact( ) && _input->mouse_in_box( { this->base_window.x, this->base_window.y }, { ( float )_container->group_width, 30 } )
		 && _input->key_pressed( VK_LBUTTON ) && !_container->base_handler[ 3 ] ) {
		state_color = !state_color;
		str_idx_color = _container->get_id( );
	}

	if ( str_idx_color == _container->get_id( ) ) {
		/* we can now use this shit */
		this->focused_element = state_color;
	}

	if ( str_idx_color == _container->get_id( ) && !_input->mouse_in_box( { this->base_window.x, this->base_window.y }, { (float)_container->group_width, 30 + (float)elem_height[ this->pop_id ] } )
		 && _input->key_pressed( VK_LBUTTON ) ) {
		state_color = false;
	}

	_container->base_handler[ 0 ] = state_color;
	_container->base_opened_state[ _container->base_handler_t::colorpicker ][ _container->get_id( ) ] = this->focused_element;

	this->hovered = _input->mouse_in_box( { this->base_window.x, this->base_window.y }, { ( float )_container->group_width, 30 + ( float )elem_height[ this->pop_id ] });
}

void evo::popup_t::bind_checkbox( std::string label, bool* value ) {
	this->checkbox_elems.push_back( elem_check( label, value ) );
}

void evo::popup_t::bind_slider_int( std::string label, int* value, int min, int max, std::string sufix ) { 
	this->elem_sliderint.push_back( elem_slider_int( label, value, min, max, sufix ) );
}

void evo::popup_t::bind_slider_float( std::string label, float* value, int min, int max, std::string sufix ) {
	this->elem_sliderfloat.push_back( elem_slider_float( label, value, min, max, sufix ) );
}

void evo::popup_t::bind_text( std::string label ) { 
	this->elem_texct.push_back( elem_text( label ) );
}

void evo::popup_t::bind_dropdown( std::string label, int* value, std::vector<std::string> items ) { 
	this->elem_drop.push_back( elem_dropdown( label, value, items ) );
}
