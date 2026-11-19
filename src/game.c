#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"
#include "L_Entity.h"
#include "L_Player.h"
#include "L_Structure.h"
#include "L_gameTime.h"
#include "L_Enemy.h"
#include "L_Structure.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}


int main(int argc,char *argv[])
{
    //local variables
    Sprite *bg;
    Entity *agu, *agu2, *agu3, *agu4, *agu5, *agu6, *struc, *struc1, * struc2, * struc3, * struc4, *planet;

    GFC_Matrix4 mat = { 0 };
    GFC_Matrix4* view = gf3d_vgraphics_get_view_matrix();
    Uint32 gameTime = SDL_GetTicks();
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init(); // DATABASE SETUP
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    // gfc_sound_init_config(); AUDIO=
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    

    agu = Player_load_from_file("models/dino.model");
    agu2 = Enemy_Pinwheel_load_from_file("models/pinwheel.json");
    agu3 = Enemy_Bracewheel_load_from_file("models/pinwheel.json");
    agu4 = Enemy_Flywheel_load_from_file("models/pinwheel.json");
    agu5 = Enemy_Soar_Wheel_load_from_file("models/pinwheel.json");
    agu6 = Enemy_Shockwheel_load_from_file("models/pinwheel.json");

    struc = Structure_Interaction_load_from_file("models/interactable.json", 4, 2, 1);
    struc1 = Structure_Interaction_load_from_file("models/interactable.json", 4, 2, 2);
    struc2 = Structure_Interaction_load_from_file("models/interactable.json", 4, 2, 3);
    struc3 = Structure_Interaction_load_from_file("models/interactable.json", 4, 2, 4);
    struc4 = Structure_Interaction_load_from_file("models/interactable.json", 4, 2, 5);
  

   
    planet = Structure_load_from_file("models/planet.json", 160, 2);

    planet->translation = gfc_vector3d(1, -250, -24);
    agu3->translation = gfc_vector3d(1, 1, 3);
    agu2->translation = gfc_vector3d(1, 1, -3);
    agu4->translation = gfc_vector3d(1, -160, -3);
    agu6->translation = gfc_vector3d(1, -160, -30);

    struc->translation = gfc_vector3d(1, -160, -80);
    struc1->translation = gfc_vector3d(30, -160, -80);
    struc2->translation = gfc_vector3d(60, -180, -80);
    struc3->translation = gfc_vector3d(30, -200, 80);
    struc4->translation = gfc_vector3d(1, -200, 80);




    agu->gravityPoint = &planet->translation;
    agu->gravityEntity = &planet;

    agu2->gravityPoint = &planet->translation;
    agu2->gravityEntity = &planet;

    agu3->gravityPoint = &planet->translation;
    agu3->gravityEntity = &planet;

    agu4->gravityPoint = &planet->translation;
    agu4->gravityEntity = &planet;

    agu5->gravityPoint = &planet->translation;
    agu5->gravityEntity = &planet;

    agu6->gravityPoint = &planet->translation;
    agu6->gravityEntity = &planet;


    struc->gravityPoint = &planet->translation;
    struc->gravityEntity = &planet;

    struc1->gravityPoint = &planet->translation;
    struc1->gravityEntity = &planet;

    struc2->gravityPoint = &planet->translation;
    struc2->gravityEntity = &planet;

    struc3->gravityPoint = &planet->translation;
    struc3->gravityEntity = &planet;

    struc4->gravityPoint = &planet->translation;
    struc4->gravityEntity = &planet;


    
    //agu = gf3d_model_load_from_file("models/dino.model");
    //gfc_matrix4_from_vectors(mat, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1));
    gfc_matrix4_view(*view, gfc_vector3d(20, 20, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 1));

    //game init
    srand(SDL_GetTicks());
    slog_sync();
   /* bg = gf2d_sprite_load_image("images/bg_flat.png");*/
    gf2d_mouse_load("actors/mouse.actor");
    // main game loop 
    
    updateLastFrameTime();
    slog_sync();
    while(!_done)
    {
       
        // Frame time update
        updateLastFrameTime();

        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        //camera updaes
        
        gf3d_vgraphics_render_start();
                //2D draws
                /*gf2d_sprite_draw_image(bg,gfc_vector2d(0,0));*/
                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                gf2d_mouse_draw();
                //3d draws
                /*gf3d_model_queue_render(agu, mat, GFC_COLOR_WHITE);*/
                Entity_Think();
                Entity_Update();
                
                

        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
