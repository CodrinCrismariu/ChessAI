#pragma once
#include <tuple>
#include <vector>

enum class Colour {None, White, Black};
enum class PieceType {None, Pawn, Rook, Bishop, Knight, Queen, King};

//Generic piece implementation - to be overridden by child
class Piece {
private:
	static const char symbol = ' ';
	static const PieceType pieceType = PieceType::None;
	static const int value = 0;
public:
	Colour colour;
	int moveCount = 0;
	int getValue();
	std::vector<std::tuple<char, char>> moveList; //storage of all possible moves after call of getMoveList
	std::vector<std::tuple<char, char>> captureMoveList; //storage of all possible capture moves after call of getCaptureMoveList
	Piece(Colour colour);
	virtual std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y);
	virtual std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y);
	virtual char getSymbol();
	virtual PieceType getPieceType();
};

class Pawn : public Piece {
private:
	static const char symbol = 'p';
	static const PieceType pieceType = PieceType::Pawn;
	static const int value = 100;
public:
	Pawn(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

class Knight : public Piece {
private:
	static const char symbol = 'N';
	static const PieceType pieceType = PieceType::Knight;
	static const int value = 300;
public:
	Knight(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

class Rook : public Piece {
private:
	static const char symbol = 'R';
	static const PieceType pieceType = PieceType::Rook;
	static const int value = 500;
public:
	Rook(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

class Bishop : public Piece {
private:
	static const char symbol = 'B';
	static const PieceType pieceType = PieceType::Bishop;
	static const int value = 320;
public:
	Bishop(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

class Queen : public Piece {
private:
	static const char symbol = 'Q';
	static const PieceType pieceType = PieceType::Queen;
	static const int value = 900;
public:
	Queen(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

class King : public Piece {
private:
	static const char symbol = 'K';
	static const PieceType pieceType = PieceType::King;
	static const int value = 0;
public:
	King(Colour colour);
	std::vector<std::tuple<char, char>> getMoveList(Piece* squares[8][8], char x, char y) override;
	std::vector<std::tuple<char, char>> getCaptureMoveList(Piece* squares[8][8], char x, char y) override;
	char getSymbol() override;
	PieceType getPieceType() override;
};

Piece::Piece(Colour colour) {
	Piece::colour = colour;
	Piece::moveCount = 0;
}

/*Returns a list of possible moves assuming the piece is at the (x, y) Position on the board
The board is needed to only generate moves within range of the piece (i.e.to avoid skipping for sliding pieces)
and that do not move to an occupied square of the same color
Legality of moves is handled in terms of checkmate is handled by the board class itself
This virtual method gets overridden in child classes.*/
std::vector<std::tuple<char, char>> Piece::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Piece::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	return Piece::captureMoveList;
}

char Piece::getSymbol() {
	return Piece::symbol;
}

PieceType Piece::getPieceType() {
	return Piece::pieceType;
}

int Piece::getValue() {
	return value;
}

//PAWN IMPLEMENTATION
Pawn::Pawn(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Pawn::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if (Pawn::colour == Colour::White) {
		if (y < 7) {
			//Move one square ahead
			if (squares[x][y + 1] == nullptr) {
				Piece::moveList.push_back(std::tuple<char, char>(x, y + 1));
			}
			//Move two squares ahead
			if (y == 1) {
				if ((squares[x][y + 1] == nullptr) && (squares[x][y + 2] == nullptr)) {
					Piece::moveList.push_back(std::tuple<char, char>(x, y + 2));
				}
			}
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y + 1] != nullptr) {
				if (squares[x + 1][y + 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y + 1] != nullptr) {
				if (squares[x - 1][y + 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1));
				}
			}
			if (y == 4) {
				if ((x > 0) && (squares[x - 1][y] != nullptr) && (squares[x - 1][y + 1] == nullptr) && (squares[x - 1][y]->getPieceType() == PieceType::Pawn) && squares[x - 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1)); //white left en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
				if ((x < 7) && (squares[x + 1][y] != nullptr) && (squares[x + 1][y + 1] == nullptr) && (squares[x + 1][y]->getPieceType() == PieceType::Pawn) && squares[x + 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1)); //white right en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
			}
		}
	}
	else if (Pawn::colour == Colour::Black) {
		if (y > 0) {
			//Move one square ahead
			if (squares[x][y - 1] == nullptr) {
				Piece::moveList.push_back(std::tuple<char, char>(x, y - 1));
			}
			//Move two squares ahead
			if (y == 6) {
				if ((squares[x][y - 1] == nullptr) && (squares[x][y - 2] == nullptr)) {
					Piece::moveList.push_back(std::tuple<char, char>(x, y - 2));
				}
			}
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y - 1] != nullptr) {
				if (squares[x + 1][y - 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y - 1] != nullptr) {
				if (squares[x - 1][y - 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1));
				}
			}
			if (y == 3) {
				if ((x > 0) && (squares[x - 1][y] != nullptr) && (squares[x - 1][y - 1] == nullptr) && (squares[x - 1][y]->getPieceType() == PieceType::Pawn) && squares[x - 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1)); //black left en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
				if ((x < 7) && (squares[x + 1][y] != nullptr) && (squares[x + 1][y - 1] == nullptr) && (squares[x + 1][y]->getPieceType() == PieceType::Pawn) && squares[x + 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1)); //black right en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
			}
		}
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Pawn::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if (Pawn::colour == Colour::White) {
		if (y < 7) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y + 1] != nullptr) {
				if (squares[x + 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y + 1] != nullptr) {
				if (squares[x - 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 1));
				}
			}
		}
	}
	else if (Pawn::colour == Colour::Black) {
		if (y > 0) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y - 1] != nullptr) {
				if (squares[x + 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y - 1] != nullptr) {
				if (squares[x - 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 1));
				}
			}
		}
	}
	return Piece::captureMoveList;
}

