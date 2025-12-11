#include "../include/Logistica.hpp"
#include <algorithm>
#include <limits>
using namespace std;

void Rota::calcularCusto(const Grafo& g){
    custoTotal = 0.0;
    for (size_t i = 0; i < sequencia.size() - 1; i++) {
        custoTotal += g.d(sequencia[i], sequencia[i+1]);
    }
}
Rota Logistica::Insercao_mais_proxima(const Grafo& G, int s) {
    int N = G.getNumGalaxias();
    vector<bool> inserido(N, false);
    int v0 = -1; //v0 é o primeiro vértice a ser inserido após s
    double min_dist = numeric_limits<double>::max();
    for(int i=0; i<N; i++) {
        if(i != s && G.d(s, i) < min_dist){
            min_dist = G.d(s, i);
            v0 = i;
        }
    }
    Rota rota;
    rota.sequencia = {s, v0, s};
    inserido[s] = true;
    inserido[v0] = true;
    int numInseridos = 2;
    while(numInseridos < N) {
        int r_asterisco = -1; //Primeiro escolher o r* mais próximo ao ciclo
        double min_dist_r = numeric_limits<double>::max();

        for(int r=0; r<N; r++) {
            if(!inserido[r]) {
                double dist_r_ciclo = numeric_limits<double>::max();
                for(int c : rota.sequencia) {
                     double d = G.d(r, c); //evita ficar fazendo comparações desnecessárias
                     if(d < dist_r_ciclo) dist_r_ciclo = d;
                }
                if(dist_r_ciclo < min_dist_r) {
                    min_dist_r = dist_r_ciclo;
                    r_asterisco = r;
                }
            }
        }
        if(r_asterisco== -1) break;
        double melhor_custo_add = numeric_limits<double>::max(); //Melhor posição para inserir r*
        int melhor_pos = -1;
        for(size_t i=0;i < rota.sequencia.size() - 1; i++){
            int u = rota.sequencia[i];
            int v = rota.sequencia[i+1];
            double aumento = G.d(u, r_asterisco) + G.d(r_asterisco, v) - G.d(u, v);
            
            if(aumento < melhor_custo_add) {
                melhor_custo_add = aumento;
                melhor_pos = i + 1;
            }
        }
        rota.sequencia.insert(rota.sequencia.begin() + melhor_pos, r_asterisco);
        inserido[r_asterisco] = true;
        numInseridos++;
    }
    rota.calcularCusto(G);
    return rota;
}
Rota Logistica::aplicarSwap(const Rota& sol, int i, int j){
    Rota nova = sol;
    swap(nova.sequencia[i], nova.sequencia[j]);
    return nova;
}

Rota Logistica::aplicarShift(const Rota& sol, int i, int j) {
    Rota nova = sol;
    int val = nova.sequencia[i];
    nova.sequencia.erase(nova.sequencia.begin() + i);
    nova.sequencia.insert(nova.sequencia.begin() + j, val);
    return nova;
}

Rota Logistica::aplicar2Opt(const Rota& sol, int i, int j) {
    Rota nova = sol;
    reverse(nova.sequencia.begin() +i, nova.sequencia.begin() + j + 1);
    return nova;
}

Rota Logistica::Busca_Local(Rota S, TipoBusca tipo, const Grafo& G, Movimento mov) {
    bool Melhoria = true;
    S.calcularCusto(G);
    Rota S_best = S;

    while(Melhoria){
        Melhoria = false;
        int n = S.sequencia.size();
        for(int i=1; i < n-2;i++) { //Loop genérico que varre a vizinhança, evitamos mexer no início e fim e operamos nos índices 1 até n-2
            for(int j=i+1; j < n-1; j++) {
                
                Rota S_linha;
                if(mov == SWAP) S_linha = aplicarSwap(S, i, j);
                else if(mov == SHIFT) S_linha = aplicarShift(S, i, j);
                else if(mov == TWO_OPT) S_linha = aplicar2Opt(S, i, j);

                S_linha.calcularCusto(G);

                if(tipo == FIRST_IMP) {
                    if(S_linha.custoTotal < S.custoTotal - 1e-9) {
                        S = S_linha;
                        Melhoria = true;
                        goto reinicio; 
                    }
                } else { //Melhor melhoria
                    if(S_linha.custoTotal < S_best.custoTotal - 1e-9) {
                        S_best = S_linha;
                        Melhoria = true; // Marca que houve melhoria para atualizar S no final
                    }
                }
            }
        }
        if(tipo == BEST_IMP&& Melhoria) {
            S = S_best;
        }
        reinicio:;
    }
    return S;
}