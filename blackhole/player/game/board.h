#ifndef _LESS_BOARD
#define _LESS_BOARD

#include "player.h"
#include "move.h"

#include <array>
#include <map>
#include <vector>
#include <cstdint>

class Board{
public:
    Board() {
        for(int i=0; i<3; ++i) 
            for(int j=0; j<36; ++j) 
                state_[i][j] = -1;
    }

    void init(std::array<std::string, 5>);
    
    // Board locations
    std::pair<Player, int> get(int k);
    Player getPlayer(int k);
    int getValue(int k);
    
    // Stone information
    std::vector<int> getStones(Player p);
    
    // Apply move
    void apply(Move);
    
    // TODO: add separate move
    //std::vector<Move> getMoves(Player);
    std::vector<int> getEmpty();
    
    // Print board
    void print();
    
    // Neighbours indices
    static std::vector<int> getNeighbours(int idx);
    
    // Index conversion functions
    static void checkIndex(int);
    static int getIndex(std::string);
    static std::string getString(int idx);
private:
    // Check index validity
    static bool valid_index(std::string);
    
    // Update board
    void set(int k, Player p, int n);
    
    // TODO: optimize to smaller state
    int state_[3][36];
    std::vector<int> stones_[2];
    
    static std::map<int, std::string> idx_to_string_;
    static std::vector<std::vector<int>> neighbours_;
};

#endif
