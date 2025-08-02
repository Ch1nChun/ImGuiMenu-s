#include "../../../inc.hpp"

static auto str_idx_color = -1;
static bool state_color = false;

static bool xcopied[ 1000 ]{ false };
static bool xpasted[ 1000 ]{ false };

namespace evo {
	__forceinline void circle( ImVec2 pos, float radius, ImColor color, float thickness, bool overwrite ) {
		if ( overwrite ) {
			externals::external.circle_a.insert( externals::external.circle_a.begin( ), externals::draw_circle( pos, radius, color, thickness ) );
		} else {
			externals::external.circle.insert( externals::external.circle.begin( ), externals::draw_circle( pos, radius, color, thickness ) );
		}
	}

	__forceinline void circle_fill( ImVec2 pos, float radius, ImColor color, bool overwrite ) {
		if ( overwrite ) {
			externals::external.fcircle_a.insert( externals::external.fcircle_a.begin( ), externals::draw_circle_fill( pos, radius, color ) );
		} else {
			externals::external.fcircle.insert( externals::external.fcircle.begin( ), externals::draw_circle_fill( pos, radius, color ) );
		}
	}

	__forceinline void rounded( int x, int y, int w, int h, int r, bool overwrite ) {
		if ( overwrite ) {
			externals::external.t_rect_a.insert( externals::external.t_rect_a.begin( ), externals::draw_thick_rect( x, y, w, h, col_t( 19, 19, 19 ), r, 1.0f ) );
		} else {
			externals::external.t_rect.insert( externals::external.t_rect.begin( ), externals::draw_thick_rect( x, y, w, h, col_t( 19, 19, 19 ), r, 1.0f ) );
		}
	}

	void grd( int x, int y, int w, int h, col_t c, col_t c2, bool a, bool overwrite, int r, ImDrawFlags flags = ImDrawFlags_RoundCornersAll ) {
		if ( overwrite ) {
			externals::external.gradient_a.insert( externals::external.gradient_a.begin( ), externals::draw_gradient( x, y, w, h, c, c2, a, r, flags ) );
		} else {
			externals::external.gradient.insert( externals::external.gradient.begin( ), externals::draw_gradient( x, y, w, h, c, c2, a, r, flags ) );
		}
	}

	__forceinline void add_grade( int x, int y, int rad, col_t c, col_t c2, float thickness, bool overwrite ) {
		circle_fill( ImVec2( x, y ), rad, c.convert( ), overwrite );
		circle( ImVec2( x, y ), rad, c2.convert( ), thickness, overwrite );
	}

	__forceinline void add_sobo( int x, int y, int w, int h, col_t c, col_t c2, int r, bool overwrite ) {
		_ext->make_rect_filled( x, y, w, h, c, r, overwrite );
		_ext->make_rect_filled_shadow( x, y, w, h, c2, r, 5, overwrite );
		//_ext->make_rect( x - 1, y - 1, w + 2, h + 2, c2, r, overwrite );
	}
}

evo::colorpicker_t::colorpicker_t( evo::col_t* color, float* hue ) {
	this->select = color;
	this->hue = hue;

	HSV new_color = rgb_to_hsv( { ( double )( ( float )select->r / 255.f ),( double )
					   ( ( float )select->g / 255.f ),( double )( ( float )select->b / 255.f ) } );

	this->cursor_pos.x = ( float )new_color.s * 158.f;
	this->cursor_pos.y = 100.f - ( ( float )new_color.v * 100.f );

	this->alpha = ( float )select->a / 255.f;
}

static evo::col_t bar_colors[ 7 ] = {
			evo::col_t( 255, 0, 0, 255 ),
			evo::col_t( 255, 255, 0, 255 ),
			evo::col_t( 0, 255, 0, 255 ),
			evo::col_t( 0, 255, 255, 255 ),
			evo::col_t( 0, 0, 255, 255 ),
			evo::col_t( 255, 0, 255, 255 ),
			evo::col_t( 255, 0, 0, 255 )
};


