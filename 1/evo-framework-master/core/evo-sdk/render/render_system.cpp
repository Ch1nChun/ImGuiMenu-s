#include "render_system.hpp"
#include "fonts.hpp"

void evo::render_t::initialize_font_system( ) {
	ImGuiIO& io = ImGui::GetIO( ); /* get imgui io */

	/* create a new cfg for each type of font */
	ImFontConfig cfg;// = new ImFontConfig( );

	cfg.PixelSnapH = true;
	cfg.OversampleH = cfg.OversampleV = 1;

	/* ranges */
	static const ImWchar ranges[ ] =
	{
	   0x0020, 0x00FF,
	   0x0400, 0x052F,
	   0x2DE0, 0x2DFF,
	   0xA640, 0xA69F,
	   0x3131, 0x3163,
	   0xAC00, 0xD79D,
	   0,
	};

	static const ImWchar ranges_ic[ ] = {

	   0x0020, 0x00FF, // Basic Latin + Latin Supplement
	   0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
	   0x2DE0, 0x2DFF, // Cyrillic Extended-A
	   0xA640, 0xA69F, // Cyrillic Extended-B
	   0xE000, 0xE226, // icons
	   0,
	};

	auto add_font = [ & ]( const char* font, float size )->void {
		io.Fonts->AddFontFromFileTTF( font, size, &cfg, ranges );

		printf( "Loaded font: %s, Size: %.1f\n", font, size );
		};


	add_font( "C:\\Windows\\Fonts\\verdana.ttf", 11.0f );
	io.Fonts->AddFontFromMemoryTTF( lexendregular, sizeof( lexendregular ), 14 );
	io.Fonts->AddFontFromMemoryCompressedTTF( MenuIcons_compressed_data, MenuIcons_compressed_size, 16.0f, &cfg, io.Fonts->GetGlyphRangesJapanese( ) );
	io.Fonts->AddFontFromMemoryTTF( arrowmod, sizeof( arrowmod ), 22.0f, &cfg, io.Fonts->GetGlyphRangesJapanese( ) );
	io.Fonts->AddFontFromMemoryTTF( icons_binary, sizeof( icons_binary ), 12, &cfg, ranges_ic );

	/* freetype build */
	//ImGuiFreeType::BuildFontAtlas( io.Fonts, ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting );
	ImGuiFreeType::BuildFontAtlas( io.Fonts, ImGuiFreeTypeBuilderFlags_Bitmap );
}

IDirect3DTexture9* backround = nullptr;
IDirect3DTexture9* preview = nullptr;
void evo::render_t::initialize_imgui( void* w, IDirect3DDevice9* device ) {
	IMGUI_CHECKVERSION( );

	ImGui::CreateContext( );
	ImGui::StyleColorsDark( );

	ImGui_ImplWin32_Init( w );
	ImGui_ImplDX9_Init( device );

	if ( backround == nullptr )
		D3DXCreateTextureFromFileInMemoryEx( device, csss, sizeof( csss ), 1920, 1080, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &backround );

	if ( preview == nullptr )
		D3DXCreateTextureFromFileInMemoryEx( device, &preview_image, sizeof( preview_image ), 397, 700, D3DX_DEFAULT, D3DPOOL_DEFAULT, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, NULL, NULL, NULL, &preview );


	/* init font system */
	this->initialize_font_system( );
}

