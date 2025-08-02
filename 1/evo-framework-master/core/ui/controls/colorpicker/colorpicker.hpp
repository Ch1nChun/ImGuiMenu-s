#pragma once

struct RGB { double r, g, b; };
struct HSV { double h, s, v; };

namespace evo {
	class colorpicker_t {
	public:
		colorpicker_t( evo::col_t* color, float* hue );
	public:
		void paint( );
		void input( );
	public:
		__forceinline HSV rgb_to_hsv( const RGB& In ) {
			HSV m_Result;
			double Min, Max, Delta;

			Min = In.r < In.g ? In.r : In.g;
			Min = Min < In.b ? Min : In.b;

			Max = In.r > In.g ? In.r : In.g;
			Max = Max > In.b ? Max : In.b;

			m_Result.v = Max;
			Delta = Max - Min;

			if ( Delta < 0.0001 ) {
				m_Result.s = 0;
				m_Result.h = 0;

				return m_Result;
			}

			if ( Max > 0 ) {
				m_Result.s = ( Delta / Max );
			} else {
				m_Result.s = 0;
				m_Result.h = NAN;

				return m_Result;
			}

			if ( In.r >= Max ) {
				m_Result.h = ( In.g - In.b ) / Delta;
			} else {
				if ( In.g >= Max ) {
					m_Result.h = 2 + ( In.b - In.r ) / Delta;
				} else {
					m_Result.h = 4 + ( In.r - In.g ) / Delta;
				}
			}

			m_Result.h *= 60;

			if ( m_Result.h < 0 )
				m_Result.h += 360;

			return m_Result;
		}
		__forceinline RGB hsv_to_rgb( const HSV& In ) {
			RGB m_Result;

			double HH, P, Q, T, FF;
			long i;

			if ( In.s <= 0 ) {
				m_Result.r = In.v;
				m_Result.g = In.v;
				m_Result.b = In.v;

				return m_Result;
			}

			HH = ( In.h >= 360 ? 0 : In.h ) / 60;
			i = ( long )HH;

			FF = HH - i;

			P = In.v * ( 1 - In.s );
			Q = In.v * ( 1 - ( In.s * FF ) );
			T = In.v * ( 1 - ( In.s * ( 1 - FF ) ) );

			switch ( i ) {
				case 0:
					m_Result.r = In.v;
					m_Result.g = T;
					m_Result.b = P;

					break;

				case 1:
					m_Result.r = Q;
					m_Result.g = In.v;
					m_Result.b = P;

					break;

				case 2:
					m_Result.r = P;
					m_Result.g = In.v;
					m_Result.b = T;

					break;

				case 3:
					m_Result.r = P;
					m_Result.g = Q;
					m_Result.b = In.v;

					break;

				case 4:
					m_Result.r = T;
					m_Result.g = P;
					m_Result.b = In.v;

					break;

				case 5:
				default:
					m_Result.r = In.v;
					m_Result.g = P;
					m_Result.b = Q;

					break;

			}

			return m_Result;
		}

	public:
		vec2_t base_window{}, pos{}, cursor_pos{};
	private:
		bool focused_element{}, modifying_hue{}, modifing_hsv{}, modifieng_alpha{}, copied{}, pasted{};
		float* hue{};
		float alpha{};
		col_t* select;
	};
}