void evo::colorpicker_t::paint( ) { 
	/* update this */
	this->pos = this->base_window;

	/* animations */
	auto anim_2 = animation_controller.get( std::to_string( _container->get_id( ) ) + "#colorpicker_focused" + animation_controller.current_child );
	anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.5f ) * ( ( _container->anim_controler > 0.f ) && this->focused_element ? 1.f : -1.f ) );

	auto modify_hue = animation_controller.get( std::to_string( _container->get_id( ) ) + "#modify" + animation_controller.current_child );
	modify_hue.adjust( modify_hue.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifying_hue && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	auto modify_hsv = animation_controller.get( std::to_string( _container->get_id( ) ) + "#modify_hsv" + animation_controller.current_child );
	modify_hsv.adjust( modify_hsv.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifing_hsv && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	auto modify_apha = animation_controller.get( std::to_string( _container->get_id( ) ) + "#modify_alpha" + animation_controller.current_child );
	modify_apha.adjust( modify_apha.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifieng_alpha && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	auto copy_col = animation_controller.get( std::to_string( _container->get_id( ) ) + "#copycolor" + animation_controller.current_child );
	copy_col.adjust( copy_col.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( xcopied[ _container->get_id( ) ] && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	auto paste_col = animation_controller.get( std::to_string( _container->get_id( ) ) + "#paste_col" + animation_controller.current_child );
	paste_col.adjust( paste_col.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( xpasted[ _container->get_id( ) ] && ( _container->anim_controler > 0.f ) ? 1.f : -1.f ) );

	evo::_render->add_rect_filled( this->base_window.x, this->base_window.y, 14, 14, this->select->modify_alpha( 255 * _container->anim_controler ), 5 );
	evo::_render->add_rect_filled_shadowed( this->base_window.x, this->base_window.y, 14, 14, this->select->modify_alpha( 100 * _container->anim_controler ), 5, 15 );

	if ( anim_2.value > 0.f ) {
		/* position */
		int w = 188;
		int h = 204;
		int x = this->pos.x + ( w / 2 );
		int y = this->pos.y + 17 + ( h / 2 );

		auto anim_x = ( w * anim_2.value ) * 0.5;
		auto anim_y = ( h * anim_2.value ) * 0.5;

		_ext->make_rect_filled( x - anim_x, y - anim_y, w * anim_2.value, h * anim_2.value, _container->window_backround, 3, false );
		_ext->make_rect_filled_shadow( x - anim_x, y - anim_y, w * anim_2.value, h * anim_2.value, _container->window_outline, 3, 10, false );

		RGB hue_color_t = hsv_to_rgb( { ( double )*this->hue * 360.f, 1.f, 1.f } );
		
		grd( x - ( 160 * anim_2.value ) * 0.5, y - ( 180 * anim_2.value ) * 0.5, 140 * anim_2.value, 140 * anim_2.value, col_t( ).modify_alpha( anim_2.value * 255 ),
			 col_t( ( int )( hue_color_t.r * 255.0f ), ( int )( hue_color_t.g * 255.0f ), ( int )( hue_color_t.b * 255.0f ) ).modify_alpha( anim_2.value * 255 ), false, false, 4 );

		grd( x - ( 160 * anim_2.value ) * 0.5, y - ( 180 * anim_2.value ) * 0.5, 140 * anim_2.value, 140 * anim_2.value, col_t( 0, 0, 0, 0 ),
			 col_t( 0, 0, 0, 255 ).modify_alpha( anim_2.value * 255 ), true, true, 4 );

		if ( modify_hue.value > 0.f ) {
			draw_list->PushClipRect( { ( float )x + 17, ( float )y + 17 }, { ( float )x + 154.f, ( float )y + 96.f }, true );
		
			add_grade( x - ( 160 * anim_2.value ) * 0.5 + ( 140.f * ( this->cursor_pos.x / 140.f ) ), y - ( 180 * anim_2.value ) * 0.5 + ( 140.f * ( this->cursor_pos.y / 140.f ) ),
					   5 * modify_hue.value, this->select->modify_alpha( anim_2.value * 255 ), col_t( 0, 0, 0 ).modify_alpha( anim_2.value * 255 ), 2, true );
		
			draw_list->PopClipRect( );
		}

		auto animmuu = animation_controller.get( std::to_string( _container->get_id( ) ) + "#colorpicker_focused22" + animation_controller.current_child );
		animmuu.adjust( animmuu.value + 3.f * animation_controller.get_min_deltatime( 0.7f ) * ( ( _container->anim_controler > 0.f ) && ( anim_2.value >= 0.99 ) ? 1.f : -1.f ) );

		
		for ( int it = 0; it < 6; it++ ) {
			grd( x + 67, y - 90 + ( 23 * it ), 15, 23, bar_colors[ it ].modify_alpha( animmuu.value * 255 ), bar_colors[ it + 1 ].modify_alpha( animmuu.value * 255 ), true, false, 3 * animmuu.value, ( it == 5 ? ImDrawFlags_RoundCornersBottom : 1 ) | ( it == 0 ? ImDrawFlags_RoundCornersTop : 1 ) );
		}

		/*
						add_grade( x + 13 + ( 158 * *this->hue ), y + 16 + 109,
						   4 * modify_hsv.value, sdk::color::col_t( ( int )( hue_color_t.r * 255.0f ), ( int )( hue_color_t.g * 255.0f ), ( int )( hue_color_t.b * 255.0f ) ), sdk::color::col_t( 0, 0, 0 ), 2, true );

		*/

		add_sobo( x + 65, y - 93 + ( 140 * *this->hue ), 19, 6, col_t( ).modify_alpha( 255 * animmuu.value ), _container->window_outline.modify_alpha( 255 * animmuu.value ), 3, true );

		grd( x - ( 160 * anim_2.value ) * 0.5, y + ( 115 * anim_2.value ) * 0.5, 140 * anim_2.value, 15 * anim_2.value, evo::col_t( 0, 0, 0 ), *this->select, false, false, 3 );
		add_sobo( ( x - ( 160 * anim_2.value ) * 0.5 ) - 3 + ( 140 * this->alpha ), y + ( 111 * anim_2.value ) * 0.5, 6, 19, col_t( ).modify_alpha( 255 * animmuu.value ), _container->window_outline.modify_alpha( 255 * animmuu.value ), 3, true );

		int y_button = y + ( 160 * anim_2.value ) * 0.5;
		_ext->make_text( x + 65 - 28, y_button + 3, _container->window_text.modify_alpha( anim_2.value * 200 ).blend( _container->window_accent, copy_col.value ), evo::fonts_t::_icons_2, "M", evo::font_flags_t::dropshadow_low );
		_ext->make_text( x + 95 - 28, y_button + 3, _container->window_text.modify_alpha( anim_2.value * 200 ).blend( _container->window_accent, paste_col.value ), evo::fonts_t::_icons_2, "N", evo::font_flags_t::dropshadow_low );
		
		if ( copy_col.value >= 0.99 ) {
			xcopied[ _container->get_id( ) ] = false;
		}

		if ( paste_col.value >= 0.99 ) {
			xpasted[ _container->get_id( ) ] = false;
		}
		
		//_ext->make_text( x + 117, y_button + 3, _container->window_text.modify_alpha( anim_2.value * 100 ).blend( _container->window_accent, paste_col.value ), evo::fonts_t::_default2, "Paste", evo::font_flags_t::dropshadow_low );
	}
}

void evo::colorpicker_t::input( ) { 
	if ( _container->can_interact( ) && _input->mouse_in_box( { this->base_window.x, this->base_window.y }, { 14, 14 } )
		 && _input->key_pressed( VK_LBUTTON ) && !_container->base_handler[ 3 ] ) {
		state_color = !state_color;
		str_idx_color = _container->get_id( );
	}

	if ( str_idx_color == _container->get_id( ) ) {
		/* we can now use this shit */
		this->focused_element = state_color;
	}

	if ( str_idx_color == _container->get_id( ) && !_input->mouse_in_box( { this->base_window.x, this->base_window.y }, { 190, 206 + 17 } )
		 && _input->key_pressed( VK_LBUTTON ) ) {
		state_color = false;
	}

	_container->base_handler[ 0 ] = state_color;
	theme::colorpicker_is_opened = this->focused_element;
	_container->base_opened_state[ _container->base_handler_t::colorpicker ][ _container->get_id( ) ] = this->focused_element;

	/* position */
	int w = 188;
	int h = 204;
	int x = this->base_window.x;
	int y = this->base_window.y + 17;

	if ( this->focused_element ) {
		if ( _input->mouse_in_box( vec2_t( x + 11, y + 11 ), vec2_t( 140, 140 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifying_hue = true;

			this->cursor_pos.x = _input->get_mouse_position( ).x - ( x + 11 );
			this->cursor_pos.y = _input->get_mouse_position( ).y - ( y + 11 );

			if ( cursor_pos.x < 0.f )
				cursor_pos.x = 0.f;

			if ( cursor_pos.x > 140.f )
				cursor_pos.x = 140.f;

			if ( cursor_pos.y < 0.f )
				cursor_pos.y = 0.f;

			if ( cursor_pos.y > 140.f )
				cursor_pos.y = 140.f;

			float saturation = cursor_pos.x / 140.f;
			float bright = 1.f - ( cursor_pos.y / 140.f );

			RGB new_color = hsv_to_rgb( { *this->hue * 360.f, saturation, bright } );
			*this->select = col_t( ( int )( new_color.r * 255.f ), ( int )( new_color.g * 255.f ), ( int )( new_color.b * 255.f ), this->select->a );

		} else if ( _input->mouse_in_box( vec2_t( x + 160, y + 13 ), vec2_t( 15, 140 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifing_hsv = true;

			*this->hue = ( _input->get_mouse_position( ).y - ( y + 13 ) ) / 140.f;

			float saturation = cursor_pos.x / 140.f;
			float bright = 1.f - ( cursor_pos.y / 140.f );

			RGB new_color = hsv_to_rgb( { *this->hue * 360.f, saturation, bright } );
			*this->select = col_t( ( int )( new_color.r * 255.f ), ( int )( new_color.g * 255.f ), ( int )( new_color.b * 255.f ), this->select->a );
		} else if ( _input->mouse_in_box( vec2_t( x + 13, y + 160 ), vec2_t( 140, 15 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifieng_alpha = true;

			this->alpha = ( _input->get_mouse_position( ).x - ( x + 13 ) ) / 140.0f;

			if ( alpha < 0.f )
				alpha = 0.f;

			if ( alpha > 1.f )
				alpha = 1.f;

			if ( alpha > 0.99f )
				alpha = 1.f;

			*this->select = col_t( this->select->r, this->select->g, this->select->b, ( this->alpha * 255.f ) );
		}

		if ( _input->mouse_in_box( vec2_t( x + 13, y + 13 ), vec2_t( 140, 140 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifying_hue = false;
		} else if ( _input->mouse_in_box( vec2_t( x + 160, y + 13 ), vec2_t( 15, 140 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifing_hsv = false;

		} else if ( _input->mouse_in_box( vec2_t( x + 13, y + 160 ), vec2_t( 140, 15 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
			this->modifieng_alpha = false;
		}

		int y_button = y + 183;
		///* copy */
		if ( _input->mouse_in_box( vec2_t( x + 130, y_button ), vec2_t( 12, 12 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
			theme::copied_color = *this->select;

			printf( "copied\n" );

			xcopied[ _container->get_id() ] = true;
		}
		
		///* pasted */
		if ( _input->mouse_in_box( vec2_t( x + 160, y_button ), vec2_t( 12, 12 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
			*this->select = theme::copied_color;
		
			printf( "pasted\n" );

			*this->select = col_t( this->select->r, this->select->g, this->select->b, ( this->alpha * 255.f ) );
			xpasted[ _container->get_id( ) ] = true;
		}
	}
}
