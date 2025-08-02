#include "../../../inc.hpp"

/* main stuff for sliding*/
static int slide[ 1000 ];
static bool slide_moved[ 1000 ];
static float slide_offset[ 1000 ];

static bool is_dropped[ 1000 ];

static auto _stored_index = -1;
static bool _state = false;
static int _time;

/* construcotr */
evo::child_t::child_t( std::string child_title, int child_id, child_pos_t position, child_size_t size, int tab ) { 
	/* child title */
	this->child_title = child_title;

	/* child id 
		note:
			* we use that to ensure properly working animations , per group / we are
				not using shared memory so our animations are linked to this id
	*/
	this->child_id = child_id;

	/* child position */
	this->position = position;

	/* child size */
	this->size = size;

	/* tabs */
	this->tab = tab;
}

/* drop child */
evo::child_t::child_t( std::vector<std::string> child_title, int child_id, child_pos_t position, child_size_t size, int tab, int* subtab ) { 
	/* child title */
	this->child_title_vec = child_title;

	/* child id
		note:
			* we use that to ensure properly working animations , per group / we are
				not using shared memory so our animations are linked to this id
	*/
	this->child_id = child_id;

	/* child position */
	this->position = position;

	/* child size */
	this->size = size;

	/* tabs */
	this->tab = tab;

	/* subtab */
	this->selected_subtab = subtab;

	/* is it dropped */
	is_dropped[ child_id ] = true;
}

/*
	* construct the entire child system
*/
void evo::child_t::construct_child( ) {
	/* size and position */
	this->construct_position( );
	this->construct_size( );

	/* construct element offsets and stuff */
	this->base_element.x = this->base_window.x + 15;
	this->base_element.y[ this->child_id ] = this->base_window.y + 42 + slide[ this->child_id ];

	/* setup slide offset */
	slide_offset[ this->child_id ] = 0.f;

	/* hoverint setup */
	this->base_hovered = evo::_input->mouse_in_box( evo::vec2_t( this->base_window.x, this->base_window.y ),
													evo::vec2_t( this->base_size.x, this->base_size.y ) );

	/* store this */
	_container->group_width = this->base_size.x - 30;

	/* paint window */
	this->paint( );
}

void evo::child_t::construct_position( ) {
	/* x */
	int original_ = theme::orx + ( theme::rx - theme::orx );
	int original_x2 = ( theme::orx + ( theme::rx - theme::orx ) ) / 2;

	/* y */
	int y_original_ = ( theme::ory + ( theme::ry - theme::ory ) ) - 84;
	int y_original_x2 = ( ( theme::ory + ( theme::ry - theme::ory ) ) - 75 ) / 2;

	int base_x = this->base_full[ this->child_id ] ? original_ - 30 : original_x2 - 20;

	if ( this->position == child_pos_t::_top_left ) {
		this->base_window = { this->base_window.x + 15, this->base_window.y + 42 };
	} else if ( this->position == child_pos_t::_top_right ) {
		this->base_window = { this->base_window.x + 15 + base_x + 10, this->base_window.y + 42 };
	} else if ( this->position == child_pos_t::_bottom_left ) {
		this->base_window = { this->base_window.x + 15, this->base_window.y + 42 + y_original_x2 };
	} else if ( this->position == child_pos_t::_botton_right ) {
		this->base_window = { this->base_window.x + 15 + base_x + 10, this->base_window.y + 42 + y_original_x2 };
	}
}

