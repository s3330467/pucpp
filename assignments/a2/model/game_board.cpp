#include "game_board.h"

//using namespace of this class
using namespace draughts;
using namespace model;

game_board::game_board(int x, int y) : x(x), y(y) {
	board = std::vector<std::vector<std::unique_ptr<piece::game_piece>>>(x);

	for(unsigned int i = 0; i < board.size(); i++) {//iterate through board in x direciton

		board[i] = std::vector<std::unique_ptr<piece::game_piece>>(y);//add column

		for(unsigned int j = 0; j < board[i].size(); j++) {//iterate through board in y direction (the column we just added)

			if(j < static_cast<unsigned int>(y/2 - 1) && j%2 == i%2) {
				//top, player 1
				board[i][j] = std::make_unique<piece::man>(
						piece::man(true));

			} else if(j > static_cast<unsigned int>(y/2) && j%2 == i%2) {
				//bottom, player 2
				board[i][j] = std::make_unique<piece::man>(
						piece::man(false));

			} else {
				//middle, no piece
				board[i][j] = std::make_unique<piece::empty>(
						piece::empty(false));
			}

		}
	}
}

const piece::game_piece * game_board::get_piece(coordinate coord) const {
	//printf("starting get_piece\n");
	std::pair<int, int> cr = coord.get_crush();

	//printf("middle get_piece\n");
	//printf("coords x:%d y:%d\n", cr.first, cr.second);
	const piece::game_piece *ptr = board[cr.first][cr.second].get();
	//printf("ending get_piece\n");
	return ptr;
}

bool game_board::make_move(move move) {
	std::pair<int, int> from_coord = move.from.get_crush();
	std::pair<int, int> to_coord = move.to.get_crush();
	if(move.type == move::VALID) {
		//non attack
		board[from_coord.first][from_coord.second].swap(board[to_coord.first][to_coord.second]);
/*
		auto attacker = board[from_coord.first][from_coord.second].release();
		auto oldplace = board[to_coord.first][to_coord.second].release();
		attacker->set_coords(coordinate::from_crush(to_coord.first, to_coord.second));
		oldplace->set_coords(coordinate::from_crush(from_coord.first, from_coord.second));

		board[from_coord.first][from_coord.second] = std::make_unique<piece::empty>(
				piece::empty(oldplace->get_coords, false));
		if(attacker->visual() == 'x' || attacker->visual() == 'o') {
			board[to_coord.first][to_coord.second] = std::make_unique<piece::man>(
					piece::man(attacker->get_coords, attacker->get_is_top()));
		} else {
			board[to_coord.first][to_coord.second] = std::make_unique<piece::king>(
					piece::king(attacker->get_coords, attacker->get_is_top()));
		}*/

		//board[from_coord.first][from_coord.second] = std::move(oldplace);
		//board[to_coord.first][to_coord.second] = std::move(attacker);
		return can_take(move.to);
	} else if(move.type == move::VALID_ATTACK) {
		//attack move
		coordinate capturedpos = get_captured(move);
		std::pair<int, int> captured_coord = capturedpos.get_crush();
		board[captured_coord.first][captured_coord.second].reset();
		board[captured_coord.first][captured_coord.second] = std::make_unique<piece::empty>(piece::empty(false));


		board[from_coord.first][from_coord.second].swap(board[to_coord.first][to_coord.second]);
/*
		auto attacker = board[from_coord.first][from_coord.second].release();
		auto oldplace = board[to_coord.first][to_coord.second].release();
		attacker->set_coords(coordinate::from_crush(to_coord.first, to_coord.second));
		oldplace->set_coords(coordinate::from_crush(from_coord.first, from_coord.second));

		board[from_coord.first][from_coord.second].reset(oldplace);
		board[to_coord.first][to_coord.second].reset(attacker);*/
		return can_take(move.to);
	} else {
		return true;
	}
}

