#include "game.h"
extern int i_window_opened;

int main(int argc,char** argv){
    if (Dungeon_init_game()){
        return -1;
    }

    //GameLoop
    while (i_window_opened){
        //Input
        Dungeon_handle_input();
        //Update

        //Draw
        Dungeon_draw();

        //Last call before cycle
        Dungeon_calculate_fps();
    }
    
    //Clear all memory before close game
    if(Dungeon_clear_memory()){
        return -1;
    }
    return 0;
}
