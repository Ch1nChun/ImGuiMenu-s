#include "blur.hpp"

#include "blur_x.hpp"
#include "blur_y.hpp"

static IDirect3DSurface9* rtBackup = nullptr;
static IDirect3DPixelShader9* blurShaderX = nullptr;
static IDirect3DPixelShader9* blurShaderY = nullptr;
static IDirect3DTexture9* blurTexture = nullptr;
static int backbufferWidth = 0;
static int backbufferHeight = 0;

static void begin_blur( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {

    const auto device = reinterpret_cast<IDirect3DDevice9*>( cmd->UserCallbackData );

    if ( !blurShaderX ) {

        device->CreatePixelShader( reinterpret_cast<const DWORD*>( blur_x.data( ) ), &blurShaderX );
    }

    if ( !blurShaderY ) {

        device->CreatePixelShader(reinterpret_cast<const DWORD*>( blur_y.data( ) ), &blurShaderY );
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

static void first_blur_pass( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {

    const auto device = reinterpret_cast<IDirect3DDevice9*>( cmd->UserCallbackData );

    device->SetPixelShader( blurShaderX );
    const float params[4] = { 1.0f / backbufferWidth };
    device->SetPixelShaderConstantF( 0, params, 1 );
}

static void second_blur_pass( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {

    const auto device = reinterpret_cast<IDirect3DDevice9*>( cmd->UserCallbackData );

    device->SetPixelShader( blurShaderY );
    const float params[4] = { 1.0f / backbufferHeight };
    device->SetPixelShaderConstantF( 0, params, 1 );
}

static void end_blur( const ImDrawList* parent_list, const ImDrawCmd* cmd ) {

    const auto device = reinterpret_cast<IDirect3DDevice9*>( cmd->UserCallbackData );

    device->SetRenderTarget( 0, rtBackup );
    rtBackup->Release( );

    device->SetPixelShader( nullptr );
    device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
}

inline void draw_blur( ImDrawList* drawList ) {

    drawList->AddCallback( begin_blur, blur::device );

    for ( int i = 0; i < 8; ++i ) {

        drawList->AddCallback( first_blur_pass, blur::device );
        drawList->AddImage( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f } );
        drawList->AddCallback( second_blur_pass, blur::device);
        drawList->AddImage( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f } );
    }

    drawList->AddCallback( end_blur, blur::device );
    drawList->AddImageRounded( blurTexture, { 0.0f, 0.0f }, { backbufferWidth * 1.0f, backbufferHeight * 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32( 255, 255, 255, 255 ), 7.f );
}
