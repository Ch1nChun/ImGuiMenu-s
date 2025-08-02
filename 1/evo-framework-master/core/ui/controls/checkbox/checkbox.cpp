#include "../../../inc.hpp"

/* checkbox constructor*/
evo::checkbox_t::checkbox_t( std::string label, bool* value, checkbox_along_t mode ) {
	/* label */
	this->label = label;

	this->type = mode;

	/* value */
	this->value = value;
}

void evo::checkbox_t::paint( ) { 
	/* animation */
	auto animation = animation_controller.get( this->label + "#active" + std::to_string( _container->get_id() ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( *this->value ? 1.f : -1.f ) );

	/* animation */
	auto animation_h = animation_controller.get( this->label + "#hoverr" + animation_controller.current_child );
	animation_h.adjust( animation_h.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( this->hovered ? 1.f : -1.f ) );

	/* checkmark lambda */
	auto draw_checkmark = [ ]( ImVec2 pos, evo::col_t col, float sz, float anim ) -> void {
		float thickness = std::fmax( sz / 5.0f, 1.0f );
		sz -= thickness * 0.5f;
		pos += ImVec2( thickness * 0.25f, thickness * 0.25f );

		float third = sz / 3.0f;
		float bx = pos.x + third;
		float by = pos.y + sz - third * 0.5f;
		draw_list->PathLineTo( ImVec2( bx - third * anim, by - third * anim ) );
		draw_list->PathLineTo( ImVec2( bx, by ) );
		draw_list->PathLineTo( ImVec2( bx + third * 2 * anim, by - third * 2 * anim ) );
		draw_list->PathStroke( col.convert( ), false, thickness );
	};

	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha( 255 * _container->anim_controler ), 2 );

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );

	_render->add_rect_filled( this->base_window.x + _container->group_width - 30, this->base_window.y + 7, 15, 15, _container->window_backround.darker( 5 ).blend( _container->window_accent, animation.value ), 2 );
	_render->add_rect_filled_shadowed( this->base_window.x + _container->group_width - 30, this->base_window.y + 7, 15, 15, _container->window_backround.darker( 5 ).blend( _container->window_accent, animation.value ), 2, 15 );

	/* draw checkmark */
	draw_checkmark( evo::macros::vec_t( this->base_window.x + _container->group_width - 30 + 2.5, this->base_window.y + 9 ), evo::col_t( 0, 0, 0, 255 * _container->anim_controler ), 10, animation.value );

	/* lets get the check name rendered */
	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ), evo::fonts_t::_default2,
							this->label.c_str( ) );
}

void evo::checkbox_t::input( ) { 
	/* input handler */
	if ( type == no ) {
		if ( evo::_input->mouse_in_box( evo::vec2_t( this->base_window.x, this->base_window.y ), evo::vec2_t( _container->group_width, 30 ) ) && evo::_input->key_pressed( VK_LBUTTON ) ) {
			*this->value = !*this->value;
		}
	} else if ( type == has_element ) {
		if ( evo::_input->mouse_in_box( evo::vec2_t( this->base_window.x + _container->group_width - 30, this->base_window.y ), evo::vec2_t( 17, 30 ) ) && evo::_input->key_pressed( VK_LBUTTON ) ) {
			*this->value = !*this->value;
		}
	}

	/* store hovered */
	this->hovered = evo::_input->mouse_in_box( evo::vec2_t( this->base_window.x, this->base_window.y ), evo::vec2_t( 17 + evo::_render->text_size( this->label.c_str( ), evo::fonts_t::_default2 ).x, 17 ) );
}
