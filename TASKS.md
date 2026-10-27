# TASKS.md — Tablero de Control de Tareas

---

## 🟢 Tareas Completadas (Done)
- [x] **Crear Fork en GitHub:** Repositorio creado en `https://github.com/jdcortor/SimulIDE-Edu`.
- [x] **Clonado Local:** Código clonado en `C:\Users\usuario\Desktop\SimulIDE-Edu`.
- [x] **Inspección de Resistor:** Análisis de `resistor.h` y `resistor.cpp`.
- [x] **Harness Setup:** Creación de `AGENTS.md`, `HISTORY.md`, `TASKS.md`, `ARCHITECTURE.md`, `VERIFICATION.md`.
- [x] **Módulo Matemático de Bandas (C++ puro):** `sandbox/test_math.cpp` validado al 100% con tests unitarios.
- [x] **Configuración del Entorno de Compilación:**
  - MSYS2 MinGW-w64 con GCC 16.2.0, Qt 5.15.19, `mingw-w64-x86_64-icu` 78 y `libwinpthread` 14.
  - Tareas de VS Code configuradas en `.vscode/tasks.json`.
  - Primera compilación completa exitosa de SimulIDE (`simulide.exe` generado a 9.28 MB).

---

## 🟡 En Progreso (In Progress)
- [/] **Integración en `resistor.cpp` y `resistor.h`:**
  - Agregar propiedades `Tolerance` (%), `PowerRating` (W) y `RealisticView` (bool).
  - Integrar el cálculo de franjas y nuevo renderizado en `Resistor::paint()`.
- [ ] **Renderizador Visual con QPainter:** Dibujar cuerpo cerámico cilíndrico realista, hombros redondeados y franjas de colores.
- [ ] **Integración en `resistor.cpp` y `resistor.h`:**
  - Agregar propiedades `Tolerance` (%), `PowerRating` (W) y `RealisticView` (bool).
  - Conectar el nuevo renderizado en `Resistor::paint()`.
- [ ] **Lógica de Sobrecarga Eléctrica:** Si $P > P_{\text{rated}}$, cambiar a textura quemada y abrir circuito.
- [ ] **Compilación Completa en `build_XX/`:** Generar el primer ejecutable `SimulIDE_2.0.0.exe` con el resistor nativo.
