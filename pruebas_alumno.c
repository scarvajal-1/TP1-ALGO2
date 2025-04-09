#include "pa2m.h"
#include "src/pokedex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ARCHIVO_PRUEBA "ejemplos/pokedex.csv"
#define ARCHIVO_VACIO "ejemplos/archivo_vacio.csv"
#define CANTIDAD_ESPERADA 9
#define NOMBRE_CONOCIDO "Pikachu"
#define ID_NOMBRE_CONOCIDO 25
#define ID_CONOCIDO 4
#define NOMBRE_ID_CONOCIDO "Charmander"
#define NOMBRE_INEXISTENTE "Mewtwo"
#define ID_INEXISTENTE 999

void prueba_abrir_archivo_valido_cantidad_correcta()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL, "Abrir archivo valido no devuelve NULL");

	if (pokedex) {
		unsigned cantidad = pokedex_cantidad_pokemones(pokedex);
		pa2m_afirmar(
			cantidad == CANTIDAD_ESPERADA,
			"La cantidad leida es la esperada (%d). Obtenido: %u",
			CANTIDAD_ESPERADA, cantidad);
	}
	pokedex_destruir(pokedex);
}

void prueba_abrir_archivo_inexistente_devuelve_null()
{
	pokedex_t *pokedex = pokedex_abrir("archivo_que_NO_existe.csv");
	pa2m_afirmar(pokedex == NULL,
		     "Abrir archivo inexistente devuelve NULL");
	pokedex_destruir(pokedex);
}

void prueba_abrir_archivo_vacio_devuelve_null()
{
	FILE *f = fopen(ARCHIVO_VACIO, "w");
	if (!f) {
		pa2m_afirmar(
			false,
			"FALLO INTERNO DE PRUEBA: No se pudo crear el archivo vacío %s",
			ARCHIVO_VACIO);
		return;
	}
	fclose(f);

	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_VACIO);

	pa2m_afirmar(pokedex == NULL,
		     "Abrir archivo existente pero vacío devuelve NULL");

	pokedex_destruir(pokedex);

	remove(ARCHIVO_VACIO);
}

void prueba_buscar_nombre_existente_encuentra_pokemon()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		pokedex != NULL,
		"Prueba Buscar Nombre Existente: Pokedex se carga correctamente");

	if (!pokedex) {
		return;
	}

	const struct pokemon *pokemon =
		pokedex_buscar_pokemon_nombre(pokedex, NOMBRE_CONOCIDO);
	pa2m_afirmar(pokemon != NULL, "Buscar '%s' existente no devuelve NULL",
		     NOMBRE_CONOCIDO);
	if (pokemon) {
		pa2m_afirmar(
			strcmp(pokemon->nombre, NOMBRE_CONOCIDO) == 0 &&
				pokemon->id == ID_NOMBRE_CONOCIDO,
			"Buscar '%s' devuelve el pokemon correcto (Nombre e ID)",
			NOMBRE_CONOCIDO);
	}
	pokedex_destruir(pokedex);
}

void prueba_buscar_nombre_inexistente_devuelve_null()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		pokedex != NULL,
		"Prueba Buscar Nombre Inexistente: Pokedex se carga correctamente");
	if (!pokedex) {
		return;
	}

	const struct pokemon *pokemon =
		pokedex_buscar_pokemon_nombre(pokedex, NOMBRE_INEXISTENTE);
	pa2m_afirmar(pokemon == NULL, "Buscar '%s' inexistente devuelve NULL",
		     NOMBRE_INEXISTENTE);

	pokedex_destruir(pokedex);
}

void prueba_buscar_id_existente_encuentra_pokemon()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		pokedex != NULL,
		"Prueba Buscar ID Existente: Pokedex se carga correctamente");
	if (!pokedex) {
		return;
	}

	const struct pokemon *pokemon =
		pokedex_buscar_pokemon_id(pokedex, ID_CONOCIDO);
	pa2m_afirmar(pokemon != NULL, "Buscar ID %d existente no devuelve NULL",
		     ID_CONOCIDO);
	if (pokemon) {
		pa2m_afirmar(
			pokemon->id == ID_CONOCIDO &&
				strcmp(pokemon->nombre, NOMBRE_ID_CONOCIDO) ==
					0,
			"Buscar ID %d devuelve el pokemon correcto (ID y Nombre esperado: %s)",
			ID_CONOCIDO, NOMBRE_ID_CONOCIDO);
	}

	pokedex_destruir(pokedex);
}

void prueba_buscar_id_inexistente_devuelve_null()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		pokedex != NULL,
		"Prueba Buscar ID Inexistente: Pokedex se carga correctamente");
	if (!pokedex) {
		return;
	}

	const struct pokemon *pokemon =
		pokedex_buscar_pokemon_id(pokedex, ID_INEXISTENTE);

	pa2m_afirmar(pokemon == NULL,
		     "Buscar ID inexistente (%u) devuelve NULL",
		     ID_INEXISTENTE);

	pokedex_destruir(pokedex);
}

bool contar_pokemon_callback(struct pokemon *pokemon, void *contador)
{
	if (pokemon != NULL && contador != NULL) {
		(*(unsigned *)contador)++;
	}
	return true;
}

void prueba_iterador_id_recorre_todos()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "Prueba Iterador: Pokedex se carga correctamente");
	if (!pokedex) {
		return;
	}

	unsigned contador_callback = 0;
	unsigned iterados = pokedex_iterar_pokemones(pokedex, ITERAR_ID,
						     contar_pokemon_callback,
						     &contador_callback);
	unsigned cantidad_real = pokedex_cantidad_pokemones(pokedex);

	pa2m_afirmar(
		iterados == cantidad_real,
		"Iterador por ID devuelve la cantidad correcta (%u). Obtenido: %u",
		cantidad_real, iterados);
	pa2m_afirmar(
		contador_callback == cantidad_real,
		"Callback de conteo fue llamado la cantidad correcta de veces "
		"(%u). Obtenido: %u",
		cantidad_real, contador_callback);

	pokedex_destruir(pokedex);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas Básicas (Abrir, Cantidad, Destruir)");
	prueba_abrir_archivo_valido_cantidad_correcta();
	prueba_abrir_archivo_vacio_devuelve_null();
	prueba_abrir_archivo_inexistente_devuelve_null();

	pa2m_nuevo_grupo("Pruebas de Búsqueda");
	prueba_buscar_nombre_existente_encuentra_pokemon();
	prueba_buscar_nombre_inexistente_devuelve_null();
	prueba_buscar_id_existente_encuentra_pokemon();
	prueba_buscar_id_inexistente_devuelve_null();

	pa2m_nuevo_grupo("Pruebas de Iteración");
	prueba_iterador_id_recorre_todos();

	return pa2m_mostrar_reporte();
}