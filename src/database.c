#include "include/database.h"
#include "include/file_utils.h"
#include <string.h>
#include <coelum/actor.h>
#include <coelum/utils.h>
#include <spr/spr.h>
#include <coelum/textures.h>


database_T* init_database()
{
    database_T* database = calloc(1, sizeof(struct DATABASE_STRUCT));
    database->filename = "application.db";

    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(database->filename, &db);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return database;
    }

    char *sql = "CREATE TABLE IF NOT EXISTS actor_definitions(id TEXT, name TEXT, tick_script TEXT, draw_script TEXT, sprite_id TEXT);"
                "CREATE TABLE IF NOT EXISTS actor_instances(id TEXT, actor_definition_id TEXT, x FLOAT, y FLOAT, z FLOAT, scene_id TEXT);"
                "CREATE TABLE IF NOT EXISTS sprites(id TEXT, name TEXT, filepath TEXT);"
                "CREATE TABLE IF NOT EXISTS scenes(id TEXT, name TEXT, bg_r INT, bg_g INT, bg_b INT)";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return database;
    } 
    
    sqlite3_close(db);

    return database;
}

database_sprite_T* init_database_sprite(char* id, char* name, char* filepath, sprite_T* sprite)
{
    database_sprite_T* database_sprite = calloc(1, sizeof(struct DATABASE_SPRITE_STRUCT));
    database_sprite->id = id;
    database_sprite->name = name; 
    database_sprite->filepath = filepath;
    database_sprite->sprite = sprite;

    return database_sprite;
}

void database_sprite_free(database_sprite_T* database_sprite)
{
    sprite_free(database_sprite->sprite);
    free(database_sprite);
}

sqlite3_stmt* database_exec_sql(database_T* database, char* sql, unsigned int do_error_checking)
{
    sqlite3* db = database->db;
	sqlite3_stmt* stmt;
	
	sqlite3_open(database->filename, &db);

	if (db == NULL)
	{
		printf("Failed to open DB\n");
		return (void*) 0;
	}

	printf("Performing query...\n");
    printf("%s\n", sql);
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (do_error_checking)
    {
        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            printf("ERROR executing query: %s\n", sqlite3_errmsg(db));
            return (void*) 0;
        }
    }

	return stmt;
}

char* database_insert_sprite(database_T* database, const char* name, sprite_T* sprite)
{
    char* id = get_random_string(16);
    char* sql_template = "INSERT INTO sprites VALUES(\"%s\", \"%s\", \"%s\")";
    char* sql = calloc(300, sizeof(char));

    char* filepath = calloc(strlen("sprites/") + strlen(name) + strlen(".bin"), sizeof(char));
    sprintf(filepath, "sprites/%s.spr", name);

    sprintf(sql, sql_template, id, name, filepath);
    printf("%s\n", sql);

    sqlite3_stmt* stmt = database_exec_sql(database, sql, 1);
    sqlite3_finalize(stmt);
    sqlite3_close(database->db);
    free(sql);

    spr_frame_T** frames = (void*) 0;
    size_t frames_size = 0;

    for (int i = 0; i < sprite->textures->size; i++)
    {
        texture_T* texture = (texture_T*) sprite->textures->items[i];
        spr_frame_T* frame = spr_init_frame_from_data(texture->data, texture->width, texture->height);
        frames_size += 1;
        frames = realloc(frames, frames_size * sizeof(struct SPR_FRAME_STRUCT*));
        frames[frames_size-1] = frame;
    }

    spr_T* spr = init_spr(
        sprite->width,
        sprite->height,
        255,
        255,
        255,
        sprite->frame_delay,
        sprite->animate,
        frames,
        frames_size
    );

    spr_write_to_file(spr, filepath);

    spr_free(spr);

    return id;
}

void database_update_sprite_name_by_id(database_T* database, const char* id, const char* name)
{
    char* sql_template = "UPDATE sprites SET name=\"%s\" WHERE id=\"%s\"";
    char* sql = calloc(strlen(sql_template) + strlen(id) + strlen(name) + 1, sizeof(char));
    sprintf(sql, sql_template, name, id);

    sqlite3_stmt* stmt = database_exec_sql(database, sql, 0);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        printf("ERROR executing query: %s\n", sqlite3_errmsg(database->db));
    }

	sqlite3_finalize(stmt);
	sqlite3_close(database->db);
}

database_sprite_T* database_get_sprite_by_id(database_T* database, const char* id)
{
    char* sql_template = "SELECT * FROM sprites WHERE id=\"%s\" LIMIT 1";
    char* sql = calloc(strlen(sql_template) + strlen(id) + 1, sizeof(char));
    sprintf(sql, sql_template, id);

    sqlite3_stmt* stmt = database_exec_sql(database, sql, 0);

    const unsigned char* name = 0;
    const unsigned char* filepath = 0;
    sprite_T* sprite = (void*) 0;

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        name = sqlite3_column_text(stmt, 1);
        filepath = sqlite3_column_text(stmt, 2);

        if (filepath)
            sprite = load_sprite_from_disk(filepath);

        break;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(database->db);

    char* id_new = calloc(strlen(id) + 1, sizeof(char));
    strcpy(id_new, id);

    char* name_new = calloc(strlen(name) + 1, sizeof(char));
    strcpy(name_new, name);

    char* filepath_new = calloc(strlen(filepath) + 1, sizeof(char));
    strcpy(filepath_new, filepath);

    
    return init_database_sprite(id_new, name_new, filepath_new, sprite);
}