/* construct size*/
void evo::child_t::construct_size( ) { 
	/* x */
	int original_ = theme::orx + ( theme::rx - theme::orx );
	int original_x2 = ( theme::orx + ( theme::rx - theme::orx ) ) / 2;

	/* y */
	int y_original_ = ( theme::ory + ( theme::ry - theme::ory ) ) - 84;
	int y_original_x2 = ( ( theme::ory + ( theme::ry - theme::ory ) ) - 93 ) / 2;

	int base_x = this->base_full[ this->child_id ] ? original_ - 30 : original_x2 - 20;

	if ( this->size == child_size_t::_half ) {
		this->base_size = { (float)base_x, (float)y_original_x2 };
	} else if ( this->size == child_size_t::_full ) {
		this->base_size = { ( float )base_x, ( float )y_original_ };
	} else if ( this->size == child_size_t::_custom ) {
		if ( this->base_custom_size_x[ this->child_id ] ) {
			this->base_size.x = this->base_id_size[ this->child_id ].x;
		} else {
			this->base_size.x = ( float )base_x;
		}

		if ( this->base_custom_size_y[ this->child_id ] ) {
			this->base_size.y = this->base_id_size[ this->child_id ].y;
		} else {
			this->base_size.y = ( float )y_original_;
		}
	}
}

/*
	* painting
*/
void evo::child_t::paint( ) { 
	evo::_render->add_rect_filled_shadowed( this->base_window.x, this->base_window.y, base_size.x, base_size.y, _container->window_outline.darker( 100 ).modify_alpha( 150 * _container->anim_controler ), 4, 20 );
	evo::_render->add_rect_filled( this->base_window.x, this->base_window.y, base_size.x, base_size.y, _container->child_backround.darker( -5 ), 4 );

	evo::_render->add_rect_filled( this->base_window.x, this->base_window.y, base_size.x, 30, _container->window_title_bar.darker( -5 ), 4 );


	//
	//evo::_render->add_rect_filled_shadowed( this->base_window.x, this->base_window.y, base_size.x, 30, _container->window_outline, 4, 15 );
	//evo::_render->add_rect_filled( this->base_window.x, this->base_window.y, base_size.x, 30, _container->child_backround.darker( -6 ), 4 );
	//
	evo::_render->add_text( this->base_window.x + 7, this->base_window.y + 5, _container->window_text, 1, this->child_title.c_str( ) );

	if ( is_dropped[ this->child_id ] ) {
		/* input system */
		{
			if ( _state && _stored_index == this->child_id && _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y + 30 ), 
																														 vec2_t( this->base_size.x, this->child_title_vec.size( ) * 23 ) ) ) {
				_state = !_state;
				//_stored_index = -1;
			}

			if ( _state && _stored_index == this->child_id && _input->key_pressed( VK_LBUTTON ) && !_input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y ),
																														  vec2_t( this->base_size.x, ( this->child_title_vec.size( ) * 23 ) + 30 ) ) ) {
				_state = false;
				//_stored_index = -1;
			}

			// _container->base_opened[ _container->base_handler_t::child_dropped ] == -1 &&
			if ( _time == -1 && _input->key_pressed( VK_LBUTTON ) &&
				 _input->mouse_in_box( vec2_t( this->base_window.x, this->base_window.y ), vec2_t( this->base_size.x, (float)30 ) ) ) {
				_state = !_state;
				_stored_index = this->child_id;
			}

			/* we are going to setup the main animation and time */
			if ( _time >= 12 )
				_time = 12;
			else if ( _time <= 0 )
				_time = 0;

			/* we are going to setup the open state */
			if ( _state )
				_time++;
			else
				_time--;

			_container->base_opened[ _container->base_handler_t::child_dropped ] = _time;


			/* check for stored index and setup trought state */
			if ( _stored_index == this->child_id )
				this->focused_child = _state;

			/* check the focus and setup it */
			_container->base_opened_state[ _container->base_handler_t::child_dropped ][ this->child_id ] = this->focused_child;
		}

		/* animation */
		auto animation = animation_controller.get( std::to_string( this->child_id ) + "#childboss" + animation_controller.current_child );
		animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( this->focused_child && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

		/* store this */
		this->anim_child_1 = animation.value;

		/* draw external */
		if ( animation.value > 0.f ) {
			evo::_ext->make_rect_filled_shadow( this->base_window.x + 5, this->base_window.y + 35, this->base_size.x - 10,
												( this->child_title_vec.size( ) * 25 ) * animation.value, _container->window_outline.darker( 100 ).modify_alpha( 150 * animation.value ), 5, 15 );

			evo::_ext->make_rect_filled( this->base_window.x + 5, this->base_window.y + 35, this->base_size.x - 10, 
										 ( this->child_title_vec.size( ) * 25 ) * animation.value, _container->child_backround.darker( -5 ), 5 );

			for ( int i = 0; i < this->child_title_vec.size( ); i++ ) {
				if ( _input->key_pressed( VK_LBUTTON ) && _input->mouse_in_box( vec2_t( this->base_window.x + 15, this->base_window.y + 40 + ( i * 23 ) ), vec2_t( this->base_size.x - 10, ( float )20 ) ) ) {
					*selected_subtab = i;
					_state = false;
					//_stored_index = -1; see it later 
				}

				/* animation */
				auto animation_2 = animation_controller.get( std::to_string( i ) + "#chltext" + animation_controller.current_child );
				animation_2.adjust( animation_2.value + 3.f * animation_controller.get_min_deltatime( 0.4f ) * ( *this->selected_subtab == i && _state ? 1.f : -1.f ) );

				evo::_ext->make_text( this->base_window.x + 15, this->base_window.y + 40 + ( i * 23 ), _container->window_text.blend( _container->window_accent, animation_2.value ).modify_alpha( 170 * animation.value ), evo::fonts_t::_default2,
									  this->child_title_vec[ i ].c_str( ) );
			}
		}

		/* drawing */
		evo::_render->add_text( this->base_window.x + this->base_size.x - 30, this->base_window.y + 5, _container->window_text, evo::fonts_t::_arrows, this->focused_child ? "a" : "d" );

		/* value selected */
		evo::_render->add_text( this->base_window.x + 5, this->base_window.y + 5, _container->window_text, evo::fonts_t::_default2, this->child_title_vec[ *this->selected_subtab ].c_str( ) );
	}
}

