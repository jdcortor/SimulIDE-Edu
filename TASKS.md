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
  - Impresión industrial estampada en el cuerpo cerámico: línea 1 con serie y potencia (`SQP 5W`, etc.) y línea 2 con valor y código de tolerancia (`10Ω J`, `4R7 K`, `0R22 J`).
  - Escala paramétrica: tanto el bloque cerámico como la separación de los terminales axiales (`pinDistance`) se agrandan progresivamente con la potencia y respetan estrictamente la retícula (grid) de 8 px.
  - Acabado visual fotorrealista con gradiente marfil refractario, biselado especular y sellos laterales de cemento oscuro.
- [x] **Compilación y Smoke Test:** Binario `Compilaciones/simulide.exe` compilado y verificado en ejecución sin bloqueos.

---

## 🟡 En Progreso (In Progress)
- [ ] Verificación por parte del usuario de ambos resistores en el lienzo.

---

## 🔴 Pendientes (Backlog)
- [ ] Ajustes finos visuales o personalizaciones adicionales que el usuario solicite.
