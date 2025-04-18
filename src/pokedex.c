#include "pokedex.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

struct pokedex {
	struct pokemon **pokemon_list_by_id;
	struct pokemon **pokemon_list_by_name;
	size_t count;
	size_t capacity;
};

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

int compare_pokemon_by_id(const void *a, const void *b)
{
	struct pokemon *p1 = *(struct pokemon **)a;
	struct pokemon *p2 = *(struct pokemon **)b;

	if (p1->id < p2->id)
		return -1;
	if (p1->id > p2->id)
		return 1;
	return 0;
}

int compare_pokemon_by_name(const void *a, const void *b)
{
	struct pokemon *p1 = *(struct pokemon **)a;
	struct pokemon *p2 = *(struct pokemon **)b;

	const char *name1 = p1->nombre ? p1->nombre : "";
	const char *name2 = p2->nombre ? p2->nombre : "";
	return strcmp(name1, name2);
}

void swap_pokemon_pointers(struct pokemon **a, struct pokemon **b)
{
	struct pokemon *temp = *a;
	*a = *b;
	*b = temp;
}

void bubble_sort(struct pokemon **arr, size_t n,
		 int (*comparator)(const void *, const void *))
{
	if (n <= 1)
		return;

	bool swapped;
	for (size_t i = 0; i < n - 1; i++) {
		swapped = false;
		for (size_t j = 0; j < n - 1 - i; j++) {
			if (comparator(&arr[j], &arr[j + 1]) > 0) {
				swap_pokemon_pointers(&arr[j], &arr[j + 1]);
				swapped = true;
			}
		}
		if (!swapped) {
			break;
		}
	}
}

