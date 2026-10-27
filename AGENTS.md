# AGENTS.md — Directivas de Desarrollo para SimulIDE-Edu

Este archivo es el **manual de operaciones y directivas obligatorias** para cualquier agente de Inteligencia Artificial que trabaje en este repositorio.

---

## 1. Identidad y Misión del Proyecto

* **Nombre del Proyecto:** `SimulIDE-Edu`
* **Tipo:** Fork oficial con enfoque didáctico y pedagógico de SimulIDE.
* **Misión:** Transformar los componentes abstractos en elementos didácticos fotorrealistas e interactivos para la enseñanza de la electrónica.
* **Componente Piloto:** Resistencia fotorrealista con:
  1. Cálculo dinámico de 4 o 5 bandas de color en tiempo real a partir del valor en ohmios.
  2. Escalado físico del cuerpo cerámico según la potencia ($1/8\text{ W}$, $1/4\text{ W}$, $1/2\text{ W}$, $1\text{ W}$, $2\text{ W}$).
  3. Detección de sobrecarga ($P > P_{\text{rated}}$) con textura carbonizada/quemado y apertura del circuito.

---

## 2. Entorno Tecnológico y Herramientas

* **Lenguaje:** C++17.
* **Framework Gráfico e Interfaz:** Qt 5.15 (`QtWidgets`, `QGraphicsScene`, `QGraphicsItem`, `QPainter`).
* **Compilador:** MinGW-w64 (`g++` / `gcc`) en Windows vía MSYS2 (`C:\msys64\mingw64\bin`).
* **Sistema de Construcción:** `qmake` + `make`.

---

## 3. Estructura de Carpetas y Dónde Trabajar

* **Código fuente de la interfaz y componentes:** `src/components/`
  * Componente Resistencia: `src/components/passive/resistors/resistor.h` y `resistor.cpp`
* **Código fuente del motor de física/solver MNA:** `src/simulator/elements/`
  * Física de la Resistencia: `src/simulator/elements/passive/e-resistor.h` y `e-resistor.cpp`
* **Recursos e Iconos:** `src/icons/` y `resources/`
* **Carpeta Oficial de Compilación:** `build_XX/`
  * Archivo de proyecto: `build_XX/SimulIDE_Build.pro`
  * Binario de salida: `build_XX/executables/SimulIDE_2.0.0/simulide.exe`
  * Archivos intermedios (`.o`, `.moc`): `build_XX/build/objects/` (mantiene `src/` limpio).
* **Carpeta de Pruebas Aisladas (Sandbox):** `sandbox/`
  * Prototipos rápidos en C++/Qt de 1 segundo de compilación antes de tocar `src/`.

---

## 4. Comandos de Compilación y Verificación

### A. Para compilar SimulIDE completo:
```cmd
cd build_XX
qmake SimulIDE_Build.pro
make -j8
```

### B. Para compilar y probar el banco aislado (Sandbox):
```cmd
cd sandbox
g++ test_resistor.cpp -o test_resistor.exe -lQt5Widgets -lQt5Gui -lQt5Core
./test_resistor.exe
```

---

## 5. Reglas de Oro y Guardrails (Restricciones Estrictas)

1. ⛔ **No tocar el Solver Matemático central sin justificación:** Solo se edita la capa gráfica (`Component`) y propiedades pedagógicas. El solver MNA en `src/simulator/` es sagrado.
2. ⛔ **Sin librerías externas pesadas:** No agregar dependencias de terceros; usar únicamente las librerías oficiales de Qt 5 y STL de C++17.
3. ⛔ **Prohibido entregar código sin verificar compilación:** Ninguna tarea se considera terminada si no compila limpiamente sin advertencias críticas.
4. ⛔ **Preservar la documentación existente:** Conservar los encabezados de copyright de Santiago González y documentar los nuevos aportes pedagógicos en español o inglés claro.
5. ⚡ **Optimización Gráfica:** Utilizar `QPixmap` en caché para las texturas del cuerpo cerámico. Jamás recalcular degradados complejos en cada milisegundo de simulación para mantener 60 FPS estables.

---

## 6. Protocolo de Trabajo en cada Sesión (Harness Loop)

1. **Lectura inicial:** Leer `TASKS.md` y `HISTORY.md` antes de actuar para entender el estado actual.
2. **Desarrollo aislado:** Desarrollar y verificar la lógica en `sandbox/`.
3. **Integración:** Trasladar el código probado a `src/`.
4. **Verificación:** Ejecutar `make` en `build_XX/`.
5. **Cierre:** Registrar la tarea completada en `TASKS.md` y documentar lecciones en `HISTORY.md`.
