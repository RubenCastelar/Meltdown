# 🧠 Meltdown Vulnerability Research & Proof of Concept

## 📖 Descripción

Este repositorio contiene un análisis técnico detallado, así como una prueba de concepto (PoC) de la vulnerabilidad **Meltdown (CVE-2017-5754)**, la cual afecta a múltiples microprocesadores modernos. Esta vulnerabilidad permite que un proceso en espacio de usuario acceda a memoria privilegiada del kernel aprovechando la ejecución especulativa y canales laterales basados en caché.

> ⚠️ Este proyecto es únicamente con fines educativos y de investigación. No está destinado para usos maliciosos.

---

## 🧠 ¿Qué es Meltdown?

**Meltdown** es una vulnerabilidad crítica descubierta en 2017 que rompe el aislamiento entre aplicaciones y el sistema operativo. Permite que procesos sin privilegios lean arbitrariamente memoria del kernel utilizando un canal lateral basado en el comportamiento de la **caché de CPU**.

### 🔍 Cómo funciona

1. **Lectura especulativa de direcciones privilegiadas**.
2. **Uso del valor especulativo como índice para cargar en caché un array controlado por el atacante**.
3. **Medición del tiempo de acceso (Flush+Reload)** para determinar el valor leído.
4. **Extracción de datos sensibles**, como contraseñas o claves de cifrado.

### 🔐 Mitigaciones

- KPTI (Kernel Page Table Isolation)
- Retpoline (para Spectre)
- Microcode updates por parte de Intel/AMD
- Actualizaciones del kernel y herramientas de detección

---

## 🧪 Estructura del proyecto

