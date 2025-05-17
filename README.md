# CVE-2017-5754 (Meltdown) - Análisis y Explotación

Este repositorio contiene el código fuente para demostrar la explotación de la vulnerabilidad **CVE-2017-5754**, conocida como **Meltdown**. Esta falla afecta a múltiples procesadores Intel y permite a procesos en espacio de usuario leer **memoria privilegiada del kernel** aprovechando la ejecución especulativa.

---

## ⚠️ Advertencia

Este código se proporciona únicamente con **fines educativos y de investigación en ciberseguridad**. **No debe usarse en sistemas en producción ni sin autorización expresa**. El uso indebido puede ser ilegal y va en contra de los términos de uso del autor.

---

## 🧠 Descripción técnica

Meltdown aprovecha una característica de los procesadores modernos llamada **ejecución especulativa**, que intenta predecir y ejecutar instrucciones antes de que se confirme si son válidas.

La vulnerabilidad permite que, aunque una instrucción falle por acceso a memoria privilegiada, las instrucciones especulativas posteriores se ejecuten temporalmente. Si en esa ejecución se accede a memoria protegida, se puede filtrar su contenido mediante efectos colaterales en la **caché del procesador**.

Esto permite que un proceso sin privilegios lea datos sensibles como:

- Contraseñas almacenadas en memoria del kernel.
- Llaves criptográficas.
- Cualquier información que resida en el espacio de direcciones privilegiado.

---

## 📁 Archivos del repositorio

- `meltdown.c`: Código fuente del exploit que lee memoria del kernel usando ejecución especulativa y análisis de tiempos de acceso a la caché.
- Se le debe pasar dos parámetros al ejecutable:
    - Dirección física a partir de la cual se va a realizar la explotación.
    - Número de bytes que se desea leer.

---

## 🛠️ Compilación

```bash
gcc -o meltdown meltdown.c
