#include <iostream>
#include <vector>
#include <limits>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

class Transporte {
    // ... [resto de la clase se mantiene igual hasta los métodos a corregir]

    void vogel() {
        vector<int> ofertaRemanente = oferta;
        vector<int> demandaRemanente = demanda;
        vector<vector<int>> asignaciones(numCentros, vector<int>(numPuertos, 0));
        int costoTotal = 0;
        bool hayAsignacionPendiente = true;

        cout << "\nResolviendo usando el metodo de aproximacion de Vogel...\n\n";

        while (hayAsignacionPendiente) {
            // Calcular penalizaciones por fila
            vector<pair<int, pair<int, int>>> penalizacionesFila;
            for (int i = 0; i < numCentros; i++) {
                if (ofertaRemanente[i] <= 0) continue;
                vector<int> costosDisponibles;
                for (int j = 0; j < numPuertos; j++) {
                    if (demandaRemanente[j] > 0) {
                        costosDisponibles.push_back(costos[i][j]);
                    }
                }
                if (costosDisponibles.size() >= 2) {
                    sort(costosDisponibles.begin(), costosDisponibles.end());
                    penalizacionesFila.push_back({costosDisponibles[1] - costosDisponibles[0], {i, -1}});
                }
            }

            // Calcular penalizaciones por columna
            vector<pair<int, pair<int, int>>> penalizacionesColumna;
            for (int j = 0; j < numPuertos; j++) {
                if (demandaRemanente[j] <= 0) continue;
                vector<int> costosDisponibles;
                for (int i = 0; i < numCentros; i++) {
                    if (ofertaRemanente[i] > 0) {
                        costosDisponibles.push_back(costos[i][j]);
                    }
                }
                if (costosDisponibles.size() >= 2) {
                    sort(costosDisponibles.begin(), costosDisponibles.end());
                    penalizacionesColumna.push_back({costosDisponibles[1] - costosDisponibles[0], {-1, j}});
                }
            }

            // Encontrar la mayor penalización
            int maxPenalizacion = -1;
            int filaSelec = -1, columnaSelec = -1;

            for (const auto& p : penalizacionesFila) {
                if (p.first > maxPenalizacion) {
                    maxPenalizacion = p.first;
                    filaSelec = p.second.first;
                    columnaSelec = -1;
                }
            }

            for (const auto& p : penalizacionesColumna) {
                if (p.first > maxPenalizacion) {
                    maxPenalizacion = p.first;
                    filaSelec = -1;
                    columnaSelec = p.second.second;
                }
            }

            if (maxPenalizacion == -1) {
                // Buscar última asignación posible
                for (int i = 0; i < numCentros; i++) {
                    for (int j = 0; j < numPuertos; j++) {
                        if (ofertaRemanente[i] > 0 && demandaRemanente[j] > 0) {
                            filaSelec = i;
                            columnaSelec = j;
                            break;
                        }
                    }
                    if (filaSelec != -1) break;
                }
            }

            if (filaSelec == -1 && columnaSelec == -1) {
                hayAsignacionPendiente = false;
                continue;
            }

            // Realizar asignación
            if (filaSelec != -1) {
                // Encontrar el menor costo en la fila seleccionada
                int menorCosto = numeric_limits<int>::max();
                int colMenorCosto = -1;
                for (int j = 0; j < numPuertos; j++) {
                    if (demandaRemanente[j] > 0 && costos[filaSelec][j] < menorCosto) {
                        menorCosto = costos[filaSelec][j];
                        colMenorCosto = j;
                    }
                }
                if (colMenorCosto != -1) {
                    int asignacion = min(ofertaRemanente[filaSelec], demandaRemanente[colMenorCosto]);
                    asignaciones[filaSelec][colMenorCosto] = asignacion;
                    costoTotal += asignacion * costos[filaSelec][colMenorCosto];
                    ofertaRemanente[filaSelec] -= asignacion;
                    demandaRemanente[colMenorCosto] -= asignacion;
                }
            } else {
                // Encontrar el menor costo en la columna seleccionada
                int menorCosto = numeric_limits<int>::max();
                int filaMenorCosto = -1;
                for (int i = 0; i < numCentros; i++) {
                    if (ofertaRemanente[i] > 0 && costos[i][columnaSelec] < menorCosto) {
                        menorCosto = costos[i][columnaSelec];
                        filaMenorCosto = i;
                    }
                }
                if (filaMenorCosto != -1) {
                    int asignacion = min(ofertaRemanente[filaMenorCosto], demandaRemanente[columnaSelec]);
                    asignaciones[filaMenorCosto][columnaSelec] = asignacion;
                    costoTotal += asignacion * costos[filaMenorCosto][columnaSelec];
                    ofertaRemanente[filaMenorCosto] -= asignacion;
                    demandaRemanente[columnaSelec] -= asignacion;
                }
            }
        }

        // Mostrar resultados
        cout << "Matriz de asignaciones:\n";
        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                cout << setw(4) << asignaciones[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\nCosto total con el metodo de Vogel: " << costoTotal << "\n\n";
    }

    void metodoHungaro() {
        cout << "\nResolviendo usando el metodo Hungaro...\n\n";
        
        // Crear una copia de la matriz de costos
        vector<vector<int>> matrizTrabajo = costos;
        int n = max(numCentros, numPuertos);
        
        // Expandir la matriz si no es cuadrada
        for (auto& fila : matrizTrabajo) {
            while (fila.size() < n) fila.push_back(0);
        }
        while (matrizTrabajo.size() < n) {
            matrizTrabajo.push_back(vector<int>(n, 0));
        }

        // Paso 1: Reducción de filas
        for (int i = 0; i < n; i++) {
            int minFila = *min_element(matrizTrabajo[i].begin(), matrizTrabajo[i].end());
            for (int j = 0; j < n; j++) {
                matrizTrabajo[i][j] -= minFila;
            }
        }

        // Paso 2: Reducción de columnas
        for (int j = 0; j < n; j++) {
            int minCol = numeric_limits<int>::max();
            for (int i = 0; i < n; i++) {
                minCol = min(minCol, matrizTrabajo[i][j]);
            }
            for (int i = 0; i < n; i++) {
                matrizTrabajo[i][j] -= minCol;
            }
        }

        vector<int> asignacionFila(n, -1);
        vector<int> asignacionColumna(n, -1);
        vector<bool> filaMarcada(n, false);
        vector<bool> columnaMarcada(n, false);

        // Función para encontrar un cero no marcado
        auto encontrarCero = [&]() -> pair<int, int> {
            for (int i = 0; i < n; i++) {
                if (!filaMarcada[i]) {
                    for (int j = 0; j < n; j++) {
                        if (!columnaMarcada[j] && matrizTrabajo[i][j] == 0) {
                            return {i, j};
                        }
                    }
                }
            }
            return {-1, -1};
        };

        // Paso 3: Encontrar asignación inicial
        int asignaciones = 0;
        while (asignaciones < n) {
            auto [i, j] = encontrarCero();
            if (i == -1) break;

            asignacionFila[i] = j;
            asignacionColumna[j] = i;
            filaMarcada[i] = true;
            columnaMarcada[j] = true;
            asignaciones++;
        }

        // Calcular el costo total
        int costoTotal = 0;
        for (int i = 0; i < numCentros; i++) {
            if (asignacionFila[i] != -1 && asignacionFila[i] < numPuertos) {
                costoTotal += costos[i][asignacionFila[i]];
            }
        }

        // Mostrar resultados
        cout << "Matriz de asignaciones:\n";
        vector<vector<int>> asignacionesFinal(numCentros, vector<int>(numPuertos, 0));
        for (int i = 0; i < numCentros; i++) {
            if (asignacionFila[i] != -1 && asignacionFila[i] < numPuertos) {
                asignacionesFinal[i][asignacionFila[i]] = min(oferta[i], demanda[asignacionFila[i]]);
            }
        }

        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                cout << setw(4) << asignacionesFinal[i][j] << " ";
            }
            cout << "\n";
        }
        
        cout << "\nCosto total con el metodo Hungaro: " << costoTotal << "\n\n";
    }
};
 
