#pragma once

namespace evo {
	class slider_float_t {
	public:
		slider_float_t( std::string label, float* value, float min, float max, std::string suffix = "" );
	public: /* we are going to use this for the limit of slider value / printlimit */
		template <typename T>
		__forceinline std::string precision( const T a_value, const int n = 3 ) {
			std::ostringstream out;
			out.precision( n );
			out << std::fixed << a_value;
			return out.str( );
		}
	public:
		void paint( );
		void input( );
	public:
		evo::vec2_t base_window{};
	private:
		std::string label{}, suffix{};

		float* value{};
		float min{}, max{};

		bool hovered{}, in_use{};
		int stored_height{}, added_height{};
	};

	class slider_int_t {
	public:
		slider_int_t( std::string label, int* value, int min, int max, std::string suffix = "" );
	public:
		void paint( );
		void input( );	
	public:
		evo::vec2_t base_window{};
	private:
		std::string label{}, suffix{};

		int* value{};
		int min{}, max{};

		bool hovered{}, in_use{};
		int stored_height{}, added_height{};
	};
}