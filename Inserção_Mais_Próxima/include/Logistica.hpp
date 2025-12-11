#ifndef LOGISTICA_HPP
#define LOGISTICA_HPP

#include "Grafo.hpp"
#include <vector>
using namespace std;

struct Rota {
    vector<int> sequencia;
    double custoTotal;
    void calcularCusto(const Grafo& g);
};
enum TipoBusca { FIRST_IMP, BEST_IMP };
enum Movimento { SWAP, SHIFT, TWO_OPT};

class Logistica {
public:
    static Rota Insercao_mais_proxima(const Grafo& G, int s);
    static Rota Busca_Local(Rota S, TipoBusca tipo, const Grafo& G, Movimento mov);
private:
    static Rota aplicarSwap(const Rota& sol, int i, int j);
    static Rota aplicarShift(const Rota& sol, int i, int j);
    static Rota aplicar2Opt(const Rota& sol, int i, int j);
};
#endif