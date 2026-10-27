# ARCHITECTURE.md — Arquitectura de Software de SimulIDE-Edu

Este documento explica cómo funciona internamente SimulIDE para que ningún desarrollador ni agente mezcle la interfaz gráfica con el motor de simulación física.

---

## 1. El Principio Fundamental: Separación de Responsabilidades

SimulIDE divide estrictamente cada componente en dos entidades:

```
                  ┌──────────────────────────────────────────────┐
                  │             COMPONENTE EN PANTALLA           │
                  │   Clase: Resistor (hereda de Comp2Pin)       │
                  │   - Maneja la apariencia (paint / QPainter)  │
                  │   - Maneja clics, propiedades y posición     │
                  │   - Frecuencia: ~60 Hz (Monitor)             │
                  └──────────────────────┬───────────────────────┘
                                         │ enlaza
                                         ▼
                  ┌──────────────────────────────────────────────┐
                  │             COMPONENTE EN EL SOLVER          │
                  │   Clase: eResistor (hereda de eElement)      │
                  │   - Maneja la física (conductancia G = 1/R)  │
                  │   - Calcula matriz nodal (MNA)               │
                  │   - Frecuencia: 1.000.000 pasos/seg (1 MHz)  │
                  └──────────────────────────────────────────────┘
```

---

## 2. El Ciclo de Vida de un Componente

1. **Construcción:**
   * El usuario arrastra el icono desde la barra lateral.
   * `Resistor::construct()` instancia un nuevo objeto `Resistor`.
2. **Registro de Propiedades:**
   * En el constructor se llama a `addPropGroup()`.
   * Esto genera automáticamente las cajas de texto en la ventana de propiedades de Qt.
3. **Pintado en Pantalla:**
   * Qt llama periódicamente a `Resistor::paint(QPainter *p, ...)`.
   * Aquí se pinta el cuerpo cerámico y las bandas calculadas.
4. **Simulación Eléctrica:**
   * Al pulsar el botón "Play", el simulador toma los `ePin` de la resistencia y construye la matriz MNA.
   * En cada paso de tiempo, calcula la corriente $I$ y tensión $V$.

---

## 3. Dónde se Aplica Nuestra Modificación

Para la **Resistencia Didáctica**:
* **En `Resistor` (Gráficos):** Modificamos `paint()` para dibujar las bandas fotorrealistas según `resistance()`.
* **En `eResistor` (Física):** Solo se añade una verificación: si la potencia disipada $P = I^2 \cdot R > P_{\text{rated}}$, se establece la resistencia interna a un valor casi infinito ($1\,\text{G}\Omega$, circuito abierto) y se avisa a la capa gráfica para mostrar la textura carbonizada.
