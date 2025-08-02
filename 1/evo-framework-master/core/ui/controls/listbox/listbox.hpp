#pragma once

namespace evo {
	class listbox_t {
	public:
		listbox_t( std::string name, int* value, std::vector<std::string>& items, std::string& filter );
	public:
		__forceinline std::string to_lower( std::string string ) {
			std::transform( string.begin( ), string.end( ), string.begin( ), ( int( * )( int ) )::tolower );
			return string;
		}

		__forceinline void get_items( ) {
			if ( this->lower_items.empty( ) || this->lower_items.size( ) != this->items->size( ) ) {
				this->lower_items.clear( );
				for ( int i = 0; i < this->items->size( ); i++ ) {
					std::string temp_s = ( *this->items )[ i ];
					this->lower_items.push_back( temp_s );
				}
			}

			if ( !this->filter && temp.empty( ) || this->temp.size( ) != this->items->size( ) && ( !this->filter || this->filter->empty( ) ) ) {
				this->temp.clear( );
				for ( int i = 0; i < this->items->size( ); i++ ) {
					this->temp.emplace_back( ( *this->items )[ i ], i );
				}
			}

			/* assume str vars */
			if ( !this->filter )
				return;

			if ( this->prev_filter == *this->filter )
				return;

			this->prev_filter = *this->filter;
			const auto l_filter = to_lower( *this->filter );

			/* convert the first letter */
			if ( !this->filter->empty( ) ) {
				if ( std::islower( ( *this->filter )[ 0 ] ) ) {
					( *this->filter )[ 0 ] = std::toupper( ( *this->filter )[ 0 ] );
				}
			}

			this->temp.clear( );

			for ( int i = 0; i < this->items->size( ); i++ ) {
				if ( this->filter != nullptr ) {
					std::string temp_s = this->items->at( i ); /* use the original case-sensitive version of the item */
					const auto l_item = to_lower( temp_s ); /* convert the item to lowercase for comparison */
					if ( !( l_item.find( l_filter ) != std::string::npos ) )
						continue;
				}

				this->temp.emplace_back( this->items->at( i ), i );
			}
		}

		__forceinline void update_item( ) { /* leave like this */
			this->get_items( );
		}

	    void scroll( int limit );

		void think( );
		void think_filter_bar( );
		void paint( );
	public:
		evo::vec2_t pos{};
	private:
		std::string name{}, prev_filter{};
		std::string* filter{};
		std::vector <std::string>* items{};
		std::vector<std::string> lower_items{};
		std::vector< std::pair<std::string, int>> temp;
		int* selected{};

		bool focused_bar{};
	};
}
