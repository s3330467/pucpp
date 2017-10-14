#include "../coordinate.h"

#pragma once

namespace draughts {
	namespace model {
		namespace piece {

			class game_piece {
				protected:
				coordinate coords;
				bool is_top;

				public:
				game_piece(coordinate coords, bool is_top) : coords(coords), is_top(is_top) {}
				draughts::model::coordinate get_coords();
				bool get_is_top();

				virtual bool is_valid(coordinate from, coordinate to) = 0;
			};
		}
	}
}
