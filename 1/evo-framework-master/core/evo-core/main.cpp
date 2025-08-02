#include "../inc.hpp"

static LPDIRECT3D9 d3d = nullptr;
static LPDIRECT3DDEVICE9 d3d_device = nullptr;
static D3DPRESENT_PARAMETERS d3d_parameters = {};
//static HWND window = nullptr;

bool create_device( HWND hwnd );
void cleanup_device( );
void reset_device( );

LRESULT WINAPI window_process( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );

int main( ) {
	/* create window */
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, window_process, 0L, 0L, GetModuleHandleA( nullptr ), nullptr, nullptr, nullptr, nullptr, "evo-frmk", nullptr };
	::RegisterClassExA( &wc );
	HWND hwnd = ::CreateWindowEx( WS_EX_APPWINDOW, wc.lpszClassName, "evo-frmk", WS_POPUP, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr );

	if ( !create_device( hwnd ) ) {
		cleanup_device( );
		::UnregisterClass( wc.lpszClassName, wc.hInstance );
		return 1;
	}

	::ShowWindow( hwnd, SW_SHOWDEFAULT );
	::UpdateWindow( hwnd );

	D3DDEVICE_CREATION_PARAMETERS params;
	d3d_device->GetCreationParameters( &params );
	evo::_render->initialize_imgui( params.hFocusWindow, d3d_device );

	evo::theme::blur_device = d3d_device;

	MSG msg;
	memset( &msg, 0, sizeof( msg ) );

	while ( msg.message != WM_QUIT ) {
		if ( ::PeekMessageA( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessageA( &msg );
			continue;
		}
		
		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );
		{
			evo::_render->add_image( 0, 0, 1920, 1090, backround, evo::col_t() );

			evo::_menu->init( );
			
			evo::_render->add_text( 10, 10, evo::col_t( ), 1, "Counter-Strike 2", 0 );
		}
		ImGui::EndFrame( );

		/* set some shit */
		d3d_device->SetVertexShader( nullptr );
		d3d_device->SetPixelShader( nullptr );
		d3d_device->SetVertexShader( nullptr );
		d3d_device->SetPixelShader( nullptr );
		d3d_device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		d3d_device->SetRenderState( D3DRS_LIGHTING, false );
		d3d_device->SetRenderState( D3DRS_FOGENABLE, false );
		d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		d3d_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		d3d_device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		d3d_device->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
		d3d_device->SetRenderState( D3DRS_ZWRITEENABLE, false );
		d3d_device->SetRenderState( D3DRS_STENCILENABLE, false );
		d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		d3d_device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
		d3d_device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, true );
		d3d_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3d_device->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
		d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		d3d_device->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
		d3d_device->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
		d3d_device->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
		d3d_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
		d3d_device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );

		d3d_device->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA( 0, 0, 0, 255 ), 1.0f, 0 );

		/* begin scene */
		if ( d3d_device->BeginScene( ) >= 0 ) {
			/* init imgui */
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

			if ( GetAsyncKeyState( VK_END ) ) {
				exit( 0 );
			}

			/* end scene */
			d3d_device->EndScene( );
		}

		/* reset */
		HRESULT result = d3d_device->Present( nullptr, nullptr, nullptr, nullptr );
		if ( result == D3DERR_DEVICELOST && d3d_device->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			reset_device( );
	}

	cleanup_device( );
	::DestroyWindow( hwnd );
	::UnregisterClassA( wc.lpszClassName, wc.hInstance );

	return 0;
}

bool create_device( HWND hwnd ) {
	if ( !( d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	// Create the D3DDevice
	ZeroMemory( &d3d_parameters, sizeof( d3d_parameters ) );
	d3d_parameters.Windowed = TRUE;
	d3d_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	d3d_parameters.EnableAutoDepthStencil = TRUE;
	d3d_parameters.AutoDepthStencilFormat = D3DFMT_D16;
	d3d_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if ( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3d_parameters, &d3d_device ) < 0 )
		return false;

	return true;
}

void cleanup_device( ) {
	if ( d3d_device ) { d3d_device->Release( ); d3d_device = nullptr; }
	if ( d3d ) { d3d->Release( ); d3d = nullptr; }
}

void reset_device( ) {
	HRESULT hr = d3d_device->Reset( &d3d_parameters );

	if ( hr == D3DERR_INVALIDCALL )
		abort( );
}

LRESULT WINAPI window_process( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp ) {
	if ( msg == WM_MOUSEMOVE )
		evo::_input->set_mouse_position( evo::vec2_t( GET_X_LPARAM( lp ), GET_Y_LPARAM( lp ) ) );

	if ( msg == WM_MOUSEWHEEL )
		evo::_input->set_mouse_wheel( GET_WHEEL_DELTA_WPARAM( wp ) / WHEEL_DELTA );

	/* return original */
	auto ret = ::DefWindowProcA( hwnd, msg, wp, lp );
	return ret;
}