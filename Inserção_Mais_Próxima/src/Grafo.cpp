#include "../include/Grafo.hpp"
using namespace std;

Grafo::Grafo(const vector<vector<double>>& matrizCompleta, const vector<string>& nomesCompletos, const vector<int>& idsDesejados) { //Construtor de subgrafo
    numGalaxias = idsDesejados.size();
    matrizDistancias.resize(numGalaxias, vector<double>(numGalaxias));
    nomesGalaxias.resize(numGalaxias);
    idsOriginais = idsDesejados;

    for (int i = 0; i < numGalaxias; i++) {
        int indiceGlobalI = idsDesejados[i] - 1; //indice na matriz completa, 0-based
        if (indiceGlobalI>= 0 && indiceGlobalI < (int)nomesCompletos.size()) { //copia nome da cidade
            nomesGalaxias[i] = nomesCompletos[indiceGlobalI];
        } else {
            nomesGalaxias[i] = "Desc " + to_string(idsDesejados[i]);
        }
        for (int j = 0; j < numGalaxias; j++) {
            int indiceGlobalJ = idsDesejados[j] - 1;
            if (indiceGlobalI < (int)matrizCompleta.size() && indiceGlobalJ < (int)matrizCompleta.size()) { //faz a cópia dos valores da matriz completa para a submatriz
                matrizDistancias[i][j] = matrizCompleta[indiceGlobalI][indiceGlobalJ];
            } else {
                matrizDistancias[i][j] = numeric_limits<double>::infinity();
            }
        }
    }
}
double Grafo::d(int i, int j) const {
    return matrizDistancias[i][j];
}

int Grafo::getNumGalaxias()const{
    return numGalaxias;
}

string Grafo::getNome(int i) const {
    if (i >= 0 && i < (int)nomesGalaxias.size()) return nomesGalaxias[i];
    return "";
}

int Grafo::getIdOriginal(int i) const {
    if (i >= 0 && i < (int)idsOriginais.size()) return idsOriginais[i];
    return 0;
}