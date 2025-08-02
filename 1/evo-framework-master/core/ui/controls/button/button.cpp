#include "../../../inc.hpp"

static short int str_indx_button = -1;
static bool state_button = false;

static bool update_it[ 1000 ]{ false };

evo::button_t::button_t( std::string name, std::function<void( )> function ) { 
	this->name = name;
	this->function = function;
}

void evo::button_t::input( ) { 
	/* update key */
	if ( !_input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( { this->pos.x, this->pos.y },
																								   { (float)_container->group_width, 23 } ) ) {
		if ( state_button )
			this->function( );

		state_button = false;
	}

	/* check colorpicker */
	if ( !theme::colorpicker_is_opened && _container->can_interact( ) &&
		 _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( { this->pos.x, this->pos.y },
																								  { ( float )_container->group_width, 23 } ) ) {
		state_button = true;
		update_it[ _container->get_id( ) ] = true;

		str_indx_button = _container->get_id( );
	}

	if ( str_indx_button == _container->get_id( ) )
		this->focused_element = state_button;
}

void evo::button_t::paint( ) {
	auto base_window = this->pos;

	auto animation = animation_controller.get( this->name + "#buttonanim2" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.2f ) * ( update_it[ _container->get_id( ) ] && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	if ( animation.value >= 0.99 ) {
		/* disable animation */
		update_it[ _container->get_id( ) ] = false;
	}

	evo::_render->add_rect_filled_shadowed( base_window.x, base_window.y, _container->group_width, 20, _container->window_accent.modify_alpha( 255 * animation.value ), 2, 10 );
	evo::_render->add_rect_filled_shadowed( base_window.x, base_window.y, _container->group_width, 20, _container->window_outline.darker(100), 2, 10 );
	evo::_render->add_rect_filled( base_window.x, base_window.y, _container->group_width, 20, _container->window_backround, 2 );

	/* lets get the check name rendered */

	evo::_render->add_text( base_window.x + _container->group_width * 0.5 -
							_render->text_size( this->name.c_str(), evo::fonts_t::_default2 ).x * 0.5, base_window.y, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->name.c_str( ), evo::font_flags_t::dropshadow_low );
}