void evo::render_t::add_text( int x, int y, col_t c, int font, const char* text, int flag ) { 
	ImGuiIO& io = ImGui::GetIO( );
	ImGui::PushFont( io.Fonts->Fonts[ font ] );

	if ( flag == font_flags_t::dropshadow ) {
		draw_list->AddText( evo::macros::vec_t( x + 1.f, y + 1.f ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
	}

	if ( flag == font_flags_t::dropshadow_low ) {
		draw_list->AddText( evo::macros::vec_t( x + 1.f, y + 1.f ), col_t( 5, 5, 5 ).modify_alpha( c.a / 2 ).convert( ), text );
	}

	if ( flag == font_flags_t::outline ) {
		draw_list->AddText( evo::macros::vec_t( x + 1, y - 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x - 1, y + 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x - 1, y - 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x + 1, y + 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );

		draw_list->AddText( evo::macros::vec_t( x, y + 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x, y - 1 ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x + 1, y ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
		draw_list->AddText( evo::macros::vec_t( x - 1, y ), col_t( 5, 5, 5 ).modify_alpha( c.a ).convert( ), text );
	}

	/* text */
	draw_list->AddText( evo::macros::vec_t( x, y ), c.convert( ), text );

	ImGui::PopFont( );
}

evo::vec2_t evo::render_t::text_size( const char* text, int font ) {
	ImGuiIO& io = ImGui::GetIO( );

	return {
		io.Fonts->Fonts[ font ]->CalcTextSizeA( io.Fonts->Fonts[ font ]->FontSize, FLT_MAX, -1.0f, text ).x,
		io.Fonts->Fonts[ font ]->CalcTextSizeA( io.Fonts->Fonts[ font ]->FontSize, FLT_MAX, -1.0f, text ).y
	};
}

void evo::render_t::add_rect_filled( int x, int y, int w, int h, evo::col_t col, int round ) {
	draw_list->AddRectFilled( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), col.convert( ), round );
}

void evo::render_t::add_rect_filled_shadowed( int x, int y, int w, int h, evo::col_t col, int round, int thickness ) {
	draw_list->AddShadowRect( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), col.convert( ), thickness, evo::macros::vec_t( ), 0, round );
}

void evo::render_t::add_rect( int x, int y, int w, int h, evo::col_t col, int round, int tickness ) {
	draw_list->AddRect( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), col.convert( ), round, 0, tickness );
}

void evo::render_t::add_filled_circle( evo::vec2_t center, float radius, evo::col_t c ) {
	draw_list->AddCircleFilled( evo::macros::vec_t( center.x, center.y ), radius, c.convert( ) );
}

evo::macros::u32_t lerp_color( evo::macros::u32_t col1, evo::macros::u32_t col2, float t ) {
		evo::macros::vec4_t c1 = ImGui::ColorConvertU32ToFloat4( col1 );
		evo::macros::vec4_t c2 = ImGui::ColorConvertU32ToFloat4( col2 );
		evo::macros::vec4_t lerped = evo::macros::vec4_t( c1.x + t * ( c2.x - c1.x ),
								c1.y + t * ( c2.y - c1.y ),
								c1.z + t * ( c2.z - c1.z ),
								c1.w + t * ( c2.w - c1.w ) );
		return ImGui::ColorConvertFloat4ToU32( lerped );
}

void evo::render_t::add_gradient_circle_filled( evo::vec2_t c, int radius, evo::col_t inner_color, evo::col_t outer_color ) {
	for ( int i = 0; i <= radius; ++i ) {
		float t = static_cast< float >( i ) / radius;
		ImU32 color = ImGui::GetColorU32( lerp_color( inner_color.convert( ), outer_color.convert( ), t ) );
		draw_list->AddCircleFilled( evo::macros::vec_t( c.x, c.y ), static_cast< float >( i ), color );
	}
}

void evo::render_t::add_gradient_vertical( int x, int y, int w, int h, evo::col_t c1, evo::col_t c2, int rounding, ImDrawFlags flags ) {
	if ( rounding != 0 ) {
		draw_list->AddRectFilledMultiColorRounded( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), _container->elem_back.convert(), c1.convert( ), c2.convert( ), c2.convert( ), c1.convert( ), rounding , flags);

	} else {
		draw_list->AddRectFilledMultiColor( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), c1.convert( ), c2.convert( ), c2.convert( ), c1.convert( ) );
	}

}

void evo::render_t::add_gradient_horizontal( int x, int y, int w, int h, evo::col_t c1, evo::col_t c2, int rounding, ImDrawFlags flags ) {
	if ( rounding != 0 ) {
		draw_list->AddRectFilledMultiColorRounded( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), _container->elem_back.convert( ), c1.convert( ), c1.convert( ), c2.convert( ), c2.convert( ), rounding, flags );
	} else {
		draw_list->AddRectFilledMultiColor( evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), c1.convert( ), c1.convert( ), c2.convert( ), c2.convert( ) );
	}
}

