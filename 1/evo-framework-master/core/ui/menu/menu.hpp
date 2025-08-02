#pragma once
#include <mutex>

namespace evo {
	class menu_t {
	public:
		void init( );
	};

	inline const auto _menu = std::make_unique< menu_t >( );
}