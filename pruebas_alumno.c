#include "pa2m.h"
#include "src/pokedex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARCHIVO_PRUEBA "ejemplos/pokedex.csv"
#define ARCHIVO_VACIO "ejemplos/archivo_vacio.csv"
#define ARCHIVO_INVALIDO "ejemplos/archivo_invalido.csv"
#define ARCHIVO_MIXTO "ejemplos/archivo_mixto.csv"

#define CANTIDAD_ESPERADA 9
#define NOMBRE_CONOCIDO "Pikachu"
#define ID_NOMBRE_CONOCIDO 25
#define ID_CONOCIDO 4
#define NOMBRE_ID_CONOCIDO "Charmander"
#define NOMBRE_INEXISTENTE "Mewtwo"
#define ID_INEXISTENTE 999
#define ID_PRIMERO 1
#define NOMBRE_PRIMER_ID "Bulbasaur"
#define NOMBRE_PRIMER_ALFA "Bulbasaur"
#define ID_NOMBRE_PRIMER_ALFA 1
#define NOMBRE_ULTIMO_ALFA "Squirtle"
#define ID_NOMBRE_ULTIMO_ALFA 7
#define CANTIDAD_ESPERADA_MIXTO 2

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
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_VACIO);

	pa2m_afirmar(pokedex == NULL,
		     "Abrir archivo existente pero vacio devuelve NULL");

	pokedex_destruir(pokedex);
}

void prueba_abrir_null_filename_devuelve_null()
{
	pokedex_t *pokedex = pokedex_abrir(NULL);
	pa2m_afirmar(pokedex == NULL,
		     "NUEVO: Abrir con nombre de archivo NULL devuelve NULL");
	pokedex_destruir(pokedex);
}

void prueba_abrir_archivo_solo_lineas_invalidas_devuelve_null()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_INVALIDO);
	pa2m_afirmar(
		pokedex == NULL,
		"NUEVO: Abrir archivo con solo lineas invalidas devuelve NULL");
	pokedex_destruir(pokedex);
}

void prueba_abrir_archivo_mixto_lee_hasta_error()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_MIXTO);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Abrir archivo mixto no devuelve NULL");
	if (pokedex) {
		unsigned cantidad = pokedex_cantidad_pokemones(pokedex);
		pa2m_afirmar(
			cantidad == CANTIDAD_ESPERADA_MIXTO,
			"NUEVO: Archivo mixto lee solo %u pokemones antes del error. Leidos: %u",
			CANTIDAD_ESPERADA_MIXTO, cantidad);
	} else {
		pa2m_afirmar(
			false,
			"Fallo la carga del pokedex mixto para esta prueba");
	}
	pokedex_destruir(pokedex);
}

void prueba_destruir_null_no_rompe()
{
	pokedex_destruir(NULL);
	pa2m_afirmar(
		true,
		"NUEVO: Llamar a pokedex_destruir con NULL no causa un fallo");
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

void prueba_buscar_con_pokedex_null()
{
	const struct pokemon *pokemon_nombre =
		pokedex_buscar_pokemon_nombre(NULL, NOMBRE_CONOCIDO);
	pa2m_afirmar(pokemon_nombre == NULL,
		     "NUEVO: Buscar nombre con Pokedex NULL devuelve NULL");
	const struct pokemon *pokemon_id =
		pokedex_buscar_pokemon_id(NULL, ID_CONOCIDO);
	pa2m_afirmar(pokemon_id == NULL,
		     "NUEVO: Buscar ID con Pokedex NULL devuelve NULL");
}

void prueba_buscar_nombre_null()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Buscar Nombre NULL: Pokedex cargado");
	if (!pokedex)
		return;
	const struct pokemon *pokemon =
		pokedex_buscar_pokemon_nombre(pokedex, NULL);
	pa2m_afirmar(pokemon == NULL,
		     "NUEVO: Buscar nombre NULL devuelve NULL");
	pokedex_destruir(pokedex);
}

void prueba_buscar_id_cero()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL, "NUEVO: Buscar ID 0: Pokedex cargado");
	if (!pokedex)
		return;
	const struct pokemon *pokemon = pokedex_buscar_pokemon_id(pokedex, 0);
	pa2m_afirmar(
		pokemon == NULL,
		"NUEVO: Buscar ID 0 devuelve NULL (asumiendo que no existe)");
	pokedex_destruir(pokedex);
}

