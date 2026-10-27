# VERIFICATION.md — Protocolo de Pruebas y Validación

El objetivo de este protocolo es **garantizar que ningún cambio rompa el simulador** y que cada componente funcione a la perfección antes de ser entregado.

---

## 1. Nivel 1: Validación Matemática Unitaria (Sandbox)

* **Qué prueba:** El algoritmo de cálculo de bandas para toda la serie normalizada (E12 / E24):
  * `0.47 Ω` -> Amarillo, Violeta, Plateado
  * `220 Ω` -> Rojo, Rojo, Marrón, Dorado
  * `10 kΩ` -> Marrón, Negro, Naranja, Dorado
  * `1 MΩ` -> Marrón, Negro, Verde, Dorado
* **Cómo se corre:**
  ```cmd
  cd sandbox
  g++ test_math.cpp -o test_math.exe
  ./test_math.exe
  ```
* **Criterio de éxito:** 100% de los casos de prueba deben arrojar los colores exactos sin fallar.

---

## 2. Nivel 2: Validación Visual con QPainter (Sandbox GUI)

* **Qué prueba:** Que el componente dibuje limpiamente a cualquier escala y nivel de zoom:
  * Proporción estética correcta del cuerpo cerámico y hombros redondeados.
  * Correcta separación y centrado de las 4/5 bandas.
  * Reflejo de brillo cilíndrico realista.
* **Cómo se corre:**
  ```cmd
  cd sandbox
  ./resistor_preview.exe
  ```

---

## 3. Nivel 3: Validación de Compilación Completa

* **Qué prueba:** Que el código fuente de todo SimulIDE compile sin errores sintácticos ni enlaces rotos:
  ```cmd
  cd build_XX
  qmake SimulIDE_Build.pro
  make -j8
  ```
* **Criterio de éxito:** `simulide.exe` se genera limpiamente en `build_XX/executables/SimulIDE_2.0.0/`.

---

## 4. Nivel 4: Validación en Circuito Real

1. Abrir `simulide.exe`.
2. Arrastrar la resistencia modificada al lienzo.
3. Conectar una fuente de $5\text{ V}$, un multímetro y tierra.
4. Cambiar el valor a $220\,\Omega$ -> Las bandas deben cambiar visualmente a **Rojo-Rojo-Marrón-Dorado**.
5. Medir con el multímetro virtual -> Debe marcar exactamente $22.7\text{ mA}$.
