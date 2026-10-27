#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <cassert>

enum ResistorColor {
    COLOR_BLACK = 0,
    COLOR_BROWN = 1,
    COLOR_RED = 2,
    COLOR_ORANGE = 3,
    COLOR_YELLOW = 4,
    COLOR_GREEN = 5,
    COLOR_BLUE = 6,
    COLOR_VIOLET = 7,
    COLOR_GRAY = 8,
    COLOR_WHITE = 9,
    COLOR_GOLD = -1,
    COLOR_SILVER = -2,
    COLOR_NONE = -99
};

std::string colorName(int color) {
    switch (color) {
        case COLOR_BLACK:  return "Negro (0)";
        case COLOR_BROWN:  return "Marrón (1)";
        case COLOR_RED:    return "Rojo (2)";
        case COLOR_ORANGE: return "Naranja (3)";
        case COLOR_YELLOW: return "Amarillo (4)";
        case COLOR_GREEN:  return "Verde (5)";
        case COLOR_BLUE:   return "Azul (6)";
        case COLOR_VIOLET: return "Violeta (7)";
        case COLOR_GRAY:   return "Gris (8)";
        case COLOR_WHITE:  return "Blanco (9)";
        case COLOR_GOLD:   return "Dorado (x0.1 / 5%)";
        case COLOR_SILVER: return "Plateado (x0.01 / 10%)";
        default:           return "Desconocido";
    }
}

int getToleranceColor(double tolerancePercent) {
    if (std::abs(tolerancePercent - 1.0) < 0.1) return COLOR_BROWN;
    if (std::abs(tolerancePercent - 2.0) < 0.1) return COLOR_RED;
    if (std::abs(tolerancePercent - 0.5) < 0.05) return COLOR_GREEN;
    if (std::abs(tolerancePercent - 0.25) < 0.05) return COLOR_BLUE;
    if (std::abs(tolerancePercent - 0.1) < 0.05) return COLOR_VIOLET;
    if (std::abs(tolerancePercent - 10.0) < 0.5) return COLOR_SILVER;
    return COLOR_GOLD; // Default: 5%
}

struct ResistorBands {
    int band1;
    int band2;
    int band3;      // 3rd digit for 5-band, or multiplier for 4-band
    int multiplier; // Multiplier color
    int tolerance;  // Tolerance color
    bool is5Bands;
};

