#include "impl.hpp"

using Microsoft::WRL::ComPtr;

static int backbufferWidth = 0;
static int backbufferHeight = 0;

static IDirect3DDevice9* device;

[[nodiscard]] static IDirect3DTexture9* create_texture( int width, int height ) noexcept
{
	IDirect3DTexture9* texture;
	device->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, nullptr );
	return texture;
}

static void copy_back_buffer_to_texture( IDirect3DTexture9* texture, D3DTEXTUREFILTERTYPE filtering ) noexcept
{
	ComPtr< IDirect3DSurface9 > backBuffer;
	if ( device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, backBuffer.GetAddressOf( ) ) == D3D_OK )
	{
		ComPtr< IDirect3DSurface9 > surface;
		if ( texture->GetSurfaceLevel( 0, surface.GetAddressOf( ) ) == D3D_OK )
			device->StretchRect( backBuffer.Get( ), nullptr, surface.Get( ), nullptr, filtering );
	}
}

static void set_render_target( IDirect3DTexture9* rtTexture ) noexcept
{
	ComPtr< IDirect3DSurface9 > surface;
	if ( rtTexture->GetSurfaceLevel( 0, surface.GetAddressOf( ) ) == D3D_OK )
		device->SetRenderTarget( 0, surface.Get( ) );
}

class c_shader_program {
public:
	void use( float uniform, int location ) const noexcept
	{
		device->SetPixelShader( pixel_shader.Get( ) );
		const float params[4] = { uniform };
		device->SetPixelShaderConstantF( location, params, 1 );
	}

	void init( const BYTE* pixelShaderSrc ) noexcept
	{
		if ( initialized )
			return;

		initialized = true;
		device->CreatePixelShader( reinterpret_cast<const DWORD*>( pixelShaderSrc ), pixel_shader.GetAddressOf( ) );
	}

private:
	ComPtr< IDirect3DPixelShader9 > pixel_shader{};
	bool initialized = false;
};

class c_blur_effect {
public:
	static void draw( ImDrawList* drawList, ImVec2 min, ImVec2 max, ImColor col, float rounding, ImDrawFlags round_flags ) noexcept
	{
		instance( )._draw( drawList, min, max, col, rounding, round_flags );
	}

	static void clear_textures( ) noexcept
	{
		if ( instance( ).blur_texture[0] )
		{
			instance( ).blur_texture[0]->Release( );
			instance( ).blur_texture[0] = nullptr;
		}

		if ( instance( ).blur_texture[1] )
		{
			instance( ).blur_texture[1]->Release( );
			instance( ).blur_texture[1] = nullptr;
		}
	}

	static void create_textures( ) noexcept
	{
		if ( !instance( ).blur_texture[0] )
			instance( ).blur_texture[0] = create_texture( backbufferWidth / blur_down_sample, backbufferHeight / blur_down_sample );
		if ( !instance( ).blur_texture[1] )
			instance( ).blur_texture[1] = create_texture( backbufferWidth / blur_down_sample, backbufferHeight / blur_down_sample );
	}

	static void create_shaders() noexcept
	{
		instance( ).blur_shaderx.init( blur_x );
		instance( ).blur_shadery.init( blur_y );
	}

private:
	D3DMATRIX backup_matrix{};

	LPDIRECT3DSURFACE9 rt_backup = nullptr;
	IDirect3DTexture9* blur_texture[2];

	c_shader_program blur_shaderx{};
	c_shader_program blur_shadery{};
	static constexpr auto blur_down_sample = 9;

	c_blur_effect( ) = default;
	c_blur_effect( const c_blur_effect& ) = delete;

	~c_blur_effect( )
	{
		if ( rt_backup )
			rt_backup->Release( );
		if ( blur_texture[0] )
			blur_texture[0]->Release( );
		if ( blur_texture[1] )
			blur_texture[1]->Release( );
	}

	static c_blur_effect& instance( ) noexcept
	{
		static c_blur_effect blurEffect;
		return blurEffect;
	}

