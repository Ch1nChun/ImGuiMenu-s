#pragma once
#include "../../inc.hpp"

#include <cstdio>
#include <array>

template <std::size_t N>
constexpr auto DecodeBase85( const char( &input )[ N ] ) noexcept {
	std::array<char, N * 4 / 5> out{};

	constexpr auto decode85Byte = [ ]( char c ) constexpr -> unsigned int { return c >= '\\' ? c - 36 : c - 35; };

	for ( std::size_t i = 0, j = 0; i < N - 1; i += 5, j += 4 ) {
		unsigned int tmp = decode85Byte( input[ i ] ) + 85 * ( decode85Byte( input[ i + 1 ] ) + 85 * ( decode85Byte( input[ i + 2 ] ) + 85 * ( decode85Byte( input[ i + 3 ] ) + 85 * decode85Byte( input[ i + 4 ] ) ) ) );
		out[ j ] = ( ( tmp >> 0 ) & 0xFF ); out[ j + 1 ] = ( ( tmp >> 8 ) & 0xFF ); out[ j + 2 ] = ( ( tmp >> 16 ) & 0xFF ); out[ j + 3 ] = ( ( tmp >> 24 ) & 0xFF );
	}

	return out;
}

// File: 'blur_x' (600 bytes)
// Exported using binary_to_compressed_c.cpp
inline constexpr auto blur_x = DecodeBase85(
	"%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####$&###J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
	"`-.:CVjSuY&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHVd;#K7#####X/'.7`7r'7$QUV$*%XHV"
	"d:i[7bmhf6##########D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#W.]/Lpu$GV+>uu#PYr.LOV%JLou$GV&5YY#Q`%/Lpv*PV(>uu#Sf./L5hJcLdfIl["
	"(>uu#Rf./L4_/GLdfIl[&5YY#Y.]/Lqu$GV+>uu#RYr.LQV%JLou$GV&5YY#S`%/Lpv*PV(>uu#Uf./L7hJcLdfIl[(>uu#Tf./L6_/GLdfIl[i>uu#_XF0L4_/GL[BA`Vi>uu#`XF0L"
	"5_/GL[BA`Vi>uu#aXF0L6_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#W(S/L5_/GLpw0YV+G:;$W(S/L3_/GLpx6cV5_/GL+G:;$V(S/L4_/GLpw0YV5_/GL+G:;$V(S/L7_/GLqv*PV"
	"4_/GL+G:;$U(S/L6_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##" );

inline constexpr auto blur_y = DecodeBase85(
	"%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####$&###J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
	"Qk;nDhpF/#&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHVd;#K7NSi.LX/'.7`7r'7$QUV$*%XHV"
	"d:i[7bmhf6##########D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#W.]/Lpu$GV+>uu#PYr.LOV%JLou$GV&5YY#Q`%/LP].JL&5YY#PYr.Lpv*PV(>uu#"
	"Rf./L4_/GLdfIl[&5YY#QYr.L)[-S[+G:;$R`%/Lou$GVOV%JL)]3][&5YY#Y.]/Lqu$GV+>uu#Sl7/LQV%JLou$GV&5YY#S`%/LP_:]L&5YY#RYr.Lpv*PV(>uu#Tf./L6_/GLdfIl["
	"&5YY#SYr.L)[-S[+G:;$T`%/Lou$GVQV%JL)]3][i>uu#_XF0L4_/GL[BA`Vi>uu#`XF0L5_/GL[BA`Vi>uu#aXF0L6_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#V(S/L4_/GLpw0YV"
	"+G:;$V(S/L3_/GLpx6cV4_/GL+G:;$V(S/L5_/GLpw0YV4_/GL+G:;$V(S/L6_/GLqv*PV4_/GL+G:;$U(S/L7_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##" );

extern IDirect3DTexture9* backround;
extern IDirect3DTexture9* preview;

namespace evo {
	enum fonts_t {
		_default,
		_default2,
		_icons,
		_arrows,
		_icons_2
	};

