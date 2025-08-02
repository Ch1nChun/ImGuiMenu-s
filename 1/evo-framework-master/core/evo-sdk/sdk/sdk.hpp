#pragma once

namespace evo { /* main namespace */
	namespace macros {
		/* render macros */
		using vec_t = ImVec2;
		using vec4_t = ImVec4;
		using u32_t = ImU32;
		using texture_id = ImTextureID;
	}

	/* color shit */
	struct col_t {
		col_t( ) : r( 255 ), g( 255 ), b( 255 ), a( 255 ) { } /* default white */
		col_t( int r, int g, int b, int a = 255 ) : r( r ), g( g ), b( b ), a( a ) { }
		~col_t( ) { }

		typedef struct {
			double h;
			double s;
			double v;
		} hsv;

		typedef struct {
			double r;
			double g;
			double b;
		} rgb;

		col_t modify_alpha( int a ) {
			return col_t( r, g, b, a );
		}

		static float base( const unsigned char col ) {
			return col / 255.f;
		}

		std::uint32_t convert( ) {
			std::uint32_t out = 0;

			out = static_cast< std::uint32_t >( this->r ) << 0;
			out |= static_cast< std::uint32_t >( this->g ) << 8;
			out |= static_cast< std::uint32_t >( this->b ) << 16;
			out |= static_cast< std::uint32_t >( this->a ) << 24;

			return out;
		}

		__forceinline col_t hex_to_rgb( std::string hex_color ) {
			if ( hex_color[ 0 ] == '#' ) {
				hex_color.erase( 0, 1 );
			}

			std::stringstream ss;
			ss << std::hex << hex_color;
			unsigned int hex_value;
			ss >> hex_value;

			int red = ( hex_value >> 16 ) & 0xff;
			int green = ( hex_value >> 8 ) & 0xff;
			int blue = hex_value & 0xff;

			return col_t( red, green, blue );
		}

		static float hue( const col_t color ) {
			float R = base( color.r );
			float G = base( color.g );
			float B = base( color.b );
			float mx = max( R, max( G, B ) );
			float mn = min( R, min( G, B ) );

			if ( mx == mn )
				return 0.f;

			float delta = mx - mn;
			float hue = 0.f;

			if ( mx == R )
				hue = ( G - B ) / delta;

			else
				if ( mx == G )
					hue = 2.f + ( B - R ) / delta;

				else
					hue = 4.f + ( R - G ) / delta;

			hue *= 60.f;

			if ( hue < 0.f )
				hue += 360.f;

			return hue / 360.f;
		}

		static float saturation( const col_t color ) {
			float R = base( color.r );
			float G = base( color.g );
			float B = base( color.b );
			float mx = max( R, max( G, B ) );
			float mn = min( R, min( G, B ) );
			float delta = mx - mn;

			if ( mx == 0.f )
				return delta;

			return delta / mx;
		}

		static float brightness( const col_t color ) {
			float R = base( color.r );
			float G = base( color.g );
			float B = base( color.b );
			return max( R, max( G, B ) );
		}

		float hue( ) const {
			return hue( *this );
		}

		float saturation( ) const {
			return saturation( *this );
		}

		float brightness( ) const {
			return brightness( *this );
		}

		static col_t from_hsb( float hue /* 0.f - 1.f*/,
							   float saturation /* 0.f - 1.f */,
							   float brightness /* 0.f - 1.f */,
							   int alpha = 255 ) {
			hue = std::clamp( hue, 0.f, 1.f );
			saturation = std::clamp( saturation, 0.f, 1.f );
			brightness = std::clamp( brightness, 0.f, 1.f );
			float h = ( hue == 1.f ) ? 0.f : ( hue * 6.f );
			float f = h - static_cast< int >( h );
			float p = brightness * ( 1.f - saturation );
			float q = brightness * ( 1.f - saturation * f );
			float t = brightness * ( 1.f - ( saturation * ( 1.f - f ) ) );

			if ( h < 1.f )
				return col_t( brightness * 255, t * 255, p * 255, alpha );

			else
				if ( h < 2.f )
					return col_t( q * 255, brightness * 255, p * 255, alpha );

				else
					if ( h < 3.f )
						return col_t( p * 255, brightness * 255, t * 255, alpha );

					else
						if ( h < 4 )
							return col_t( p * 255, q * 255, brightness * 255, alpha );

						else
							if ( h < 5 )
								return col_t( t * 255, p * 255, brightness * 255, alpha );

							else
								return col_t( brightness * 255, p * 255, q * 255, alpha );
		}

		col_t blend( const col_t& other, float fraction ) const {
			int r_d = r - other.r;
			int g_d = g - other.g;
			int b_d = b - other.b;
			int a_d = a - other.a;

			int blended_r = r - ( r_d * fraction );
			int blended_g = g - ( g_d * fraction );
			int blended_b = b - ( b_d * fraction );
			int blended_a = a - ( a_d * fraction );

			return col_t{ blended_r, blended_g, blended_b, blended_a };
		}

		col_t darker( int amount ) {
			int red = r;
			int green = g;
			int blue = b;

			// reduce the RGB values by the given amount
			red = std::fmax( 0, red - amount );
			green = std::fmax( 0, green - amount );
			blue = std::fmax( 0, blue - amount );

			return col_t( red, green, blue, a );
		}

