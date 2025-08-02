#include "../../../inc.hpp"
#include <algorithm>

/* 
	* note: constructor for window
	* note: initialize the elements
*/
evo::window_t::window_t( std::string window_name, evo::vec2_t* pos, evo::vec2_t size, std::vector<std::string> tabs, int* selected ) {
	this->window_name = window_name;

	/* pos */
	{
		this->pos = pos;
	}

	/* size */
	{
		this->size = size;
	}

	/* tab and subtab */
	{
		this->selected = selected;
	}

	/* initialize container */
	/*
		* note: we initialize this here because window gets always drawn 
		* init containter
		* init input
		* init id handling
	*/
	_input->init_input( );

	/* set id */
	_container->set_id( 0 );

	/* id base open reset */
	_container->base_opened[ _container->base_handler_t::combo ] = -1;
	_container->base_opened[ _container->base_handler_t::multicombo ] = -1;
	_container->base_opened[ _container->base_handler_t::bind ] = -1;

	_container->initialize_container( );

	/* init input */
	this->input( );

	/* initialize painting */
	this->paint( );

	/* bind container tabs */
	this->tabs = tabs;

	/* construct tabs */
	this->construct_tabs( );
}

/*
	* note: paint the actual window
*/
void evo::window_t::paint( ) {
	/*
		* note: main window rect, draw elements over it
		* note: after that we've got shadow shit
	*/
	evo::_render->add_rect_filled( this->pos->x, this->pos->y, this->size.x, this->size.y, _container->window_backround, 5 );
	evo::_render->add_rect_filled_shadowed( this->pos->x, this->pos->y, this->size.x, this->size.y, _container->window_backround, 5, 15 );
	
	evo::_render->add_rect_filled( this->pos->x, this->pos->y, this->size.x, 30, _container->window_title_bar, 5 );
	evo::_render->add_rect_filled( this->pos->x, this->pos->y + this->size.y - 30, this->size.x, 30, _container->window_title_bar, 5 );

	evo::_render->add_text( this->pos->x + 7, this->pos->y + 5, _container->window_accent, 1, "amnesia", 0 );
	evo::_render->add_text( this->pos->x + 7, this->pos->y + this->size.y - 25, _container->window_accent, 1, "Counter-Strike 2", 0 );
	evo::_render->add_text( this->pos->x + 7 + _render->text_size( "Counter-Strike 2", 1 ).x, 
							this->pos->y + this->size.y - 25, _container->window_text.modify_alpha( 150 * _container->anim_controler ), 1, " Developer", 0 );

	/*
		* note: draw this at final so no element will overwrite this rect
	*/
	evo::_render->add_rect( this->pos->x, this->pos->y, this->size.x, this->size.y, _container->window_outline, 5, 1 );
}

