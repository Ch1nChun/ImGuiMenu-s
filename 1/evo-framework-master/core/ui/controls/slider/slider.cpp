#include "../../../inc.hpp"

evo::slider_float_t::slider_float_t( std::string label, float* value, float min, float max, std::string suffix ) {
	this->label = label;
	this->value = value;
	this->min = min;
	this->max = max;
	this->suffix = suffix;
}

void evo::slider_float_t::paint( ) { 
	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha(255 * _container->anim_controler), 2 );

	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ),
							evo::fonts_t::_default2, this->label.c_str( ) );

	auto animation = animation_controller.get( this->label + "#makebigger" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.2f ) * ( this->hovered || this->in_use ? 1.f : -1.f ) );

	int _height{ 2 };
	int _added_height{};

	/* slider value */
	auto min_delta = *this->value - this->min;
	auto delta = this->max - this->min;
	auto total = ( min_delta / delta ) * ( _container->group_width );
	total = std::fmin( total, ( _container->group_width  ) );

	if ( total > this->min ) {

		evo::_render->clip_rect( this->base_window.x, this->base_window.y, _container->group_width,
								 30, [ & ]( ) {
		evo::_render->add_rect_filled( this->base_window.x + 1, this->base_window.y + 28 - _height, total - 2, _height, _container->window_accent.modify_alpha( 255 * animation.value ), 2 );
		evo::_render->add_rect_filled_shadowed( this->base_window.x + 1, this->base_window.y + 28 - _height, total - 2, _height, _container->window_accent.modify_alpha( 255 * animation.value ), 2, 10 );
								 } );
	}

	std::string full_txt{};
	full_txt = precision( *this->value, 1 ) + this->suffix;

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );


	evo::_render->add_text( this->base_window.x + _container->group_width - 15 - _render->text_size( full_txt.c_str(), 1 ).x, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ),
							evo::fonts_t::_default2, full_txt.c_str( ) );
}

void evo::slider_float_t::input( ) { 
	auto x = _container->group_width;
	auto delta = this->max - this->min;
	static auto str_index = -1;

	int text_size = evo::_render->text_size( this->label.c_str( ), evo::fonts_t::_default2 ).y;
	this->hovered = evo::_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y ), vec2_t( _container->group_width, 30 ) );

	if ( _container->can_interact( ) && !theme::colorpicker_is_opened ) {
		if ( _input->key_pressed( VK_LBUTTON ) & 1 && this->hovered ) {
			str_index = _container->get_id( );
		}
	}

	if ( GetAsyncKeyState( VK_LBUTTON ) && str_index == _container->get_id( ) ) {
		this->in_use = true;

		float normalized_pos = ( _input->get_mouse_position( ).x - this->base_window.x ) / ( _container->group_width );
		float target_value = this->min + delta * normalized_pos;

		*this->value = animation_controller.lerp_single( *this->value, target_value, 0.2f );

		/* gheto clamping */
		if ( *this->value < this->min )
			*this->value = this->min;

		if ( *this->value > this->max )
			*this->value = this->max;
	}

	/* update index */
	if ( !GetAsyncKeyState( VK_LBUTTON ) ) {
		this->in_use = false;
		str_index = -1;
	}
}

evo::slider_int_t::slider_int_t( std::string label, int* value, int min, int max, std::string suffix ) {
	this->label = label;
	this->value = value;
	this->min = min;
	this->max = max;
	this->suffix = suffix;
}

void evo::slider_int_t::paint( ) { 
	_render->add_rect_filled( this->base_window.x, this->base_window.y, _container->group_width,
							  30, _container->window_backround.modify_alpha( 255 * _container->anim_controler ), 2 );

	evo::_render->add_text( this->base_window.x + 10, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ),
							evo::fonts_t::_default2, this->label.c_str( ) );

	auto animation = animation_controller.get( this->label + "#makebigger" + std::to_string( _container->get_id( ) ) + animation_controller.current_child );
	animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.2f ) * ( this->hovered || this->in_use ? 1.f : -1.f ) );

	int _height{ 2 };
	int _added_height{};

	/* slider value */
	auto min_delta = *this->value - this->min;
	auto delta = static_cast< float >( this->max - this->min );
	auto total = ( min_delta / delta ) * ( _container->group_width );
	total = std::fmin( total, ( _container->group_width ) );

	if ( total > this->min ) {

		evo::_render->clip_rect( this->base_window.x, this->base_window.y, _container->group_width,
								 30, [ & ]( ) {
									 evo::_render->add_rect_filled( this->base_window.x + 1, this->base_window.y + 28 - _height, total - 2, _height, _container->window_accent.modify_alpha( 255 * animation.value ), 2 );
									 evo::_render->add_rect_filled_shadowed( this->base_window.x + 1, this->base_window.y + 28 - _height, total - 2, _height, _container->window_accent.modify_alpha( 255 * animation.value ), 2, 10 );
								 } );
	}

	std::string full_txt{};
	full_txt = std::to_string( *this->value ) + this->suffix;

	_render->add_rect( this->base_window.x, this->base_window.y, _container->group_width,
					   30, _container->window_outline.modify_alpha( 80 * _container->anim_controler ), 2, 1 );


	evo::_render->add_text( this->base_window.x + _container->group_width - 15 - _render->text_size( full_txt.c_str( ), 1 ).x, this->base_window.y + 5, _container->window_text.modify_alpha( 155 * _container->anim_controler ),
							evo::fonts_t::_default2, full_txt.c_str( ) );
}

void evo::slider_int_t::input( ) { 
	auto x = _container->group_width;
	auto delta = this->max - this->min;
	static auto str_index = -1;

	int text_size = evo::_render->text_size( this->label.c_str( ), evo::fonts_t::_default2 ).y;
	this->hovered = evo::_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y ), vec2_t( _container->group_width, 30 ) );

	if ( _container->can_interact( ) && !theme::colorpicker_is_opened ) {
		if ( _input->key_pressed( VK_LBUTTON ) & 1 && this->hovered ) {
			str_index = _container->get_id( );
		}
	}

	if ( GetAsyncKeyState( VK_LBUTTON ) && str_index == _container->get_id( ) ) {
		this->in_use = true;

		float normalized_pos = ( _input->get_mouse_position( ).x - this->base_window.x ) / ( _container->group_width );
		float target_value = this->min + delta * normalized_pos;

		*this->value = animation_controller.lerp_single( *this->value, target_value, this->max < 30 ? 1.f : 0.2f );

		std::cout << std::to_string( *this->value ) + " v\n";

		/* gheto clamping */
		if ( *this->value < this->min )
			*this->value = this->min;

		if ( *this->value > this->max )
			*this->value = this->max;
	}

	/* update index */
	if ( !GetAsyncKeyState( VK_LBUTTON ) ) {
		this->in_use = false;
		str_index = -1;
	}
}
