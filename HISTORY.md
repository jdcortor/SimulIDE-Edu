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

---

## [2026-10-27] — Sesión 3: Implementación del Resistor Realista y Mecanismo de Sobrecarga

### 1. Hitos Alcanzados:
* **Punto de Control Seguro:** Creación de commit `5e1d59c0` y Git tag permanente `checkpoint-pre-resistor`.
* **Física del Resistor (`e-resistor.h` / `e-resistor.cpp`):**
  * Incorporación de `m_powerRating` ($P_{\text{rated}}$) y estado `m_isBurnt`.
  * Implementación de `voltChanged()`: cálculo en tiempo real de potencia disipada $P = V \cdot I$.
  * Lógica de sobrecarga: si $P > P_{\text{rated}}$, se invoca `burnOut()`, ajustando la admitancia a $10^{-9}\,\text{S}$ (circuito abierto de $1\,\text{G}\Omega$) y notificando a la vista gráfica.
* **Componente Visual Realista (`resistor.h` / `resistor.cpp`):**
  * Exposición de propiedades en el panel de control: `Resistance`, `PowerRating`, `Tolerance`, `Bands` (4 o 5), `Realistic` (bool), `Burnt` (bool).
  * Renderizado `QPainterPath` con relieve 3D: hombros y extremos abultados estilo axial clásico ("dogbone"), gradiente de iluminación cilíndrico y terminales metálicos ajustados.
  * Bandas de colores dinámicas calculadas automáticamente en tiempo real según el código de colores EIA.
  * Textura carbonizada de quemado con degradado radial y fisura central cuando se detecta sobrecarga.
  * Escala geométrica del cuerpo según la potencia seleccionada ($1/8\text{ W}$ a $2\text{ W}$).
* **Compilación y Despliegue:** Binario generado y enlazado directamente en `Compilaciones/simulide.exe`.

---

## [2026-10-27] — Sesión 4: Detección Continua de Sobrecarga, 6 Bandas y Tolerancia Desplegable

### 1. Hitos Alcanzados:
* **Detección de Sobrecarga Continua en Simulación:**
  * Se conectó `Resistor` al bucle de actualización periódica del simulador mediante `Simulator::self()->addToUpdateList( this )`.
  * Se implementó `updateStep()` para calcular la disipación en cada tick de simulación: $P = V^2 \cdot G$. Si $P > P_{\text{rated}}$, se invoca de inmediato `burnOut()`, abriendo el circuito en la matriz MNA y mostrando la textura quemada.
  * Se registraron los callbacks de nodo con `m_ePin[0]->changeCallBack( this )` en `stamp()`.
* **Soporte para Resistencias de 6 Bandas (EIA-96):**
  * Se integró la 6ª banda para el coeficiente de temperatura de resistencia (TCR en ppm/K).
  * Menú desplegable para TCR (100 ppm marrón, 50 ppm rojo, 25 ppm amarillo, 15 ppm naranja, 10 ppm azul, 5 ppm violeta).
  * El color de fondo del cuerpo adopta un tono azul-índigo (`#788CD7`) característico de componentes de alta precisión.
* **Tolerancia y Bandas en Listas Desplegables (Enum):**
  * La tolerancia ahora es una lista desplegable (`StrProp` enum) con valores normalizados: `0.1 %`, `0.25 %`, `0.5 %`, `1 %`, `2 %`, `5 %`, `10 %`, `20 %`.
  * La selección de bandas ahora es un selector desplegable: `4 Bands`, `5 Bands`, `6 Bands`.
* **Compilación Exitosa:** Compilado al 100% en `Compilaciones/simulide.exe`.

---

## [2026-10-27] — Sesión 5: Coeficiente de Temperatura Condicional, Eliminación de Burnt Manual, Reseteo Automático y Banner de Advertencia

### 1. Hitos Alcanzados:
* **Coeficiente de Temperatura (TCR) Exclusivo para 6 Bandas:**
  * Se sobreescribió `slotProperties()` en `Resistor` para invocar `m_propDialog->showProp("TCR", m_bands == 6)`.
  * Se vinculó el cambio dinámico en `setBands()` y `setBandsStr()` junto a `m_propDialog->adjustWidgets()` para que el campo aparezca u oculte automáticamente adaptando el alto del diálogo sin reiniciar la ventana.
* **Eliminación del Atributo Manual `Burnt`:**
  * Se retiró `Burnt` del grupo de propiedades de la GUI. La condición de quemado ahora ocurre de forma puramente física por sobrecarga eléctrica en la simulación.