std::vector<move> game_board::available_moves(bool top_player) const {
	//printf("starting available moves\n");
	std::vector<move> available_moves;
	std::vector<move> piece_moves;
	const piece::game_piece *current_piece, *capture_piece;
	coordinate capture_coords;
	bool attackmoveexists = false;
	int asdf = 0;

	for(unsigned int i = 0; i < board.size(); i++) {//iterate through board in x direciton

		for(unsigned int j = 0; j < board[i].size(); j++) {//iterate through board in y direction (the column we just added)
			coordinate coord = coordinate::from_crush(i, j);
			current_piece = get_piece(coord);
			if(current_piece == nullptr) continue;
			if(current_piece->get_is_top() != top_player) continue;//check current player ownes this piece
			piece_moves = current_piece->get_valid_moves(coord);

					asdf++;
			for(unsigned int k = 0; k < piece_moves.size(); k++){
				//validate coordinates
				if(!valid(piece_moves.at(k).from) || !valid(piece_moves.at(k).to)) {
					auto fromtemp = piece_moves.at(k).from.get_crush();
					auto totemp = piece_moves.at(k).to.get_crush();
					printf("%d not valid from'x:%d y:%d' to'x:%d y:%d'\n", asdf, fromtemp.first, fromtemp.second, totemp.first, totemp.second);
					continue;
				}

				if(piece_moves.at(k).type == move::VALID_ATTACK){

					coordinate capture_coords = get_captured(piece_moves.at(k));
					capture_piece = get_piece(capture_coords);

					//check destination is free
					if(get_piece(piece_moves.at(k).to)->visual() != ' ') {
						auto temptopair = piece_moves.at(k).to.get_uncrush();
						printf("%d ATT: destination not free x:%d y:%d '%c'\n", asdf, temptopair.first, temptopair.second, get_piece(piece_moves.at(k).to)->visual());
						continue;
					}
					//check capture piece is there
					if(capture_piece->visual() == ' ') {
						printf("%d ATT: can't capture what's not there\n", asdf);
						continue;
					}
					if(capture_piece->get_is_top() == current_piece->get_is_top()) {
						printf("%d ATT: can't capture your own man\n", asdf);
						continue;
					}
					//available_moves.resize( available_moves.size() + piece_moves.size());//valiant effort, but we dont actually know how big it needs to be
					available_moves.push_back(piece_moves.at(k));
					printf("%d ATT: attack move added!\n", asdf);
					attackmoveexists = true;
				}

				if(piece_moves.at(k).type == move::VALID && !attackmoveexists) {
					//auto asdf = piece_moves.at(k).to;
					auto tempfrom = piece_moves.at(k).to;
					if(get_piece(tempfrom)->visual() != ' ') {
						auto tempfrompair = tempfrom.get_uncrush();
						printf("%d destination not free x:%d y:%d\n", asdf, tempfrompair.first, tempfrompair.second);
						//printf("%d destination not free x:%d y:%d '%c'\n", asdf, temptopair.first, temptopair.second, get_piece(piece_moves.at(k).to)->visual());
						continue;
					}
					available_moves.push_back(piece_moves.at(k));
					printf("%d normal move added!\n", asdf);
				}
			}
		}
	}
	if(attackmoveexists){
		//remove any non-attack moves that were added prior to the attack being found
		auto it = available_moves.begin();
		while(it != available_moves.end()) {
			if(it->type == move::VALID_ATTACK) {
				//no non-attack moves after the first attack move
				break;
			}
			//remove the move
			it = available_moves.erase(it);//this increments the iterator
			continue;
		}
	}
	//printf("finishing available moves\n");
	return available_moves;
}

bool game_board::can_take(coordinate piece_coord) const {
	const piece::game_piece *piece = get_piece(piece_coord);

	if(piece == nullptr || piece->visual() == ' ') return false;
	std::vector<move> piece_moves = piece->get_valid_moves(piece_coord);

	for(auto it = piece_moves.begin(); it != piece_moves.end(); it++) {
		
		if(it->type != move::VALID_ATTACK) continue;//dont do non-attack
		if(get_piece(it->to) != nullptr && get_piece(it->to)->visual() != ' ') continue;//dont do if there is a piece at destination

		coordinate capture_pos = get_captured(*it);
		const piece::game_piece *capture = get_piece(capture_pos);

		//if they are on opposite teams, return true
		if(capture->get_is_top() != piece->get_is_top()) return true;
	}
	//no valid attack move, return false
	return false;
}

coordinate game_board::get_captured(move move) {
	if(move.type != move::VALID_ATTACK) {
		return coordinate::from_uncrush(-1, -1);
	}

	auto to_coord = move.to.get_uncrush();
	auto from_coord = move.from.get_uncrush();
	int dx = to_coord.first - from_coord.first;
	int dy = to_coord.second - from_coord.second;
	coordinate capture_pos = coordinate::from_uncrush(from_coord.first + dx, from_coord.second + dy);

	return capture_pos;
}