pokedex_t *pokedex_abrir(const char *archivo)
{
	if (archivo == NULL) {
		return NULL;
	}

	FILE *file = fopen(archivo, "r");
	if (!file) {
		perror("Error al abrir el archivo");
		return NULL;
	}

	pokedex_t *pokedex = malloc(sizeof(pokedex_t));
	if (!pokedex) {
		fclose(file);
		perror("Error al asignar memoria a la pokedex");
		return NULL;
	}

	pokedex->capacity = INITIAL_CAPACITY;
	pokedex->count = 0;
	pokedex->pokemon_list_by_id =
		malloc(pokedex->capacity * sizeof(struct pokemon *));
	pokedex->pokemon_list_by_name =
		malloc(pokedex->capacity * sizeof(struct pokemon *));

	if (!pokedex->pokemon_list_by_id || !pokedex->pokemon_list_by_name) {
		perror("Error al asignar memoria a la lista de pokemones");
		free(pokedex->pokemon_list_by_id);
		free(pokedex->pokemon_list_by_name);
		free(pokedex);
		fclose(file);
		return NULL;
	}

	char *line = NULL;
	size_t line_capacity = 0;
	size_t line_size = 0;
	int c;
	unsigned line_number = 0;
	bool fatal_error = false;
	bool stop_reading = false;

	while (!fatal_error && !stop_reading) {
		line_size = 0;

		while ((c = fgetc(file)) != EOF && c != '\n') {
			if (line_size + 1 >= line_capacity) {
				size_t new_capacity = (line_capacity == 0) ?
							      128 :
							      line_capacity * 2;

				if (new_capacity < line_capacity) {
					fprintf(stderr,
						"Error: Capacidad de linea excederia limites\n");
					fatal_error = true;
					break;
				}

				char *temp = realloc(line, new_capacity);
				if (!temp) {
					perror("Error en realloc al leer linea");
					fatal_error = true;
					break;
				}
				line = temp;
				line_capacity = new_capacity;
			}
			line[line_size] = (char)c;
			line_size++;
		}

		if (fatal_error) {
			break;
		}

		if (c == EOF && line_size == 0) {
			stop_reading = true;
			continue;
		}

		if (line_size >= line_capacity) {
			size_t new_capacity = line_capacity + 1;
			char *temp = realloc(line, new_capacity);
			if (!temp) {
				perror("Error en realloc final para null terminador");
				fatal_error = true;
				break;
			}
			line = temp;
			line_capacity = new_capacity;
		}

		line[line_size] = '\0';
		line_number++;

		char *ptr = line;
		char *pokemon_name_start = NULL;
		char *pokemon_name_end = NULL;
		char *dynamic_pokemon_name = NULL;
		struct pokemon *new_pokemon = NULL;

		unsigned id = 0;
		int offset = 0;

		if (sscanf(ptr, "%u%n", &id, &offset) != 1 ||
		    ptr[offset] != ';') {
			fprintf(stderr,
				"Error linea %u: Formato invalido (ID no encontrado o seguido de "
				"';'). Contenido: '%s'\n",
				line_number, line);
			stop_reading = true;
			continue;
		}

		ptr += offset + 1;

		pokemon_name_start = ptr;
		pokemon_name_end = strchr(ptr, ';');

		if (!pokemon_name_end) {
			fprintf(stderr,
				"Error linea %u: Formato invalido (Falta ';' despues del "
				"nombre). Contenido: '%s'\n",
				line_number, line);
			stop_reading = true;
			continue;
		}

		size_t name_size =
			(size_t)(pokemon_name_end - pokemon_name_start);
		dynamic_pokemon_name = malloc(name_size + 1);

		if (!dynamic_pokemon_name) {
			perror("Error al asignar memoria para el nombre del pokemon");
			fatal_error = true;
			break;
		}

		strncpy(dynamic_pokemon_name, pokemon_name_start, name_size);
		dynamic_pokemon_name[name_size] = '\0';

		ptr = pokemon_name_end + 1;

		char temp_type[50];
		unsigned strength = 0, skill = 0, intelligence = 0;

		if (sscanf(ptr, "%49[^;];%u;%u;%u", temp_type, &strength,
			   &skill, &intelligence) != 4) {
			fprintf(stderr,
				"Error linea %u: Formato invalido(tipo/stats). Contenido "
				"restante: '%s'.\n",
				line_number, ptr);
			free(dynamic_pokemon_name);
			stop_reading = true;
			continue;
		}

		tipo_pokemon pokemon_type = string_to_type(temp_type);

		if (pokemon_type == (tipo_pokemon)-1) {
			fprintf(stderr,
				"Error linea %u: Tipo de pokemon invalido '%s'.\n",
				line_number, temp_type);
			free(dynamic_pokemon_name);
			stop_reading = true;
			continue;
		}

		if (pokedex->count == pokedex->capacity) {
			size_t new_capacity = (pokedex->capacity == 0) ?
						      INITIAL_CAPACITY :
						      pokedex->capacity * 2;
			struct pokemon **temp_id = realloc(
				pokedex->pokemon_list_by_id,
				new_capacity * sizeof(struct pokemon *));
			struct pokemon **temp_name = realloc(
				pokedex->pokemon_list_by_name,
				new_capacity * sizeof(struct pokemon *));

			if (!temp_id || !temp_name) {
				perror("Error al redimensionar la lista de pokemones");
				free(dynamic_pokemon_name);
				fatal_error = true;
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
			fatal_error = true;
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

	free(line);
	fclose(file);

	if (fatal_error) {
		fprintf(stderr,
			"Error fatal de memoria durante la lectura del archivo.\n");
		pokedex_destruir(pokedex);
		return NULL;
	}

	if (pokedex->count == 0) {
		fprintf(stderr,
			"Advertencia: No se leyeron Pokemon validos del archivo '%s'.\n",
			archivo);
		free(pokedex->pokemon_list_by_id);
		free(pokedex->pokemon_list_by_name);
		free(pokedex);
		return NULL;
	}

	if (pokedex->count > 1) {
		printf("Ordenando %zu Pokemon (Bubble Sort)...",
		       pokedex->count);
		bubble_sort(pokedex->pokemon_list_by_id, pokedex->count,
			    compare_pokemon_by_id);
		bubble_sort(pokedex->pokemon_list_by_name, pokedex->count,
			    compare_pokemon_by_name);
		printf(" ¡Hecho!\n");
	}

	return pokedex;
}

unsigned pokedex_cantidad_pokemones(pokedex_t *pokedex)
{
	if (pokedex == NULL) {
		return 0;
	}

	return (unsigned)pokedex->count;
}

const struct pokemon *pokedex_buscar_pokemon_nombre(pokedex_t *pokedex,
						    const char *nombre)
{
	if (pokedex == NULL || nombre == NULL) {
		return NULL;
	}

	if (pokedex->count == 0) {
		return NULL;
	}

	int lowest = 0;
	int highest = (int)pokedex->count - 1;

	while (lowest <= highest) {
		int half = lowest + (highest - lowest) / 2;

		struct pokemon *middle_pokemon =
			pokedex->pokemon_list_by_name[half];

		const char *name_half_ptr =
			middle_pokemon->nombre ? middle_pokemon->nombre : "";
		int comparison = strcmp(nombre, name_half_ptr);

		if (comparison == 0) {
			return middle_pokemon;
		} else if (comparison < 0) {
			highest = half - 1;
		} else {
			lowest = half + 1;
		}
	}
	return NULL;
}

const struct pokemon *pokedex_buscar_pokemon_id(pokedex_t *pokedex, unsigned id)
{
	if (pokedex == NULL) {
		return NULL;
	}

	if (pokedex->count == 0) {
		return NULL;
	}

	int lowest = 0;
	int highest = (int)pokedex->count - 1;

	while (lowest <= highest) {
		int half = lowest + (highest - lowest) / 2;

		struct pokemon *middle_pokemon =
			pokedex->pokemon_list_by_id[half];

		unsigned middle_id = middle_pokemon->id;

		if (id == middle_id) {
			return middle_pokemon;
		} else if (id < middle_id) {
			highest = half - 1;
		} else {
			lowest = half + 1;
		}
	}
	return NULL;
}

unsigned pokedex_iterar_pokemones(pokedex_t *pokedex, enum modo_iteracion modo,
				  bool (*funcion)(struct pokemon *, void *),
				  void *ctx)
{
	if (pokedex == NULL || funcion == NULL) {
		return 0;
	}

	struct pokemon **list_to_iterate = NULL;

	if (modo == ITERAR_ID) {
		list_to_iterate = pokedex->pokemon_list_by_id;
	} else if (modo == ITERAR_NOMBRE) {
		list_to_iterate = pokedex->pokemon_list_by_name;
	} else {
		return 0;
	}

	if (list_to_iterate == NULL) {
		return 0;
	}

	unsigned iterator_counter = 0;
	for (size_t i = 0; i < pokedex->count; i++) {
		struct pokemon *actual_pokemon = list_to_iterate[i];
		iterator_counter++;

		bool can_continue = funcion(actual_pokemon, ctx);

		if (!can_continue) {
			break;
		}
	}
	return iterator_counter;
}

void pokedex_destruir(pokedex_t *pokedex)
{
	if (pokedex == NULL) {
		return;
	}

	for (size_t i = 0; i < pokedex->count; i++) {
		struct pokemon *pokemon_to_free =
			pokedex->pokemon_list_by_id[i];

		if (pokemon_to_free != NULL) {
			free((void *)pokemon_to_free->nombre);
			free(pokemon_to_free);
		}
	}

	free(pokedex->pokemon_list_by_id);
	free(pokedex->pokemon_list_by_name);
	free(pokedex);
}