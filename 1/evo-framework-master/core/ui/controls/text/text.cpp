#include "../../../inc.hpp"

evo::text_t::text_t( std::string label ) {
	this->label = label;
}

void evo::text_t::paint( ) {
	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha( 255 * _container->anim_controler ), 2 );

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );


	/* lets get the check name rendered */
	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->label.c_str( ) );

}
