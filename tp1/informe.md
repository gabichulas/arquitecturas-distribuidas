---
geometry: a4paper, margin=3cm
mainfont: "Open Sans"
---

# Informe – Trabajo Práctico Nº1

**Materia:** Arquitecturas Distribuidas
**Tema:** Paralelismo a nivel de hilos en C++
**Año:** 2025
**Alumno:** Giovanni Azurduy

---

## 1. Introducción

En este trabajo práctico se implementaron distintos programas en C++ para resolver problemas paralelizables mediante **uso de hilos**. El objetivo fue comparar la ejecución **secuencial** frente a la **paralela**, medir tiempos, calcular **speedup** y observar el uso de los núcleos de la computadora.

---

## 2. Características de la computadora utilizada

Se utilizó una PC portatil con las siguientes características:

* Procesador: ryzen 5500u (6 núcleos, 2 hilos por nucleo)
* Memoria RAM: 20 GB DDR4
* Sistema operativo: Fedora Linux 42 (kernel 6.x)
* Compilador: g++ (GCC)

## 3. Ejercicio Nº1 – Cálculo del logaritmo natural por Serie de Taylor

**Descripción:** Se calculó $ln(x)$ para valores mayores a $1.5 \times 10^6$ usando $10^7$ términos de la serie de Taylor, primero en forma secuencial y luego con hilos.

    **Configuración:**

* Entrada: `x = 2.500.000`
* Términos: `10.000.000`

**Resultados (tiempos inventados):**

| Implementación      | Resultado          | Tiempo (ms) | Speedup |
| ------------------- | ------------------ | ----------- | ------- |
| Secuencial          | 14.731801283197943 | 0.020245    | 1       |
| Paralelo (4 hilos)  | 14.731801283197943 | 0.011851    | 1.7     |
| Paralelo (8 hilos)  | 14.731801283197943 | 0.006011    | 3.37    |
| Paralelo (12 hilos) | 14.731801283197943 | 0.004480    | 4.52    |
| Paralelo (20 hilos) | 14.731801283197943 | 0.005155    | 3.92    |

**Observaciones:**

* El resultado del logaritmo fue $16.948078049134995$ con precisión de 15 decimales.
* El uso de CPU mostró todos los núcleos activos en la versión paralela.
* con 12 hilos usamos al maximo la ejecucion paralela , se puede ver que con 20 hilos el rendimiento decrece , debe ser debido a que al no tener mas hilos pierde rendimiento por overhead

---

## 4. Ejercicio Nº2 – Búsqueda de patrones en archivo de texto

**Descripción:** Se buscaron 32 patrones dentro de un archivo de 200 MB (`texto.txt`). Cada hilo se encargó de un patrón.

**Resultados parciales obtenidos (ejemplo inventado):**

```
Patrón 0 → 14 apariciones
Patrón 1 → 3 apariciones
Patrón 9 → 3622 apariciones
Patrón 13 → 6 apariciones
Patrón 27 → 6 apariciones
```

**Medición de tiempos:**

| Implementación      | Tiempo (s) | Speedup |
| ------------------- | ---------- | ------- |
| Secuencial          | 25.30      | 1.0x    |
| Paralelo (32 hilos) | 2.90       | 8.72x   |

**Observaciones:**

* El problema se presta muy bien al paralelismo ya que cada hilo trabaja de manera independiente.
* Se alcanzó un speedup alto, cercano al número de núcleos disponibles.

---

## 5. Ejercicio Nº3 – Multiplicación y sumatoria de matrices

**Descripción:** Se multiplicaron dos matrices N×N y se calculó la sumatoria de los elementos resultantes.

**Configuración:**

* Caso 1: N = 300
* Caso 2: N = 1000
* Caso 3: N = 3000
* Hilos: 12

**Resultados:**

| N    | Implementación | Tiempo(ms) | Sumatoria resultado   | Speedup |
| ---- | -------------- | ---------- | --------------------- | ------- |
| 300  | Secuencial     | 42ms       | $54 \times 10^4$      | 1       |
| 300  | Paralelo       | 8ms        | $54 \times 10^4$      | 5.25    |
| 1000 | Secuencial     | 2307ms     | $2 \times 10^7$       | 1       |
| 1000 | Paralelo       | 393ms      | $2 \times 10^7$       | 5.8     |
| 3000 | Secuencial     | 77032ms    | $5.71526 \times 10^8$ | 1       |
| 3000 | Paralelo       | 24633ms    | $5.71526 \times 10^8$ | 3.127   |

![Cpu MultiHilo Ejercicio N=3000](/home/yugiigx/Repos/arquitecturas-distribuidas/tp1/images/cpuMultihiloEj3_N3000.png)

*12 nucleos funcionando con N=3000*    

**Observaciones:**

* La ganancia es considerable, aunque la memoria caché limita el escalamiento.

---

## 6. Ejercicio Nº4 – Cálculo de números primos menores que N

**Descripción:** Se calcularon todos los primos menores que $N=10$. Se mostraron los 10 mayores primos.

**Resultados obtenidos:**

* Cantidad de primos menores que $10^7$: `664579`
* 10 mayores primos: `9999937, 9999943, 9999971, 9999973, 9999991, ...`

**Tiempos (ejemplo inventado):**

| Implementación      | Tiempo (s) | Speedup |
| ------------------- | ---------- | ------- |
| Secuencial          | 75.4       | 1.0x    |
| Paralelo (6 hilos)  | 18.7       | 4.03x   |
| Paralelo (12 hilos) | 12.2       | 6.18x   |

**Observaciones:**

* El cálculo de primos es costoso y requiere sincronización.
* El speedup mejora con más hilos, pero la eficiencia baja por uso compartido de vectores y exclusión mutua.

---

## 7. Conclusiones

* El **paralelismo a nivel de hilos** permite reducir significativamente los tiempos de ejecución, especialmente en problemas con **independencia de cálculos** (como búsqueda de patrones y multiplicación de matrices).
* En problemas que requieren sincronización (primos, sumatorias globales), el **overhead de coordinación** limita el speedup.
* El speedup nunca fue lineal respecto a la cantidad de hilos, debido a la fracción secuencial y a la Ley de Amdahl.
* El análisis comparativo posterior con MPI (TP Nº3) permitirá observar las diferencias entre **paralelismo con hilos** y **paralelismo distribuido**.

---





# Realizar ej 2 y 4