/* set position to base window */
void evo::child_t::set_position( evo::vec2_t position ) { 
	this->base_window += position;
}

/* get base window position */
evo::vec2_t evo::child_t::get_position( ) {
	return this->base_window;
}

/* set group visible */
void evo::child_t::set_visible( bool visible ) { 
	this->base_visible = visible;
}

/* get group visible */
bool evo::child_t::get_visible( ) {
	return this->base_visible;
}

/* limit drawing so elements wont draw out of our child */
void evo::child_t::limit_drawing( ) {
	draw_list->PushClipRect( evo::macros::vec_t( this->base_window.x, this->base_window.y + 30 ),
							 evo::macros::vec_t( this->base_window.x + this->base_size.x, 
												 this->base_window.y + this->base_size.y ), true );
}

/* pop clip rect / restart drawing */
void evo::child_t::restart_drawing( ) { 
	draw_list->PopClipRect( );
}

bool evo::child_t::hovered( ) { 
	return this->base_hovered && this->anim_child_1 <= 0.f;
}

/* make child have full width */
void evo::child_t::full_child( ) { 
	this->base_full[ this->child_id ] = true;
}

/* enable custom size */
void evo::child_t::enable_custom_size( child_flags_t flags ) {
	if ( flags == child_flags_t::_custom_x ) {
		this->base_custom_size_x[ this->child_id ] = true;
	}

	if ( flags == child_flags_t::_custom_y ) {
		this->base_custom_size_y[ this->child_id ] = true;
	}
}

void evo::child_t::set_size( evo::vec2_t size ) { 
	if ( this->base_custom_size_x[ this->child_id ] ) {
		this->base_id_size[ this->child_id ].x = size.x;
	}

	if ( this->base_custom_size_y[ this->child_id ] ) {
		this->base_id_size[ this->child_id ].y = size.y;
	}
}

