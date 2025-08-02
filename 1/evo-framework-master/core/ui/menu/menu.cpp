#include "../../inc.hpp"


void evo::menu_t::init( ) { 
	std::vector< std::string > tabs = { "i", "k", "3", "5", "6" };
	static bool a[ 5 ]{ false };
	static bool aaaa[ 5 ]{ false };
	static bool a2 = false;
	static float b = 30.f;
	static int c, c2 = 0;
	vec2_t menu_spawn22{ 100, 100 };
	static int rage_1 = 0;
	static int rage_2 = 0;

	static std::string imd;
	static int aa, aaa;
	static float aa222, aaa222;

	// evo::child_t::make_listbox( std::string label, int* value, std::vector<std::string>& items, std::string& filter )
	static int test11 = 0;
	static int test12 = 0;
	static std::vector<std::string> item_bos{};

	static evo::col_t coll;
	static float collh;
	static int rage_3 = 0;

	static int keybind[ 2 ]{ 0 };

	std::vector<std::string> rage_sub = { "Child - 1", "Child - 2", "Child - 3" };
	std::vector<std::string> rage_sub2 = { "Child - Sub2: 1", "Child - Sub2: 2", "Child - Sub2: 3" };

	auto window = new evo::window_t( "Deathrow cs2", &evo::theme::menu_spawn, evo::theme::menu_size, tabs, &evo::theme::selected_tab );
	{
		{
			auto child = new evo::child_t( "Aimbot", 0, evo::child_pos_t::_top_left, evo::child_size_t::_full, 0 );
			{
				window->make_child( child );

				child->make_checkbox_colorpicker( "Enabled - " + std::to_string( 0 ), &a[ 0 ], &coll, &collh );


				child->make_slider_float( "Hitchance", &b, 0, 100, "" );
				child->make_slider_int( "Minimum damage", &c, 0, 100, "" );
				child->make_dropdown( "Target selection", &c2, {
					"Damage", "Crosshair", "Lag"
									  } );

				{
					auto multi = new evo::multi_dropdown_t( "Multipoint" );
					
					for ( int m = 0; m < 5; m++ ) {
						multi->insert_item( "Element - " + std::to_string( m ), &aaaa[ m ] );
					}

					child->obj( multi );
				}

				child->make_text( "Element keybind." );
				child->make_keybind( &keybind[ 0 ], &keybind[ 1 ] );

				child->make_textbox( "Textbox", &imd );
				child->make_checkbox( "Enabled - " + std::to_string( 2 ), &a[ 2 ] );
				child->make_button( "Button", [ & ]( ) { } );

				child->enable_scrolling( );
			} delete child;

			auto child2 = new evo::child_t( rage_sub, 1, evo::child_pos_t::_top_right, evo::child_size_t::_full, 0, &test11 );
			{
				window->make_child( child2 );

				{
					auto pop = new evo::popup_t(0, "Accuracy settings" );
					pop->bind_checkbox( "Check 1", &aaaa[ 5 ] );
					pop->bind_checkbox( "Check 2", &aaaa[ 1 ] );
					pop->bind_slider_int( "Slider Int", &aaa, 0, 100, "" );
					pop->bind_slider_float( "Slider Float", &aa222, 0, 100, "" );
					pop->bind_text( "Just a text!" );
					child2->obj( pop );
				}

				child2->make_checkbox_colorpicker( "Enabled - " + std::to_string( 0 ), &a[ 0 ], &coll, &collh );


			//	child2->make_listbox( "Listbox 1", &test12, rage_sub2, imd );

			} delete child2;
		}
	} 
	delete window;

	evo::externals::_ext_b->begin( );
	
	// popup stuff going the latest
	evo::externals::_ext_b_p->begin_popup( );
}
