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
- [x] **Implementación Nativa del Resistor Realista y Educativo:**
  - Propiedades integradas: `Resistance` (Ω), `PowerRating` (W), `Tolerance` (lista desplegable con valores estándar: 0.1%, 0.25%, 0.5%, 1%, 2%, 5%, 10%, 20%), `Bands` (lista desplegable: 4, 5 o 6 bandas), `TCR` (ppm/K para la 6ª banda), `Realistic` (bool), `Burnt` (bool).
  - Renderizador visual con `QPainter`: cuerpo cilíndrico cerámico con hombros redondeados en relieve 3D, soporte para 4, 5 y 6 bandas en tiempo real y textura carbonizada en caso de sobrecarga.
  - Escala geométrica del cuerpo según potencia nominal ($1/8\text{ W}$, $1/4\text{ W}$, $1/2\text{ W}$, $1\text{ W}$, $2\text{ W}$).
  - Física de sobrecarga eléctrica en `e-resistor.cpp` y `Resistor::updateStep()`: monitoreo en tiempo real acoplado a la lista de actualización del simulador (`addToUpdateList`), detección continua de $P = V^2/R > P_{\text{rated}}$, desconexión a circuito abierto ($1\,\text{G}\Omega$) y quemado visual instantáneo en vivo durante la simulación.
- [x] **Compilación Exitosa:** Binario `Compilaciones/simulide.exe` compilado y actualizado al 100% sin errores.

---

## 🟡 En Progreso (In Progress)
- [/] **Verificación Interactiva en Simulación:**
  - Abrir `Compilaciones/simulide.exe`.
  - Probar arrastrar la resistencia, cambiar valores de ohmios en el panel de propiedades y verificar el cambio de franjas de color.
  - Probar conectar una fuente de alto voltaje para verificar el efecto de sobrecarga y quemado.

---

## 🔴 Pendientes (Backlog)
- [ ] Ajustes finos visuales o personalizaciones adicionales que el usuario solicite.