void evo::child_t::obj( checkbox_t* checkbox ) { 
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		checkbox->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		if ( this->hovered( ) ) {
			/*
				* update checkbox if we hover the child
			*/
			checkbox->input( );
		}

		/* paint checkbox */
		checkbox->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete checkbox;
}

void evo::child_t::obj( slider_float_t* slider ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		slider->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		if ( this->hovered( ) ) {
			/*
				* update checkbox if we hover the child
			*/
			slider->input( );
		}

		/* paint checkbox */
		slider->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete slider;
}

void evo::child_t::obj( slider_int_t* slider ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		slider->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		if ( this->hovered( ) ) {
			/*
				* update checkbox if we hover the child
			*/
			slider->input( );
		}

		/* paint checkbox */
		slider->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete slider;
}

void evo::child_t::obj( combo_t* combo ) { 
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* update checkbox position */
		combo->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::combo ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				combo->input( );
			}
		} else if ( _container->base_opened_state[ _container->base_handler_t::combo ][ _container->get_id( ) ] ) {
			combo->input( );
		}

		/* do some sanity checks after setting viewport */
		if ( !_container->base_opened_state[ _container->base_handler_t::combo ][ _container->get_id( ) ] )
			this->limit_drawing( ); /* set the limit of drawing even when the open state is not true */

		/* paint checkbox */
		combo->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete combo;
}

void evo::child_t::obj( multi_dropdown_t* combo ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* update checkbox position */
		combo->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::multicombo ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				combo->input( );
			}
		} else if ( _container->base_opened_state[ _container->base_handler_t::multicombo ][ _container->get_id( ) ] ) {
			combo->input( );
		}

		/* do some sanity checks after setting viewport */
		if ( !_container->base_opened_state[ _container->base_handler_t::multicombo ][ _container->get_id( ) ] )
			this->limit_drawing( ); /* set the limit of drawing even when the open state is not true */

		/* paint checkbox */
		combo->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete combo;
}

void evo::child_t::obj( colorpicker_t* colorpicker ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		this->limit_drawing( ); /* set the limit of drawing even when the open state is not true */

		/* update checkbox position */
		colorpicker->base_window = {
			// - 30 + 15 + 10
			this->base_element.x + this->base_size.x - 80, this->base_element.y[ this->child_id ] - 28
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::colorpicker ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				colorpicker->input( );
			}
		} else {
			colorpicker->input( );
		}

		/* paint checkbox */
		colorpicker->paint( );

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete colorpicker;
}

void evo::child_t::obj( keybind_t* keybind ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		this->limit_drawing( ); /* set the limit of drawing even when the open state is not true */

		// to do: bind name stored ... etc
		auto text_s = _render->text_size( bind_name[ _container->get_id( ) ].c_str( ), evo::fonts_t::_default2 );

		/* update checkbox position */
		keybind->base_window = {
			this->base_element.x + ( this->base_size.x - text_s.x ) - 55, this->base_element.y[ this->child_id ] - 28
		};

		/* run input */
		if ( this->hovered( ) ) {
			keybind->input( );
		}

		/* paint normal */
		keybind->paint( );

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::bind ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) { /* run input list */
				keybind->input_list( );
			}
		} else {
			keybind->input_list( ); /* run input list */
		}

		/* paint list */
		keybind->paint_list( );

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete keybind;
}

void evo::child_t::obj( text_t* text ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		text->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};
	
		/* paint checkbox */
		text->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35;
		slide_offset[ this->child_id ] += 35;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete text;
}

void evo::child_t::obj( textbox_t* text ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		text->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::textbox ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				text->input( );
			}
		} else {
			text->input( );
		}

		/* paint checkbox */
		text->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 35 + 25 * text->grab_animation_data[ _container->get_id() ];
		slide_offset[ this->child_id ] += 35 + 25 * text->grab_animation_data[ _container->get_id( ) ];

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete text;
}

