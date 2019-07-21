#include "include/main.h"
#include "include/scene_menu.h"
#include "include/scene_scene_editor.h"
#include "include/scene_scene_designer.h"
#include "include/scene_actor_editor.h"
#include "include/scene_sprite_editor.h"
#include "include/colors.h"
#include "include/image_utils.h"
#include "include/database.h"
#include <coelum/main.h>
#include <coelum/theatre.h>


extern theatre_T* THEATRE;

main_state_T* MAIN_STATE;
database_T* DATABASE;


main_state_T* init_main_state()
{
    main_state_T* main_state = calloc(1, sizeof(struct MAIN_STATE_STRUCT));
    main_state->scene_id = (void*) 0;

    return main_state;
}

int main(int argc, char* argv[])
{
    coelum_init();

    MAIN_STATE = init_main_state();
    DATABASE = init_database();

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
