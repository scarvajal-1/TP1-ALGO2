#include "src/pokedex.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
	tipo_pokemon searched_type;
	unsigned pokemons_found;
} context_to_search_for_t;

static tipo_pokemon string_to_type(const char *str_type)
{
	if (!str_type) {
		return (tipo_pokemon)-1;
	}
	if (strcmp(str_type, "A") == 0) {
		return TIPO_AGUA;
	}
	if (strcmp(str_type, "F") == 0) {
		return TIPO_FUEGO;
	}
	if (strcmp(str_type, "P") == 0) {
		return TIPO_PLANTA;
	}
	if (strcmp(str_type, "R") == 0) {
		return TIPO_ROCA;
	}
	if (strcmp(str_type, "E") == 0) {
		return TIPO_ELECTRICO;
	}
	if (strcmp(str_type, "N") == 0) {
		return TIPO_NORMAL;
	}
	if (strcmp(str_type, "L") == 0) {
		return TIPO_LUCHA;
	}

	return (tipo_pokemon)-1;
}

const char *type_to_string(tipo_pokemon type)
{
	switch (type) {
	case TIPO_AGUA:
		return "AGUA";
	case TIPO_FUEGO:
		return "FUEGO";
	case TIPO_PLANTA:
		return "PLANTA";
	case TIPO_ROCA:
		return "ROCA";
	case TIPO_ELECTRICO:
		return "ELECTRICO";
	case TIPO_NORMAL:
		return "NORMAL";
	case TIPO_LUCHA:
		return "LUCHA";
	default:
		return "DESCONOCIDO";
	}
}

static void print_pokemon_details(const struct pokemon *pokemon)
{
	if (!pokemon) {
		return;
	}

	printf("  ID: %u | Nombre: %-15s | Tipo: %-10s | F: %3u | D: %3u | I: %3u\n",
	       pokemon->id, pokemon->nombre, type_to_string(pokemon->tipo),
	       pokemon->fuerza, pokemon->destreza, pokemon->inteligencia);
}

static bool show_pokemon_simple(struct pokemon *pokemon, void *ctx)
{
	(void)ctx;
	print_pokemon_details(pokemon);
	return true;
}

static bool show_only_if_its_the_type(struct pokemon *pokemon, void *ctx)
{
	if (!pokemon || !ctx) {
		return true;
	}

	context_to_search_for_t *context = ctx;
	if (pokemon->tipo == context->searched_type) {
		print_pokemon_details(pokemon);
		context->pokemons_found++;
	}
	return true;
}

static void print_how_to_use_program(const char *program_name)
{
	fprintf(stdout, "Uso:\n");
	fprintf(stdout, "  %s <pokedex.csv> mostrar <nombre|id>\n",
		program_name);
	fprintf(stdout, "  %s <pokedex.csv> buscar <nombre|id|tipo> <valor>\n",
		program_name);
}

static int handle_show_command(pokedex_t *pokedex, const char *criterion)
{
	if (strcmp(criterion, "nombre") == 0) {
		printf("--- Pokemon ordenados por NOMBRE ---\n");
		pokedex_iterar_pokemones(pokedex, ITERAR_NOMBRE,
					 show_pokemon_simple, NULL);
	} else if (strcmp(criterion, "id") == 0) {
		printf("--- Pokemon ordenados por ID ---\n");
		pokedex_iterar_pokemones(pokedex, ITERAR_ID,
					 show_pokemon_simple, NULL);
	} else {
		fprintf(stderr,
			"Error: Criterio '%s' invalido para 'mostrar'. Use 'nombre' o 'id'\n",
			criterion);
		return 1;
	}
	printf("-----------------------------------\n");
	return 0;
}