void evo::child_t::obj( listbox_t* listbox ) { 
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		listbox->pos = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::textbox ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				listbox->think_filter_bar( );
			}
		} else {
			listbox->think_filter_bar( );
		}

		/* paint checkbox */
		listbox->update_item( );
		listbox->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 153 + 57; // x + 23
		slide_offset[ this->child_id ] += 153 + 57;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete listbox;
}

void evo::child_t::obj( button_t* button ) { 
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		button->pos = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( this->hovered( ) ) {
			button->input( );
		}

		/* paint checkbox */
		button->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 33;
		slide_offset[ this->child_id ] += 33;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete button;
}


void evo::child_t::obj( esp_builder_t* esp_builder ) {
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		/* do not draw out of child */
		this->limit_drawing( );

		/* update checkbox position */
		esp_builder->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( this->hovered( ) ) {
			esp_builder->input( );
		}

		/* paint checkbox */
		esp_builder->paint( );

		/* update element spacing */
		this->base_element.y[ this->child_id ] += 33;
		slide_offset[ this->child_id ] += 33;

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete esp_builder;
}

void evo::child_t::obj( popup_t* popup ) { 
	if ( get_visible( ) ) { /* draw only if the visible flag is on */
		this->limit_drawing( ); /* set the limit of drawing even when the open state is not true */

		/* update checkbox position */
		popup->base_window = {
			this->base_element.x, this->base_element.y[ this->child_id ]
		};

		/* we want to update the object if we hover the group */
		if ( !_container->base_opened_state[ _container->base_handler_t::colorpicker ][ _container->get_id( ) ] ) {
			if ( this->hovered( ) ) {
				popup->input( );
			}
		} else {
			popup->input( );
		}

		/* paint checkbox */
		popup->paint( );

		this->base_element.y[ this->child_id ] += 35 + ( ( popup->track_size[ popup->pop_id ] + 5) * popup->track_animation[ popup->pop_id ] );
		slide_offset[ this->child_id ] += 35 + ( ( popup->track_size[ popup->pop_id ] + 5 ) * popup->track_animation[ popup->pop_id ] );

		/* assign to this object a new index */
		_container->set_id( _container->get_id( ) + 1 );

		/* restart drawing, we touched the point */
		this->restart_drawing( );
	} delete popup;
}

void evo::child_t::make_checkbox( std::string label, bool* value, checkbox_along_t along ) { 
	this->obj( new checkbox_t( label, value, along ) );
}

// bettwer way.
void evo::child_t::make_checkbox_colorpicker( std::string label, bool* value, evo::col_t* color, float* hue ) { 
	this->obj( new checkbox_t( label, value, checkbox_along_t::has_element ) );
	this->obj( new colorpicker_t( color, hue ) );
}

void evo::child_t::make_slider_float( std::string label, float* value, float min, float max, std::string suffix ) {
	this->obj( new slider_float_t( label, value, min, max, suffix ) );
}

void evo::child_t::make_slider_int( std::string label, int* value, int min, int max, std::string suffix ) {
	this->obj( new slider_int_t( label, value, min, max, suffix ) );
}

void evo::child_t::make_dropdown( std::string label, int* value, std::vector<std::string> items ) { 
	this->obj( new combo_t( label, value, items ) );
}

void evo::child_t::make_colorpicker( evo::col_t* color, float* hue ) {
	this->obj( new colorpicker_t( color, hue ) );
}

void evo::child_t::make_text( std::string label ) {
	this->obj( new text_t( label ) );
}

void evo::child_t::make_textbox( std::string label, std::string* value ) {
	this->obj( new textbox_t( label, value ) );
}

void evo::child_t::make_listbox( std::string label, int* value, std::vector<std::string>& items, std::string& filter ) {
	this->obj( new listbox_t( label, value, items, filter ) );
}

