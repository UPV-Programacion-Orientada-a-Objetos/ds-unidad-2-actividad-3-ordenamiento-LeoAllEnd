#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "DataSource.h"
#include "SerialSource.h"
#include "FileSource.h"
#include "CircularBuffer.h"

using namespace std;

#define BUFFER_SIZE 1000
#define MAX_CHUNKS 100

void getChunkFilename(int chunkIndex, char* buffer) {
    sprintf(buffer, "chunk_%d.tmp", chunkIndex);
}

int phase1_acquisition(const char* portName) {
    cout << "Iniciando Sistema de Ordenamiento Externo E-Sort..." << endl;
    cout << "Conectando a puerto " << portName << " (Arduino)... ";

    SerialSource* serial = new SerialSource(portName);

    if (!serial->isConnected()) {
        cout << "Error: No se pudo conectar." << endl;
        delete serial;
        return -1;
    }

    cout << "Conectado." << endl;
    cout << "Iniciando Fase 1: Adquisicion de datos..." << endl << endl;

    CircularBuffer buffer(BUFFER_SIZE);
    int chunkCount = 0;
    int totalDataRead = 0;

    while (serial->hasMoreData() && chunkCount < MAX_CHUNKS) {
        int value = serial->getNext();

        if (value == -1) {
            continue;
        }

        cout << "Leyendo -> " << value << endl;
        totalDataRead++;

        if (!buffer.insert(value)) {
            cout << "Buffer lleno. Ordenando internamente..." << endl;

            buffer.sort();

            cout << "Buffer ordenado: [";
            Node* current = buffer.getHead();
            if (current != NULL) {
                Node* start = current;
                bool first = true;
                do {
                    if (!first) cout << ", ";
                    cout << current->data;
                    first = false;
                    current = current->next;
                } while (current != start);
            }
            cout << "]" << endl;

            char filename[50];
            getChunkFilename(chunkCount, filename);

            cout << "Escribiendo " << filename << "... ";

            FILE* chunkFile = fopen(filename, "w");
            if (chunkFile != NULL) {
                current = buffer.getHead();
                if (current != NULL) {
                    Node* start = current;
                    do {
                        fprintf(chunkFile, "%d\n", current->data);
                        current = current->next;
                    } while (current != start);
                }
                fclose(chunkFile);
                cout << "OK." << endl;
            } else {
                cout << "Error al escribir archivo." << endl;
            }

            buffer.clear();
            cout << "Buffer limpiado." << endl << endl;

            chunkCount++;

            buffer.insert(value);
        }

        if (totalDataRead >= BUFFER_SIZE * 3) {
            cout << "(Deteniendo lectura para prueba...)" << endl;
            break;
        }
    }

    if (buffer.getSize() > 0) {
        cout << "Procesando datos restantes..." << endl;
        cout << "Buffer con " << buffer.getSize() << " elementos. Ordenando internamente..." << endl;

        buffer.sort();

        cout << "Buffer ordenado: [";
        Node* current = buffer.getHead();
        if (current != NULL) {
            Node* start = current;
            bool first = true;
            do {
                if (!first) cout << ", ";
                cout << current->data;
                first = false;
                current = current->next;
            } while (current != start);
        }
        cout << "]" << endl;

        char filename[50];
        getChunkFilename(chunkCount, filename);

        cout << "Escribiendo " << filename << "... ";

        FILE* chunkFile = fopen(filename, "w");
        if (chunkFile != NULL) {
            current = buffer.getHead();
            if (current != NULL) {
                Node* start = current;
                do {
                    fprintf(chunkFile, "%d\n", current->data);
                    current = current->next;
                } while (current != start);
            }
            fclose(chunkFile);
            cout << "OK." << endl;
        }

        buffer.clear();
        chunkCount++;
    }

    delete serial;

    cout << endl << "Fase 1 completada. " << chunkCount << " chunks generados." << endl << endl;

    return chunkCount;
}

void phase2_merge(int K) {
    cout << "Iniciando Fase 2: Fusion Externa (K-Way Merge)" << endl;
    cout << "Abriendo " << K << " archivos fuente..." << endl;

    DataSource** sources = new DataSource*[K];
    int* currentValues = new int[K];
    bool* hasData = new bool[K];

    for (int i = 0; i < K; i++) {
        char filename[50];
        getChunkFilename(i, filename);
        sources[i] = new FileSource(filename);

        if (sources[i]->hasMoreData()) {
            currentValues[i] = sources[i]->getNext();
            hasData[i] = sources[i]->hasMoreData();
        } else {
            hasData[i] = false;
        }
    }

    cout << "K=" << K << ". Fusion en progreso..." << endl;

    FILE* outputFile = fopen("output.sorted.txt", "w");

    if (outputFile == NULL) {
        cout << "Error: No se pudo crear archivo de salida." << endl;

        for (int i = 0; i < K; i++) {
            delete sources[i];
        }
        delete[] sources;
        delete[] currentValues;
        delete[] hasData;
        return;
    }

    int mergeCount = 0;
    while (true) {
        int minIndex = -1;
        int minValue = 0;

        for (int i = 0; i < K; i++) {
            if (hasData[i] || (i < K && sources[i]->hasMoreData())) {
                if (!hasData[i] && sources[i]->hasMoreData()) {
                    currentValues[i] = sources[i]->getNext();
                    hasData[i] = true;
                }

                if (hasData[i]) {
                    if (minIndex == -1 || currentValues[i] < minValue) {
                        minIndex = i;
                        minValue = currentValues[i];
                    }
                }
            }
        }

        if (minIndex == -1) {
            break;
        }

        fprintf(outputFile, "%d\n", minValue);

        if (mergeCount % 100 == 0 || mergeCount < 20) {
            cout << " - Escribiendo " << minValue << endl;
        }
        mergeCount++;

        hasData[minIndex] = false;
        if (sources[minIndex]->hasMoreData()) {
            currentValues[minIndex] = sources[minIndex]->getNext();
            hasData[minIndex] = true;
        }
    }

    fclose(outputFile);

    cout << endl << "Fusion completada. Archivo final: output.sorted.txt" << endl;
    cout << "Total de elementos fusionados: " << mergeCount << endl;

    cout << "Liberando memoria... ";
    for (int i = 0; i < K; i++) {
        delete sources[i];
    }
    delete[] sources;
    delete[] currentValues;
    delete[] hasData;
    cout << "Sistema apagado." << endl;
}

int main() {
    #ifdef _WIN32
        const char* portName = "\\\\.\\COM3";
    #else
        const char* portName = "/dev/ttyUSB0";
    #endif

    int chunkCount = phase1_acquisition(portName);

    if (chunkCount <= 0) {
        cout << "Error en Fase 1. Abortando." << endl;
        return 1;
    }

    phase2_merge(chunkCount);

    return 0;
}