	enum font_flags_t {
		none,
		dropshadow,
		dropshadow_low,
		outline
	};

	enum text_animation_t {
		left_to_right,
		middle_pulse,
		tiny_color
	};

	class render_t {
	public:
		void initialize_font_system( );
		void initialize_imgui( void* w, IDirect3DDevice9* device );

		void add_text( int x, int y, col_t c, int font, const char* text, int /* or font_flags_t */ flag = font_flags_t::none );
		void add_rect_filled( int x, int y, int w, int h, evo::col_t col, int round );
		void add_rect_filled_shadowed( int x, int y, int w, int h, evo::col_t col, int round, int thickness );
		void add_rect( int x, int y, int w, int h, evo::col_t col, int round, int tickness );
		void add_filled_circle( evo::vec2_t center, float radius, evo::col_t c );
		void add_gradient_circle_filled( evo::vec2_t c, int radius, evo::col_t inner_color, evo::col_t outer_color );
		void add_gradient_vertical( int x, int y, int w, int h, evo::col_t c1, evo::col_t c2, int rounding = 0, ImDrawFlags flags = ImDrawFlags_RoundCornersAll );
		void add_gradient_horizontal( int x, int y, int w, int h, evo::col_t c1, evo::col_t c2, int rounding = 0, ImDrawFlags flags = ImDrawFlags_RoundCornersAll );
		void add_drop_shadow( int x, int y, int w, int h, evo::col_t main_col, int size, int alpha_a );
		void add_image( int x, int y, int w, int h, evo::macros::texture_id user_texture_id, evo::col_t c );

		void bind_animation( int id, std::string text, evo::col_t color, int font, int x, int y, text_animation_t animation_type );

		void draw_backround_blur( float alpha );
		void draw_backround_blur_custom( float alpha, evo::col_t color );

		void clip_rect( float x, float y, float w, float h, std::function<void( )> function );

		evo::vec2_t text_size( const char* text, int font );
		std::vector<ImFont*> fonts{};
	};

	inline const auto _render = std::make_unique<render_t>( );

	namespace externals {
		struct draw_rect {
			draw_rect( int pos_x, int pos_y, int width, int height, evo::col_t c, int rounding = 0 ) {
				this->pos_x = pos_x;
				this->pos_y = pos_y;
				this->width = width;
				this->height = height;
				this->c = c;
				this->rounding = rounding;
			}

			int pos_x;
			int pos_y;
			int width;
			int height;
			int rounding;
			col_t c;
		};

		struct draw_rect_shd {
			draw_rect_shd( int pos_x, int pos_y, int width, int height, evo::col_t c, int rounding = 0, int shd = 15 ) {
				this->pos_x = pos_x;
				this->pos_y = pos_y;
				this->width = width;
				this->height = height;
				this->c = c;
				this->rounding = rounding;
				this->shd = shd;
			}

			int pos_x;
			int pos_y;
			int width;
			int height;
			int rounding;
			int shd;
			col_t c;
		};

		struct draw_circle {
			draw_circle( ImVec2 pos, float radius, ImColor color, float thickness ) {
				this->pos = pos;
				this->radius = radius;
				this->color = color;
				this->thickness = thickness;
			}

			ImVec2 pos;
			ImColor color;
			float radius;
			float thickness;
		};

		struct draw_circle_fill {
			draw_circle_fill( ImVec2 pos, float radius, ImColor color ) {
				this->pos = pos;
				this->radius = radius;
				this->color = color;
			}

			ImVec2 pos;
			ImColor color;
			float radius;
		};

		struct draw_thick_rect {
			draw_thick_rect( int x, int y, int w, int h, evo::col_t c, int r, int t ) {
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->c = c;
				this->r = r;
				this->t = t;
			}

			int x, y, w, h, r, t;
			col_t c;
		};

		struct draw_image {
			draw_image( int x, int y, int w, int h, ImTextureID texture, col_t c ) {
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->texture = texture;
				this->c = c;
			}

