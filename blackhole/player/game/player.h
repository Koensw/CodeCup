#ifndef _LESS_PLAYER
#define _LESS_PLAYER

#include <vector>
#include <cstdint>

class Player{
public:
    enum Color{
        EMPTY = 0,
        RED = 1,
        BLUE = 2,
        JURY = 3,
    };
    
    static void init(Color me);
    
    static Player& getMe();
    static Player& getThem();
    bool isMe();
    bool isThem();

    Color getColor();
    int getID();
            
    static Player getByColor(Color color);
    static Player getByID(int_fast16_t id);
private:    
    Player(Color c): color_(c) {}
    
    Color color_;
    
    static std::vector<Player> players_;
    
    static Color me_;
    static Color them_;
};

#endif