char Pawn::getSymbol() {
	return Pawn::symbol;
}

PieceType Pawn::getPieceType() {
	return Pawn::pieceType;
}

//KNIGHT IMPLEMENTATION
Knight::Knight(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}
std::vector<std::tuple<char, char>> Knight::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if ((x - 1 >= 0) && (y + 2 <= 7)) {
		if ((squares[x - 1][y + 2] == nullptr) || squares[x - 1][y + 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 2));
		}
	}
	if ((x - 1 >= 0) && (y - 2 >= 0)) {
		if ((squares[x - 1][y - 2] == nullptr) || squares[x - 1][y - 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 2));
		}
	}
	if ((x + 1 <= 7) && (y + 2 <= 7)) {
		if ((squares[x + 1][y + 2] == nullptr) || squares[x + 1][y + 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 2));
		}
	}
	if ((x + 1 <= 7) && (y - 2 >= 0)) {
		if ((squares[x + 1][y - 2] == nullptr) || squares[x + 1][y - 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 2));
		}
	}
	if ((x - 2 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 2][y + 1] == nullptr) || squares[x - 2][y + 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 2, y + 1));
		}
	}
	if ((x - 2 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 2][y - 1] == nullptr) || squares[x - 2][y - 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 2, y - 1));
		}
	}
	if ((x + 2 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 2][y + 1] == nullptr) || squares[x + 2][y + 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 2, y + 1));
		}
	}
	if ((x + 2 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 2][y - 1] == nullptr) || squares[x + 2][y - 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 2, y - 1));
		}
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Knight::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if ((x - 1 >= 0) && (y + 2 <= 7)) {
		if ((squares[x - 1][y + 2] != nullptr) && squares[x - 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 2));
		}
	}
	if ((x - 1 >= 0) && (y - 2 >= 0)) {
		if ((squares[x - 1][y - 2] != nullptr) && squares[x - 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 2));
		}
	}
	if ((x + 1 <= 7) && (y + 2 <= 7)) {
		if ((squares[x + 1][y + 2] != nullptr) && squares[x + 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 2));
		}
	}
	if ((x + 1 <= 7) && (y - 2 >= 0)) {
		if ((squares[x + 1][y - 2] != nullptr) && squares[x + 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 2));
		}
	}
	if ((x - 2 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 2][y + 1] != nullptr) && squares[x - 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 2, y + 1));
		}
	}
	if ((x - 2 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 2][y - 1] != nullptr) && squares[x - 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 2, y - 1));
		}
	}
	if ((x + 2 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 2][y + 1] != nullptr) && squares[x + 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 2, y + 1));
		}
	}
	if ((x + 2 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 2][y - 1] != nullptr) && squares[x + 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 2, y - 1));
		}
	}
	return Piece::captureMoveList;
}