/*
	* note: input the actual shared element
*/
void evo::window_t::input( ) { 
	/* menu dragging */
	{
		static bool drag{ false };
		static evo::vec2_t offset{};

		/* define delta */
		evo::vec2_t delta = evo::_input->get_mouse_position( ) - offset;

		/* setup dragging */
		if ( drag && !GetAsyncKeyState( VK_LBUTTON ) ) {
			drag = false;
			theme::dragging = false;
		}

		/* update delta */
		if ( drag && GetAsyncKeyState( VK_LBUTTON ) ) {
			*this->pos = delta;

			theme::dragging = true;
		}

		/* update the menu position */
		if ( evo::_input->mouse_in_box( evo::vec2_t( this->pos->x, this->pos->y ),
										evo::vec2_t( 210, 30 ) ) ) {
			drag = true;		
			offset = evo::_input->get_mouse_position( ) - *this->pos;
		}

		/* clamp */
		if ( this->pos->x < 0 ) { this->pos->x = 0; }
		if ( this->pos->y < 0 ) { this->pos->y = 0; }
	}
	
	/* menu resize */
	{
		/* update the menu position */
		static bool resizing{ false };

		if ( GetAsyncKeyState( VK_LBUTTON ) ) {
			if ( evo::_input->mouse_in_box( evo::vec2_t( ( this->pos->x + this->size.x ) - 10, ( this->pos->y + this->size.y ) - 10 ),
													  evo::vec2_t( 20, 20 ) ) && !resizing ) {
				resizing = true;
				theme::resizing = true;
			}
		} else {
			resizing = false;
			theme::resizing = false;
		}

		if ( resizing ) {
			this->size.x = evo::_input->get_mouse_position( ).x - this->pos->x;
			this->size.y = evo::_input->get_mouse_position( ).y - this->pos->y;

			evo::theme::menu_size = this->size;

			evo::theme::rx = evo::theme::menu_size.x;
			evo::theme::ry = evo::theme::menu_size.y;
		}

		/* clamp gheto */
		if ( this->size.x < evo::theme::orx ) {
			this->size.x = evo::theme::orx;
		}
		if ( this->size.x > 1500 ) {
			this->size.x = 1500; this->is_resizing = false;
		}
		if ( this->size.y < evo::theme::ory ) {
			this->size.y = evo::theme::ory;
		}
		if ( this->size.y > 1500 ) {
			this->size.y = 1500; this->is_resizing = false;
		}
		if ( evo::theme::menu_size.x < evo::theme::orx ) {
			evo::theme::menu_size.x = evo::theme::orx;
		}
		if ( evo::theme::menu_size.y < evo::theme::ory ) {
			evo::theme::menu_size.y = evo::theme::ory;
		}

		if ( evo::theme::rx < evo::theme::orx ) {
			evo::theme::rx = evo::theme::orx;
		}
		if ( evo::theme::ry < evo::theme::ory ) {
			evo::theme::ry = evo::theme::ory;
		}

		/* store */
		this->is_resizing = resizing;
	}
}

/*
	* note: initialize the whole tab system
	* note: drawing tho bro
*/
void evo::window_t::construct_tabs( ) { 
	if ( this->tabs.empty( ) ) {
		return;
		/* abort runing */
	}

	/* we've bound the tabs and now we have them inited */
	/* draw everything */
	auto form_id{ 0 };
	int tab_spacing = 17; 

	/* start position */
	int start_x = this->pos->x + this->size.x - 161;

	/* iterate between tabs vector size */
	for ( int i = 0; i < this->tabs.size( ); ++i ) {
		/* calc tab size */
		evo::vec2_t tab_size = {
					evo::_render->text_size(
						this->tabs[ i ].c_str( ), evo::fonts_t::_icons ).x, evo::_render->text_size( this->tabs[ i ].c_str( ), evo::fonts_t::_icons ).y
		};

		/* tab positions */
		evo::vec2_t tab_pos = evo::vec2_t( start_x, this->pos->y + 6 );

		/* run input - mouse in box */
		if ( evo::_input->mouse_in_box( evo::vec2_t( start_x, this->pos->y + 6 ), evo::vec2_t( 30, 30 ) ) ) {
			if ( evo::_input->key_pressed( VK_LBUTTON ) ) {
				*this->selected = i;
			}
		}

		/* animation */
		auto animation = animation_controller.get( "animation_tabs" + std::to_string( i ) + animation_controller.current_child );
		animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.5f ) * ( ( *this->selected == i ) && _container->anim_controler > 0.f ? 1.f : -1.f ) );

		/* draw tab */
		evo::_render->add_text( start_x, this->pos->y + 7, _container->window_text.blend( _container->window_accent, animation.value ).modify_alpha( 170 * _container->anim_controler ),
								evo::fonts_t::_icons, this->tabs[ i ].c_str( ) );

		/* iterate start_x */
		start_x += tab_size.x + tab_spacing;
	}
}

/* add a new child */
void evo::window_t::make_child( child_t* child ) {
	/* set visible flag */
	child->set_visible( child->tab == *this->selected );

	/* render the child if its visible */
	if ( child->get_visible( ) ) {
		child->set_position( *this->pos );
		child->construct_child( );
	}
}
  