#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>
#include <string>
#include <limits>
using namespace std;

class Grafo {
private:
    int numGalaxias;
    vector<vector<double>> matrizDistancias;
    vector<string> nomesGalaxias;
    vector<int> idsOriginais; // Mapeia índice local para ID original no grafo completo

public:
    Grafo(const vector<vector<double>>& matrizCompleta, //Criação de subgrafo baseado na matriz completa e uma lista de ids desejados, por ex: iddesejado = 1,7,8 cria grafo 3x3
          const vector<string>& nomesCompletos,
          const vector<int>&idsDesejados);
          
    double d(int i, int j) const;
    int getNumGalaxias() const;
    string getNome(int i) const;
    int getIdOriginal(int i) const;
};

#endif