void prueba_buscar_primer_ultimo_nombre()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Buscar Primer/Ultimo Nombre: Pokedex cargado");
	if (!pokedex)
		return;
	const struct pokemon *primero =
		pokedex_buscar_pokemon_nombre(pokedex, NOMBRE_PRIMER_ALFA);
	pa2m_afirmar(primero != NULL &&
			     strcmp(primero->nombre, NOMBRE_PRIMER_ALFA) == 0,
		     "NUEVO: Se encuentra el primer pokemon por nombre ('%s')",
		     NOMBRE_PRIMER_ALFA);
	const struct pokemon *ultimo =
		pokedex_buscar_pokemon_nombre(pokedex, NOMBRE_ULTIMO_ALFA);
	pa2m_afirmar(ultimo != NULL &&
			     strcmp(ultimo->nombre, NOMBRE_ULTIMO_ALFA) == 0,
		     "NUEVO: Se encuentra el ultimo pokemon por nombre ('%s')",
		     NOMBRE_ULTIMO_ALFA);
	pokedex_destruir(pokedex);
}

void prueba_buscar_primer_ultimo_id()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Buscar Primer/Ultimo ID: Pokedex cargado");
	if (!pokedex)
		return;
	const struct pokemon *primero =
		pokedex_buscar_pokemon_id(pokedex, ID_PRIMERO);
	pa2m_afirmar(primero != NULL && primero->id == ID_PRIMERO,
		     "NUEVO: Se encuentra el primer pokemon por ID (%u)",
		     ID_PRIMERO);
	pokedex_destruir(pokedex);
}

bool contar_pokemon_callback(struct pokemon *pokemon, void *contador)
{
	if (pokemon != NULL && contador != NULL) {
		(*(unsigned *)contador)++;
	}
	return true;
}

typedef struct {
	unsigned contador;
	unsigned detener_en;
} contexto_detener;

bool detener_en_n_callback(struct pokemon *pokemon, void *ctx)
{
	if (!pokemon || !ctx)
		return false;
	contexto_detener *contexto = ctx;
	contexto->contador++;
	return contexto->contador < contexto->detener_en;
}

typedef struct {
	unsigned id_anterior;
	bool orden_ok;
} contexto_orden_id;

bool verificar_orden_id_callback(struct pokemon *pokemon, void *ctx)
{
	if (!pokemon || !ctx)
		return false;
	contexto_orden_id *contexto = ctx;
	if (pokemon->id < contexto->id_anterior && contexto->id_anterior != 0) {
		contexto->orden_ok = false;
		return false;
	}
	contexto->id_anterior = pokemon->id;
	return true;
}

typedef struct {
	char nombre_anterior[100];
	bool orden_ok;
	bool primer_elemento;
} contexto_orden_nombre;

bool verificar_orden_nombre_callback(struct pokemon *pokemon, void *ctx)
{
	if (!pokemon || !ctx)
		return false;
	contexto_orden_nombre *contexto = ctx;

	if (!contexto->primer_elemento &&
	    strlen(contexto->nombre_anterior) > 0) {
		if (strcmp(pokemon->nombre, contexto->nombre_anterior) < 0) {
			contexto->orden_ok = false;
			return false;
		}
	}

	strncpy(contexto->nombre_anterior, pokemon->nombre, 99);
	contexto->nombre_anterior[99] = '\0';
	contexto->primer_elemento = false;
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

void prueba_iterador_nombre_recorre_todos()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador Nombre Todos: Pokedex cargado");
	if (!pokedex)
		return;
	unsigned contador_callback = 0;
	unsigned iterados = pokedex_iterar_pokemones(pokedex, ITERAR_NOMBRE,
						     contar_pokemon_callback,
						     &contador_callback);
	unsigned cantidad_real = pokedex_cantidad_pokemones(pokedex);
	pa2m_afirmar(iterados == cantidad_real,
		     "NUEVO: Iterador Nombre devuelve cantidad correcta (%u)",
		     cantidad_real);
	pa2m_afirmar(contador_callback == cantidad_real,
		     "NUEVO: Callback de conteo llamado cantidad correcta (%u)",
		     cantidad_real);
	pokedex_destruir(pokedex);
}

