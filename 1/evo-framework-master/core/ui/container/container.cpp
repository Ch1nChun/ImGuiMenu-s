#include "../../inc.hpp"

void evo::container_t::initialize_container( ) {
	static bool open{ false };

	if ( evo::_input->key_pressed( VK_INSERT ) )
		open = !open;

	/* run animation */
	auto animation = animation_controller.get( "animation_open" + std::to_string( 0 ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.5f ) * ( open ? 1.f : -1.f ) );

	this->window_backround = evo::col_t( 23, 23, 23 ).modify_alpha( animation.value * 255 );
	this->window_outline = evo::col_t( 55, 52, 55 ).modify_alpha( animation.value * 255 );
	this->window_title_bar = evo::col_t( 29, 29, 29 ).darker( -2 ).modify_alpha( animation.value * 255 );
	this->window_title_bar_second = evo::col_t( ).hex_to_rgb( "#FF2D3240" ).modify_alpha( animation.value * 255 );
	this->window_outline_second = evo::col_t( ).hex_to_rgb( "#FF313544" ).modify_alpha( animation.value * 255 );
	this->window_text = evo::col_t( ).modify_alpha( animation.value * 200 );

	this->window_accent = evo::col_t( 255, 112, 112 ).modify_alpha( animation.value * 200 );

	this->child_backround = evo::col_t( 24, 24, 24 ).modify_alpha( animation.value * 255 );
	this->elem_back = evo::col_t( ).hex_to_rgb( "#FF353A48" ).modify_alpha( animation.value * 255 );

	/* store this motherfucker */
	this->anim_controler = animation.value;
}

int evo::container_t::get_id( ) {
	return this->base_element_id;
}

bool evo::container_t::can_interact( ) {
	return 
		this->base_opened[ base_handler_t::combo ] == -1 &&
		this->base_opened[ base_handler_t::multicombo ] == -1 &&
		this->base_opened[ base_handler_t::bind ] == -1;
}

void evo::container_t::set_id( const uintptr_t last ) { 
	this->base_element_id = last;
}