char Knight::getSymbol() {
	return Knight::symbol;
}

PieceType Knight::getPieceType() {
	return Knight::pieceType;
}

//ROOK IMPLEMENTATION
Rook::Rook(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Rook::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Rook::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + i));
	}
	return Piece::captureMoveList;
}

char Rook::getSymbol() {
	return Rook::symbol;
}

PieceType Rook::getPieceType() {
	return Rook::pieceType;
}

//BISHOP IMPLEMENTATION
Bishop::Bishop(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Bishop::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Bishop::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::captureMoveList;
}

char Bishop::getSymbol() {
	return Bishop::symbol;
}
PieceType Bishop::getPieceType() {
	return Bishop::pieceType;
}
//QUEEN IMPLEMENTATION
Queen::Queen(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Queen::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Queen::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::captureMoveList;
}

char Queen::getSymbol() {
	return Queen::symbol;
}

PieceType Queen::getPieceType() {
	return Queen::pieceType;
}

//KING IMPLEMENTATION - Todo: Castling
King::King(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> King::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if ((x - 1 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 1][y + 1] == nullptr) || (King::colour != squares[x - 1][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1));
		}
	}
	if ((x - 1 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 1][y - 1] == nullptr) || (King::colour != squares[x - 1][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1));
		}
	}
	if ((x + 1 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 1][y + 1] == nullptr) || (King::colour != squares[x + 1][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1));
		}
	}
	if ((x + 1 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 1][y - 1] == nullptr) || (King::colour != squares[x + 1][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1));
		}
	}
	if ((x + 1 <= 7)) {
		if ((squares[x + 1][y] == nullptr) || (King::colour != squares[x + 1][y]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y));
		}
	}
	if ((x - 1 >= 0)) {
		if ((squares[x - 1][y] == nullptr) || (King::colour != squares[x - 1][y]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y));
		}
	}
	if ((y + 1 <= 7)) {
		if ((squares[x][y + 1] == nullptr) || (King::colour != squares[x][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x, y + 1));
		}
	}
	if ((y - 1 >= 0)) {
		if ((squares[x][y - 1] == nullptr) || (King::colour != squares[x][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x, y - 1));
		}
	}
	//White queenside castling
	if ((King::colour == Colour::White) && (x == 4) && (y == 0) && (squares[3][0] == nullptr) && (squares[2][0] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(2, 0));
	//White kingside castling
	if ((King::colour == Colour::White) && (x == 4) && (y == 0) && (squares[6][0] == nullptr) && (squares[5][0] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(6, 0));
	//Black queenside castling
	if ((King::colour == Colour::Black) && (x == 4) && (y == 7) && (squares[3][7] == nullptr) && (squares[2][7] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(2, 7));
	//Black kingside castling
	if ((King::colour == Colour::Black) && (x == 4) && (y == 7) && (squares[6][7] == nullptr) && (squares[5][7] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(6, 7));
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> King::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if ((x - 1 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 1][y + 1] != nullptr) && (King::colour != squares[x - 1][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 1));
		}
	}
	if ((x - 1 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 1][y - 1] != nullptr) && (King::colour != squares[x - 1][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 1));
		}
	}
	if ((x + 1 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 1][y + 1] != nullptr) && (King::colour != squares[x + 1][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 1));
		}
	}
	if ((x + 1 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 1][y - 1] != nullptr) && (King::colour != squares[x + 1][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 1));
		}
	}
	if ((x + 1 <= 7)) {
		if ((squares[x + 1][y] != nullptr) && (King::colour != squares[x + 1][y]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y));
		}
	}
	if ((x - 1 >= 0)) {
		if ((squares[x - 1][y] != nullptr) && (King::colour != squares[x - 1][y]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y));
		}
	}
	if ((y + 1 <= 7)) {
		if ((squares[x][y + 1] != nullptr) && (King::colour != squares[x][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + 1));
		}
	}
	if ((y - 1 >= 0)) {
		if ((squares[x][y - 1] != nullptr) && (King::colour != squares[x][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - 1));
		}
	}
	return Piece::captureMoveList;
}

char King::getSymbol() {
	return King::symbol;
}


PieceType King::getPieceType() {
	return King::pieceType;
}