			int x, y, w, h;
			col_t c;
			ImTextureID texture;
		};

		struct draw_text {
			draw_text( float pos_x, float pos_y, std::string text, col_t c, int font_index = 0, font_flags_t flags = font_flags_t::none ) {
				this->pos_x = pos_x;
				this->pos_y = pos_y;
				this->text = text;
				this->c = c;
				this->font_index = font_index;
				this->flags = flags;
			}

			float pos_x;
			float pos_y;
			int font_index;
			font_flags_t flags;
			std::string text;
			col_t c;
		};

		struct draw_gradient {
			draw_gradient( int pos_x, int pos_y, int width, int height, col_t c, col_t c1, bool change, int r, ImDrawFlags flags = ImDrawFlags_RoundCornersAll ) {
				this->pos_x = pos_x;
				this->pos_y = pos_y;
				this->width = width;
				this->height = height;
				this->c = c;
				this->c1 = c1;
				this->change = change;
				this->r = r;
				this->flags = flags;
			}

			int pos_x;
			int pos_y;
			int width;
			int height;
			int r;
			bool change;
			col_t c;
			col_t c1;
			ImDrawFlags flags;
		};

		struct externals_t {
			/* normal draw */
			std::vector< draw_rect > rect_filled{};
			std::vector< draw_rect_shd > rect_filled_shd{};
			std::vector< draw_thick_rect > rect{};
			std::vector< draw_gradient > gradient{};
			std::vector< draw_circle > circle{};
			std::vector< draw_circle_fill > fcircle{};
			std::vector< draw_thick_rect > t_rect{};

			/* after draw */
			std::vector< draw_rect > rect_filled_a{};
			std::vector< draw_rect > rect_filled_a2{};
			std::vector< draw_rect_shd > rect_filled_shd_a{};
			std::vector< draw_thick_rect > rect_a{};
			std::vector< draw_gradient > gradient_a{};
			std::vector< draw_circle > circle_a{};
			std::vector< draw_circle_fill > fcircle_a{};
			std::vector< draw_thick_rect > t_rect_a{};

			std::vector< draw_text > text{};
			std::vector< draw_image > image{};
		}; inline externals_t external;

		struct externals_pop_t {
			/* normal draw */
			std::vector< draw_rect > rect_filled{};
			std::vector< draw_rect_shd > rect_filled_shd{};
			std::vector< draw_thick_rect > rect{};
			std::vector< draw_gradient > gradient{};
			std::vector< draw_circle > circle{};
			std::vector< draw_circle_fill > fcircle{};
			std::vector< draw_thick_rect > t_rect{};

			/* after draw */
			std::vector< draw_rect > rect_filled_a{};
			std::vector< draw_rect > rect_filled_a2{};
			std::vector< draw_rect_shd > rect_filled_shd_a{};
			std::vector< draw_thick_rect > rect_a{};
			std::vector< draw_gradient > gradient_a{};
			std::vector< draw_circle > circle_a{};
			std::vector< draw_circle_fill > fcircle_a{};
			std::vector< draw_thick_rect > t_rect_a{};

			std::vector< draw_text > text{};
			std::vector< draw_image > image{};
		}; inline externals_pop_t pop_external;

