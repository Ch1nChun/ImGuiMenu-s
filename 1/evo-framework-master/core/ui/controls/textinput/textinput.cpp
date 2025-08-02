#include "../../../inc.hpp"

float _blink2;

static short int str_indx_textbar_list2 = -1;
static bool state_textbar_list2 = false;

evo::textbox_t::textbox_t( std::string label, std::string* value ) { 
	this->label = label;
	this->value = value;
}

void evo::textbox_t::input( ) { 
	if ( state_textbar_list2 && str_indx_textbar_list2 == _container->get_id( ) && _input->key_pressed( VK_LBUTTON ) ) {
		state_textbar_list2 = !state_textbar_list2;
		str_indx_textbar_list2 = -1;
	}

	if ( !theme::colorpicker_is_opened && _container->can_interact( ) && _input->key_pressed( VK_LBUTTON ) &&
		_input->mouse_in_box( { this->base_window.x, this->base_window.y }, { ( float )( _container->group_width ), 30 } ) ) {
		state_textbar_list2 = !state_textbar_list2;
		str_indx_textbar_list2 = _container->get_id( );
	}

	if ( str_indx_textbar_list2 == _container->get_id( ) )
		this->focused = state_textbar_list2;

	_container->base_opened_state[ _container->base_handler_t::textbox ][ _container->get_id( ) ] = this->focused;

	if ( this->focused ) {
		std::string str = *this->value;

		for ( int i = 0; i < 255; i++ ) {
			if ( _input->key_pressed( i ) ) {
				if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT ) {
					str_indx_textbar_list2 = -1;
					return;
				}

				if ( strlen( str.c_str( ) ) != 0 ) {
					if ( GetAsyncKeyState( VK_BACK ) ) {
						*this->value = str.substr( 0, strlen( str.c_str( ) ) - 1 );
					}
				}

				if ( strlen( str.c_str( ) ) < 27 && i != NULL && upper_case[ i ] != nullptr ) {
					if ( GetAsyncKeyState( VK_SHIFT ) ) {
						*this->value = str + upper_case[ i ];
					} else {
						*this->value = str + lower_case[ i ];
					}

					return;
				}

				if ( strlen( str.c_str( ) ) < 27 && i == 32 ) {
					*this->value = str + " ";
					return;
				}
			}
		}
	}
}

void evo::textbox_t::paint( ) { 
	auto animation = animation_controller.get( this->label + "#makeacc22861" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.2f ) * ( this->focused && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	this->grab_animation_data[ _container->get_id( ) ] = animation.value;

	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30 + ( 25 * animation.value ), _container->window_backround.modify_alpha( 255 * _container->anim_controler ), 2 );

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30 + ( 25 * animation.value ), _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );

	_render->add_rect_filled( this->base_window.x, this->base_window.y + 30, _container->group_width,
							  1, _container->window_outline.modify_alpha( 255 * animation.value ), 2 );


	/* lets get the check name rendered */
	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->label.c_str( ) );

	std::string to_draw{ *this->value };
	if ( GetTickCount64( ) >= _blink2 )
		_blink2 = GetTickCount64( ) + 800;
	
	if ( this->focused && GetTickCount64( ) > ( _blink2 - 400 ) )
		to_draw += "|";
	
	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 32 * animation.value, _container->window_text.modify_alpha( 155 * animation.value ), evo::fonts_t::_default2,
							to_draw.c_str( ), evo::font_flags_t::dropshadow_low );
}