static int handle_search_command(pokedex_t *pokedex, const char *criterion,
				 const char *search_value_str)
{
	printf("--- Buscando por %s: %s ---\n", criterion, search_value_str);
	int return_code = 0;

	if (strcmp(criterion, "nombre") == 0) {
		const struct pokemon *found_pokemon =
			pokedex_buscar_pokemon_nombre(pokedex,
						      search_value_str);
		if (found_pokemon) {
			printf("Pokemon encontrado:\n");
			print_pokemon_details(found_pokemon);
		} else {
			printf("No se encontro Pokemon con nombre '%s'\n",
			       search_value_str);
		}
	} else if (strcmp(criterion, "id") == 0) {
		char *endptr;
		unsigned long search_id_ul =
			strtoul(search_value_str, &endptr, 10);

		if (endptr == search_value_str || *endptr != '\0' ||
		    search_id_ul == 0 || search_id_ul > UINT_MAX) {
			fprintf(stderr,
				"Error: ID invalido '%s'. Debe ser un numero entero positivo\n",
				search_value_str);
			return_code = 1;
		} else {
			unsigned search_id = (unsigned)search_id_ul;
			const struct pokemon *found_pokemon =
				pokedex_buscar_pokemon_id(pokedex, search_id);
			if (found_pokemon) {
				printf("Pokemon encontrado:\n");
				print_pokemon_details(found_pokemon);
			} else {
				printf("No se encontro Pokemon con ID %u\n",
				       search_id);
			}
		}
	} else if (strcmp(criterion, "tipo") == 0) {
		tipo_pokemon search_type_enum =
			string_to_type(search_value_str);

		if (search_type_enum == (tipo_pokemon)-1) {
			fprintf(stderr,
				"Error: Tipo de Pokemon invalido '%s'\n",
				search_value_str);
			return_code = 1;
		} else {
			context_to_search_for_t context = {
				.searched_type = search_type_enum,
				.pokemons_found = 0
			};
			printf("Pokemon encontrados de tipo '%s':\n",
			       search_value_str);
			pokedex_iterar_pokemones(pokedex, ITERAR_ID,
						 show_only_if_its_the_type,
						 &context);

			if (context.pokemons_found == 0) {
				printf("No se encontraron Pokemon de tipo '%s'\n",
				       search_value_str);
			} else {
				printf("(Total encontrados: %u)\n",
				       context.pokemons_found);
			}
		}
	} else {
		fprintf(stderr,
			"Error: Criterio '%s' invalido para 'buscar'. Use 'nombre', 'id' o 'tipo'\n",
			criterion);
		return_code = 1;
	}
	printf("-----------------------------------\n");
	return return_code;
}

int main(int argc, char const *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Error: Argumentos insuficientes\n");
		print_how_to_use_program(argv[0]);
		return 1;
	}

	const char *pokedex_filename = argv[1];
	const char *command = argv[2];
	const char *criterion = argv[3];

	printf("Cargando Pokedex desde '%s'...\n", pokedex_filename);
	pokedex_t *pokedex = pokedex_abrir(pokedex_filename);
	if (!pokedex) {
		fprintf(stderr, "Error fatal: No se pudo cargar la Pokedex\n");
		return 1;
	}

	printf("Pokedex cargada con %u Pokemon\n\n",
	       pokedex_cantidad_pokemones(pokedex));

	int return_code = 0;
	if (strcmp(command, "mostrar") == 0) {
		if (argc != 4) {
			fprintf(stderr,
				"Error: Numero incorrecto de argumentos para 'mostrar'\n");
			print_how_to_use_program(argv[0]);
			return_code = 1;
		} else {
			return_code = handle_show_command(pokedex, criterion);
			if (return_code != 0) {
				print_how_to_use_program(argv[0]);
			}
		}
	} else if (strcmp(command, "buscar") == 0) {
		if (argc != 5) {
			fprintf(stderr,
				"Error: Numero incorrecto de argumentos para 'buscar'\n");
			print_how_to_use_program(argv[0]);
			return_code = 1;
		} else {
			const char *search_value_str = argv[4];
			return_code = handle_search_command(pokedex, criterion,
							    search_value_str);
			if (return_code != 0) {
				print_how_to_use_program(argv[0]);
			}
		}
	} else {
		fprintf(stderr,
			"Error: Comando '%s' desconocido. Use 'mostrar' o 'buscar'\n",
			command);
		print_how_to_use_program(argv[0]);
		return_code = 1;
	}

	printf("\nLiberando memoria de la Pokedex...\n");
	pokedex_destruir(pokedex);
	printf("Memoria liberada.\n");

	return return_code;
}
