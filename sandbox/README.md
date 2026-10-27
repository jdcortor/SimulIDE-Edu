# Sandbox — Banco de Pruebas Aislado de SimulIDE-Edu

Esta carpeta contiene prototipos ligeros, pruebas unitarias y programas independientes para probar componentes gráficos y matemáticos en menos de 1 segundo de compilación antes de incrustarlos en el código fuente principal de SimulIDE (`src/`).

---

## Archivos Disponibles:

1. **`preview_ceramic.cpp` / `preview_ceramic.html`:**
   - Prototipo verificado de la Resistencia Cerámica de Potencia (`5W47RJ`), con terminales horizontales negros y cuerpo blanco puro con serigrafía compacta.

2. **`template_element.cpp` (Plantilla para Nuevos Elementos):**
   - Código base listo con ventana Qt, zoom (1x a 5x), retícula de 8 px de SimulIDE y controles interactivos.
   - Solo abra este archivo, edite la función `drawComponent()` y presione `F5` para diseñar cualquier nuevo elemento al instante.

3. **`test_math.cpp`:**
   - Pruebas matemáticas unitarias de cálculo de bandas EIA, tolerancias y TCR.

4. **`run_preview.bat`:**
   - Script ejecutable directo para compilar y abrir el previsualizador con un solo doble clic.
