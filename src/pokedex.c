#include "pokedex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 10
#define MAX_LENGTH 256

struct pokedex {
    struct pokemon** pokemon_list_by_id;
    struct pokemon** pokemon_list_by_name;
    size_t count;
    size_t capacity;
};

tipo_pokemon string_to_type(const char* str_type) {
    if(!str_type){
        return (tipo_pokemon)-1;
    }
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


int comparar_pokemon_por_id(const void* a, const void* b) {
    struct pokemon* p1 = *(struct pokemon**)a;
    struct pokemon* p2 = *(struct pokemon**)b;

    if (p1->id < p2->id) return -1;
    if (p1->id > p2->id) return 1;
    return 0;
}


int comparar_pokemon_por_nombre(const void* a, const void* b){
    struct pokemon* p1 = *(struct pokemon**)a;
    struct pokemon* p2 = *(struct pokemon**)b;

    const char* nombre1 = p1->nombre ? p1->nombre : "";
    const char* nombre2 = p2->nombre ? p2->nombre : "";
    return strcmp(nombre1, nombre2);
}


void swap_pokemon_pointers(struct pokemon** a, struct pokemon** b) {
    struct pokemon* temp = *a;
    *a = *b;
    *b = temp;
}


void bubble_sort(struct pokemon** arr, size_t n, int (*comparador)(const void* , const void* )) {
    if (n <= 1) return;

    bool swapped;
    for (size_t i = 0; i < n - 1; i++) {
        swapped = false;
        for (size_t j = 0; j < n - 1 - i; j++) {
            if (comparador(&arr[j], &arr[j + 1]) > 0) {
                swap_pokemon_pointers(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
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

    pokedex->capacity = INITIAL_CAPACITY;
    pokedex->count = 0;
    pokedex->pokemon_list_by_id = malloc(pokedex->capacity * sizeof(struct pokemon*));
    pokedex->pokemon_list_by_name = malloc(pokedex->capacity * sizeof(struct pokemon*));

    if(!pokedex->pokemon_list_by_id || !pokedex->pokemon_list_by_name){
        perror("Error al asignar memoria a la lista de pokemones");
        free(pokedex->pokemon_list_by_id);
        free(pokedex->pokemon_list_by_name);
        free(pokedex);
        fclose(file);
        return NULL;
    }

    char line[MAX_LENGTH];
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

        if(!pokemon_name_end){
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

        if(sscanf(ptr, "%49[^;];%u;%u;%u", temp_type, &strength, &skill, &intelligence) != 4){
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
            size_t new_capacity = pokedex->capacity == 0 ? INITIAL_CAPACITY : pokedex->capacity * 2;
            struct pokemon** temp_id = realloc(pokedex->pokemon_list_by_id, new_capacity * sizeof(struct pokemon*));
            struct pokemon** temp_name = realloc(pokedex->pokemon_list_by_name, new_capacity * sizeof(struct pokemon*));
            
            if(!temp_id || !temp_name){
                perror("Error al redimensionar la lista de pokemones");
                free(dynamic_pokemon_name);
                error_found = true;
                break;
            }

            pokedex->pokemon_list_by_id = temp_id;
            pokedex->pokemon_list_by_name = temp_name;
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


        pokedex->pokemon_list_by_id[pokedex->count] = new_pokemon;
        pokedex->pokemon_list_by_name[pokedex->count] = new_pokemon;
        pokedex->count++;


    }

    fclose(file);


    if(error_found){
        fprintf(stderr, "Error procesando el archivo\n");

        if(pokedex){
            for (size_t i = 0; i < pokedex->count; i++){
                if(pokedex->pokemon_list_by_id[i]){
                    free((void*)pokedex->pokemon_list_by_id[i]->nombre);
                    free(pokedex->pokemon_list_by_id[i]);
                }
            }

            free(pokedex->pokemon_list_by_id);
            free(pokedex->pokemon_list_by_name);
            free(pokedex);
            
        }

        return NULL;
    }

    if (pokedex->count == 0) {

        fprintf(stderr, "Advertencia: No se leyeron Pokemon validos del archivo '%s'.\n", archivo);
        free(pokedex->pokemon_list_by_id);
        free(pokedex->pokemon_list_by_name);
        free(pokedex);
        return NULL;
    }

    if (pokedex->count > 1) {

        printf("Ordenando %zu Pokemon (Bubble Sort)...", pokedex->count);
        fflush(stdout);
        bubble_sort(pokedex->pokemon_list_by_id, pokedex->count, comparar_pokemon_por_id);
        bubble_sort(pokedex->pokemon_list_by_name, pokedex->count, comparar_pokemon_por_nombre);
        printf(" ¡Hecho!\n");
    }

    return pokedex;
}


unsigned pokedex_cantidad_pokemones(pokedex_t *pokedex) {
    if (pokedex == NULL) {
        return 0;
    }

    return (unsigned)pokedex->count;
}


const struct pokemon *pokedex_buscar_pokemon_nombre(pokedex_t *pokedex, const char *nombre) {
    if (pokedex == NULL || nombre == NULL) {
        return NULL;
    }

    if (pokedex->count == 0) {
        return NULL;
    }

    int bajo = 0;
    int alto = (int)pokedex->count - 1;

    while (bajo <= alto) {

        int medio = bajo + (alto - bajo) / 2;

        struct pokemon* pokemon_medio = pokedex->pokemon_list_by_name[medio];

        const char* nombre_medio_ptr = pokemon_medio->nombre ? pokemon_medio->nombre : "";
        int comparacion = strcmp(nombre, nombre_medio_ptr);

        if (comparacion == 0) {
            return pokemon_medio;
        } else if (comparacion < 0) {
            alto = medio - 1;
        } else {
            bajo = medio + 1;
        }
    }
    return NULL;
}


const struct pokemon *pokedex_buscar_pokemon_id(pokedex_t *pokedex, unsigned id) {
    if (pokedex == NULL) {
        return NULL;
    }

    if (pokedex->count == 0) {
        return NULL;
    }

    int bajo = 0;
    int alto = (int)pokedex->count - 1;

    while (bajo <= alto) {
        int medio = bajo + (alto - bajo) / 2;

        struct pokemon* pokemon_medio = pokedex->pokemon_list_by_id[medio];

        unsigned id_medio = pokemon_medio->id;

        if (id == id_medio) {
            return pokemon_medio;
        } else if (id < id_medio) {
            alto = medio - 1;
        } else {
            bajo = medio + 1;
        }
    }
    return NULL;
}


unsigned pokedex_iterar_pokemones(pokedex_t *pokedex, enum modo_iteracion modo, bool (*funcion)(struct pokemon *, void *), void *ctx){
    if (pokedex == NULL || funcion == NULL) {
        return 0;
    }

    struct pokemon** lista_a_iterar = NULL;

    if (modo == ITERAR_ID) {
        lista_a_iterar = pokedex->pokemon_list_by_id;
    } else if (modo == ITERAR_NOMBRE) {
        lista_a_iterar = pokedex->pokemon_list_by_name;
    } else {
        return 0;
    }

    if (lista_a_iterar == NULL) {
         return 0;
    }

    unsigned contador_iterados = 0;
    for (size_t i = 0; i < pokedex->count; i++) {

        struct pokemon* poke_actual = lista_a_iterar[i];
        contador_iterados++;

        bool continuar = funcion(poke_actual, ctx);

        if (!continuar) {
            break;
        }
    }
    return contador_iterados;
}


void pokedex_destruir(pokedex_t *pokedex) {
    if (pokedex == NULL) {
        return;
    }

    for (size_t i = 0; i < pokedex->count; i++) {
        struct pokemon* pokemon_a_liberar = pokedex->pokemon_list_by_id[i];

        if (pokemon_a_liberar != NULL) {

            free((void*)pokemon_a_liberar->nombre);
            free(pokemon_a_liberar);
        }
    }

    free(pokedex->pokemon_list_by_id);
    free(pokedex->pokemon_list_by_name);

    free(pokedex);
}