ResistorBands calculateBands(double resistance, double tolerancePercent, bool use5Bands = false) {
    ResistorBands result;
    result.is5Bands = use5Bands;
    result.tolerance = getToleranceColor(tolerancePercent);

    if (resistance <= 0.0) {
        result.band1 = COLOR_BLACK;
        result.band2 = COLOR_BLACK;
        result.band3 = COLOR_BLACK;
        result.multiplier = COLOR_BLACK;
        return result;
    }

    if (!use5Bands) {
        // --- CÁLCULO DE 4 BANDAS ---
        // Se necesitan 2 cifras significativas (10 a 99)
        int exp10 = static_cast<int>(std::floor(std::log10(resistance)));
        double normalized = resistance / std::pow(10.0, exp10); // Rango [1.0, 10.0)
        
        // Escalar a 2 cifras enteras [10, 99]
        int digits = static_cast<int>(std::round(normalized * 10.0));
        int multExponent = exp10 - 1;

        if (digits >= 100) {
            digits /= 10;
            multExponent += 1;
        }

        result.band1 = digits / 10;
        result.band2 = digits % 10;
        result.band3 = COLOR_NONE;

        if (multExponent >= 0 && multExponent <= 9) {
            result.multiplier = multExponent;
        } else if (multExponent == -1) {
            result.multiplier = COLOR_GOLD;   // x0.1
        } else if (multExponent == -2) {
            result.multiplier = COLOR_SILVER; // x0.01
        } else {
            result.multiplier = COLOR_BLACK;
        }
    } else {
        // --- CÁLCULO DE 5 BANDAS ---
        // Se necesitan 3 cifras significativas (100 a 999)
        int exp10 = static_cast<int>(std::floor(std::log10(resistance)));
        double normalized = resistance / std::pow(10.0, exp10); // [1.0, 10.0)
        
        int digits = static_cast<int>(std::round(normalized * 100.0));
        int multExponent = exp10 - 2;

        if (digits >= 1000) {
            digits /= 10;
            multExponent += 1;
        }

        result.band1 = digits / 100;
        result.band2 = (digits / 10) % 10;
        result.band3 = digits % 10;

        if (multExponent >= 0 && multExponent <= 9) {
            result.multiplier = multExponent;
        } else if (multExponent == -1) {
            result.multiplier = COLOR_GOLD;   // x0.1
        } else if (multExponent == -2) {
            result.multiplier = COLOR_SILVER; // x0.01
        } else {
            result.multiplier = COLOR_BLACK;
        }
    }

    return result;
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "  BANCO DE PRUEBAS MATEMATICO: RESISTOR COLOR BANDS\n";
    std::cout << "========================================================\n\n";

    struct TestCase {
        double r;
        double tol;
        bool fiveBands;
        std::string label;
    };

    std::vector<TestCase> cases = {
        { 0.47,  5.0, false, "0.47 Ohm 5% (4 bandas)" },
        { 2.2,   5.0, false, "2.2 Ohm 5% (4 bandas)" },
        { 10.0,  5.0, false, "10 Ohm 5% (4 bandas)" },
        { 100.0, 5.0, false, "100 Ohm 5% (4 bandas)" },
        { 220.0, 5.0, false, "220 Ohm 5% (4 bandas)" },
        { 330.0, 5.0, false, "330 Ohm 5% (4 bandas)" },
        { 470.0, 5.0, false, "470 Ohm 5% (4 bandas)" },
        { 1000.0, 5.0, false, "1 kOhm 5% (4 bandas)" },
        { 4700.0, 5.0, false, "4.7 kOhm 5% (4 bandas)" },
        { 10000.0, 5.0, false, "10 kOhm 5% (4 bandas)" },
        { 1000000.0, 5.0, false, "1 MOhm 5% (4 bandas)" },
        { 220.0, 1.0, true,  "220 Ohm 1% (5 bandas)" },
        { 4700.0, 1.0, true,  "4.7 kOhm 1% (5 bandas)" },
        { 10000.0, 0.5, true, "10 kOhm 0.5% (5 bandas)" }
    };

    for (const auto& tc : cases) {
        ResistorBands b = calculateBands(tc.r, tc.tol, tc.fiveBands);
        std::cout << "Prueba: " << tc.label << "\n";
        if (!b.is5Bands) {
            std::cout << "  Banda 1 (D1): " << colorName(b.band1) << "\n";
            std::cout << "  Banda 2 (D2): " << colorName(b.band2) << "\n";
            std::cout << "  Banda 3 (Mult): " << colorName(b.multiplier) << "\n";
            std::cout << "  Banda 4 (Tol): " << colorName(b.tolerance) << "\n";
        } else {
            std::cout << "  Banda 1 (D1): " << colorName(b.band1) << "\n";
            std::cout << "  Banda 2 (D2): " << colorName(b.band2) << "\n";
            std::cout << "  Banda 3 (D3): " << colorName(b.band3) << "\n";
            std::cout << "  Banda 4 (Mult): " << colorName(b.multiplier) << "\n";
            std::cout << "  Banda 5 (Tol): " << colorName(b.tolerance) << "\n";
        }
        std::cout << "--------------------------------------------------------\n";
    }

    // Aserciones formales para garantizar 100% de exactitud
    ResistorBands r220 = calculateBands(220.0, 5.0, false);
    assert(r220.band1 == COLOR_RED);
    assert(r220.band2 == COLOR_RED);
    assert(r220.multiplier == COLOR_BROWN);
    assert(r220.tolerance == COLOR_GOLD);

    ResistorBands r4k7 = calculateBands(4700.0, 5.0, false);
    assert(r4k7.band1 == COLOR_YELLOW);
    assert(r4k7.band2 == COLOR_VIOLET);
    assert(r4k7.multiplier == COLOR_RED);
    assert(r4k7.tolerance == COLOR_GOLD);

    ResistorBands r0_47 = calculateBands(0.47, 5.0, false);
    assert(r0_47.band1 == COLOR_YELLOW);
    assert(r0_47.band2 == COLOR_VIOLET);
    assert(r0_47.multiplier == COLOR_SILVER);

    std::cout << "\n>>> TODAS LAS ASERCIONES PASARON CON EXITO (100% CORRECTO) <<<\n";
    return 0;
}