* **Reinicio Automático del Estado de Quemado:**
  * Se implementó `initialize()` en `eResistor` invocando `setIsBurnt(false)` en cada parada o reinicio de la simulación (`Simulator::startSim()` y `Simulator::stopSim()`).
  * Al reiniciar la simulación o modificar la resistencia/potencia, la resistencia recupera su aspecto original, restaura su conductancia y remueve la advertencia.
* **Mensaje Educativo de Advertencia:**
  * Al quemarse la resistencia por exceso de corriente, se dibuja un banner rojo intenso con texto en blanco en negrita: `"La resistencia se ha quemado por exceso de corriente"`, con flecha indicadora apuntando al cuerpo carbonizado.
  * Se expandió el área delimitadora (`m_area`) en `updateDimensions()` para que el motor gráfico de Qt repinte y limpie el banner sin dejar artefactos visuales en el lienzo.
* **Compilación y Verificación:** Enlace y generación limpia de `Compilaciones/simulide.exe`.

---

## [2026-10-27] — Sesión 6: Depuración de Deadlock de Simulación, Purga de Burnt en Carbón y Creación del Resistor Cerámico de Potencia

### 1. Diagnóstico del Congelamiento (AppHangB1 - Event ID 1002):
* **Causa Raíz:** Se identificó que la llamada a modificaciones directas de `QGraphicsItem` (`prepareGeometryChange()`, redimensionado de pines y `update()`) invocadas dentro de callbacks de simulación del circuito o `updateStep()` generaban un interbloqueo (*deadlock*) con el hilo de cálculo de simulación en `QtConcurrent::run`. El hilo principal de la GUI quedaba esperando en `m_CircuitFuture.waitForFinished()` mientras el hilo de simulación intentaba adquirir el lock de la escena de Qt, provocando el cuelgue definitivo de la aplicación.
* **Acción correctiva:** Se erradicó por completo la lógica de quemado de `e-resistor.h/cpp` y `resistor.h/cpp`. El cálculo MNA de la resistencia queda 100% limpio y lineal, eliminando toda posibilidad de interbloqueo.

### 2. Hitos y Características Implementadas:
* **Resistor de Carbón (`Carbon Resistor`):**
  * Renombrado en la biblioteca de componentes a `tr("Carbon Resistor")`.
  * Se preservó el cálculo y dibujo dinámico de 4, 5 y 6 bandas EIA fotorrealista.
  * Menú desplegable para Tolerancia (`0.1 %` a `20 %`).
  * Menú desplegable para TCR visible dinámicamente **únicamente** cuando se seleccionan 6 bandas.
  * Escala física del cuerpo cilíndrico según potencia (1/8 W a 2 W).
* **Nuevo Componente: Resistor Cerámico de Potencia (`CeramicResistor`):**
  * Nueva clase `CeramicResistor` derivada de `Comp2Pin` y `eResistor`, registrada en la biblioteca bajo la categoría `Resistors`.
  * Potencias normalizadas de disipación alta: `5 W, 7 W, 10 W, 15 W, 20 W, 25 W`.
  * Tolerancias normalizadas con letras de código industrial EIA: `±1 % (F), ±2 % (G), ±5 % (J), ±10 % (K), ±20 % (M)`.
  * Marcaje industrial serigrafiado en el cuerpo:
    * Línea 1: Serie y potencia nominal (p. ej. `SQP 5W`, `SQP 10W`, `SQP 25W`).
    * Línea 2: Resistencia y código de tolerancia con formato estándar (p. ej. `10Ω J`, `4R7 K`, `0R22 J`, `1K J`).
  * Escalamiento paramétrico y separación de terminales:
    * Tanto el bloque cerámico rectangular como la distancia entre pines terminales (`pinDistance`) crecen en función de la potencia seleccionada.
    * La distancia de pines está alineada estrictamente con múltiplos de 8 píxeles (6 a 16 celdas de cuadrícula) para garantizar conexiones sin quiebres ni desalineaciones en el lienzo de circuitos.
  * Renderizado fotorrealista de casing cerámico refractario marfil (`#F8F6F0`), biselado 3D, sombras de oclusión, terminales axiales metálicos y sellos de cemento oscuro.
* **Compilación y Smoke Test:**
  * Makefile regenerado mediante `qmake.exe`.
  * Compilación y enlace exitosos con `mingw32-make.exe -j4`.
  * Ejecutable verificado en ejecución en `Compilaciones/simulide.exe`.