void evo::child_t::make_multibox( std::string label, std::vector<evo::multi_dropdown_system> items ) {
	auto multibox = new evo::multi_dropdown_t( label );

	for ( int i = 0; i < items.size( ); i++ ) {
		multibox->insert_item( items[ i ].name, items[ i ].value );
	}

	this->obj( multibox );
}

void evo::child_t::make_keybind( int* key, int* type ) {
	this->obj( new keybind_t( key, type ) );
}

void evo::child_t::make_button( std::string label, std::function<void( )> func ) {
	this->obj( new button_t( label, func ) );
}

/*
	* this is pretty much beta testing scrolling function
	* alpha and bugged i think, we will see 
*/
void evo::child_t::enable_scrolling( ) { 
	if ( !this->get_visible( ) ) {
		return;

		/* is not visible do not run this shit */
	}

	if ( !( slide_offset[ this->child_id ] > this->base_size.y - 42 ) ) {
		slide[ this->child_id ] = 0;

		return;
	}

	auto animation_h = animation_controller.get( this->child_title + "#hoverr" + std::to_string( this->child_id ) + animation_controller.current_child );
	animation_h.adjust( animation_h.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->hovered( ) ? 1.f : -1.f ) );


	auto scroll_process = [ this ]( ) {
		if ( slide[ this->child_id ] > 0 )
			slide[ this->child_id ] = 0;

		if ( slide[ this->child_id ] < ( this->base_size.y - 42 ) - slide_offset[ this->child_id ] )
			slide[ this->child_id ] = ( this->base_size.y - 42 ) - slide_offset[ this->child_id ];
	};

	if ( _container->can_interact( ) && hovered( ) && _input->get_mouse_wheel( ) != 0 ) {
		slide[ this->child_id ] += _input->get_mouse_wheel( ) * 10;
		_input->set_mouse_wheel( 0 );
		scroll_process( );
	}

	int _max = slide_offset[ this->child_id ] - this->base_size.y + 61.f;
	int _scroll_process = ( ( float )slide[ this->child_id ] / ( float )slide_offset[ this->child_id ] ) * ( this->base_size.y - 65.f ) * -1;
	float _scroll_pos_max = _max / ( float )slide_offset[ this->child_id ] * ( this->base_size.y - 65.f );
	static int old[ 1000 ];
	
	if ( old[ this->child_id ] != _max ) {
		scroll_process( );
		old[ this->child_id ] = _max;
	}

	evo::_render->add_rect_filled( this->base_window.x + 4 + this->base_size.x - 10, this->base_window.y + 42, 3, this->base_size.y - 53, _container->window_backround.darker( 5 ).modify_alpha(255 * animation_h.value), 10 );
	evo::_render->add_rect_filled_shadowed( this->base_window.x + 4 + this->base_size.x - 10, this->base_window.y + 42, 3, this->base_size.y - 53, _container->window_outline.darker( 10 ).modify_alpha( 255 * animation_h.value ), 10, 5 );

	evo::_render->clip_rect( this->base_window.x + this->base_size.x + 4 - 10, this->base_window.y + 42, 3, this->base_size.y - 53, [ & ]( ) {
		evo::_render->add_rect_filled( this->base_window.x + 4 + this->base_size.x - 10, this->base_window.y + 42 + _scroll_process,
								   3, this->base_size.y - 53 - _scroll_pos_max + 10, _container->window_accent.modify_alpha( 255 * animation_h.value ), 10 );
	} );

	evo::_render->clip_rect( this->base_window.x + 4 + this->base_size.x - 20, this->base_window.y + 42, 20, this->base_size.y - 53, [ & ]( ) {
		evo::_render->add_rect_filled_shadowed( this->base_window.x + 4 + this->base_size.x - 10, this->base_window.y + 42 + _scroll_process,
		3, this->base_size.y - 53 - _scroll_pos_max + 10, _container->window_accent.modify_alpha( 255 * animation_h.value ), 10, 10 );
	} );
}
