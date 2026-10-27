# HISTORY.md — Registro Persistente de Sesiones y Decisiones

Este archivo guarda la **memoria histórica del proyecto**, los errores encontrados, las soluciones aplicadas y las decisiones de arquitectura. Debe actualizarse al final de cada jornada de trabajo.

---

## [2026-10-26] — Sesión 1: Creación del Fork y Fundamentos del Harness

### 1. Hitos Alcanzados:
* Se creó el repositorio oficial del fork en GitHub: `https://github.com/jdcortor/SimulIDE-Edu`.
* Se clonó el código fuente completo en el entorno local: `C:\Users\usuario\Desktop\SimulIDE-Edu`.
* Se inspeccionó la arquitectura nativa del componente resistencia:
  * Archivos localizados: `src/components/passive/resistors/resistor.h` y `resistor.cpp`.
  * Se confirmó que el renderizado actual es un simple `p->drawRect(m_area)`.
* Se diseñó e implementó la arquitectura de **Harness Engineering** con los 5 archivos base (`AGENTS.md`, `HISTORY.md`, `TASKS.md`, `ARCHITECTURE.md`, `VERIFICATION.md`).

### 2. Decisiones Arquitectónicas (ADRs):
* **ADR-001 (Framework):** Mantener el desarrollo sobre **Qt 5.15** en lugar de forzar Qt 6.
  * *Motivo:* SimulIDE oficial está optimizado y altamente probado en Qt 5.15. Qt 6 rompe compatibilidad con eventos del ratón y `QPainter`.
* **ADR-002 (Rendimiento Gráfico):** Renderizado paramétrico con caché de texturas (`QPixmap`).
  * *Motivo:* Evita saturar la CPU; el cálculo de bandas solo se ejecuta cuando el usuario cambia el valor numérico, no en cada fotograma.
* **ADR-003 (Carpeta de Compilación):** Utilizar `build_XX/` para compilación completa y `sandbox/` para prototipos rápidos.
  * *Motivo:* Evita contaminar `src/` con miles de archivos objeto intermedios (`.o`, `.moc`).

### 3. Problemas Detectados y Soluciones:
* *Problema:* El enfoque previo con subcircuitos XML (`.sim1` + `.package`) bloqueaba el menú de clic derecho para editar la resistencia libremente.
* *Solución:* Modificar directamente la clase C++ nativa `Resistor`, donde las propiedades se exponen directamente a la interfaz de Qt mediante `addPropGroup` / `DoubProp`.

---

## [2026-10-26] — Sesión 2: Depuración del Toolchain MSYS2/Qt5 y Primera Compilación Exitosa

### 1. Hitos Alcanzados:
* Implementación y verificación al 100% del motor matemático de cálculo de 4 y 5 bandas en [`sandbox/test_math.cpp`](file:///C:/Users/usuario/Desktop/SimulIDE-Edu/sandbox/test_math.cpp).
* Resolución integral de dependencias binarias en MSYS2 MinGW-w64:
  * Reemplazo de paquete obsoleto `winpthreads-git 10.0` por `mingw-w64-x86_64-libwinpthread 14.0` para solucionar el fallo de punto de entrada `0xC0000139` (`clock_gettime64`).
  * Actualización de ICU a `mingw-w64-x86_64-icu 78.3` para resolver dependencia `0xC0000135` en `Qt5Core.dll` y `uic.exe`.
  * Instalación de `mingw-w64-x86_64-make` y creación de alias `make.exe` y wrapper `move` para compatibilidad multiplataforma en `SimulIDE.pri`.
* Configuración de la tarea de compilación en [`.vscode/tasks.json`](file:///C:/Users/usuario/Desktop/SimulIDE-Edu/.vscode/tasks.json).
* **Compilación completa y exitosa de SimulIDE:** binario generado en `build_XX/executables/SimulIDE_2.0.0-/simulide.exe` (9.28 MB).