void evo::render_t::add_drop_shadow( int x, int y, int w, int h, evo::col_t main_col, int size, int alpha_a ) {
	this->add_gradient_horizontal( x, y - size, w, size, main_col.modify_alpha( 0 ), main_col.modify_alpha( alpha_a ) );
	this->add_gradient_horizontal( x, y + h, w, size, main_col.modify_alpha( alpha_a ), main_col.modify_alpha( 0 ) );

	this->add_gradient_vertical( x - size, y, size, h, main_col.modify_alpha( 0 ), main_col.modify_alpha( alpha_a ) );
	this->add_gradient_vertical( x + w, y, size, h, main_col.modify_alpha( alpha_a ), main_col.modify_alpha( 0 ) );

}

void evo::render_t::add_image( int x, int y, int w, int h, evo::macros::texture_id user_texture_id, evo::col_t c ) {
	draw_list->AddImage( user_texture_id, evo::macros::vec_t( x, y ), evo::macros::vec_t( x + w, y + h ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), c.convert( ) );
}

void evo::render_t::bind_animation( int id, std::string text, evo::col_t color, int font, int x, int y, text_animation_t animation_type ) { 
	static int animation_[ 1000 ]; /* iterate */
	
	int w_of_text = this->text_size( text.c_str( ), font ).x;
	int h_of_text = this->text_size( text.c_str( ), font ).y;
	
	switch ( animation_type ) {
		case text_animation_t::left_to_right: {	
			auto animation = animation_controller.get( text + std::to_string( id ) + animation_controller.current_child );
			animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.025f ) * ( ( animation_[ id ] < w_of_text ) ? 1.f : -1.f ) );

			if ( animation.value >= 0.99f ) {
				animation_[ id ] = w_of_text;
			} else if ( animation.value <= 0.f ) {
				animation_[ id ] = 0;
			}
			
			this->clip_rect( x, y, w_of_text * animation.value, h_of_text, [ & ]( ) {
				this->add_text( x, y, color, font, text.c_str( ) );
			} );
		} break;
		case text_animation_t::middle_pulse: {
			auto animation = animation_controller.get( text + std::to_string( id ) + animation_controller.current_child );
			animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.025f ) * ( ( animation_[ id ] < w_of_text ) ? 1.f : -1.f ) );

			if ( animation.value >= 0.99f ) {
				animation_[ id ] = w_of_text;
			} else if ( animation.value <= 0.f ) {
				animation_[ id ] = 0;
			}

			this->clip_rect( x + ( w_of_text / 2 ) - ( ( ( w_of_text * 2 ) / 2 ) * ( animation.value / 2 ) ), y, 
							 ( ( w_of_text * 2 ) / 2 ) * animation.value, h_of_text, [ & ]( ) {
				this->add_text( x, y, color, font, text.c_str( ) );
			} );
		} break;
		case text_animation_t::tiny_color: {
			auto animation = animation_controller.get( text + std::to_string( id ) + animation_controller.current_child );
			animation.adjust( animation.value + 3.f * animation_controller.get_min_deltatime( 0.025f ) * ( ( animation_[ id ] < w_of_text ) ? 1.f : -1.f ) );

			if ( animation.value >= 0.99f ) {
				animation_[ id ] = w_of_text;
			} else if ( animation.value <= 0.f ) {
				animation_[ id ] = 0;
			}

			this->clip_rect( ( x - 15 ) + ( w_of_text * animation.value ), y, 45, h_of_text, [ & ]( ) {
				this->add_text( x, y, color, font, text.c_str( ) );
			} );
		} break;
	}
}

