# TASKS.md — Tablero de Control de Tareas

---

## 🟢 Tareas Completadas (Done)
- [x] **Crear Fork en GitHub:** Repositorio creado en `https://github.com/jdcortor/SimulIDE-Edu`.
- [x] **Clonado Local:** Código clonado en `C:\Users\usuario\Desktop\SimulIDE-Edu`.
- [x] **Harness Setup & Checkpoints:** Creación de los archivos de harness, commit seguro y etiqueta `checkpoint-pre-resistor`.
- [x] **Módulo Matemático de Bandas (C++ puro):** `sandbox/test_math.cpp` validado al 100% con tests unitarios.
- [x] **Configuración del Entorno de Compilación y Portabilidad:**
  - MSYS2 MinGW-w64 con GCC 16.2.0, Qt 5.15.19, `mingw-w64-x86_64-icu` 78 y `libwinpthread` 14.
  - Despliegue completo y autónomo en carpeta `Compilaciones/` con `windeployqt` y `resources/data`.
- [x] **Resistor de Carbón (Carbon Resistor):**
  - Eliminación completa de la física/animación de quemado y banderas de advertencia para garantizar estabilidad absoluta y cero bloqueos (`AppHangB1`).
  - Cálculo dinámico de 4, 5 y 6 bandas EIA con visualización cilíndrica 3D realista.
  - Lista desplegable de tolerancias (0.1%, 0.25%, 0.5%, 1%, 2%, 5%, 10%, 20%).
  - Coeficiente de temperatura (TCR) visible dinámicamente en el diálogo de propiedades solo cuando se eligen 6 bandas.
  - Escala geométrica según potencia nominal (1/8 W a 2 W).
- [x] **Resistencia de Cerámica de Alta Potencia (Ceramic Power Resistor):**
  - Nueva clase `CeramicResistor` registrada en la paleta bajo "Resistors".
  - Potencias elevadas: 5 W, 7 W, 10 W, 15 W, 20 W y 25 W.
  - Tolerancias normalizadas con letras de código EIA: ±1 % (F), ±2 % (G), ±5 % (J), ±10 % (K), ±20 % (M).
  - Impresión industrial compacta de una sola línea: potencia + valor + letra de tolerancia (ej: `5W47RJ`, `10W10RJ`, `25W0R22J`).
  - Terminales axiales horizontales de color negro (`#000000`) alineados a la retícula de 8 px.
  - Casing cerámico rectangular blanco puro con gradiente suave 3D y bordes nítidos.
- [x] **Preparación del Entorno Sandbox y VSCode para Nuevos Componentes:**
  - Configuración completa de `.vscode/c_cpp_properties.json`, `.vscode/tasks.json`, `.vscode/launch.json` y `.vscode/settings.json`.
  - Instalación y verificación del depurador `gdb.exe` para compilación y depuración con `F5` o botón de Play en 1 segundo.
  - Creación de `sandbox/template_element.cpp`: plantilla lista para diseñar visualmente cualquier nuevo componente con retícula, zoom y controles interactivos.
  - `sandbox/run_preview.bat`: lanzador directo de 1 clic para previsualización.
- [x] **Compilación Exitosa de SimulIDE:** Binario `Compilaciones/simulide.exe` compilado y actualizado al 100%.

---

## 🟡 En Progreso (In Progress)
- [ ] Verificación por parte del usuario de ambos resistores en el lienzo.

---

## 🔴 Pendientes (Backlog)
- [ ] Ajustes finos visuales o personalizaciones adicionales que el usuario solicite.
