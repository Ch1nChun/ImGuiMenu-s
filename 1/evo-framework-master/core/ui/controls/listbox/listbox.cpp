#include "../../../inc.hpp"

/* we are not going to do anything here since im lazy and i wont ever touch again this file */
static int scroll_pos_list[ 1000 ];
static bool scroll_state_list[ 1000 ];

inline float _blink;
inline static short int str_indx_textbar_list = -1;
inline static bool state_textbar_list = false;

evo::listbox_t::listbox_t( std::string name, int* value, std::vector<std::string>& items, std::string& filter ) {
	this->name = name;
	this->selected = value;
	this->items = &items;
	this->filter = &filter;
}

void evo::listbox_t::scroll( int limit ) {
	static size_t lost[ 1000 ];
	if ( lost[ _container->get_id( ) ] != this->temp.size( ) ) {
		scroll_pos_list[ _container->get_id( ) ] = 0;
		lost[ _container->get_id( ) ] = this->temp.size( );
	}

	bool hovered = _input->mouse_in_box( { this->pos.x, this->pos.y }, { ( float )( _container->group_width ), ( float )153 } );

	if ( !scroll_state_list[ _container->get_id( ) ] && this->temp.size( ) > 7 ) {
		float ratio = 7 / static_cast< float >( this->temp.size( ) );
		float some_shit = this->pos.y;

		float size_ratio = 7 / static_cast< float >( this->temp.size( ) );
		size_ratio *= ( float )( ( float )( _container->group_width ) );

		float height_delta = some_shit + size_ratio - ( ( float )( ( float )( _container->group_width ) ) );
		if ( height_delta > 0 )
			some_shit -= height_delta;

		float pos_ratio = some_shit / ( float )( ( float )( _container->group_width ) );
		float m_prikol = pos_ratio * this->temp.size( );

		if ( _input->get_mouse_wheel( ) != 0 && hovered ) {
			scroll_pos_list[ _container->get_id( ) ] += _input->get_mouse_wheel( ) * ( -1 );
			_input->set_mouse_wheel( 0 );

			if ( scroll_pos_list[ _container->get_id( ) ] < 0 )
				scroll_pos_list[ _container->get_id( ) ] = 0;

			if ( scroll_pos_list[ _container->get_id( ) ] > static_cast< int >( m_prikol ) )
				scroll_pos_list[ _container->get_id( ) ] = static_cast< int >( m_prikol );
		}

		evo::_render->add_rect_filled( this->pos.x - 10 + ( float )( _container->group_width ), 
									   this->pos.y + 28, 3, 140, _container->window_backround.darker( 5 ), 10 );

		evo::_render->add_rect_filled_shadowed( this->pos.x - 10 + ( float )( _container->group_width ),
									   this->pos.y + 28, 3, 140, _container->window_outline, 10, 5 );

		evo::_render->clip_rect( this->pos.x - 10 + ( float )( _container->group_width ),
								 this->pos.y + 28, 3, 140, [ & ]( ) {
									 evo::_render->add_rect_filled( this->pos.x - 10 + ( float )( _container->group_width ),
									 this->pos.y + 28 + scroll_pos_list[ _container->get_id( ) ], 3, 140 - m_prikol, _container->window_accent, 10 );
		} );

		evo::_render->clip_rect( this->pos.x - 20 + ( float )( _container->group_width ),
								 this->pos.y + 28, 20, 140, [ & ]( ) {
									 evo::_render->add_rect_filled_shadowed( this->pos.x - 10 + ( float )( _container->group_width ),
									 this->pos.y + 28 + scroll_pos_list[ _container->get_id( ) ], 3, 140 - m_prikol, _container->window_accent, 10, 10 );
		} );
	}
}

