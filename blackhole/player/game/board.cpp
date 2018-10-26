#include "board.h"

#include "move.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unistd.h>

std::map<int, std::string> Board::idx_to_string_;
std::vector<std::vector<int>> Board::neighbours_;

static int _get_index_helper(std::string str) {
    assert(str.size() == 2);
    int r = str.at(0) - 'A';
    int c = str.at(1) - '1';
    
    int a = 36 - (8 - r) * (9 - r) / 2 + c;
    return a;
}
void Board::checkIndex(int idx) {
    if(idx < 0 || idx >= 36) {
        throw std::domain_error("Index " + std::to_string(idx) + " is not legal");
    }
}
int Board::getIndex(std::string str) {
    auto idx = _get_index_helper(str);
#ifdef DEBUG
    checkIndex(idx);
#endif
    return idx;
}
bool Board::valid_index(std::string str) {
    auto a = _get_index_helper(str);
    if(a < 0 || a >= 36) {
        return false;
    }
    return getString(a) == str;
}
std::string Board::getString(int idx) {
    assert(idx < 36);
    return idx_to_string_[idx];
}

void Board::init(std::array<std::string, 5> jury_moves){
    // Fill index to string lookup
    int idx = 0;
    for(int i=0; i<8; ++i) {
        for(int j=0; j<(8-i); ++j) {
            std::string str = "A1";
            str[0] += i;
            str[1] += j;
            
            idx_to_string_[idx++] = str;
        }
    }
    
    // Determine neighbours
    for(int i=0; i<8; ++i) {
        for(int j=0; j<(8-i); ++j) {
            std::string str = "A1";
            str[0] += i;
            str[1] += j;
            
            std::vector<int> neighbours;
            for(int i=-1; i<=1; ++i) {
                for(int j=-1; j<=1; ++j) {
                    if(i == j) continue;
                    std::string nstr = str;
                    nstr[0] += i;
                    nstr[1] += j;
                    
                    if(valid_index(nstr)) neighbours.push_back(getIndex(nstr));
                }
            }
            neighbours_.push_back(neighbours);
        }
    }
    
    // Fill stones array
    for(int i=0; i<2; ++i){
        for(int j=0; j<15; ++j) {
            stones_[i].push_back(j+1);
        }
    }
    
    // Apply jury moves
    for(auto& move : jury_moves) {
        set(getIndex(move), Player::getByColor(Player::JURY), 0);
    }
}

void Board::apply(Move mv) {
    // TODO: add checks
    auto nid = mv.getPlayer().getID()-1;
    auto iter = std::find(stones_[nid].begin(), stones_[nid].end(), mv.getNum());
    stones_[nid].erase(iter);
    Board::set(mv.getIndex(), mv.getPlayer(), mv.getNum());
}

std::vector<int> Board::getEmpty() {
    std::vector<int> result;
    for(int i=0; i<36; ++i) {
        Player::Color col = getPlayer(i).getColor();
        if(col == Player::EMPTY) {
            result.push_back(i);
        }
    }
    return result;
}

std::vector<int> Board::getStones(Player pl) {
    // TODO: add checks
    return stones_[pl.getID()-1];
}

std::pair<Player, int> Board::get(int k){
    for(int i=0; i<3; ++i) {
        if(state_[i][k] != -1) {
            return std::make_pair(Player::getByID(i+1), state_[i][k]);
        }
    }
    
    return std::make_pair(Player::getByColor(Player::EMPTY), 0);
}
Player Board::getPlayer(int k) {
    return get(k).first;
}
int Board::getValue(int k) {
    return get(k).second;
}

std::vector<int> Board::getNeighbours(int idx) {
    assert(0 <= idx && idx < 36);
    return neighbours_[idx];
}

static int print_grid[8][32];
void Board::print(){
    if(!isatty(fileno(stderr))) return;
    
    for(int i=0; i<8; ++i) {
        for(int j=0; j<32; ++j) {
            print_grid[i][j] = -1;
        }
    }
    
    int k = 0;
    for(int i=0; i<8; ++i) {
        int si = i; int sj = 16 - 2*i;
        for(int j=0; j<(8-i); ++j) {
            print_grid[si][sj] = k;
            ++si; ++sj;
            ++k;
        }
    }
    
    for(int i=0; i<8; ++i) {
        for(int j=0; j<32; ++j) {
            if(print_grid[i][j] == -1) std::cerr << " ";
            else {
                Player::Color col = getPlayer(print_grid[i][j]).getColor();
                std::string s;
                switch(col){
                    case Player::RED:
                        s = "\x1B[31;1m";
                        break;
                    case Player::BLUE:
                        s = "\x1B[36;1m";
                        break;
                    case Player::JURY:
                        s = "\x1B[33;1m";
                        break;
                    default:
                        break;
                }
                
                if(col == Player::EMPTY) { 
                    s = "..";
                } else {
                    //s += b.getValue(print_grid[i][j]);
                    std::string num = std::to_string(getValue(print_grid[i][j]));
                    s += num;
                    if(num.size() == 1) s += " ";
                    s += "\x1B[0m";
                }
                
                std::cerr << s;
            }
        }
        std::cerr << std::endl;
    }
}

void Board::set(int k, Player p, int n){
    state_[p.getID()-1][k] = n;
}
