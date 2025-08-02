#pragma once

namespace evo {
	struct multi_dropdown_system {
		multi_dropdown_system( std::string name, bool* value ) {
			this->name = name;
			this->value = value;
		}

		std::string name{};
		bool* value{};
	};

	class multi_dropdown_t {
	public:
		multi_dropdown_t( std::string label );
	public:
		void paint( );
		void input( );

		void insert_item( std::string label, bool* value );
	public:
		evo::vec2_t base_window{};
	private:
		std::string label{};
		std::vector <multi_dropdown_system> items{};

		bool hovered{}, focused{};
	};
}