/* blur stuff */
static IDirect3DSurface9* rtBackup = nullptr;
static IDirect3DPixelShader9* blurShaderX = nullptr;
static IDirect3DPixelShader9* blurShaderY = nullptr;
static IDirect3DTexture9* blurTexture = nullptr;
static int backbufferWidth = 0;
static int backbufferHeight = 0;

static void BeginBlur( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {
	const auto device = reinterpret_cast< IDirect3DDevice9* >( cmd->UserCallbackData );

	if ( !blurShaderX ) {
		device->CreatePixelShader( reinterpret_cast< const DWORD* >( blur_x.data( ) ), &blurShaderX );
	}

	if ( !blurShaderY ) {
		device->CreatePixelShader( reinterpret_cast< const DWORD* >( blur_y.data( ) ), &blurShaderY );
	}

	IDirect3DSurface9* backBuffer;
	device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );
	D3DSURFACE_DESC desc;
	backBuffer->GetDesc( &desc );

	if ( backbufferWidth != desc.Width || backbufferHeight != desc.Height ) {
		if ( blurTexture )
			blurTexture->Release( );

		backbufferWidth = desc.Width;
		backbufferHeight = desc.Height;
		device->CreateTexture( desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &blurTexture, nullptr );
	}

	device->GetRenderTarget( 0, &rtBackup );

	{
		IDirect3DSurface9* surface;
		blurTexture->GetSurfaceLevel( 0, &surface );
		device->StretchRect( backBuffer, NULL, surface, NULL, D3DTEXF_NONE );
		device->SetRenderTarget( 0, surface );
		surface->Release( );
	}

	backBuffer->Release( );

	device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
}

static void FirstBlurPass( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {
	const auto device = reinterpret_cast< IDirect3DDevice9* >( cmd->UserCallbackData );

	device->SetPixelShader( blurShaderX );
	const float params[ 4 ] = { 1.0f / backbufferWidth };
	device->SetPixelShaderConstantF( 0, params, 1 );
}

static void SecondBlurPass( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {
	const auto device = reinterpret_cast< IDirect3DDevice9* >( cmd->UserCallbackData );

	device->SetPixelShader( blurShaderY );
	const float params[ 4 ] = { 1.0f / backbufferHeight };
	device->SetPixelShaderConstantF( 0, params, 1 );
}

static void EndBlur( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {
	const auto device = reinterpret_cast< IDirect3DDevice9* >( cmd->UserCallbackData );

	device->SetRenderTarget( 0, rtBackup );
	rtBackup->Release( );

	device->SetPixelShader( nullptr );
	device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
}

void evo::render_t::draw_backround_blur( float alpha ) { 
	draw_list->AddCallback( BeginBlur, theme::blur_device );

	for ( int i = 0; i < 8; ++i ) {
		draw_list->AddCallback( FirstBlurPass, theme::blur_device );
		draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32( 255, 255, 255, 255 * alpha ), 5.f );
		draw_list->AddCallback( SecondBlurPass, theme::blur_device );
		draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32( 255, 255, 255, 255 * alpha ), 5.f );
	}

	draw_list->AddCallback( EndBlur, theme::blur_device );
	draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32( 255, 255, 255, 255 * alpha ), 5.f );
}

void evo::render_t::draw_backround_blur_custom( float alpha, evo::col_t color ) { 
	draw_list->AddCallback( BeginBlur, theme::blur_device );

	for ( int i = 0; i < 8; ++i ) {
		draw_list->AddCallback( FirstBlurPass, theme::blur_device );
		draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, color.modify_alpha( 255 * alpha ).convert( ), 5.f );
		draw_list->AddCallback( SecondBlurPass, theme::blur_device );
		draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, color.modify_alpha( 255 * alpha ).convert( ), 5.f );
	}

	draw_list->AddCallback( EndBlur, theme::blur_device );
	draw_list->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, color.modify_alpha( 255 * alpha ).convert( ), 5.f );

}

void evo::render_t::clip_rect( float x, float y, float w, float h, std::function< void( ) > function ) {
	draw_list->PushClipRect( { x, y }, { x + w, y + h }, true );

	function( );

	draw_list->PopClipRect( );
}

