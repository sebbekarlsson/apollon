#include "include/database.h"
#include <string.h>
#include <coelum/actor.h>
#include <coelum/utils.h>


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

database_sprite_T* init_database_sprite(const unsigned char* id, const unsigned char* name, const unsigned char* filepath)
{
    database_sprite_T* database_sprite = calloc(1, sizeof(struct DATABASE_SPRITE_STRUCT));
    database_sprite->id = id;
    database_sprite->name = name; 
    database_sprite->filepath = filepath;

    return database_sprite;
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
    sprintf(filepath, "sprites/%s.bin", name);

    sprintf(sql, sql_template, id, name, filepath);
    printf("%s\n", sql);

    sqlite3_stmt* stmt = database_exec_sql(database, sql, 1);
    sqlite3_finalize(stmt);
    sqlite3_close(database->db);
    free(sql);

    return id;
}

dynamic_list_T* database_get_all_sprites(database_T* database)
{
    dynamic_list_T* sprites = init_dynamic_list(sizeof(struct DATABASE_SPRITE_STRUCT*));

    sqlite3_stmt* stmt = database_exec_sql(database, "SELECT * FROM sprites", 0);

    printf("Got results:\n");
	while (sqlite3_step(stmt) != SQLITE_DONE) {
        const unsigned char* id = sqlite3_column_text(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* filepath = sqlite3_column_text(stmt, 2);

        dynamic_list_append(
            sprites,
            init_database_sprite(
                id,
                name,
                filepath
            )
        );
	}

	sqlite3_finalize(stmt);

	sqlite3_close(database->db);
    getc(stdin);

    return sprites;
}
