#include "../../../inc.hpp"

evo::esp_builder_t::esp_builder_t( ) {
	/* constructor, initialize smth */
}

struct box_t {
	int x, y, w, h;
};

inline static int elm_top{ }, elm_bottom{ }, elm_left{ }, elm_right{ },
	elm_top2{ }, elm_bottom2{ }, elm_left2{ }, elm_right2{ };

inline static bool is_bt[ 24 ]{}, is_tt[ 32 /* items */ ]{};
inline static bool is_bb[ 24 ]{}, is_tb[ 32 /* items */ ]{};
inline static bool is_br[ 24 ]{}, is_tr[ 32 /* items */ ]{};
inline static bool is_bl[ 24 ]{}, is_tl[ 32 /* items */ ]{};

static bool item_is_getting_dragged[ 1000 ]{ false }, update_item{ false }, is_bar[ 6 ][ 64 ]{ false };
static int top_items{}, bottom_items{};
static std::vector<std::string> t_items, b_items, r_items, l_items;
static std::vector<std::string> bt_items, bb_items, br_items, bl_items;
static int elements_top, elements_bot, elements_rig, elements_lef;
static int belements_top, belements_bot, belements_rig, belements_lef;
static evo::vec2_t basic_size[ 64 ]{};

/* this is pretty much gheto */

