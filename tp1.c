#include "src/pokedex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
	tipo_pokemon tipo_buscado;
	unsigned encontrados;
} context_to_search_for_t;


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


const char* type_to_string(tipo_pokemon type) {
    switch (type) {
        case TIPO_AGUA:     return "AGUA";
        case TIPO_FUEGO:    return "FUEGO";
        case TIPO_PLANTA:   return "PLANTA";
        case TIPO_ROCA:     return "ROCA";
        case TIPO_ELECTRICO:return "ELECTRICO";
        case TIPO_NORMAL:   return "NORMAL";
        case TIPO_LUCHA:    return "LUCHA";
        default:            return "DESCONOCIDO";
    }
}


void print_pokemon_details(const struct pokemon* pokemon) {
	if(!pokemon){
		return;
	}

	printf("  ID: %u | Nombre: %-15s | Tipo: %-10s | F: %3u | D: %3u | I: %3u\n",
           pokemon->id,
           pokemon->nombre,
           type_to_string(pokemon->tipo),
           pokemon->fuerza,
           pokemon->destreza,
           pokemon->inteligencia);
}


bool show_pokemon_simple(struct pokemon *pokemon, void *ctx) {
    (void)ctx;
    print_pokemon_details(pokemon);
    return true;
}


bool show_only_if_its_the_type(struct pokemon* pokemon, void* ctx) {
    if (!pokemon || !ctx) {
		return true;
	}
		
    context_to_search_for_t* contexto = ctx;
    if (pokemon->tipo == contexto->tipo_buscado) {
        print_pokemon_details(pokemon);
        contexto->encontrados++;
    }
    return true;
}


void print_how_to_use_program(const char* nombre_programa) {
	fprintf(stderr, "Uso:\n");
	fprintf(stderr, "  %s <pokedex.csv> mostrar <nombre|id>\n", nombre_programa);
	fprintf(stderr, "  %s <pokedex.csv> buscar <nombre|id|tipo> <valor>\n", nombre_programa);
}


int main(int argc, char const* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Error: Argumentos insuficientes\n");
        print_how_to_use_program(argv[0]);
        return 1;
    }

    const char* archivo_pokedex = argv[1];
    const char* comando = argv[2];
    const char* criterio = argv[3];

    printf("Cargando Pokedex desde '%s'...\n", archivo_pokedex);
    pokedex_t* pokedex = pokedex_abrir(archivo_pokedex);

    if (!pokedex) {
        fprintf(stderr, "Error fatal: No se pudo cargar la Pokedex.\n");
        return 1;
    }

    printf("Pokedex cargada con %u Pokémon.\n\n", pokedex_cantidad_pokemones(pokedex));

    int return_code = 0;

    if (strcmp(comando, "mostrar") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: Número incorrecto de argumentos para 'mostrar'\n");
            print_how_to_use_program(argv[0]);
            return_code = 1;
        } else {
            if (strcmp(criterio, "nombre") == 0) {
                printf("--- Pokémon ordenados por NOMBRE ---\n");
                pokedex_iterar_pokemones(pokedex, ITERAR_NOMBRE, show_pokemon_simple, NULL);
            } else if (strcmp(criterio, "id") == 0) {
                printf("--- Pokémon ordenados por ID ---\n");
                pokedex_iterar_pokemones(pokedex, ITERAR_ID, show_pokemon_simple, NULL);
            } else {
                fprintf(stderr, "Error: Criterio '%s' inválido para 'mostrar'. Use 'nombre' o 'id'\n", criterio);
                print_how_to_use_program(argv[0]);
                return_code = 1;
            }
            printf("-----------------------------------\n");
        }
    }
    else if (strcmp(comando, "buscar") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Error: Número incorrecto de argumentos para 'buscar'.\n");
            print_how_to_use_program(argv[0]);
            return_code = 1;
        } else {
            const char* valor_buscado_str = argv[4];
            printf("--- Buscando por %s: %s ---\n", criterio, valor_buscado_str);

            if (strcmp(criterio, "nombre") == 0) {
                const struct pokemon* encontrado = pokedex_buscar_pokemon_nombre(pokedex, valor_buscado_str);
                if (encontrado) {
                    printf("Pokémon encontrado:\n");
                    print_pokemon_details(encontrado);
                } else {
                    printf("No se encontró Pokémon con nombre '%s'.\n", valor_buscado_str);
                }
            } else if (strcmp(criterio, "id") == 0) {
                unsigned id_buscado = (unsigned)atoi(valor_buscado_str);
                if (id_buscado == 0 && strcmp(valor_buscado_str, "0") != 0) {
                    fprintf(stderr, "Error: ID inválido '%s'. Debe ser un número entero positivo\n", valor_buscado_str);
                    return_code = 1;
                } else {
                    const struct pokemon* encontrado = pokedex_buscar_pokemon_id(pokedex, id_buscado);
                    if (encontrado) {
                        printf("Pokémon encontrado:\n");
                        print_pokemon_details(encontrado);
                    } else {
                        printf("No se encontró Pokémon con ID %u.\n", id_buscado);
                    }
                }
            } else if (strcmp(criterio, "tipo") == 0) {
                tipo_pokemon tipo_buscado_enum = string_to_type(valor_buscado_str);

                if (tipo_buscado_enum == (tipo_pokemon)-1) {
                    fprintf(stderr, "Error: Tipo de Pokémon inválido '%s'.\n", valor_buscado_str);
                    return_code = 1;
                } else {
                    context_to_search_for_t contexto = {
                        .tipo_buscado = tipo_buscado_enum,
                        .encontrados = 0
                    };
                    printf("Pokémon encontrados de tipo '%s':\n", valor_buscado_str);
                    pokedex_iterar_pokemones(pokedex, ITERAR_ID, show_only_if_its_the_type, &contexto);

                    if (contexto.encontrados == 0) {
                        printf("No se encontraron Pokémon de tipo '%s'.\n", valor_buscado_str);
                    } else {
                        printf("(Total encontrados: %u)\n", contexto.encontrados);
                    }
                }
            } else {
                fprintf(stderr, "Error: Criterio '%s' inválido para 'buscar'. Use 'nombre', 'id' o 'tipo'.\n", criterio);
                print_how_to_use_program(argv[0]);
                return_code = 1;
            }
             printf("-----------------------------------\n");
        }
    }
    else {
        fprintf(stderr, "Error: Comando '%s' desconocido. Use 'mostrar' o 'buscar'.\n", comando);
        print_how_to_use_program(argv[0]);
        return_code = 1;
    }

    printf("\nLiberando memoria de la Pokedex...\n");
    pokedex_destruir(pokedex);
    printf("Memoria liberada.\n");

    return return_code;
}
