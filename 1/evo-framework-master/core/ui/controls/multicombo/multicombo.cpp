#include "../../../inc.hpp"

inline static auto str_index_dropdown = -1;
inline static bool state_dropdown = false;
inline static int time_dropdown;

evo::multi_dropdown_t::multi_dropdown_t( std::string label ) {
	this->label = label;
}

void evo::multi_dropdown_t::paint( ) { 
	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha( 255 * _container->anim_controler ), 2 );

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );

	auto animation = animation_controller.get( this->label + "#animgov22ertes" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->hovered || this->focused && ( _container->anim_controler > 0.f ) ? 0.5f : -1.f ) );

	auto animation2 = animation_controller.get( this->label + "#focusem22anim" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation2.adjust( animation2.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( this->focused && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	std::string to_display;
	for ( int i = 0; i < items.size( ); i++ ) {
		if ( *items[ i ].value ) {
			if ( to_display != "" )
				to_display.append( ", " );

			to_display.append( items[ i ].name );
		}
	}

	if ( to_display == "" )
		to_display.append( "None" );

	auto text_size_all = _render->text_size( to_display.c_str( ), evo::fonts_t::_default2 );
	if ( base_window.x + 2 + text_size_all.x > base_window.x + 200 ) {
		to_display.resize( 10 );
		to_display.append( "..." );
	}

	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ).blend( _container->window_accent, animation.value ), evo::fonts_t::_default2,
							animation.value > 0 ? to_display.c_str( ) : this->label.c_str( ) );

	evo::_render->add_text( this->base_window.x + _container->group_width - 23, this->base_window.y + 5, _container->window_text.blend( _container->window_accent, animation.value ).modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->focused ? "-" : "+" );

	if ( animation2.value > 0.f ) {
		_ext->make_rect_filled( this->base_window.x, this->base_window.y + 32, _container->group_width, ( ( this->items.size( ) * 22 ) + 6 ) * animation2.value, _container->window_backround.modify_alpha( 255 * animation2.value ), 2 );
		_ext->make_rect( this->base_window.x, this->base_window.y + 32, _container->group_width, ( ( this->items.size( ) * 22 ) + 6 ) * animation2.value, _container->window_outline.modify_alpha( 80 * animation2.value ), 2, true );

		for ( int i = 0; i < this->items.size( ); i++ ) {
			if ( _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( vec2_t( this->base_window.x + 15, this->base_window.y + 32 + ( i * 22 ) ), vec2_t( _container->group_width - 10, ( float )20 ) ) ) {
				//*value = i;
				//state_dropdown = false;
				//str_index_dropdown = -1;

				*this->items[ i ].value = !*this->items[ i ].value;
			}

			auto animation_2 = animation_controller.get( std::to_string( i ) + "#chltext" + animation_controller.current_child );
			animation_2.adjust( animation_2.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( *this->items[ i ].value && state_dropdown ? 1.f : -1.f ) );

			evo::_ext->make_text( this->base_window.x + 10, this->base_window.y + 35 + ( i * 22 ), _container->window_text.blend( _container->window_accent, animation_2.value ).modify_alpha( 170 * animation.value ), evo::fonts_t::_default2,
								  this->items[ i ].name.c_str( ) );
		}

	}
}

void evo::multi_dropdown_t::input( ) { 
	int text_size = evo::_render->text_size( this->label.c_str( ), evo::fonts_t::_default2 ).y;
	this->hovered = evo::_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y + text_size + 8 ), vec2_t( _container->group_width, 20 ) );

	if ( str_index_dropdown == _container->get_id( ) && _input->key_pressed( VK_LBUTTON )
		 && !_input->mouse_in_box( this->base_window + evo::vec2_t( 0, 32 ), { ( float )_container->group_width - 2, float( this->items.size( ) ) * 23 } ) ) {
		state_dropdown = !state_dropdown;
		str_index_dropdown = -1;

		std::cout << "2\n";
	}

	if ( !_container->base_handler[ 0 ] && !theme::colorpicker_is_opened && _container->base_opened[ _container->base_handler_t::combo ] == -1 && time_dropdown == -1 &&
		 _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( this->base_window, { ( float )_container->group_width - 2, 30 } ) ) {
		state_dropdown = !state_dropdown;
		str_index_dropdown = _container->get_id( );

		std::cout << "1\n";
	}

	/* we are going to setup the main animation and time */
	if ( time_dropdown >= 12 )
		time_dropdown = 12;
	else if ( time_dropdown <= 0 )
		time_dropdown = 0;

	state_dropdown ? time_dropdown++ : time_dropdown--;

	/* other checks for sanity */
	_container->base_opened[ _container->base_handler_t::multicombo ] = time_dropdown;

	/* check for stored index and setup trought state */
	if ( str_index_dropdown == _container->get_id( ) )
		this->focused = state_dropdown;

	/* check the focus and setup it */
	_container->base_opened_state[ _container->base_handler_t::multicombo ][ _container->get_id( ) ] = this->focused;
}

void evo::multi_dropdown_t::insert_item( std::string label, bool* value ) { 
	this->items.push_back( multi_dropdown_system{ label, value } );
}
