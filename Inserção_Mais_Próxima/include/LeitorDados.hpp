#ifndef LEITORDADOS_HPP
#define LEITORDADOS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

class LeitorDados {
private:
    static void processarLinhaGalaxias(string linha, vector<string>& nomes) { // Função auxiliar para processar uma linha do arquivo de galáxias
        linha.erase(remove(linha.begin(), linha.end(), '\r'), linha.end());
        stringstream ss(linha);
        string segmento;
        vector<string> colunas;
        
        while (getline(ss, segmento, ',')) {
            colunas.push_back(segmento);
        }
        if (colunas.size() >= 2) { //Coluna 1: ID, Coluna 2: Nome
            try {
                if (!colunas[0].empty()) {
                    int id = stoi(colunas[0]);
                    if (id >= 1 && id <= 48) nomes[id - 1] = colunas[1];
                }
            } catch (...) {}
        }
        if (colunas.size()>= 4) { 
            try {
                if (!colunas[2].empty()) {
                    int id = stoi(colunas[2]);
                    if (id >= 1 && id <= 48) nomes[id - 1] = colunas[3];
                }
            } catch (...) {}
        }
    }

public:
    static vector<vector<double>> lerMatriz(const string& caminho){
        vector<vector<double>> matriz;
        ifstream arquivo(caminho);
        string linha_matriz;

        if (!arquivo.is_open()) {
            cerr << "ERRO: Não foi possível abrir o arquivo: " << caminho << endl;
            return matriz;
        }
        bool primeiraLinha = true; //Tenta detectar cabeçalho na matriz se começar com letras, porém normalmeente não tem
        
        while (getline(arquivo, linha_matriz)) {
            linha_matriz.erase(remove(linha_matriz.begin(), linha_matriz.end(), '\r'), linha_matriz.end());
            if (linha_matriz.empty()) continue;

            if (primeiraLinha) {
                primeiraLinha = false;
                bool temLetra = false; // Se linha contiver letras, é cabeçalho
                for(char c : linha_matriz) {
                    if (isalpha(c)) { temLetra = true; break; }
                    if (isdigit(c)) break; // Achou número antes de letra, é dado
                }
                if (temLetra) continue; 
            }
            vector<double> linhaDados;
            stringstream ss(linha_matriz);
            string distancia;
            while (getline(ss, distancia, ',')) {
                if (distancia.empty()){
                    linhaDados.push_back(0.0);
                }else {
                    try {
                        linhaDados.push_back(stod(distancia));
                    } catch (...) {
                        linhaDados.push_back(0.0);
                    }
                }
            }
            if (!linhaDados.empty()) {
                matriz.push_back(linhaDados);
            }
        }
        arquivo.close();
        return matriz;
    }
    static vector<string> lernomesGalaxia(const string& caminho) {
        vector<string> nomes(48);
        ifstream arquivo(caminho);
        string linha;

        if (!arquivo.is_open()) {
            cerr << "ERRO: Não foi possivel abrir o arquivo de galáxias: " << caminho << endl;
            return nomes;
        }
        if (getline(arquivo, linha)) { //Lê a primeira linha para verificar se é cabeçalho
            bool ehCabecalho = false;
            string temp = linha;
            temp.erase(remove(temp.begin(), temp.end(), '\r'), temp.end());
            if (!temp.empty() && !isdigit(temp[0])) {
                ehCabecalho = true;
            }

            if (!ehCabecalho) { //Se for diferente de cabeçalho, processa a linha
                processarLinhaGalaxias(linha, nomes);
            }
        }
        while (getline(arquivo, linha)) { //Processa as linhas restantes
            processarLinhaGalaxias(linha, nomes);
        }
        arquivo.close();
        return nomes;
    }
};

#endif