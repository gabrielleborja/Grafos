/* PROBLEMA DO CAIXEIRO VIAJANTE (PCV) - Algoritmo Genético

Problema: Qual é o caminho mais curto para passar por n cidades e voltar para a cidade de origem?
Complexidade: Para N cidades existem (N-1)! Rotas
Solução proposta: Algoritmo Genético

PASSO A PASSO DO SLIDE
1. Início - Geração da População Inicial
2. Fitness - Avaliar cada cromossomo da população
3. Nova Geração - Seleção, Cruzamento, Mutação
    3.1. Seleção - Aleatório, Ranking, Roleta, Torneio, Elitismo
    3.2. Cruzamento - 1X, 2X, UX, SX, PMX, OX, CX, ...
    3.3. Mutação - De acordo com uma taxa / probabilidade
4. Renovar - Renova a população
    4.1. Renovação - Ranking, Roleta, Torneio, Elitismo ...
5. Teste - Satisfez? FIM, senão vá para 3

PASSO A PASSO ALGORITMOS ESCOLHIDOS
1. Geração de rotas aleatórias (população)
2. Calculo da distância ou tempo total em cada rota
3. Seleção, Cruzamento, Mutação
    3.1. Torneio entre x indivíduos aleatórios
    3.2. Crossover de ordem (OX) de um ponto único
    3.3. Swap entre indices aleatóios no indivíduo
    3.4. Calculo da distância ou tempo total em cada nova rota
4. Renova a população
    4.1. Elitismo, o melhor indivíduo da geração se mantém para a próxima
    4.2. Geração aleatória do resto da população
5. Repetir passo 3 até atingir x gerações
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

using namespace std;

// constantes
const int NUM_GERACOES = 700;
const int TAM_POPULACAO = 500;
const double TAXA_TORNEIO_SELECAO = 0.01; // 1% da população
const double TAXA_MUTACAO = 0.05; // 5%
const int NUM_EXECUCOES = 20;

// struct para cada indivíduo da população
struct Individuo {
    vector<int> rota;
    double custoTotal;
};

// função para embaralhar um vetor
void embaralharVetor(vector<int>& v){
    static random_device rd;
    static mt19937 gerador(rd());
    shuffle(v.begin(), v.end(), gerador);
}

// função para gerar um inteiro aleatorio entre dois números
int numeroAleatorioEntre(int min, int max){
    static random_device rd;
    static mt19937 gerador(rd());
    uniform_int_distribution<int> distribuicao(min, max);
    return distribuicao(gerador);
}

// função para calcular o valor total de uma rota com base na matriz escolhida no problema, as medidas são distância ou tempo
double calcularCusto(const vector<vector<double>>& matrizPCV, const vector<int>& rota){
    double custo = 0.0;
    for (size_t i = 0; i < rota.size(); i++){
        if (i != rota.size()-1){
            int cidadeA = rota[i]-1;
            int cidadeB = rota[i+1]-1;
            custo += matrizPCV[cidadeA][cidadeB];
        } else {
            // somando custo de retorno a cidade inicial
            int cidadeA = rota[i]-1;
            int cidadeB = rota[0]-1;
            custo += matrizPCV[cidadeA][cidadeB];
        }
    }
    return custo;
}

// função para gerar uma população de individuos de tamanho TAM_POPULACAO, cada um contendo uma rota 100% aleatória de cidades
vector<Individuo> gerarPopulacao(const vector<vector<double>>& matrizPCV, const vector<int>& cidades){
    vector<Individuo> populacao;
    for (size_t i = 0; i < TAM_POPULACAO; i++){
        Individuo individuo;
        vector<int> rota = cidades;
        embaralharVetor(rota);
        individuo.rota = rota;
        individuo.custoTotal = calcularCusto(matrizPCV, rota);
        populacao.push_back(individuo);
    }
    return populacao;
}

// função para imprimir a população em lista
void imprimirPopulacao(const vector<Individuo>& populacao){
    int i = 0;
    for (auto& ind : populacao){
        cout << i++ << "|Rota: ";
        for (int c : ind.rota){
            cout << c << " ";
        }
        cout << "|Distancia: " << ind.custoTotal << endl;
    }
}

// função para selecionar 1 indivíduo por meio de um torneio entre TAXA_TORNEIO_SELECAO individuos, retornando o individuo com a menor distância entre os selecionados
Individuo selecionarIndividuo(const vector<Individuo>& populacao){
    int melhorPosicao = -1;
    double melhorDistancia = 1000000;
    // calcula x% da população para o torneio
    int numParticipantes = max(1, (int)(TAM_POPULACAO*TAXA_TORNEIO_SELECAO));
    for (size_t i = 0; i < numParticipantes; i++){
        int indiceAleatorio = numeroAleatorioEntre(0, TAM_POPULACAO-1);
        if (populacao[indiceAleatorio].custoTotal < melhorDistancia){
            melhorPosicao = indiceAleatorio;
            melhorDistancia = populacao[indiceAleatorio].custoTotal;
        }
    }
    // retorna o indivíduo com menor rota
    return populacao[melhorPosicao];
}

// função para "cruzar" duas rotas da população utilizando o crossover de ordem (OX) de um ponto único e gerar 2 filhos a partir desse ponto
vector<Individuo> cruzarIndividuos(const Individuo& paiA, const Individuo& paiB){
    Individuo filho1, filho2;
    int tamanhoRota = paiA.rota.size();
    // ponto de corte aleatorio na rota
    int pontoDeCorte = numeroAleatorioEntre(1, tamanhoRota);
    vector<bool> visitadosA(49, false), visitadosB(49, false);
    // copia as cidades do paiA a partir do ponto de corte
    for (size_t i = 0; i < tamanhoRota; i++){
        if (i >= pontoDeCorte){
            int cidadeA = paiA.rota[i];
            filho1.rota.push_back(cidadeA);
            visitadosA[cidadeA]=true;
            int cidadeB = paiB.rota[i];
            filho2.rota.push_back(cidadeB);
            visitadosB[cidadeB]=true;
        }
    }
    // preenche o restante com a ordem do paiB sem repetir
    for (size_t i = 0; i < tamanhoRota; i++){
        int cidadeB = paiB.rota[i];
        if (visitadosA[cidadeB] == false){
            filho1.rota.push_back(cidadeB);
        }
        int cidadeA = paiA.rota[i];
        if (visitadosB[cidadeA] == false){
            filho2.rota.push_back(cidadeA);
        }
    }
    // retorna os dois filhos criados
    return {filho1, filho2};
}

// função para criar uma mutação no filho com base na constante TAXA_MUTACAO
void mutacaoIndividuo(Individuo& filho){
    int dado = numeroAleatorioEntre(1, 100);
    if (dado <= (TAXA_MUTACAO * 100)){
        int tamanhoRota = filho.rota.size();
        int posicao1 = numeroAleatorioEntre(0, tamanhoRota-1);
        int posicao2 = numeroAleatorioEntre(0, tamanhoRota-1);
        // garante a traco de duas posições diferentes
        while (posicao1 == posicao2){
            posicao2 = numeroAleatorioEntre(0, tamanhoRota-1);
        }
        // troca duas cidades aleatórias de posição
        swap(filho.rota[posicao1], filho.rota[posicao2]);
    }
}

// função para buscar o melhor individuo daquela população após a etapa 3: seleção, cruzamento e mutação
Individuo melhorIndividuo(const vector<Individuo>& populacao){
    int melhorPosicao = 0;
    double melhorDistancia = populacao[0].custoTotal;
    for (size_t i = 1; i < populacao.size(); i++){
        if (populacao[i].custoTotal < melhorDistancia){
            melhorPosicao = i;
            melhorDistancia = populacao[i].custoTotal;
        }
    }
    return populacao[melhorPosicao];
}

// função do algoritmo genético
Individuo algoritmoGenetico(const vector<vector<double>>& matrizPCV, const vector<int>& cidades){
    // 1. população Inicial
    vector<Individuo> populacao = gerarPopulacao(matrizPCV, cidades);
    Individuo melhorDoMundo;
    melhorDoMundo.custoTotal = 1000000;
    int geracaoAtual = 0;
    while (geracaoAtual < NUM_GERACOES){
        vector<Individuo> novaPopulacao;
        // 4.1 eletismo
        Individuo melhorDaGeracao = melhorIndividuo(populacao);
        novaPopulacao.push_back(melhorDaGeracao);
        // verifica se é o melhor de todas as gerações desta execução
        if (melhorDaGeracao.custoTotal < melhorDoMundo.custoTotal){
            melhorDoMundo = melhorDaGeracao;
        }
        // preenche o resto da população
        while (novaPopulacao.size() < TAM_POPULACAO){
            // 3.1 seleção - torneio
            Individuo pai1 = selecionarIndividuo(populacao);
            Individuo pai2 = selecionarIndividuo(populacao);
            // 3.2 cruzamento - ox
            vector<Individuo> filhos = cruzarIndividuos(pai1, pai2);
            // 3.3 mutação
            mutacaoIndividuo(filhos[0]);
            mutacaoIndividuo(filhos[1]);
            // 2. cálculo da distância ou tempo total em cada rota
            filhos[0].custoTotal = calcularCusto(matrizPCV, filhos[0].rota);
            filhos[1].custoTotal = calcularCusto(matrizPCV, filhos[1].rota);
            // adiciona na nova população
            novaPopulacao.push_back(filhos[0]);
            if (novaPopulacao.size() < TAM_POPULACAO){
                novaPopulacao.push_back(filhos[1]);
            }
        }
        populacao = novaPopulacao;
        geracaoAtual++;
    }
    return melhorDoMundo;
}

void imprimirCidades(vector<int> rota, const vector<string>& nomesCidades){
    // "roda" o vetor para a primeira cidade ser angicos
    auto it = find(rota.begin(), rota.end(), 1);
    if (it != rota.end()){
        rotate(rota.begin(), it, rota.end());
    }
    for (size_t i = 0; i < rota.size(); i++){
        int indiceCidade = rota[i]-1; 
        cout << nomesCidades[indiceCidade];
        if (i < rota.size()-1){
            cout << " -> ";
        }
    }
    cout << " -> " << nomesCidades[rota[0]-1] << endl;
}

// função para executar o algoritmo NUM_EXECUCOES vezes e gerar um relatório, informando o melhor resultado, a média dos resultados e o número de vezes que atingiu o melhor resultado
void executarMuitasVezes(const vector<vector<double>>& matriz, const vector<int>& cidades, const vector<string>& nomesCidades){
    vector<double> resultados;
    double somaResultados = 0.0;
    double melhorDeTodos = 1000000;
    vector<int> melhorRotaDeTodos;
    cout << "--------------------------------------------------" << endl;
    for (int i = 0; i < NUM_EXECUCOES; i++){
        Individuo resultado = algoritmoGenetico(matriz, cidades);
        resultados.push_back(resultado.custoTotal);
        somaResultados += resultado.custoTotal;
        if (resultado.custoTotal < melhorDeTodos){
            melhorDeTodos = resultado.custoTotal;
            melhorRotaDeTodos = resultado.rota;
        }
        cout << "Execucao " << (i+1) << ": " << resultado.custoTotal << endl;
    }
    double media = somaResultados / NUM_EXECUCOES;
    int contagemMelhor = 0;
    for(double r : resultados){
        if (r == melhorDeTodos){ 
            contagemMelhor++;
        }
    }
    cout << "--------------------------------------------------" << endl;
    cout << "Melhor Solucao: " << melhorDeTodos << endl;
    cout << "Media: " << media << endl;
    cout << "Vezes que atingiu a melhor solucao: " << contagemMelhor << endl;
    cout << "Rota: ";
    imprimirCidades(melhorRotaDeTodos, nomesCidades);
    cout << "--------------------------------------------------" << endl;
}

// função para facilitar a criação de um vetor com valores de 1 até n
vector<int> criarVetor(int n) {
    vector<int> v(n);
    iota(v.begin(), v.end(), 1);
    return v;
}

// função para carregar a matriz de dados do PCV
vector<vector<double>> carregarMatriz(const string& nomeArquivo){
    ifstream arquivo(nomeArquivo);
    vector<vector<double>> matriz;
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return matriz;
    }
    string linha;
    while (getline(arquivo, linha)){
        vector<double> linhaValores;
        stringstream ss(linha);
        string valor;
        while (getline(ss, valor, ',')){
            linhaValores.push_back(stod(valor));
        }
        matriz.push_back(linhaValores);
    }
    arquivo.close();
    cout << nomeArquivo << " carregado com sucesso!" << endl;
    return matriz;
}
// função para carregar as cidades do PCV
vector<string> carregarCidades(const string& nomeArquivo){
    ifstream arquivo(nomeArquivo);
    vector<string> cidades;
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return cidades;
    }
    string linha;
    while (getline(arquivo, linha)){
        cidades.push_back(linha);
    }
    arquivo.close();
    cout << nomeArquivo << " carregado com sucesso!" << endl;
    return cidades;
}

void imprimirTabelaProblemas(){
    cout << "\n|================= PROBLEMAS ===================|" << endl;
    cout << "| Problemas   | Medida  | n  | Cidades          |" << endl;
    cout << "|-----------------------------------------------|" << endl;
    cout << "| Problema 1  | Km      | 48 | 1 a 48           |" << endl;
    cout << "| Problema 2  | Min     | 48 | 1 a 48           |" << endl;
    cout << "| Problema 3  | Km      | 36 | 1 a 36           |" << endl;
    cout << "| Problema 4  | Min     | 36 | 1 a 36           |" << endl;
    cout << "| Problema 5  | Km      | 24 | 1 a 24           |" << endl;
    cout << "| Problema 6  | Min     | 24 | 1 a 24           |" << endl;
    cout << "| Problema 7  | Km      | 12 | 1 a 12           |" << endl;
    cout << "| Problema 8  | Min     | 12 | 1 a 12           |" << endl;
    cout << "| Problema 9  | Km      | 7  | 1,7,8,9,10,11,12 |" << endl;
    cout << "| Problema 10 | Min     | 7  | 1,7,8,9,10,11,12 |" << endl;
    cout << "| Problema 11 | Km      | 6  | 1 a 6            |" << endl;
    cout << "| Problema 12 | Min     | 6  | 1 a 6            |" << endl;
    cout << "|-----------------------------------------------|" << endl;
    cout << "| Pressione 0 para sair                         |" << endl;
    cout << "|-----------------------------------------------|" << endl;
    cout << "\nEscolha: ";
}

int main(){
    // carregando dados
    vector<vector<double>> distanciasPCV = carregarMatriz("PCV__Matriz_do_problema - Km.csv");
    vector<vector<double>> minutosPCV = carregarMatriz("PCV__Matriz_do_problema - Min.csv");
    vector<string> cidadesPCV = carregarCidades("PCV__Matriz_do_problema - Cidades.csv");

    int opcao = -1;
    while (opcao != 0) {
        imprimirTabelaProblemas();
        cin >> opcao;

        const vector<vector<double>>* matrizSelecionada = nullptr;
        vector<int> cidades;

        switch (opcao) {
            case 1:
                matrizSelecionada = &distanciasPCV;
                cidades = criarVetor(48);
                break;
            case 2:
                matrizSelecionada = &minutosPCV;
                cidades = criarVetor(48);
                break;
            case 3:
                matrizSelecionada = &distanciasPCV;
                cidades = criarVetor(36);
                break;
            case 4:
                matrizSelecionada = &minutosPCV;
                cidades = criarVetor(36);
                break;
            case 5:
                matrizSelecionada = &distanciasPCV;
                cidades = criarVetor(24);
                break;
            case 6:
                matrizSelecionada = &minutosPCV;
                cidades = criarVetor(24);
                break;
            case 7:
                matrizSelecionada = &distanciasPCV;
                cidades = criarVetor(12);
                break;
            case 8:
                matrizSelecionada = &minutosPCV;
                cidades = criarVetor(12);
                break;
            case 9:  
                matrizSelecionada = &distanciasPCV;  
                cidades = {1, 7, 8, 9, 10, 11, 12};  
                break;
            case 10: 
                matrizSelecionada = &minutosPCV; 
                cidades = {1, 7, 8, 9, 10, 11, 12}; 
                break;
            case 11:
                matrizSelecionada = &distanciasPCV;
                cidades = criarVetor(6);
                break;
            case 12:
                matrizSelecionada = &minutosPCV;
                cidades = criarVetor(6);
                break;
            case 0:
                cout << "Ate a proxima!" << endl;
                break;
            default:
                cout << "Opcao invalida!" << endl;
        }

        if (opcao >= 1 && opcao <= 12) {
            executarMuitasVezes(*matrizSelecionada, cidades, cidadesPCV);
        }
    }

    return 0;
}