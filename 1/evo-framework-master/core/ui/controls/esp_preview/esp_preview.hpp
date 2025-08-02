#pragma once

namespace evo {
	enum draggable_cond : int {
		c_left = 0,
		c_right = 1,
		c_top = 2,
		c_bottom = 3,
		c_center = 4,
		c_pool = 5,
		c_in_move = 6
	};

	enum items_esp {
		name_esp
	};

	enum item_type {
		text,
		bar
	};

	struct move_databse {
		evo::rect_t rect;
		draggable_cond condition;
	};

	struct movable_item {
		std::string item_name{};
		vec2_t temp_pos{}, basic_pos{};

		int item_id{}, drag_cond{ 4 }, item_type{ }, font{ 1 };

		movable_item( std::string item_name, vec2_t& temp_pos, vec2_t& basic_pos, int item_id, int drag_cond, int item_type ) {
			this->item_name = item_name;
			this->temp_pos = temp_pos;
			this->basic_pos = basic_pos;
			this->item_id = item_id;
			this->drag_cond = drag_cond;
			this->item_type = item_type;
		}
	};

	class esp_builder_t {
	public:
		esp_builder_t( );
	public:
		void paint( );
		void input( );
		void set_font( int element_id, int font );
		void reset_position( );
	public:
		evo::vec2_t base_window{};
		std::vector<movable_item> items{};
	private:

	};
}