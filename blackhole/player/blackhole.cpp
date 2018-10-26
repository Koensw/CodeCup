/*
 * AI Player Blackhole Codecup
 * 
 * (c) Koen Wolters 2016
 */

#include <algorithm>
#include <iostream>

#include "util/rand.h"
#include "game/player.h"
#include "game/board.h"
#include "game/move.h"

#include <climits>
#include <cassert>

int main(int argc, char** argv){
    std::cerr << "R KBHT" << std::endl;
    
    // Initializer global seed
    rand_gen.seed();
    
    bool check_mode = false;
    if(argc > 1 && argv[1][0] == 'c') {
        std::cerr << "MODE CHECK" << std::endl;
        check_mode = true;
    }
    
    // Read board initialization (unless short check is enabled)
    std::array<std::string, 5> jury_moves;
    for(int i=0; i<5; ++i) {
        std::string s;
        if(!check_mode) {
            std::cin >> s;
        } else {
            s = "A1";
            s[1] += i;
        }
        jury_moves.at(i) = s;
    }
    Board brd;
    
    // Initialize player
    std::string str;
    std::cin >> str;
    
    if(str == "Start") {
        Player::init(Player::RED);
        brd.init(jury_moves);
    } else {
        Player::init(Player::BLUE);
        brd.init(jury_moves);
        
        Move move(str, Player::getThem());
        brd.apply(move);
    }
    
    while(true) {
        std::vector<int> moves = brd.getEmpty();
        if(moves.size() == 1) break;
        
        // Find 'best' index
        int best_idx = -1; int best_score = 0; int mx = INT_MIN; 
        for(auto& idx : moves) {
            int their_score = 0;
            auto ngbs = Board::getNeighbours(idx);
            int mx_score = -1;
            for(auto& ngb : ngbs) {
                if(brd.getPlayer(ngb).getColor() == Player::EMPTY) {
                    auto side_ngbs = Board::getNeighbours(ngb);
                    int cur_score = 0;
                    for(auto& side_ngb : side_ngbs) {
                        if(brd.getPlayer(side_ngb).isThem()) cur_score += brd.getValue(side_ngb);
                        if(brd.getPlayer(side_ngb).isMe()) cur_score -= brd.getValue(side_ngb);
                    }
                    if(cur_score > mx_score) mx_score = cur_score;
                    their_score += cur_score;
                }
                if(brd.getPlayer(ngb).isMe()) {
                    --their_score;
                }
                
            }
            
            if(their_score > mx) {
                mx = their_score;
                best_score = mx_score;
                best_idx = idx; 
            }
        }
                
        // If the best cell has negative score then block the best cell of them if possible
        if(best_score == -1) {
            for(auto& idx : moves) {
                auto ngbs = Board::getNeighbours(idx);
                bool opt = true;
                int cur_score = 0;
                for(auto& ngb : ngbs) {
                    if(brd.getPlayer(ngb).getColor() == Player::EMPTY) {
                        opt = false;
                        break;
                    }
                    if(brd.getPlayer(ngb).isThem()) cur_score += brd.getValue(ngb);
                    if(brd.getPlayer(ngb).isMe()) cur_score -= brd.getValue(ngb);
                }
                
                if(opt && cur_score > mx) {
                    mx = cur_score;
                    best_idx = idx;
                    best_score = 0;
                }
            }
        }
                
        // Find best stone (try to get enough to cancel problem otherwise pick highest)
        auto stones = brd.getStones(Player::getMe());
        auto iter = std::lower_bound(stones.begin(), stones.end(), best_score-1);
        if(iter == stones.end()) {
            iter = --stones.end();
        }
        int num = *iter;
       
        // Apply it
        Move move(best_idx, num, Player::getMe());
        brd.apply(move);
        std::cout << move.getString() << std::endl;
        
        // Handle other player
        std::string str;
        std::cin >> str;
        if(std::cin.fail() || str == "Quit") break;
       
        Move their_move(str, Player::getThem());
        brd.apply(their_move);
    }
    
    if(check_mode) {
        // Print board
        brd.print();
    }
}