void evo::esp_builder_t::paint( ) {
	this->base_window.y += 10;

	// image scale.
	auto scale = 2.3f;

	// calculate sizes.
	auto image_size = ImVec2( 397 / scale, 700 / scale );

	// scale for original.
	int original_ = ( theme::rx - theme::orx ) / 4;
	int original_y = ( theme::ry - theme::ory ) / 2;

	std::vector<bool> esp_elements = {
		true,
		true,
		true,
		true,
	};

	_render->add_image( this->base_window.x + 25 + original_, this->base_window.y + 15 + original_y,
						image_size.x, image_size.y, preview, evo::col_t( ).modify_alpha( 255 * _container->anim_controler ) );

	// player origin.
	box_t box;
	box.x = this->base_window.x + 25 + original_;
	box.y = this->base_window.y + 17 + original_y;
	box.w = 397 / scale;
	box.h = 700 / scale;

	// 0 - top
	// 1 - bottom

	std::vector<move_databse> area = {
		move_databse{evo::rect_t( this->base_window.x + 25 + original_, this->base_window.y - 8 + original_y, 397 / scale, 25 ), draggable_cond::c_top}, // up
		move_databse{evo::rect_t( this->base_window.x + 25 + original_, this->base_window.y + 17 + original_y + 700 / scale, 397 / scale, 25 ), draggable_cond::c_bottom}, // down
		move_databse{evo::rect_t( this->base_window.x + original_, this->base_window.y + 17 + original_y, 25, 700 / scale ), draggable_cond::c_left}, // down
		move_databse{evo::rect_t( this->base_window.x + 25 + original_ + 397 / scale, this->base_window.y + 17 + original_y, 25, 700 / scale ), draggable_cond::c_right}, // down
	};

	if ( true ) {
		_render->add_rect( box.x, box.y, box.w, box.h, col_t( ), 0, 1 );
	}

	static bool inited[ 64 ]{ false };
	static vec2_t posx[ 64 ]{};

	{
		auto t_width = _render->text_size( "dutu1337", this->items.size( ) > 0 ? this->items[ 0 ].font : evo::fonts_t::_default2 );

		if ( !inited[ 0 ] ) {
			posx[ 0 ] = vec2_t( box.x + box.w * 0.5 - t_width.x * 0.5, box.y - 10 - ( t_width.y / 2 ) );
			inited[ 0 ] = true;
		}

		if ( theme::resizing || theme::dragging ) {
			posx[ 0 ] = vec2_t( box.x + box.w * 0.5 - t_width.x * 0.5, box.y - 10 - ( t_width.y / 2 ) );
		}

		auto name_esp = movable_item( "name_esp", posx[ 0 ],
									  posx[ 0 ], 0, 0, 0 );

		this->items.push_back( name_esp );
	}

	{
		auto t_width = _render->text_size( "scar20", this->items.size( ) > 0 ? this->items[ 0 ].font : evo::fonts_t::_default2 );

		if ( !inited[ 1 ] ) {
			posx[ 1 ] = vec2_t( box.x + box.w * 0.5 - t_width.x * 0.5, box.y - 10 + box.h + ( t_width.y / 2 ) );
			inited[ 1 ] = true;
		}

		if ( theme::resizing || theme::dragging ) {
			posx[ 1 ] = vec2_t( box.x + box.w * 0.5 - t_width.x * 0.5, box.y - 10 + box.h + ( t_width.y / 2 ) );
		}

		auto weapon_esp = movable_item( "weapon_esp", posx[ 1 ],
										posx[ 1 ], 1, 0, 0 );

		this->items.push_back( weapon_esp );
	}

	{
		if ( !inited[ 2 ] ) {
			posx[ 2 ] = vec2_t( box.x - 8, box.y + 1 );
			basic_size[ 0 ] = { ( float )4, ( float )box.h - 1 };
			inited[ 2 ] = true;
		}

		if ( theme::resizing || theme::dragging ) {
			posx[ 2 ] = vec2_t( box.x - 8, box.y + 1 );
			basic_size[ 0 ] = { ( float )4, ( float )box.h - 1 };
		}

		auto healthbar = movable_item( "healthbar", posx[ 2 ],
									   posx[ 2 ], 2, 0, 0 );

		this->items.push_back( healthbar );
	}

	{
		if ( !inited[ 3 ] ) {
			posx[ 3 ] = vec2_t( box.x + 1, box.y + box.h + 1 );
			basic_size[ 1 ] = { ( float )box.w - 1, ( float )4 };
			inited[ 3 ] = true;
		}

		if ( theme::resizing || theme::dragging ) {
			posx[ 3 ] = vec2_t( box.x + 1, box.y + box.h + 1 );
			basic_size[ 1 ] = { ( float )box.w - 1, ( float )4 };
		}

		auto ammobar = movable_item( "ammobar", posx[ 3 ],
									 posx[ 3 ], 3, 0, 0 );

		this->items.push_back( ammobar );
	}

	{
		if ( !inited[ 3 ] ) {
			posx[ 3 ] = vec2_t( box.x + 1, box.y + box.h + 1 );
			basic_size[ 1 ] = { ( float )box.w - 1, ( float )4 };
			inited[ 3 ] = true;
		}

		if ( theme::resizing || theme::dragging ) {
			posx[ 3 ] = vec2_t( box.x + 1, box.y + box.h + 1 );
			basic_size[ 1 ] = { ( float )box.w - 1, ( float )4 };
		}

		auto hk = movable_item( "hk", posx[ 3 ],
								posx[ 3 ], 3, 0, 0 );

		this->items.push_back( hk );
	}

	this->items[ 0 ].font = evo::fonts_t::_default2;
	this->items[ 1 ].font = evo::fonts_t::_default2;

	auto update_dragging_pos = [ & ]( int id ) -> void {
		this->items[ id ].temp_pos.x = _input->get_mouse_position( ).x;
		this->items[ id ].temp_pos.y = _input->get_mouse_position( ).y;

		this->items[ id ].basic_pos.x = this->items[ id ].temp_pos.x;
		this->items[ id ].basic_pos.y = this->items[ id ].temp_pos.y;
	};

	if ( esp_elements[ 0 ] ) {
		int e_id = 0;
		auto height = _render->text_size( "dutu1337", this->items[ e_id ].font );

		/* input */
		if ( _input->mouse_in_box( vec2_t( this->items[ e_id ].temp_pos.x, this->items[ e_id ].temp_pos.y ), vec2_t( height.x, height.y ) ) ) {
			if ( GetAsyncKeyState( VK_LBUTTON ) ) {
				item_is_getting_dragged[ e_id ] = true;
			}
		}

		if ( item_is_getting_dragged[ e_id ] ) {
			update_dragging_pos( e_id );

			for ( int _area = 0; _area < area.size( ); _area++ ) {
				if ( _input->mouse_in_box( vec2_t( area[ _area ].rect.x, area[ _area ].rect.y ), vec2_t( area[ _area ].rect.w, area[ _area ].rect.h ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
					this->items[ e_id ].drag_cond = ( int )area[ _area ].condition;

					if ( this->items[ e_id ].drag_cond == draggable_cond::c_top ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w * 0.5 - height.x * 0.5 ), ( float )( box.y - 10 - ( height.y / 2 ) ) };
						theme::cond_id[ e_id ] = draggable_cond::c_top;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_bottom ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w * 0.5 - height.x * 0.5 ), ( float )( box.y - 10 + box.h + ( height.y / 2 ) ) };

						theme::cond_id[ e_id ] = draggable_cond::c_bottom;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_left ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x - height.x - 2 ), ( float )( box.y - 10 + ( height.y / 2 ) + 2 ) };
		

						theme::cond_id[ e_id ] = draggable_cond::c_left;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_right ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w + 2 ), ( float )( box.y - 10 + ( height.y / 2 ) + 2 ) };
						
						theme::cond_id[ e_id ] = draggable_cond::c_right;
					}

					posx[ e_id ] = this->items[ e_id ].temp_pos;

					item_is_getting_dragged[ e_id ] = false;
				}
			}
		}

		_render->add_text( this->items[ e_id ].basic_pos.x, this->items[ e_id ].basic_pos.y, col_t( ), this->items[ 0 ].font, "dutu1337" );
	}

	if ( esp_elements[ 1 ] ) {
		int e_id = 1;
		auto height = _render->text_size( "dutu1337", this->items[ e_id ].font );

		/* input */
		if ( _input->mouse_in_box( vec2_t( this->items[ e_id ].temp_pos.x, this->items[ e_id ].temp_pos.y ), vec2_t( height.x, height.y ) ) ) {
			if ( GetAsyncKeyState( VK_LBUTTON ) ) {
				item_is_getting_dragged[ e_id ] = true;
			}
		}

		if ( item_is_getting_dragged[ e_id ] ) {
			update_dragging_pos( e_id );

			for ( int _area = 0; _area < area.size( ); _area++ ) {
				if ( _input->mouse_in_box( vec2_t( area[ _area ].rect.x, area[ _area ].rect.y ), vec2_t( area[ _area ].rect.w, area[ _area ].rect.h ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
					this->items[ e_id ].drag_cond = ( int )area[ _area ].condition;

					if ( this->items[ e_id ].drag_cond == draggable_cond::c_top ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w * 0.5 - height.x * 0.5 ), ( float )( box.y - 10 - ( height.y / 2 ) ) };
						theme::cond_id[ e_id ] = draggable_cond::c_top;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_bottom ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w * 0.5 - height.x * 0.5 ), ( float )( box.y - 10 + box.h  + ( height.y / 2 ) ) };
						theme::cond_id[ e_id ] = draggable_cond::c_bottom;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_left ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x - height.x - 2 ), ( float )( box.y - 10 + ( height.y / 2 ) + 2 ) };
						theme::cond_id[ e_id ] = draggable_cond::c_left;
					} else if ( this->items[ e_id ].drag_cond == draggable_cond::c_right ) {
						this->items[ e_id ].temp_pos = { ( float )( box.x + box.w + 2 ), ( float )( box.y - 10 + ( height.y / 2 ) + 2 ) };
						theme::cond_id[ e_id ] = draggable_cond::c_right;
					}

					posx[ e_id ] = this->items[ e_id ].temp_pos;

					item_is_getting_dragged[ e_id ] = false;
				}
			}
		}

		_render->add_text( this->items[ e_id ].basic_pos.x, this->items[ e_id ].basic_pos.y, col_t( ), this->items[ e_id ].font, "scar20" );
		/* main system
			it starts now */
	}

}

void evo::esp_builder_t::input( ) {

}

void evo::esp_builder_t::set_font( int element_id, int font ) {
	this->items[ element_id ].font = font;
}

void evo::esp_builder_t::reset_position( ) {

}
