#include "animation_system.hpp"
#include <algorithm>
#include <map>

// - hashed id 
// - animation float
std::map<unsigned int, float> stack;

void anim_context_t::adjust( float adjust_to, bool clamp ) {
	auto animation = stack.find( id );
	if ( animation == stack.end( ) ) {
		stack.insert( { id, 0.f } );
		animation = stack.find( id );
	}

	if ( clamp ) {
		animation->second = std::clamp( adjust_to, 0.f, 1.f );
	} else {
		animation->second = adjust_to;
	}
}

anim_context_t animation_controller_t::get( std::string str ) {

	unsigned int hash = std::hash<std::string>( )( str + current_child );

	auto animation = stack.find( hash );

	if ( animation == stack.end( ) ) {
		stack.insert( { hash, 0.f } );
		animation = stack.find( hash );
	}

	return { animation->second, hash };
}

float animation_controller_t::ease( e_ease_type type, float in ) {
	switch ( type ) {
		case in_circ:
			return 1 - sqrt( 1 - pow( in, 2 ) );
			break;
		case out_circ:
			return sqrt( 1 - pow( in - 1, 2 ) );
			break;
		case in_out_circ:
			return in < 0.5 ? ( 1 - sqrt( 1 - pow( 2 * in, 2 ) ) ) / 2 : ( sqrt( 1 - pow( -2 * in + 2, 2 ) ) + 1 ) / 2;
			break;
		case in_quad:
			return in * in;
			break;
		case out_quad:
			return 1 - ( 1 - in ) * ( 1 - in );
			break;
		case in_out_quad:
			return in < 0.5 ? 2 * in * in : 1 - pow( -2 * in + 2, 2 ) / 2;
			break;
		default:
			return in;
	}

	return in;
}

void animation_controller_t::clear_stack( ) {
	stack.clear( );
}