#ifndef _LESS_MOVE
#define _LESS_MOVE

#include "player.h"

#include <string>
#include <utility>

class Move{
public:
    Move(std::string str, Player player);
    Move(int idx, int num, Player player);
    
    int getIndex() const;
    int getNum() const;
    std::pair<int, int> getIndexNum() const;
    
    Player getPlayer() const;
    
    std::string getString();
private:
    int idx_;
    int num_;
    Player player_;
};

#endif