		static hsv rgb_to_hsv( int r, int g, int b ) {
			rgb in = { ( float )( r / 255 ), ( float )( g / 255 ) , ( float )( b / 255 ) };
			hsv         out;
			double      min, max, delta;

			min = in.r < in.g ? in.r : in.g;
			min = min < in.b ? min : in.b;

			max = in.r > in.g ? in.r : in.g;
			max = max > in.b ? max : in.b;

			out.v = max;
			delta = max - min;
			if ( delta < 0.00001 ) {
				out.s = 0;
				out.h = 0;
				return out;
			}
			if ( max > 0.0 ) {
				out.s = ( delta / max );
			} else {
				out.s = 0.0;
				out.h = NAN;
				return out;
			}
			if ( in.r >= max )
				out.h = ( in.g - in.b ) / delta;
			else
				if ( in.g >= max )
					out.h = 2.0 + ( in.b - in.r ) / delta;
				else
					out.h = 4.0 + ( in.r - in.g ) / delta;

			out.h *= 60.0;

			if ( out.h < 0.0 )
				out.h += 360.0;

			return out;
		}

		static col_t hsv_to_rgb( double h, double s, double v ) {
			double      hh, p, q, t, ff;
			long        i;
			rgb         out;

			if ( s <= 0.0 ) {
				out.r = v;
				out.g = v;
				out.b = v;
				return col_t( out.r * 255.f, out.g * 255.f, out.b * 255.f );
			}
			hh = h;
			if ( hh >= 360.0 ) hh = 0.0;
			hh /= 60.0;
			i = ( long )hh;
			ff = hh - i;
			p = v * ( 1.0 - s );
			q = v * ( 1.0 - ( s * ff ) );
			t = v * ( 1.0 - ( s * ( 1.0 - ff ) ) );

			switch ( i ) {
				case 0:
					out.r = v;
					out.g = t;
					out.b = p;
					break;
				case 1:
					out.r = q;
					out.g = v;
					out.b = p;
					break;
				case 2:
					out.r = p;
					out.g = v;
					out.b = t;
					break;

				case 3:
					out.r = p;
					out.g = q;
					out.b = v;
					break;
				case 4:
					out.r = t;
					out.g = p;
					out.b = v;
					break;
				case 5:
				default:
					out.r = v;
					out.g = p;
					out.b = q;
					break;
			}
			return col_t( int( out.r * 255.f ), int( out.g * 255.f ), int( out.b * 255.f ) );
		}

		int r, g, b, a;
	};

	/* color shit */
	struct fcol_t {
		fcol_t( ) = default;
		fcol_t( float _r, float _g, float _b, float _a = 1.0f ) :
			r( _r ), g( _g ), b( _b ), a( _a ) { }

		fcol_t( int _r, int _g, int _b, int _a = 255 ) {
			set_color( _r, _g, _b, _a );
		}

		__forceinline void set_color( float _r, float _g, float _b, float _a = 1.0f ) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

		float r, g, b, a;
	};

	/* vec2 shit */
	struct vec2_t {
		/* default builders */
		vec2_t( ) : x( 0 ), y( 0 ) { }
		vec2_t( float x, float y ) : x( x ), y( y ) { }
		~vec2_t( ) { }

		/* operators */
		bool operator==( const vec2_t& src ) const {
			return ( src.x == this->x ) && ( src.y == y );
		}

		bool operator!=( const vec2_t& src ) const {
			return ( src.x != this->x ) || ( src.y != y );
		}

		float& operator[]( int i ) {
			return ( ( float* )this )[ i ];
		}

		float operator[]( int i ) const {
			return ( ( float* )this )[ i ];
		}

		/* vec - vec */
		vec2_t& operator-=( const vec2_t& v ) {
			this->x -= v.x; this->y -= v.y;
			return *this;
		}

		vec2_t& operator-=( float fl ) {
			this->x -= fl;
			this->y -= fl;

			return *this;
		}

		vec2_t& operator-( vec2_t v ) {
			vec2_t temp;
			temp.x = this->x - v.x;
			temp.y = this->y - v.y;

			return temp;
		}

		/* vec + vec */
		vec2_t& operator+=( const vec2_t& v ) {
			this->x += v.x; this->y += v.y;
			return *this;
		}

		vec2_t& operator+=( float fl ) {
			this->x += fl;
			this->y += fl;

			return *this;
		}

		vec2_t& operator+( vec2_t v ) {
			vec2_t temp;
			temp.x = this->x + v.x;
			temp.y = this->y + v.y;

			return temp;
		}

		void set( float x /*= 0.0f*/, float y /*= 0.0f */ ) {
			this->x = x;
			this->y = y;
		}

		vec2_t& operator=( const vec2_t& v ) {
			x = v.x;
			y = v.y;
			return *this;
		}

		/* > */
		bool operator<( const vec2_t& in ) const {
			return ( x < in.x && y < in.y );
		}

		bool operator>( const vec2_t& in ) const {
			return ( x > in.x && y > in.y );
		}

		/* parameters */
		float x;
		float y;
	};

	class rect_t {
	public:
		int x, y, w, h;

		rect_t( ) : x( 0 ), y( 0 ), w( 0 ), h( 0 ) { }
		rect_t( int x, int y, int w, int h ) : x( x ), y( y ), w( w ), h( h ) { }
		~rect_t( ) { }
	};

}