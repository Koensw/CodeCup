#include "move.h"

#include "board.h"

Move::Move(std::string str, Player player): player_(player) {
#ifdef DEBUG
    if(str.size() < 4 || str[2] != '=') {
        throw std::domain_error("String " + str + " is not a valid move");
    }
#endif
    idx_ = Board::getIndex(str.substr(0, 2));
    num_ = std::atoi(str.substr(3).c_str());
#ifdef DEBUG
    if(num_ < 1 || num_ > 15) {
        throw std::domain_error("String " + str + " is not a valid move");
    }
#endif
}

Move::Move(int idx, int num, Player player): idx_(idx), num_(num), player_(player) {
#ifdef DEBUG
    Board::checkIndex(idx);
    if(num_ < 1 || num_ > 15) {
        throw std::domain_error("Move " + std::to_string(idx) + "=" + std::to_string(num) + " is not a valid move");
    }
#endif
}

int Move::getIndex() const {
    return idx_;
}
int Move::getNum() const {
    return num_;
}
std::pair<int, int> Move::getIndexNum() const {
    return std::make_pair(idx_, num_);
}

Player Move::getPlayer() const {
    return player_;
}

std::string Move::getString() {
    return Board::getString(idx_) + "=" + std::to_string(num_);
}
