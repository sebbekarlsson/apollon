#include "include/main.h"
#include "include/scene_menu.h"
#include "include/scene_scene_editor.h"
#include "include/scene_scene_designer.h"
#include "include/scene_actor_editor.h"
#include "include/scene_sprite_editor.h"
#include "include/scene_text_editor.h"
#include "include/colors.h"
#include "include/image_utils.h"
#include "include/modal_manager.h"
#include <coelum/main.h>
#include <coelum/theatre.h>
#include <coelum/textures.h>


extern theatre_T* THEATRE;

main_state_T* MAIN_STATE;
database_T* DATABASE;

modal_manager_T* MODAL_MANAGER;


texture_T* TEXTURE_CHECKBOX_SHEET;
texture_T* TEXTURE_CHECKBOARD;

sprite_T* SPRITE_CHECKBOARD;
sprite_T* SPRITE_BROKEN;
sprite_T* SPRITE_PENCIL;
sprite_T* SPRITE_ERASOR;


void load_textures()
{
    TEXTURE_CHECKBOX_SHEET = get_texture("res/img/checkbox_sheet.png", GL_RGBA);
    TEXTURE_CHECKBOARD = get_texture("res/img/checkboard.png", GL_RGB);

    dynamic_list_T* sprite_checkboard_list = init_dynamic_list(sizeof(struct TEXTURE_STRUCT*));
    dynamic_list_append(sprite_checkboard_list, TEXTURE_CHECKBOARD);

    SPRITE_CHECKBOARD = init_sprite(
        sprite_checkboard_list,
        1.0f,
        TEXTURE_CHECKBOARD->width,
        TEXTURE_CHECKBOARD->height
    );

    SPRITE_BROKEN = init_sprite_from_file("res/img/broken.png", GL_RGBA, 0.0f, 16, 16);
    SPRITE_PENCIL = init_sprite_from_file("res/img/pencil_16.png", GL_RGBA, 0.0f, 16, 16);
    SPRITE_ERASOR = init_sprite_from_file("res/img/erasor.png", GL_RGBA, 0.0f, 16, 16);
}

main_state_T* init_main_state()
{
    main_state_T* main_state = calloc(1, sizeof(struct MAIN_STATE_STRUCT));
    main_state->scene_id = (void*) 0;
    main_state->modal_is_active = 0;

    return main_state;
}

int main(int argc, char* argv[])
{
    coelum_init();

    load_textures();

    MAIN_STATE = init_main_state();
    DATABASE = init_database();
    MODAL_MANAGER = init_modal_manager();

    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_text_editor()); // just for testing stuff

    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_menu());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_scene_editor());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_scene_designer());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_actor_editor());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_sprite_editor());

    if (coelum_main(argc, argv))
    {
        printf("Coelum crashed.");
    }

    return 0;
}
