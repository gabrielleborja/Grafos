#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "include/Grafo.hpp"
#include "include/Logistica.hpp"
#include "include/LeitorDados.hpp"
#include "header_cosmico.cpp"
using namespace std;

struct InstanciaProblema {
    int id;
    string tipo; // Km (Consumo de Matéria Escura) ou Min (Salto Temporal)
    vector<int> setores; // ids das setores (planetas, luas, sistemas) de 1 a 48
};
vector<int> intervalo(int inicio, int fim) { // função para gerar sequência, por ex: 1 a 10
    vector<int> v;
    for(int i=inicio; i<=fim; i++) v.push_back(i);
    return v;
}

int main() {

    imprimirCabecalhoCosmico();

    cout << "\n>>> INICIANDO PROTOCOLO DE NAVEGAÇÃO TRAPPIST-1 v9.8 <<<" << endl;
    cout << ">>> CONECTANDO AO BANCO DE DADOS DA CONFEDERAÇÃO GALÁCTICA..." << endl; //Aqui vai chamar o LeitorDados que eu criei e ler os arquivos CSV

    cout << "\n[SISTEMA] Carregando Cartas Estelares de Consumo (Combustivel)..." << endl;
    auto matrizKm = LeitorDados::lerMatriz("PCV__Matriz_do_problema - Km.csv");
    cout << "   -> Dados de Consumo carregados. Matriz de " << matrizKm.size()<< " setores." << endl;

    cout << "[SISTEMA] Carregando Cartas de Dobra Espacial (Tempo)..." << endl;
    auto matrizMin = LeitorDados::lerMatriz("PCV__Matriz_do_problema - Min.csv");
    cout << "   -> Dados de Tempo carregados. Matriz de " << matrizMin.size() << " setores." << endl;

    cout << "[SISTEMA] Identificando Sistemas Planetarios..." << endl;
    auto nomesGalaxia = LeitorDados::lernomesGalaxia("PCV__Matriz_do_problema-Galaxia.csv");
    cout << "   -> Catalogo Universal carregado. " << nomesGalaxia.size() << " mundos identificados." << endl;

    if (matrizKm.size() < 47 || matrizMin.size() < 47) {
        cerr << "ERRO: Matrizes lidas estão incompletas e/ou vazias. Cartas Estelares corrompidas. Abortar missão..." << endl;
        return 1;
    }
    vector<InstanciaProblema> problemas; //define as instancias a serem resolvidas (problemas)
    // Bloco 01 - Km
    problemas.push_back({1, "Km", intervalo(1, 48)});    // Inst 1: 48 Corpos do universo
    problemas.push_back({2, "Km", intervalo(1, 36)});   // Inst 2: 36 Corpos do universo
    problemas.push_back({3, "Km", intervalo(1, 24)});   // Inst 3: 24 Corpos do universo
    problemas.push_back({4, "Km", intervalo(1, 12)});   // Inst 4: 12 Corpos do universo
    problemas.push_back({5, "Km", {1, 7, 8, 9, 10, 11, 12}}); //Inst 5: 7 setores aleatórias
    problemas.push_back({6, "Km", intervalo(1, 6)});   // Inst 5: 6 Corpos do universo
    // Bloco 02 - Min
    problemas.push_back({7, "Min", intervalo(1, 48)});    // Inst 7: 48 Corpos do universo
    problemas.push_back({8, "Min", intervalo(1, 36)});   // Inst 8: 36 Corpos do universo
    problemas.push_back({9, "Min", intervalo(1, 24)});   // Inst 9: 24 Corpos do universo
    problemas.push_back({10, "Min", intervalo(1, 12)});  // Inst 10: 12 Corpos do universo
    problemas.push_back({11, "Min", {1, 7, 8, 9, 10, 11, 12}}); //Inst 11: 7 setores aleatórias
    problemas.push_back({12, "Min", intervalo(1, 6)});  // Inst 12: 6 Corpos do universo
    
    cout << "\n=====================================================================================================" << endl;
    cout << "                           DIÁRIO DE BORDO - MISSÕES DA FROTA ESTELAR CAIXEIRO VIAJANTE" << endl;
    cout << "=====================================================================================================\n" << endl;
    cout << left 
              << setw(5) << "Missão | " 
              << setw(6) << "Recurso | "
              << setw(4) << "Alvos | "
              << setw(12) << "HIMP |" 
              << setw(12) << "BL(SWAP) |"
              << setw(12) << "BL(Shift) | "
              << setw(12) << "BL(2-Opt) | "
              << "Trajetória otimizada (Sistema TRAPPIST)" << endl;
    cout << "-----------------------------------------------------------------------------------------------------" << endl;

    for (const auto& prob : problemas) {
        const auto& matrizRef = (prob.tipo == "Km") ? matrizKm : matrizMin; //seleciona a matriz correta de acordo com o string tipo
        Grafo g(matrizRef, nomesGalaxia, prob.setores); //criando um subgrafo com os setores desejadas

        Rota solInicial = Logistica::Insercao_mais_proxima(g, 0); //A partir de logística, chama o algoritmo de inserção mais próxima para obter a solução inicial
        Rota solSwap = Logistica::Busca_Local(solInicial, BEST_IMP, g, SWAP); //Chama a busca local SWAP
        Rota solShift = Logistica::Busca_Local(solInicial, BEST_IMP, g, SHIFT); //Chama a busca local Shift
        Rota sol2Opt = Logistica::Busca_Local(solInicial, BEST_IMP, g, TWO_OPT);//Chama a busca local 2-Opt
        Rota* melhorSol = &sol2Opt;
        if (solSwap.custoTotal < melhorSol->custoTotal) melhorSol = &solSwap;
        if (solShift.custoTotal < melhorSol->custoTotal) melhorSol = &solShift;
        cout << left 
                  << setw(5) << prob.id 
                  << setw(6) << prob.tipo
                  << setw(4) << prob.setores.size()
                  << setw(15) << fixed << setprecision(2) << solInicial.custoTotal
                  << setw(15) << solSwap.custoTotal
                  << setw(15) << solShift.custoTotal
                  << setw(15) << sol2Opt.custoTotal << "[ ";      
        // Imprime a rota com os nomes das setores
        for (size_t i = 0; i < melhorSol->sequencia.size(); i++) {
            string nome = g.getNome(melhorSol->sequencia[i]);
            if (nome.empty()) { //Caso dê vazio, imprime o id original para saber que existe a cidade alí
                cout << g.getIdOriginal(melhorSol->sequencia[i]);
            } else {
                cout << nome;
            }
            if (i < melhorSol->sequencia.size() - 1) {
                cout << " -> ";
            }
        }
        cout << " ]" << endl;
    }
    cout << "\n=====================================================================================================" << endl;
    cout << "Legenda: HIMP = Heurística de Inserção Mais Próxima | BL = Busca Local" << endl;

    return 0;
}