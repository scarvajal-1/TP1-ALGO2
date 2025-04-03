#include "pokedex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct pokedex {
    struct pokemon** pokemon_list;
    size_t count;
    size_t capacity;
};

tipo_pokemon string_to_type(const char* str_type) {
    if (strcmp(str_type, "AGUA") == 0){
        return TIPO_AGUA;
    }
    if (strcmp(str_type, "FUEGO") == 0){
        return TIPO_FUEGO;
    }
    if (strcmp(str_type, "PLANTA") == 0){
        return TIPO_PLANTA;
    }
    if (strcmp(str_type, "ROCA") == 0){
        return TIPO_ROCA;
    }
    if (strcmp(str_type, "ELECTRICO") == 0){
        return TIPO_ELECTRICO;
    }
    if (strcmp(str_type, "NORMAL") == 0){
        return TIPO_NORMAL;
    }
    if (strcmp(str_type, "LUCHA") == 0){
        return TIPO_LUCHA;
    }

    return (tipo_pokemon)-1;
}

pokedex_t *pokedex_abrir(const char *archivo){

    FILE* file = fopen(archivo, "r");
    if(!file){
        perror("Error al abrir el archivo");
        return NULL;
    }

    pokedex_t* pokedex = malloc(sizeof(pokedex_t));
    if(!pokedex){
        fclose(file);
        perror("Error al asignar memoria a la pokedex");
        return NULL;
    }

    pokedex->pokemon_list = NULL;
    pokedex->count = 0;
    pokedex->capacity = 10;
    pokedex->pokemon_list = malloc(pokedex->capacity * sizeof(struct pokemon*));

    if(!pokedex->pokemon_list){
        free(pokedex);
        fclose(file);
        perror("Error al asignar memoria a la lista de pokemones");
        return NULL;
    }

    char line[256];
    bool error_found = false;
    unsigned line_number = 0;


    while(fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        char* ptr = line;
        char* pokemon_name_start = NULL;
        char* pokemon_name_end = NULL;
        char* dynamic_pokemon_name = NULL;
        struct pokemon* new_pokemon = NULL;


        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0){
            continue;
        }

        unsigned id = 0;
        int offset = 0;

        if(sscanf(ptr, "%u%n", &id, &offset) != 1 || ptr[offset] != ';') {
            fprintf(stderr, "Error linea %u: Formato invalido (ID no encontrado o seguido de ';'). Contenido: '%s'\n", line_number, line);
            error_found = true;
            break;
        }

        ptr += offset + 1;

        pokemon_name_start = ptr;
        pokemon_name_end = strchr(ptr, ';');
    
        if(pokemon_name_end == NULL){
            fprintf(stderr, "Error linea %u: Formato invalido (Falta ';' despues del nombre). Contenido: '%s'\n", line_number, line);
            error_found = true;
            break;
        }

        
        size_t name_size = pokemon_name_end - pokemon_name_start;
        dynamic_pokemon_name = malloc(name_size + 1);
        if(!dynamic_pokemon_name){
            perror("Error al asignar memoria para el nombre del pokemon");
            error_found = true;
            break;
        }

        strncpy(dynamic_pokemon_name, pokemon_name_start, name_size);
        dynamic_pokemon_name[name_size] = '\0';


        ptr = pokemon_name_end + 1;

        char temp_type[50];
        unsigned strength = 0, skill = 0, intelligence = 0;
        if(sscanf(ptr, "%49[^;];%u;%u;%u", temp_type, strength, skill, intelligence) != 4){
            fprintf(stderr, "Error linea %u: Formato invalido(tipo/stats). Contenido restante: '%s'.\n", line_number, ptr);
            free(dynamic_pokemon_name);
            error_found = true;
            break;
        }


        tipo_pokemon pokemon_type = string_to_type(temp_type);
        if(pokemon_type == (tipo_pokemon)-1){
            fprintf(stderr, "Error linea %u: Tipo de pokemon invalido '%s'.\n", line_number, temp_type);
            free(dynamic_pokemon_name);
            error_found = true;
            break;
        }

        if(pokedex->count == pokedex->capacity) {
            size_t new_capacity = pokedex->capacity == 0 ? 10 : pokedex->capacity * 2;
            struct pokemon** temp = realloc(pokedex->pokemon_list, new_capacity * sizeof(struct pokemon*));
            
            if(!temp){
                perror("Error al redimensionar la lista de pokemones");
                free(dynamic_pokemon_name);
                error_found = true;
                break;
            }

            pokedex->pokemon_list = temp;
            pokedex->capacity = new_capacity;
        }

        new_pokemon = malloc(sizeof(struct pokemon));
        if (!new_pokemon) {
            perror("Error al reservar memoria para el nuevo pokemon");
            free(dynamic_pokemon_name);
            error_found = true;
            break; 
        }


        new_pokemon->id = id;
        new_pokemon->nombre = dynamic_pokemon_name;
        new_pokemon->tipo = pokemon_type;
        new_pokemon->fuerza = strength;
        new_pokemon->destreza = skill;
        new_pokemon->inteligencia = intelligence;


        pokedex->pokemon_list[pokedex->count] = new_pokemon;
        pokedex->count++;


    }


    fclose(file);

    
}