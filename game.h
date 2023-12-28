#include <SDL.h>
#include "data_struct.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "dungeon_struct.h"
#include "map_input.h"

//Window var
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
int i_window_width = 1280;
int i_window_height = 720;
int i_pixel_size = 16;
int i_window_opened;

//TileSetVar
const char file_path[] = "2D Pixel Dungeon Asset Pack\\character and tileset\\Dungeon_Tileset.png";

//GlobalVar
float f_global_scale = 1;

//Fps var
Uint64 i_frequency;
Uint64 i_start_time;
Uint64 i_end_time;
double d_delta_time;
int i_fps;

//DrawManagerVar
List* first_layer_list;
List* second_layer_list;
List* ui_layer_list;

//Camera2d
actor_2d camera_2d;

//List Actors
List* actors;

//------------------------------------------------------------------------InitGame------------------------------------------------------------------------
void Dungeon_add_actor_to_draw_list(rendered_actor_2d* actor){
    Node* actor_node = malloc(sizeof(Node));
    actor_node->data = actor;
    actor_node->next = NULL;

    switch (actor->layer)
    {
        case First:
            first_layer_list = Data_struct_list_add(first_layer_list, actor_node);
            break;
        case Second:
            second_layer_list = Data_struct_list_add(second_layer_list, actor_node);
            break;
        case Ui:
            ui_layer_list = Data_struct_list_add(ui_layer_list, actor_node);
            break;
        case Last:
            break;
    }
}
void Dungeon_add_character_to_main_list(character* actor){
    Node* actor_node = malloc(sizeof(Node));
    actor_node->data = actor;
    actor_node->next = NULL;

    actors = Data_struct_list_add(actors,actor_node);
}
void Dungeon_add_rendered_actor_to_main_list(rendered_actor_2d* actor){
    Node* actor_node = malloc(sizeof(Node));
    actor_node->data = actor;
    actor_node->next = NULL;

    actors = Data_struct_list_add(actors,actor_node);
}

rendered_actor_2d* Dungeon_create_rendered_actor(float pos_x, float pos_y,int tile_x, int tile_y,enum Layer layer) {
    rendered_actor_2d* new_character = malloc(sizeof(rendered_actor_2d));
    if (!new_character) {
        SDL_Log("Error allocating memory for character");
        return NULL;
    }

    *new_character = create_rendered_actor(pos_x, pos_y, 1, tile_x, tile_y,layer);

    Dungeon_add_actor_to_draw_list(new_character);
    Dungeon_add_rendered_actor_to_main_list(new_character);

    return new_character;
}

character* Dungeon_create_character(float pos_x, float pos_y,int tile_x, int tile_y,enum Layer layer,int move_speed, int playerID ) {
    character* new_character = malloc(sizeof(character));
    if (!new_character) {
        SDL_Log("Error allocating memory for character");
        return NULL;
    }

    *new_character = create_character(pos_x, pos_y, 1, tile_x, tile_y,layer,move_speed,playerID);

    Dungeon_add_actor_to_draw_list(&new_character->rendered_actor);
    Dungeon_add_character_to_main_list(new_character);

    return new_character;
}

