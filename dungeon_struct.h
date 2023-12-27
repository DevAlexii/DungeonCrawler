enum Layer { First , Second , Ui, Last};

typedef struct actor_2d{
    //Position on screen
    float f_pos_x;
    float f_pos_y;
}actor_2d;

actor_2d create_actor(float pos_x ,float pos_y){

    actor_2d new_actor;
    new_actor.f_pos_x = pos_x;
    new_actor.f_pos_y = pos_y;
    
    return new_actor;
}

typedef struct rendered_actor_2d{
    actor_2d actor;
    //Has to be rendered
    int i_visible;
    //Tile to draw
    int i_tile_x;
    int i_tile_y;
    //Layer
    enum Layer layer;
}rendered_actor_2d;

rendered_actor_2d create_rendered_actor(float pos_x, float pos_y,int visible, int tile_x, int tile_y, enum Layer layer){
    rendered_actor_2d new_actor;
    new_actor.actor = create_actor(pos_x, pos_y);
    new_actor.i_visible = visible;
    new_actor.i_tile_x = tile_x;
    new_actor.i_tile_y = tile_y;
    new_actor.layer = layer;
    
    return new_actor;
}

typedef struct character{
    rendered_actor_2d rendered_actor;
    int i_movement_speed;
    int i_n_player_controller;
}character;

/**
 * Create new character with given data
 *
 * @param pos_x - Initial pos X for actor.
 * @param pos_y - Initial pos Y for actor.
 * @param visible - value <= 0 Invisible , value > 0 Visible.
 * @param tile_x - Tile x.
 * @param tile_y - Tile y.
 * @param layer - Actor layer.
 * @param move_speed - Max movement speed.
 * @param n_player_controller - Controller ID , value less or equal zero not possessed, value greater 0 actor be possessed.
 * @return created character.
 */
character create_character(float pos_x, float pos_y, int visible, int tile_x, int tile_y, enum Layer layer, int move_speed, int n_player_controller) {
    character new_actor;

    new_actor.rendered_actor = create_rendered_actor(pos_x, pos_y, visible, tile_x, tile_y, layer);
    new_actor.i_movement_speed = move_speed;
    new_actor.i_n_player_controller = n_player_controller == 0 ? -1 : n_player_controller;

    return new_actor;
}