void evo::external_macro_t::make_rect_filled( int x, int y, int w, int h, col_t c, int r, bool overwrite ) {
	if ( overwrite ) {
		externals::external.rect_filled_a.insert( externals::external.rect_filled_a.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	} else {
		externals::external.rect_filled.insert( externals::external.rect_filled.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	}
}

void evo::external_macro_t::make_rect_filledex( int x, int y, int w, int h, col_t c, int r, bool overwrite ) {
	if ( overwrite ) {
		externals::external.rect_filled_a2.insert( externals::external.rect_filled_a2.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	} else {
		externals::external.rect_filled.insert( externals::external.rect_filled.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	}
}

void evo::external_macro_t::make_rect( int x, int y, int w, int h, col_t c, int r, int t, bool overwrite ) {
	if ( overwrite ) {
		externals::external.rect_a.insert( externals::external.rect_a.begin( ), externals::draw_thick_rect( x, y, w, h, c, r, t ) );
	} else {
		externals::external.rect.insert( externals::external.rect.begin( ), externals::draw_thick_rect( x, y, w, h, c, r, t ) );
	}
}

void evo::external_macro_t::make_text( int x, int y, col_t c, int fi, std::string text, font_flags_t flags ) {
	externals::external.text.insert( externals::external.text.begin( ), externals::draw_text( x, y, text, c, fi, flags ) );
}

void evo::external_macro_t::make_rect_filled_shadow( int x, int y, int w, int h, col_t c, int r, int s, bool overwrite ) {
	if ( overwrite ) {
		externals::external.rect_filled_shd_a.insert( externals::external.rect_filled_shd_a.begin( ), externals::draw_rect_shd( x, y, w, h, c, r, s ) );
	} else {
		externals::external.rect_filled_shd.insert( externals::external.rect_filled_shd.begin( ), externals::draw_rect_shd( x, y, w, h, c, r, s ) );
	}
}

void evo::external_macro_t2::make_rect_filled( int x, int y, int w, int h, col_t c, int r, bool overwrite ) {
	if ( overwrite ) {
		externals::pop_external.rect_filled_a.insert( externals::pop_external.rect_filled_a.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	} else {
		externals::pop_external.rect_filled.insert( externals::pop_external.rect_filled.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	}
}

void evo::external_macro_t2::make_rect_filledex( int x, int y, int w, int h, col_t c, int r, bool overwrite ) {
	if ( overwrite ) {
		externals::pop_external.rect_filled_a2.insert( externals::pop_external.rect_filled_a2.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	} else {
		externals::pop_external.rect_filled.insert( externals::pop_external.rect_filled.begin( ), externals::draw_rect( x, y, w, h, c, r ) );
	}
}

void evo::external_macro_t2::make_rect( int x, int y, int w, int h, col_t c, int r, int t, bool overwrite ) {
	if ( overwrite ) {
		externals::pop_external.rect_a.insert( externals::pop_external.rect_a.begin( ), externals::draw_thick_rect( x, y, w, h, c, r, t ) );
	} else {
		externals::pop_external.rect.insert( externals::pop_external.rect.begin( ), externals::draw_thick_rect( x, y, w, h, c, r, t ) );
	}
}

void evo::external_macro_t2::make_text( int x, int y, col_t c, int fi, std::string text, font_flags_t flags ) {
	externals::pop_external.text.insert( externals::pop_external.text.begin( ), externals::draw_text( x, y, text, c, fi, flags ) );
}

void evo::external_macro_t2::make_rect_filled_shadow( int x, int y, int w, int h, col_t c, int r, int s, bool overwrite ) {
	if ( overwrite ) {
		externals::pop_external.rect_filled_shd_a.insert( externals::pop_external.rect_filled_shd_a.begin( ), externals::draw_rect_shd( x, y, w, h, c, r, s ) );
	} else {
		externals::pop_external.rect_filled_shd.insert( externals::pop_external.rect_filled_shd.begin( ), externals::draw_rect_shd( x, y, w, h, c, r, s ) );
	}
}