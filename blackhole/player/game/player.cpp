#include "player.h"

#include <cassert>
#include <iostream>

std::vector<Player> Player::players_;
Player::Color Player::me_;
Player::Color Player::them_;

void Player::init(Color col_me){
    players_.push_back(Player(Player::EMPTY));
    players_.push_back(Player(Player::RED));
    players_.push_back(Player(Player::BLUE));
    players_.push_back(Player(Player::JURY));
    
    me_ = col_me;
    them_ = static_cast<Player::Color>(!(static_cast<int>(col_me)-1)+1);
}

Player& Player::getMe() {
    return players_.at(static_cast<int>(Player::me_));
}
Player& Player::getThem() {
    return players_.at(static_cast<int>(Player::them_));
}

bool Player::isMe() {
    return color_ == getMe().color_;
}
bool Player::isThem() {
    return color_ == getThem().color_;
}

Player::Color Player::getColor() {
    return this->color_;
}
int Player::getID(){
    return static_cast<int>(this->color_);
}

Player Player::getByColor(Player::Color color){
    return getByID(static_cast<int>(color));
}
Player Player::getByID(int_fast16_t id){
    assert(0 <= id && id <= 3);
    return players_[id];
}