		class popup_external_drawing {
		public:
			__forceinline void begin_popup( ) {
				for ( size_t i{ 0 }; i < external.rect_filled.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled[ i ].pos_x, external.rect_filled[ i ].pos_y,
											  external.rect_filled[ i ].width, external.rect_filled[ i ].height, external.rect_filled[ i ].c, external.rect_filled[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_shd.size( ); i++ ) {
					_render->add_rect_filled_shadowed( external.rect_filled_shd[ i ].pos_x, external.rect_filled_shd[ i ].pos_y,
													   external.rect_filled_shd[ i ].width, external.rect_filled_shd[ i ].height, external.rect_filled_shd[ i ].c, external.rect_filled_shd[ i ].rounding, external.rect_filled_shd[ i ].shd );
				}

				for ( size_t i{ 0 }; i < external.rect.size( ); i++ ) {
					_render->add_rect( external.rect[ i ].x, external.rect[ i ].y,
									   external.rect[ i ].w, external.rect[ i ].h, external.rect[ i ].c, external.rect[ i ].r, external.rect[ i ].t );
				}

				for ( size_t i{ 0 }; i < external.gradient.size( ); i++ ) {
					if ( external.gradient[ i ].change ) {
						_render->add_gradient_horizontal( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
														  external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1, external.gradient[ i ].r, external.gradient[ i ].flags );
					} else {
						_render->add_gradient_vertical( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
														external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1, external.gradient[ i ].r, external.gradient[ i ].flags );
					}
				}

				for ( size_t i{ 0 }; i < external.circle.size( ); i++ ) {
					draw_list->AddCircle( external.circle[ i ].pos, external.circle[ i ].radius, external.circle[ i ].color, 0, external.circle[ i ].thickness );
				}

				for ( size_t i{ 0 }; i < external.fcircle.size( ); i++ ) {
					draw_list->AddCircleFilled( external.fcircle[ i ].pos, external.fcircle[ i ].radius, external.fcircle[ i ].color, 0 );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_a.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled_a[ i ].pos_x, external.rect_filled_a[ i ].pos_y,
											  external.rect_filled_a[ i ].width, external.rect_filled_a[ i ].height, external.rect_filled_a[ i ].c, external.rect_filled_a[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_a2.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled_a2[ i ].pos_x, external.rect_filled_a2[ i ].pos_y,
											  external.rect_filled_a2[ i ].width, external.rect_filled_a2[ i ].height, external.rect_filled_a2[ i ].c, external.rect_filled_a2[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_shd_a.size( ); i++ ) {
					_render->add_rect_filled_shadowed( external.rect_filled_shd_a[ i ].pos_x, external.rect_filled_shd_a[ i ].pos_y,
													   external.rect_filled_shd_a[ i ].width, external.rect_filled_shd_a[ i ].height, external.rect_filled_shd_a[ i ].c, external.rect_filled_shd_a[ i ].rounding, external.rect_filled_shd_a[ i ].shd );
				}

				for ( size_t i{ 0 }; i < external.rect_a.size( ); i++ ) {
					_render->add_rect( external.rect_a[ i ].x, external.rect_a[ i ].y,
									   external.rect_a[ i ].w, external.rect_a[ i ].h, external.rect_a[ i ].c, external.rect_a[ i ].r, external.rect_a[ i ].t );
				}

				for ( size_t i{ 0 }; i < external.gradient_a.size( ); i++ ) {
					if ( external.gradient_a[ i ].change ) {
						_render->add_gradient_horizontal( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
														  external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1, external.gradient_a[ i ].r, external.gradient_a[ i ].flags );
					} else {
						_render->add_gradient_vertical( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
														external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1, external.gradient_a[ i ].r, external.gradient_a[ i ].flags );
					}
				}

				for ( size_t i{ 0 }; i < external.circle_a.size( ); i++ ) {
					draw_list->AddCircle( external.circle_a[ i ].pos, external.circle_a[ i ].radius, external.circle_a[ i ].color, 0, external.circle_a[ i ].thickness );
				}

				for ( size_t i{ 0 }; i < external.fcircle_a.size( ); i++ ) {
					draw_list->AddCircleFilled( external.fcircle_a[ i ].pos, external.fcircle_a[ i ].radius, external.fcircle_a[ i ].color, 0 );
				}

				for ( size_t i{ 0 }; i < external.text.size( ); i++ ) {
					_render->add_text( external.text[ i ].pos_x, external.text[ i ].pos_y,
									   external.text[ i ].c, external.text[ i ].font_index, external.text[ i ].text.c_str( ), external.text[ i ].flags );
				}

				for ( size_t i{ 0 }; i < external.image.size( ); i++ ) {
					_render->add_image( external.image[ i ].x, external.image[ i ].y, external.image[ i ].w, external.image[ i ].h,
										external.image[ i ].texture, external.image[ i ].c );
				}

				/* clear */
				external.rect_filled.clear( );
				external.rect_filled_shd.clear( );
				external.rect.clear( );
				external.gradient.clear( );
				external.circle.clear( );
				external.fcircle.clear( );
				external.t_rect.clear( );
				external.rect_filled_a.clear( );
				external.rect_filled_a2.clear( );
				external.rect_filled_shd_a.clear( );
				external.gradient_a.clear( );
				external.rect_a.clear( );
				external.circle_a.clear( );
				external.fcircle_a.clear( );
				external.t_rect_a.clear( );
				external.text.clear( );
				external.image.clear( );
			}
		};

		class external_drawing {
		public:
			__forceinline void begin( ) {
				for ( size_t i{ 0 }; i < external.rect_filled.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled[ i ].pos_x, external.rect_filled[ i ].pos_y,
											  external.rect_filled[ i ].width, external.rect_filled[ i ].height, external.rect_filled[ i ].c, external.rect_filled[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_shd.size( ); i++ ) {
					_render->add_rect_filled_shadowed( external.rect_filled_shd[ i ].pos_x, external.rect_filled_shd[ i ].pos_y,
													   external.rect_filled_shd[ i ].width, external.rect_filled_shd[ i ].height, external.rect_filled_shd[ i ].c, external.rect_filled_shd[ i ].rounding, external.rect_filled_shd[ i ].shd );
				}

				for ( size_t i{ 0 }; i < external.rect.size( ); i++ ) {
					_render->add_rect( external.rect[ i ].x, external.rect[ i ].y,
									   external.rect[ i ].w, external.rect[ i ].h, external.rect[ i ].c, external.rect[ i ].r, external.rect[ i ].t );
				}

				for ( size_t i{ 0 }; i < external.gradient.size( ); i++ ) {
					if ( external.gradient[ i ].change ) {
						_render->add_gradient_horizontal( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
														  external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1, external.gradient[ i ].r, external.gradient[ i ].flags );
					} else {
						_render->add_gradient_vertical( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
														external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1, external.gradient[ i ].r, external.gradient[ i ].flags );
					}
				}

				for ( size_t i{ 0 }; i < external.circle.size( ); i++ ) {
					draw_list->AddCircle( external.circle[ i ].pos, external.circle[ i ].radius, external.circle[ i ].color, 0, external.circle[ i ].thickness );
				}

				for ( size_t i{ 0 }; i < external.fcircle.size( ); i++ ) {
					draw_list->AddCircleFilled( external.fcircle[ i ].pos, external.fcircle[ i ].radius, external.fcircle[ i ].color, 0 );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_a.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled_a[ i ].pos_x, external.rect_filled_a[ i ].pos_y,
											  external.rect_filled_a[ i ].width, external.rect_filled_a[ i ].height, external.rect_filled_a[ i ].c, external.rect_filled_a[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_a2.size( ); i++ ) {
					_render->add_rect_filled( external.rect_filled_a2[ i ].pos_x, external.rect_filled_a2[ i ].pos_y,
											  external.rect_filled_a2[ i ].width, external.rect_filled_a2[ i ].height, external.rect_filled_a2[ i ].c, external.rect_filled_a2[ i ].rounding );
				}

				for ( size_t i{ 0 }; i < external.rect_filled_shd_a.size( ); i++ ) {
					_render->add_rect_filled_shadowed( external.rect_filled_shd_a[ i ].pos_x, external.rect_filled_shd_a[ i ].pos_y,
													   external.rect_filled_shd_a[ i ].width, external.rect_filled_shd_a[ i ].height, external.rect_filled_shd_a[ i ].c, external.rect_filled_shd_a[ i ].rounding, external.rect_filled_shd_a[ i ].shd );
				}

				for ( size_t i{ 0 }; i < external.rect_a.size( ); i++ ) {
					_render->add_rect( external.rect_a[ i ].x, external.rect_a[ i ].y,
									   external.rect_a[ i ].w, external.rect_a[ i ].h, external.rect_a[ i ].c, external.rect_a[ i ].r, external.rect_a[ i ].t );
				}

				for ( size_t i{ 0 }; i < external.gradient_a.size( ); i++ ) {
					if ( external.gradient_a[ i ].change ) {
						_render->add_gradient_horizontal( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
														  external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1, external.gradient_a[ i ].r, external.gradient_a[ i ].flags );
					} else {
						_render->add_gradient_vertical( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
														external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1, external.gradient_a[ i ].r, external.gradient_a[ i ].flags );
					}
				}

				for ( size_t i{ 0 }; i < external.circle_a.size( ); i++ ) {
					draw_list->AddCircle( external.circle_a[ i ].pos, external.circle_a[ i ].radius, external.circle_a[ i ].color, 0, external.circle_a[ i ].thickness );
				}

				for ( size_t i{ 0 }; i < external.fcircle_a.size( ); i++ ) {
					draw_list->AddCircleFilled( external.fcircle_a[ i ].pos, external.fcircle_a[ i ].radius, external.fcircle_a[ i ].color, 0 );
				}

				for ( size_t i{ 0 }; i < external.text.size( ); i++ ) {
					_render->add_text( external.text[ i ].pos_x, external.text[ i ].pos_y,
									   external.text[ i ].c, external.text[ i ].font_index, external.text[ i ].text.c_str( ), external.text[ i ].flags );
				}

				for ( size_t i{ 0 }; i < external.image.size( ); i++ ) {
					_render->add_image( external.image[ i ].x, external.image[ i ].y, external.image[ i ].w, external.image[ i ].h,
										external.image[ i ].texture, external.image[ i ].c );
				}

				/* clear */
				external.rect_filled.clear( );
				external.rect_filled_shd.clear( );
				external.rect.clear( );
				external.gradient.clear( );
				external.circle.clear( );
				external.fcircle.clear( );
				external.t_rect.clear( );
				external.rect_filled_a.clear( );
				external.rect_filled_a2.clear( );
				external.rect_filled_shd_a.clear( );
				external.gradient_a.clear( );
				external.rect_a.clear( );
				external.circle_a.clear( );
				external.fcircle_a.clear( );
				external.t_rect_a.clear( );
				external.text.clear( );
				external.image.clear( );
			}
		};

		inline const auto _ext_b = std::make_unique< external_drawing>( );
		inline const auto _ext_b_p = std::make_unique< popup_external_drawing>( );
	}

	class external_macro_t {
	public:
		void make_rect_filled( int x, int y, int w, int h, col_t c, int r, bool overwrite = false );
		void make_rect_filledex( int x, int y, int w, int h, col_t c, int r, bool overwrite );
		void make_rect( int x, int y, int w, int h, col_t c, int r, int t, bool overwrite = false );
		void make_text( int x, int y, col_t c, int fi, std::string text, font_flags_t flags = font_flags_t::none );
		void make_rect_filled_shadow( int x, int y, int w, int h, col_t c, int r, int s, bool overwrite = false );
	};

	inline const auto _ext = std::make_unique< external_macro_t >( );

	class external_macro_t2 {
	public:
		void make_rect_filled( int x, int y, int w, int h, col_t c, int r, bool overwrite = false );
		void make_rect_filledex( int x, int y, int w, int h, col_t c, int r, bool overwrite );
		void make_rect( int x, int y, int w, int h, col_t c, int r, int t, bool overwrite = false );
		void make_text( int x, int y, col_t c, int fi, std::string text, font_flags_t flags = font_flags_t::none );
		void make_rect_filled_shadow( int x, int y, int w, int h, col_t c, int r, int s, bool overwrite = false );
	};

	inline const auto _ext_b = std::make_unique< external_macro_t2 >( );
}