void prueba_iterador_con_pokedex_o_funcion_null()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador NULLs: Pokedex cargado para prueba");

	unsigned iterados_null_pokedex = pokedex_iterar_pokemones(
		NULL, ITERAR_ID, contar_pokemon_callback, NULL);
	pa2m_afirmar(iterados_null_pokedex == 0,
		     "NUEVO: Iterar con Pokedex NULL devuelve 0");

	if (pokedex) {
		unsigned iterados_null_funcion = pokedex_iterar_pokemones(
			pokedex, ITERAR_ID, NULL, NULL);
		pa2m_afirmar(iterados_null_funcion == 0,
			     "NUEVO: Iterar con Funcion NULL devuelve 0");
	}
	pokedex_destruir(pokedex);
}

void prueba_iterador_modo_invalido()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador Modo Inválido: Pokedex cargado");
	if (!pokedex)
		return;
	unsigned contador_callback = 0;
	unsigned iterados = pokedex_iterar_pokemones(pokedex,
						     (enum modo_iteracion)99,
						     contar_pokemon_callback,
						     &contador_callback);
	pa2m_afirmar(iterados == 0,
		     "NUEVO: Iterar con modo invalido devuelve 0");
	pokedex_destruir(pokedex);
}

void prueba_iterador_detiene_temprano()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador Detener: Pokedex cargado");
	if (!pokedex)
		return;

	contexto_detener contexto = { 0, 3 };
	unsigned iterados = pokedex_iterar_pokemones(
		pokedex, ITERAR_ID, detener_en_n_callback, &contexto);

	pa2m_afirmar(
		iterados == contexto.detener_en,
		"NUEVO: Iterador devuelve %u cuando callback detiene en %u",
		contexto.detener_en, iterados);
	pa2m_afirmar(
		contexto.contador == contexto.detener_en,
		"NUEVO: Callback fue llamado %u veces cuando se detiene en %u",
		contexto.detener_en, contexto.contador);

	pokedex_destruir(pokedex);
}

void prueba_iterador_verifica_orden_id()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador Orden ID: Pokedex cargado");
	if (!pokedex)
		return;

	contexto_orden_id contexto = { 0, true };
	pokedex_iterar_pokemones(pokedex, ITERAR_ID,
				 verificar_orden_id_callback, &contexto);

	pa2m_afirmar(contexto.orden_ok,
		     "NUEVO: Iterador por ID mantiene el orden ascendente");

	pokedex_destruir(pokedex);
}

void prueba_iterador_verifica_orden_nombre()
{
	pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
	pa2m_afirmar(pokedex != NULL,
		     "NUEVO: Iterador Orden Nombre: Pokedex cargado");
	if (!pokedex)
		return;

	contexto_orden_nombre contexto = { "", true, true };
	pokedex_iterar_pokemones(pokedex, ITERAR_NOMBRE,
				 verificar_orden_nombre_callback, &contexto);

	pa2m_afirmar(contexto.orden_ok,
		     "NUEVO: Iterador por Nombre mantiene el orden alfabetico");

	pokedex_destruir(pokedex);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas Pokedex Abrir/Cantidad/Destruir");
	prueba_abrir_archivo_valido_cantidad_correcta();
	prueba_abrir_archivo_vacio_devuelve_null();
	prueba_abrir_archivo_inexistente_devuelve_null();
	prueba_abrir_null_filename_devuelve_null();
	prueba_abrir_archivo_solo_lineas_invalidas_devuelve_null();
	prueba_abrir_archivo_mixto_lee_hasta_error();
	prueba_destruir_null_no_rompe();

	pa2m_nuevo_grupo("Pruebas Pokedex Buscar");
	prueba_buscar_nombre_existente_encuentra_pokemon();
	prueba_buscar_nombre_inexistente_devuelve_null();
	prueba_buscar_id_existente_encuentra_pokemon();
	prueba_buscar_id_inexistente_devuelve_null();
	prueba_buscar_con_pokedex_null();
	prueba_buscar_nombre_null();
	prueba_buscar_id_cero();
	prueba_buscar_primer_ultimo_nombre();
	prueba_buscar_primer_ultimo_id();

	pa2m_nuevo_grupo("Pruebas Pokedex Iterar");
	prueba_iterador_id_recorre_todos();
	prueba_iterador_nombre_recorre_todos();
	prueba_iterador_con_pokedex_o_funcion_null();
	prueba_iterador_modo_invalido();
	prueba_iterador_detiene_temprano();
	prueba_iterador_verifica_orden_id();
	prueba_iterador_verifica_orden_nombre();

	return pa2m_mostrar_reporte();
}