void evo::listbox_t::think( ) {
	/* animations */
	auto anim_1 = animation_controller.get( this->name + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	if ( !this->temp.empty( ) ) {
		int drawn_items = 0;
		int items_to_draw = 7;

		for ( int i{ scroll_pos_list[ _container->get_id( ) ] }; ( i < this->temp.size( ) && drawn_items < items_to_draw ); i++ ) {
			bool hovered = _input->mouse_in_box(
				{ this->pos.x + 7, this->pos.y + 24 + ( 22 * drawn_items ) },
				{ ( float )( _container->group_width ), 20 }
			);

			if ( hovered && _input->key_pressed( VK_LBUTTON ) && _container->can_interact( )
				 && !_container->base_handler[ 0 ] && !theme::colorpicker_is_opened )
				*this->selected = this->temp[ i ].second;

			/* animations */
			auto anim_2 = animation_controller.get( this->name + std::to_string( i ) + "sa#lut" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( ( _container->anim_controler > 0.f ) && ( hovered || *this->selected == this->temp[ i ].second ) ? 1.f : -1.f ) );

			auto base_window = this->pos;
			evo::_render->add_text( base_window.x + 7, base_window.y + 24 + ( 22 * drawn_items ), _container->window_text.blend( 
				_container->window_accent, anim_2.value
			).modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
									this->temp[ i ].first.c_str( ) );

			drawn_items++;
		}

		/* handle scrolling */
		this->scroll( 0 );
	}
}

void evo::listbox_t::think_filter_bar( ) {
	int text_size = evo::_render->text_size( this->name.c_str( ), evo::fonts_t::_default2 ).y;
	int text_width = evo::_render->text_size( this->name.c_str( ), evo::fonts_t::_default2 ).x;

	if ( state_textbar_list && str_indx_textbar_list == _container->get_id( ) && _input->key_pressed( VK_LBUTTON ) ) {
		state_textbar_list = !state_textbar_list;
		str_indx_textbar_list = -1;
	}

	if ( !theme::colorpicker_is_opened && _container->can_interact( ) && _input->key_pressed( VK_LBUTTON ) &&
		 _input->mouse_in_box( { this->pos.x, this->pos.y + text_size + 8 }, { ( float )( _container->group_width ), 20 } ) ) {
		state_textbar_list = !state_textbar_list;
		str_indx_textbar_list = _container->get_id( );
	}

	if ( str_indx_textbar_list == _container->get_id( ) )
		this->focused_bar = state_textbar_list;

	_container->base_opened_state[ _container->base_handler_t::textbox ][ _container->get_id( ) ] = this->focused_bar;

	if ( this->focused_bar ) {
		std::string str = *this->filter;

		for ( int i = 0; i < 255; i++ ) {
			if ( _input->key_pressed( i ) ) {
				if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT ) {
					str_indx_textbar_list = -1;
					return;
				}

				if ( strlen( str.c_str( ) ) != 0 ) {
					if ( GetAsyncKeyState( VK_BACK ) ) {
						*this->filter = str.substr( 0, strlen( str.c_str( ) ) - 1 );
					}
				}

				if ( strlen( str.c_str( ) ) < 27 && i != NULL && upper_case[ i ] != nullptr ) {
					if ( GetAsyncKeyState( VK_SHIFT ) ) {
						*this->filter = str + upper_case[ i ];
					} else {
						*this->filter = str + lower_case[ i ];
					}

					return;
				}

				if ( strlen( str.c_str( ) ) < 27 && i == 32 ) {
					*this->filter = str + " ";
					return;
				}
			}
		}
	}
}

void evo::listbox_t::paint( ) {
	/* paint down */
	/* lets get the check name rendered */
	auto base_window = this->pos;
	evo::_render->add_text( base_window.x, base_window.y - 1, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->name.c_str( ), evo::font_flags_t::dropshadow_low );


	/* some vars */
	int text_size = evo::_render->text_size( this->name.c_str( ), evo::fonts_t::_default2 ).y;
	int text_width = evo::_render->text_size( this->name.c_str( ), evo::fonts_t::_default2 ).x;
	int height = 153 + 23;

	evo::_render->add_rect_filled_shadowed( base_window.x, base_window.y + text_size + 8, _container->group_width, height, _container->window_outline.darker( 100 ), 2, 15 );
	evo::_render->add_rect_filled( base_window.x, base_window.y + text_size + 8, _container->group_width, height, _container->window_backround, 2 );

	evo::_render->add_rect_filled_shadowed( base_window.x, base_window.y + text_size + 8, _container->group_width, 20, _container->window_outline.darker( 100 ).modify_alpha( 100 * _container->anim_controler ), 2, 10 );
	evo::_render->add_rect_filled( base_window.x, base_window.y + text_size + 8, _container->group_width, 20, _container->window_backround, 2 );

	std::string to_draw{ *this->filter };
	if ( GetTickCount64( ) >= _blink )
		_blink = GetTickCount64( ) + 800;

	if ( this->focused_bar && GetTickCount64( ) > ( _blink - 400 ) )
		to_draw += "|";

	evo::_render->add_text( base_window.x + 5, base_window.y + text_size + 8, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							to_draw.c_str( ) );

	/* create space */
	this->pos.y += 23;

	/* we are going to init think in our paint function */
	this->think( );
}