	static void begin( const ImDrawList*, const ImDrawCmd* ) noexcept
	{
		instance( )._begin( );
	}
	static void first_pass( const ImDrawList*, const ImDrawCmd* ) noexcept
	{
		instance( )._first_pass( );
	}
	static void second_pass( const ImDrawList*, const ImDrawCmd*) noexcept
	{
		instance( )._second_pass( );
	}
	static void end( const ImDrawList*, const ImDrawCmd* ) noexcept
	{
		instance( )._end( );
	}

	void _begin( ) noexcept
	{
		device->GetRenderTarget( 0, &rt_backup );
		copy_back_buffer_to_texture( blur_texture[0], D3DTEXF_LINEAR );
		device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		device->SetRenderState( D3DRS_SCISSORTESTENABLE, false );

		device->GetVertexShaderConstantF( 0, &backup_matrix.m[0][0], 4 );

		const D3DMATRIX projection{ { { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f / ( backbufferWidth / blur_down_sample ), 1.0f / ( backbufferHeight / blur_down_sample ), 0.0f, 1.0f } } };
		device->SetVertexShaderConstantF( 0, &projection.m[0][0], 4 );
	}

	void _first_pass( ) noexcept
	{
		blur_shaderx.use( 1.0f / ( backbufferWidth / blur_down_sample ), 0 );
		set_render_target( blur_texture[1] );
	}

	void _second_pass( ) noexcept
	{
		blur_shadery.use( 1.0f / ( backbufferHeight / blur_down_sample ), 0 );
		set_render_target( blur_texture[0] );
	}

	void _end( ) noexcept
	{
		device->SetRenderTarget( 0, rt_backup );
		rt_backup->Release( );

		device->SetPixelShader( nullptr );
		device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
	}

	void _draw( ImDrawList* d, ImVec2 min, ImVec2 max, ImColor col, float rounding, ImDrawFlags round_flags = ImDrawFlags_RoundCornersAll ) noexcept
	{
		create_textures( );
		create_shaders( );

		if ( !blur_texture[0] || !blur_texture[1] )
			return;

		d->AddCallback( &begin, nullptr );
		for ( int i = 0; i < 8; ++i ) {
			d->AddCallback( &first_pass, nullptr );
			d->AddImage( blur_texture[0], { -1.0f, -1.0f }, { 1.0f, 1.0f } );
			d->AddCallback( &second_pass, nullptr );
			d->AddImage( blur_texture[1], { -1.0f, -1.0f }, { 1.0f, 1.0f } );
		}
		d->AddCallback( &end, nullptr );
		d->AddCallback( ImDrawCallback_ResetRenderState, nullptr );
		d->AddImageRounded( blur_texture[0], min, max, { min.x / backbufferWidth, min.y / backbufferHeight }, { max.x / backbufferWidth, max.y / backbufferHeight }, ImColor( col.Value.x, col.Value.y, col.Value.z, col.Value.w * GImGui->Style.Alpha ), rounding, round_flags );
	}
};

void imgui_blur::set_device( LPDIRECT3DDEVICE9 device ) noexcept
{
	::device = device;
}

void imgui_blur::clear_blur_textures( ) noexcept
{
	c_blur_effect::clear_textures( );
}

void imgui_blur::on_device_reset( ) noexcept
{
	c_blur_effect::clear_textures( );
}

void imgui_blur::new_frame( ) noexcept
{
	const int width = ImGui::GetIO().DisplaySize.x;
	const int height = ImGui::GetIO().DisplaySize.y;
	if ( backbufferWidth != static_cast<int>( width ) || backbufferHeight != static_cast<int>( height ) ) {
		c_blur_effect::clear_textures( );
		backbufferWidth = static_cast<int>( width );
		backbufferHeight = static_cast<int>( height );
		return;
	}
}

void imgui_blur::create_blur( ImDrawList* d, ImVec2 min, ImVec2 max, ImColor col, float rounding, ImDrawFlags round_flags ) noexcept 
{
	c_blur_effect::draw( d, min, max, col, rounding, round_flags );
}