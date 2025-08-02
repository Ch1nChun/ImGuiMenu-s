#pragma once

namespace evo {
	struct elem_check {
		elem_check( std::string label, bool* value ) {
			this->label = label;
			this->value = value;
		}

		std::string label;
		bool* value;
	};

	struct elem_slider_int {
		elem_slider_int( std::string label, int* value, int min, int max, std::string sufix ) {
			this->label = label;
			this->value = value;
			this->min = min;
			this->max = max;
			this->sufix = sufix;
		}

		std::string label, sufix;
		int* value;

		int min, max;
	};

	struct elem_slider_float {
		elem_slider_float( std::string label, float* value, int min, int max, std::string sufix ) {
			this->label = label;
			this->value = value;
			this->min = min;
			this->max = max;
			this->sufix = sufix;
		}

		std::string label, sufix;
		float* value;

		int min, max;
	};

	struct elem_text {
		elem_text( std::string label ) {
			this->label = label;
		}

		std::string label;
	};

	struct elem_dropdown {
		elem_dropdown( std::string label, int* value, std::vector<std::string> items ) {
			this->label = label;
			this->value = value;
			this->items = items;
		}

		std::string label;
		int* value;
		std::vector<std::string> items;

		bool focused{ false };
	};

	class popup_t {
	public:
		popup_t( int pop_id, std::string popup );
	private:
		void handle_checkbox( );
		void handle_slider_int( );
		void handle_slider_float( );
		void handle_text( );
		void handle_dropdown( );
	public:
		void paint( );
		void input( );
	private:
		template <typename T>
		__forceinline std::string precision( const T a_value, const int n = 3 ) {
			std::ostringstream out;
			out.precision( n );
			out << std::fixed << a_value;
			return out.str( );
		}
	public:
		void bind_checkbox( std::string label, bool* value );
		void bind_slider_int( std::string label, int* value, int min, int max, std::string sufix = "" );
		void bind_slider_float( std::string label, float* value, int min, int max, std::string sufix = "" );
		void bind_text( std::string label );
		void bind_dropdown( std::string label, int* value, std::vector<std::string> items );
	public:
		vec2_t base_window;
	private:
		std::string popup;
		bool focused_element;
		bool hovered;

		std::vector< elem_check> checkbox_elems;
		std::vector< elem_slider_int> elem_sliderint;
		std::vector< elem_slider_float> elem_sliderfloat;
		std::vector< elem_text> elem_texct;
		std::vector< elem_dropdown> elem_drop;

	public:
		int pop_id;
		float track_animation[ 1000 ];
		int track_size[ 1000 ];
	};
}