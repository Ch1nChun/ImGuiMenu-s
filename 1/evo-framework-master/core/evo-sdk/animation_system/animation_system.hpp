#pragma once
#include <string>
#pragma warning ( disable : 4244 )

struct anim_context_t {
	float value;
	unsigned int id;

	void adjust( float adjust_to, bool clamp = true );
};

enum e_ease_type {
	in_circ = 0 << 0,
	out_circ = 1 << 0,
	in_out_circ = 2 << 0,
	in_quad = 3 << 0,
	out_quad = 4 << 0,
	in_out_quad = 5 << 0,
};

struct animation_controller_t {

	std::string current_child;

	anim_context_t get( std::string str );

	float ease( e_ease_type type, float in );

	void clear_stack( );

	__forceinline float lerp( float x, float goal, float time ) {
		return float( x + ( goal - x ) * time );
	}

	__forceinline float lerp_single( float a, float b, float t ) {
		return ( 1 - t ) * a + t * b;
	}

	__forceinline float lerp_ease( float a, float b, float t, bool easeIn, bool easeOut ) {
		if ( easeIn && easeOut ) {
			// Ease-in-out
			float smoothT = t * t * ( 3.0f - 2.0f * t );
			return a + smoothT * ( b - a );
		} else if ( easeIn ) {
			// Ease-in
			float smoothT = t * t;
			return a + smoothT * ( b - a );
		} else if ( easeOut ) {
			// Ease-out
			float smoothT = 1.0f - ( 1.0f - t ) * ( 1.0f - t );
			return a + smoothT * ( b - a );
		} else {
			// Linear (no easing)
			return ( 1 - t ) * a + t * b;
		}
	}


	__forceinline float get_min_deltatime( float scale = 1.f ) {
		return ( 1.f / 0.2f ) * 0.010 * scale;
	}

}; inline animation_controller_t animation_controller;