int Dungeon_init_game(){
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("Error initializing SDL: %s", SDL_GetError);
        return -1;
    }

    //Init window
    window = SDL_CreateWindow("Dungeon Crawler",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,i_window_width,i_window_height,0);

    if (!window){
        SDL_Log("Error creating SDL Window: %s", SDL_GetError);
        SDL_Quit();
        return -1;
    }

    //Generate Renderer
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer){
        SDL_Log("Error creating SDL Renderer: %s", SDL_GetError);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    //Generate Pixels data
    int width,height,channels;
    unsigned char* pixels = stbi_load(file_path,&width,&height,&channels,0);

    if (!pixels){
        SDL_Log("Error loading Image: %s", SDL_GetError);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //Print image size to know if it is loaded correctly
    //SDL_Log("Image size: W %d, H %d C: %d\n", width, height, channels);

    //Generate Texture from pixels data
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,width,height);
    if (!texture){
        SDL_Log("Error creating SDL Texture: %s", SDL_GetError());
        stbi_image_free(pixels);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);

    if (SDL_UpdateTexture(texture, NULL, pixels, width * 4) != 0)
    {
        SDL_Log("Error update texture: %s", SDL_GetError());
        stbi_image_free(pixels);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    stbi_image_free(pixels);

    //Data to calculate fps
    i_frequency = SDL_GetPerformanceFrequency();
    i_start_time = SDL_GetPerformanceCounter();

    //Initialize Lists
    first_layer_list = Data_struct_create_list();
    second_layer_list = Data_struct_create_list();
    ui_layer_list = Data_struct_create_list();
    actors = Data_struct_create_list();

    //Create actors
    character* tester_layer_0 = Dungeon_create_character(i_window_width * 0.5, i_window_height * 0.5, 9, 9, First, 235, 1);
    character* tester_2_layer_0 = Dungeon_create_character(20, 0, 9, 9, First, 0, 0);
    character* tester2 = Dungeon_create_character(0, 0, 8, 9, Second, 0, 0);
    rendered_actor_2d* tester3 = Dungeon_create_rendered_actor(i_window_width * 0.5, 0, 1, 8, Ui);

    actor_2d camera_2d = create_actor(
        tester_layer_0->rendered_actor.actor.f_pos_x,
        tester_layer_0->rendered_actor.actor.f_pos_y
        );

    i_window_opened = 1;
    return 0;
}

//------------------------------------------------------------------------Input----------------------------------------------------------------------------
void Dungeon_move_camera(float amount_X,float amount_Y){
    camera_2d.f_pos_x += amount_X;
    camera_2d.f_pos_y += amount_Y;

    Node* current_node = actors->head;
    while (current_node)
    {
        if ((character*)current_node->data){
            if (((character*)current_node->data)->rendered_actor.layer == Ui){
                ((character*)current_node->data)->rendered_actor.actor.f_pos_x += amount_X;
                ((character*)current_node->data)->rendered_actor.actor.f_pos_y += amount_Y;
            }
        }
        else if((rendered_actor_2d*)current_node->data){
            if (((rendered_actor_2d*)current_node->data)->layer == Ui){
                ((rendered_actor_2d*)current_node->data)->actor.f_pos_x += amount_X;
                ((rendered_actor_2d*)current_node->data)->actor.f_pos_y += amount_Y;
            }
        }
        current_node = current_node->next;
    }
}
void Dungeon_handle_input(){
    //Window input
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            i_window_opened =  0;
        }
        if (event.type == SDL_MOUSEWHEEL) {
            f_global_scale += event.wheel.y * 0.1f;
            if (f_global_scale < 0.1f) {
                f_global_scale = 0.1f;
            }

            Node* current_actor = actors->head;
            while (current_actor) {
                character* actor = (character*)current_actor->data;
                if (actor->i_n_player_controller == 1) {
                    float new_camera_x = actor->rendered_actor.actor.f_pos_x - (i_window_width * 0.5) / f_global_scale;
                    float new_camera_y = actor->rendered_actor.actor.f_pos_y - (i_window_height * 0.5) / f_global_scale;

                    camera_2d.f_pos_x = new_camera_x;
                    camera_2d.f_pos_y = new_camera_y;
                    break;
                }
                current_actor = current_actor->next;
            }   
        }   
    }
    //Keyboard input
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
    if (keyboard[Map_input_exit()]){
        i_window_opened =  0;
    }
    if (keyboard[Map_input_move_right()]){
        Node* current_actor = actors->head;
        while (current_actor){
            character* actor = (character*)current_actor->data;
            if (actor->i_n_player_controller == 1)
            {
                Dungeon_move_camera(1,0);
                actor->rendered_actor.actor.f_pos_x += actor->i_movement_speed * d_delta_time;
                break;
            }
            current_actor = current_actor->next;
        }
    }
    if (keyboard[Map_input_move_left()]){
        Node* current_actor = actors->head;
        while (current_actor){
            character* actor = (character*)current_actor->data;
            if (actor->i_n_player_controller == 1)
            {
                Dungeon_move_camera(-1,0);
                actor->rendered_actor.actor.f_pos_x-= actor->i_movement_speed * d_delta_time;
                break;
            }
            current_actor = current_actor->next;
        }
    }
    if (keyboard[Map_input_move_up()]){
        Node* current_actor = actors->head;
        while (current_actor){
            character* actor = (character*)current_actor->data;
            if (actor->i_n_player_controller == 1)
            {
                Dungeon_move_camera(0,-1);
                actor->rendered_actor.actor.f_pos_y -= actor->i_movement_speed * d_delta_time;
                break;
            }
            current_actor = current_actor->next;
        }
    }
    if (keyboard[Map_input_move_down()]){
        Node* current_actor = actors->head;
        while (current_actor){
            character* actor = (character*)current_actor->data;
            if (actor->i_n_player_controller == 1)
            {
                Dungeon_move_camera(0,1);
                actor->rendered_actor.actor.f_pos_y += actor->i_movement_speed * d_delta_time;
                break;
            }
            current_actor = current_actor->next;
        }
    }
}
//------------------------------------------------------------------------Draw-----------------------------------------------------------------------------
int Dungeon_get_tile(const int offsetX, const int offsetY, const int width, const int height, int *xCoord, int *yCoord)
{
    if (offsetX < 0 || offsetY < 0)
    {
        return -1;
    }

    const int tile_w = 16;
    const int tile_h = 16;

    int xvalue = tile_w * offsetX;
    int yvalue = tile_h * offsetY;

    if (xvalue > width - tile_w || yvalue > height - tile_h)
    {
        return -1;
    }

    *xCoord = xvalue;
    *yCoord = yvalue;

    return 0;
}
int Dungeon_draw_actor(rendered_actor_2d rendered_actor){

    if (rendered_actor.i_visible <= 0){
        return 0;
    }
    
    SDL_FRect dest_container = {
        (rendered_actor.actor.f_pos_x - camera_2d.f_pos_x) * f_global_scale, 
        (rendered_actor.actor.f_pos_y - camera_2d.f_pos_y) * f_global_scale, 
        16 * f_global_scale, 
        16 * f_global_scale
    };

    SDL_FRect camera = {0, 0, i_window_width, i_window_height};

    if(!SDL_HasIntersectionF(&dest_container, &camera))
    {
        return 0;
    }

    int width;
    int height;

    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Rect offsetRect = {0, 0, i_pixel_size, i_pixel_size};

    if(Dungeon_get_tile(rendered_actor.i_tile_x, rendered_actor.i_tile_y, width, height, &offsetRect.x, &offsetRect.y) < 0){
        return -1;
    }

    SDL_RenderCopyF(renderer, texture, &offsetRect, &dest_container);

    return 0;
}
int Dungeon_draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Draw first layer
    Node *current = first_layer_list->head;
    while (current != NULL) {
        rendered_actor_2d* current_actor = (rendered_actor_2d*)current->data;
        Dungeon_draw_actor((*current_actor));
        current = current->next;
    }

    // Draw second layer
    current = second_layer_list->head;
    while (current != NULL) {
        rendered_actor_2d *current_actor = (rendered_actor_2d *)current->data;
        Dungeon_draw_actor(*current_actor);
        current = current->next;
    }

    // Draw ui layer
    current = ui_layer_list->head;
    while (current != NULL) {
        rendered_actor_2d *current_actor = (rendered_actor_2d *)current->data;
        Dungeon_draw_actor(*current_actor);
        current = current->next;
    }

    SDL_RenderPresent(renderer);

    return 0;
}
//------------------------------------------------------------------------Fps-------------------------------------------------------------------------------
void Dungeon_calculate_fps(){
    i_end_time = SDL_GetPerformanceCounter();
    d_delta_time = ((double)(i_end_time - i_start_time))/i_frequency;
    i_start_time = i_end_time;
    i_fps = (int)(1.0 / d_delta_time);

    //Decomment For Debug count fps
    //SDL_Log("FPS: %d", i_fps);
}
//--------------------------------------------------------------------Clear memory--------------------------------------------------------------------------
int Dungeon_clear_memory(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}