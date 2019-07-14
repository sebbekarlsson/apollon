#include <coelum/main.h>
#include <coelum/theatre.h>
#include "include/scene_menu.h"
#include "include/scene_scene_editor.h"
#include "include/scene_actor_editor.h"
#include "include/scene_sprite_editor.h"
#include "include/colors.h"
#include "include/image_utils.h"
#include "include/database.h"


extern theatre_T* THEATRE;

database_T* DATABASE;

int main(int argc, char* argv[])
{
    coelum_init();

    DATABASE = init_database();

    database_serialize(DATABASE);

    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_menu());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_scene_editor());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_actor_editor());
    scene_manager_register_scene(THEATRE->scene_manager, (scene_T*) init_scene_sprite_editor());

    if (coelum_main(argc, argv))
    {
        printf("Coelum crashed.");
    }

    return 0;
}
