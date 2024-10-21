#include <iostream>
#include <vector>
#include <limits>
#include <sstream>
#include <algorithm>

using namespace std;

class Transporte {
public:
    int numCentros, numPuertos;
    vector<vector<int>> costos;
    vector<int> oferta, demanda;

    // Función para validar que el valor ingresado sea numérico
    template<typename T>
    T obtenerEntradaNumerica() {
        T valor;
        string entrada;
        while (true) {
            getline(cin, entrada);
            stringstream ss(entrada);
            if (ss >> valor && ss.eof()) {
                return valor;
            }
            cout << "Error: ingrese un numero valido.\nIntente de nuevo: ";
        }
    }

    void ingresarDatos() {
        cout << "Ingrese numero de centros logisticos: ";
        numCentros = obtenerEntradaNumerica<int>();

        cout << "Ingrese numero de puertos: ";
        numPuertos = obtenerEntradaNumerica<int>();

        oferta.resize(numCentros);
        demanda.resize(numPuertos);
        costos.resize(numCentros, vector<int>(numPuertos));

        cout << "Ingrese la oferta de cada centro:\n";
        for (int i = 0; i < numCentros; i++) {
            cout << "Centro " << i + 1 << ": ";
            oferta[i] = obtenerEntradaNumerica<int>();
        }

        cout << "Ingrese la demanda de cada puerto:\n";
        for (int j = 0; j < numPuertos; j++) {
            cout << "Puerto " << j + 1 << ": ";
            demanda[j] = obtenerEntradaNumerica<int>();
        }

        cout << "Ingrese la matriz de costos (por contenedor):\n";
        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                cout << "Costo de Centro " << i + 1 << " a Puerto " << j + 1 << ": ";
                costos[i][j] = obtenerEntradaNumerica<int>();
            }
        }

        mostrarMatriz();
    }

    // Función para mostrar la matriz de costos, oferta y demanda
    void mostrarMatriz() {
        cout << "\nMatriz de Costos:\n";
        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                cout << costos[i][j] << "\t";
            }
            cout << "| " << oferta[i] << " (Oferta)\n";
        }
        cout << "-------------------------------------\n";
        for (int j = 0; j < numPuertos; j++) {
            cout << demanda[j] << "\t";
        }
        cout << "(Demanda)\n\n";
    }

    // Método de Menor Costo
    void menorCosto() {
        vector<int> ofertaRemanente = oferta;
        vector<int> demandaRemanente = demanda;
        vector<vector<int>> asignaciones(numCentros, vector<int>(numPuertos, 0));
        int costoTotal = 0;

        cout << "Resolviendo usando el metodo de menor costo...\n";
        
        while (true) {
            int menorCosto = numeric_limits<int>::max();
            int fila = -1, columna = -1;

            for (int i = 0; i < numCentros; i++) {
                for (int j = 0; j < numPuertos; j++) {
                    if (ofertaRemanente[i] > 0 && demandaRemanente[j] > 0 && costos[i][j] < menorCosto) {
                        menorCosto = costos[i][j];
                        fila = i;
                        columna = j;
                    }
                }
            }

            if (fila == -1 || columna == -1) break; 

            int asignacion = min(ofertaRemanente[fila], demandaRemanente[columna]);
            asignaciones[fila][columna] = asignacion;
            costoTotal += asignacion * costos[fila][columna];

            ofertaRemanente[fila] -= asignacion;
            demandaRemanente[columna] -= asignacion;
        }

        cout << "Costo total con el metodo de menor costo: " << costoTotal << "\n";
    }

    // Método de Esquina Noroeste
    void esquinaNoroeste() {
        vector<int> ofertaRemanente = oferta;
        vector<int> demandaRemanente = demanda;
        vector<vector<int>> asignaciones(numCentros, vector<int>(numPuertos, 0));
        int costoTotal = 0;

        cout << "Resolviendo usando el metodo de esquina noroeste...\n";

        int i = 0, j = 0;
        while (i < numCentros && j < numPuertos) {
            int asignacion = min(ofertaRemanente[i], demandaRemanente[j]);
            asignaciones[i][j] = asignacion;
            costoTotal += asignacion * costos[i][j];

            ofertaRemanente[i] -= asignacion;
            demandaRemanente[j] -= asignacion;

            if (ofertaRemanente[i] == 0) i++;
            if (demandaRemanente[j] == 0) j++;
        }

        cout << "Costo total con el metodo de esquina noroeste: " << costoTotal << "\n";
    }

    // Método de Aproximación de Vogel
    void voguel() {
        vector<int> ofertaRemanente = oferta;
        vector<int> demandaRemanente = demanda;
        vector<vector<int>> asignaciones(numCentros, vector<int>(numPuertos, 0));
        int costoTotal = 0;

        cout << "Resolviendo usando el metodo de aproximacion de Vogel...\n";

        while (true) {
            vector<int> penalizacionFila(numCentros, -1), penalizacionColumna(numPuertos, -1);

            for (int i = 0; i < numCentros; i++) {
                if (ofertaRemanente[i] > 0) {
                    int min1 = numeric_limits<int>::max(), min2 = numeric_limits<int>::max();
                    for (int j = 0; j < numPuertos; j++) {
                        if (demandaRemanente[j] > 0) {
                            if (costos[i][j] < min1) {
                                min2 = min1;
                                min1 = costos[i][j];
                            } else if (costos[i][j] < min2) {
                                min2 = costos[i][j];
                            }
                        }
                    }
                    penalizacionFila[i] = min2 - min1;
                }
            }

            for (int j = 0; j < numPuertos; j++) {
                if (demandaRemanente[j] > 0) {
                    int min1 = numeric_limits<int>::max(), min2 = numeric_limits<int>::max();
                    for (int i = 0; i < numCentros; i++) {
                        if (ofertaRemanente[i] > 0) {
                            if (costos[i][j] < min1) {
                                min2 = min1;
                                min1 = costos[i][j];
                            } else if (costos[i][j] < min2) {
                                min2 = costos[i][j];
                            }
                        }
                    }
                    penalizacionColumna[j] = min2 - min1;
                }
            }

            int maxPenalizacion = -1;
            int fila = -1, columna = -1;

            for (int i = 0; i < numCentros; i++) {
                if (penalizacionFila[i] > maxPenalizacion) {
                    maxPenalizacion = penalizacionFila[i];
                    fila = i;
                    columna = -1;
                }
            }

            for (int j = 0; j < numPuertos; j++) {
                if (penalizacionColumna[j] > maxPenalizacion) {
                    maxPenalizacion = penalizacionColumna[j];
                    fila = -1;
                    columna = j;
                }
            }

            if (fila == -1 && columna == -1) break;

            if (fila != -1) {
                for (int j = 0; j < numPuertos; j++) {
                    if (demandaRemanente[j] > 0) {
                        int asignacion = min(ofertaRemanente[fila], demandaRemanente[j]);
                        asignaciones[fila][j] = asignacion;
                        costoTotal += asignacion * costos[fila][j];
                        ofertaRemanente[fila] -= asignacion;
                        demandaRemanente[j] -= asignacion;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < numCentros; i++) {
                    if (ofertaRemanente[i] > 0) {
                        int asignacion = min(ofertaRemanente[i], demandaRemanente[columna]);
                        asignaciones[i][columna] = asignacion;
                        costoTotal += asignacion * costos[i][columna];
                        ofertaRemanente[i] -= asignacion;
                        demandaRemanente[columna] -= asignacion;
                        break;
                    }
                }
            }
        }

        cout << "Costo total con el metodo de aproximacion de Vogel: " << costoTotal << "\n";
    }

    // Método Húngaro para asignación de mínimo costo
    void metodoHungaro() {
        cout << "Resolviendo usando el metodo Húngaro...\n";

        // Paso 1: Reducción de filas
        for (int i = 0; i < numCentros; i++) {
            int min_val = *min_element(costos[i].begin(), costos[i].end());
            for (int j = 0; j < numPuertos; j++) {
                costos[i][j] -= min_val;
            }
        }

        // Paso 2: Reducción de columnas
        for (int j = 0; j < numPuertos; j++) {
            int min_val = numeric_limits<int>::max();
            for (int i = 0; i < numCentros; i++) {
                min_val = min(min_val, costos[i][j]);
            }
            for (int i = 0; i < numCentros; i++) {
                costos[i][j] -= min_val;
            }
        }

        // Paso 3: Crear marcadores para las filas y columnas
        vector<bool> filaMarcada(numCentros, false), columnaMarcada(numPuertos, false);
        vector<vector<int>> asignaciones(numCentros, vector<int>(numPuertos, 0));

        // Asignar ceros en filas y columnas
        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                if (costos[i][j] == 0 && !filaMarcada[i] && !columnaMarcada[j]) {
                    asignaciones[i][j] = 1; // Asignar esta posición
                    filaMarcada[i] = true;
                    columnaMarcada[j] = true;
                }
            }
        }

        // Imprimir la asignación resultante
        cout << "Asignación resultante:\n";
        for (int i = 0; i < numCentros; i++) {
            for (int j = 0; j < numPuertos; j++) {
                if (asignaciones[i][j] == 1) {
                    cout << "Centro " << i + 1 << " asignado a Puerto " << j + 1 << "\n";
                }
            }
        }
    }
};

int main() {
    Transporte t;

    // Ingresar los datos de oferta, demanda y costos
    t.ingresarDatos();

    // Ejecutar los métodos de asignación
    t.menorCosto();
    t.esquinaNoroeste();
    t.voguel();
    t.metodoHungaro();

    return 0;
}



