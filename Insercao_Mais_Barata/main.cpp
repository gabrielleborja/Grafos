#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <climits>

using namespace std;

vector<vector<double>> matriz_adjacencia;//matriz de distâncias do grafo(km ou minutos)
vector<string> cidades;//vértices do grafo

// Função responsavel pela leitura das cidades, recebendo o numero de cidades como parametro
void recebe_cidades(int qtd_cidades){
    // ifstream arquivo_cidades("PCV__Matriz_do_problema-Cidades copy.csv");
    ifstream arquivo_cidades("PCV__Matriz_do_problema-Cidades.csv");

    if (!arquivo_cidades.is_open()) {
        cerr << "Erro ao abrir o arquivo com nome das cidades." << endl;
        return;
    }

    //ler as cidades
    string linhas_cidades;
    for (int i = 0; i < qtd_cidades; i++) {
        getline(arquivo_cidades, linhas_cidades);

        string id, nome;
        stringstream ss(linhas_cidades);

        getline(ss, id, ','); 
        getline(ss, nome, ',');

        cidades.push_back(nome); 
    }

    arquivo_cidades.close();
}

// Função para imprimir os nomes das cidades (teste de leitura)
void imprime_cidades(){
    cout << "Cidades:" << endl;
    for (const auto& cidade : cidades) {
        cout << cidade << endl;
    }
}

// Função responsavel pela leitura da matriz de distâncias, recebe o numero da quantidade de cidades
void recebe_matriz(int qtd_cidades){
    // ifstream arquivo_matriz("PCV__Matriz_do_problema - Min.csv");
    ifstream arquivo_matriz("PCV__Matriz_do_problema - Min copy.csv");
    // ifstream arquivo_matriz("PCV__Matriz_do_problema - Km.csv");
    // ifstream arquivo_matriz("PCV__Matriz_do_problema - Km copy.csv");

    if (!arquivo_matriz.is_open()) {
        cerr << "Erro ao abrir o arquivo com a matriz de distancias." << endl;
        return;
    }

    // inicializa matriz 
    matriz_adjacencia.resize(qtd_cidades);
    for (auto& linha : matriz_adjacencia) {
        linha.reserve(qtd_cidades);
    }

    string linha_matriz;

    for (int i = 0; i < qtd_cidades; i++) {
        getline(arquivo_matriz, linha_matriz);

        string distancia;
        stringstream ss(linha_matriz);

        // ler cada valor da linha para inserir na matriz
        for (int j = 0; j < qtd_cidades; j++) {
            getline(ss, distancia, ',');

            // campo vazio = 0
            if (distancia.empty()){
                distancia = "0";
            }

            matriz_adjacencia[i].push_back(stod(distancia));
        }
    }

    arquivo_matriz.close();
}

// Função para imprimir a matriz de distâncias (teste de leitura)
void imprime_matriz(){
    cout << "Matriz de Distancias:" << endl;
    for (const auto& linha : matriz_adjacencia) {
        for (const auto& valor : linha) {
            cout << valor << " ";
        }
        cout << endl;
    }
}

// Função que encontra o vizinho mais próximo (retorna a rota encontrada pelo algoritmo, ids das cidades)
vector<int> vizinho_mais_proximo(int cidade_atual, vector<bool>& visitado, vector<int>& rota, double& custo_total) {
    
    // verifica se a rota é a cidade atual ou está vazia, se sim adiciona a cidade atual na rota
    if (rota.empty() || rota.back() != cidade_atual) {
        cout << "-------------------------------------" << endl;
        cout << "Cidade visantando agora: " << cidades[cidade_atual] << endl;
        cout << "------------------------------------- \n" << endl;
        rota.push_back(cidade_atual);
    }

    visitado[cidade_atual] = true; // marca a cidade atual como visitada
    int proxima_cidade = -1; // variavel para armazenar a proxima cidade
    double menor_distancia =  numeric_limits<double>::max(); // variavel para armazenar a menor distancia

    if (rota.size() == cidades.size()) {
        custo_total += matriz_adjacencia[cidade_atual][0]; // adiciona o custo para voltar a cidade de saida
        rota.push_back(0); // adiciona a cidade de iniciio em busca de fechar o ciclo hamiltoniano
        return rota; // se todas as cidades foram visitadas, retorna a rota
    }

    // enquanto houver cidades nao visitadas, busca a mais proxima (com menor distancia)
    for (int i = 0; i < cidades.size(); i++) {
        if (!visitado[i] && matriz_adjacencia[cidade_atual][i] > 0) { // se a cidade não foi visitada e a distância for maior que 0

            if (matriz_adjacencia[cidade_atual][i] < menor_distancia) { // verifica se a distancia for menor que a menor distancia já encontrada
                menor_distancia = matriz_adjacencia[cidade_atual][i]; // setar a nova menor distancia
                proxima_cidade = i; // marca para ir a proxima cidade

                // cout << "\nDistancia de " << cidades[cidade_atual] << " para " << cidades[proxima_cidade] << ": " << menor_distancia << endl;
                cout << "Distancia para a proxima cidade (" << cidades[proxima_cidade] << "): " << menor_distancia << endl;
            }
        } else { // se a cidade já estiver visitada ou a distancia for 0, só continua o ciclo
            continue;
        }
    }

    custo_total += menor_distancia; // atualiza o custo total da rota
    cout << "Custo parcial da rota: " << custo_total << endl;

    // condição de parada: se todas as cidades foram visitadas, retorna a rota
    if (proxima_cidade == -1) {
        return rota;
    } 

    return vizinho_mais_proximo(proxima_cidade, visitado, rota, custo_total); // chama recursivamente o algoritmo para as próximas cidades
}

// Imprime a rota encontrada
void imprime_rota(const vector<int>& rota) {
    cout << "Rota encontrada:" << endl;
    for (const auto& cidade_index : rota) {
        cout << cidades[cidade_index] << " -> ";
    }
    cout << endl; // volta ao inicio
}

// Função de busca local swap
vector<int> busca_local_swap(vector<int>& rota, double& custo_total) {
    bool melhoria = true;

    // repete até não encontrar mais melhorias
    while (melhoria) {
        melhoria = false;

        // percorre todos os pares de cidades (menos a primeira e última) para tentar melhorar o custo da rota
        for (int i = 1; i < rota.size() - 1; i++) {
            for (int j = i + 1; j < rota.size() - 1; j++) {

                vector<int> rota_temp = rota;

                // Troca as cidades (em pares) da rota atual
                int temp = rota_temp[i];
                rota_temp[i] = rota_temp[j];
                rota_temp[j] = temp;

                // Recalcula o custo total da rota após a troca
                double novo_custo_total = 0.0;
                for (int k = 0; k < rota_temp.size() - 1; k++) {
                    novo_custo_total += matriz_adjacencia[rota_temp[k]][rota_temp[k + 1]];
                }
                novo_custo_total += matriz_adjacencia[rota_temp[rota_temp.size() - 1]][rota_temp[0]];

                // ver se o custo novo é melhor que o curto já existente
                if (novo_custo_total < custo_total) {
                    custo_total = novo_custo_total;
                    cout << "Rota Swap encontrada com novo custo: " << custo_total << endl;

                    melhoria = true;  // marca que uma melhoria foi encontrada
                    rota = rota_temp; // atualiza a rota para a nova rota melhorada
                    break; // sai do loop interno para reiniciar a busca
                }
            }

            if (melhoria) {
                break; // sai do loop externo para reiniciar a busca
            }
        }

    }
    // Se nenhuma troca melhorou o custo, retorna a rota original
    return rota;
}

// Função de busca local insertion
vector<int> busca_local_insertion(vector<int>& rota, double& custo_total){
    bool melhoria = true;

    // repete até não encontrar mais melhorias
    while (melhoria) {
        melhoria = false;

        // percorre todas as cidades (menos a primeira e última) para tentar melhorar o custo da rota
        for (int i = 1; i < rota.size() - 1; i++) {
            vector<int> rota_temp = rota;

            // remove a cidade da posição i
            int cidade_removida = rota_temp[i];
            rota_temp.erase(rota_temp.begin() + i);

            // tenta colocar a cidade removida em outra posição tentando diminuit o custo total da rota
            for (int j = 0; j < rota_temp.size() - 1; j++) {
                // insere a cidade removida na nova posição j
                rota_temp.insert(rota_temp.begin() + j, cidade_removida);

                // faz o calculo novament do custo total da rota após a nova inserção
                double novo_custo_total = 0.0;
                for (int k = 0; k < rota_temp.size() - 1; k++) {
                    novo_custo_total += matriz_adjacencia[rota_temp[k]][rota_temp[k + 1]];
                }
                novo_custo_total += matriz_adjacencia[rota_temp[rota_temp.size() - 1]][rota_temp[0]];

                // ver se o custo novo é melhor que o curto já existente
                if (novo_custo_total < custo_total) {
                    custo_total = novo_custo_total;
                    cout << "Rota Insertion encontrada com novo custo: " << custo_total << endl;
                    melhoria = true;  // marca que uma melhoria foi encontrada

                    rota = rota_temp; // atualiza a rota para a nova rota melhorada
                    break; // sai do loop interno para reiniciar a busca
                    
                } else {
                    // se a rota não for melhor, desfaz a inserção  e mantem a rota já edxistente
                    rota_temp.erase(rota_temp.begin() + j);
                }
            } 
        }
    }
        
    return rota;
}

// Função de busca local 2-opt
vector<int> busca_local_2opt(vector<int>& rota, double& custo_total){
    bool melhoria = true;

    // repete até não encontrar mais melhorias
    while (melhoria) {
        melhoria = false;

        // percorre todos os pares de cidades (menos a primeira e última) para tentar melhorar o custo da rota
        for (int i = 1; i < rota.size() - 2; i++) {
            for (int j = i + 1; j < rota.size() - 1; j++) {

                //cria cópia da rota atual para realizar a inversão
                vector<int> nova_rota = rota;

                //inverte a cópia da rota entre as posições i e j
                reverse(nova_rota.begin() + i, nova_rota.begin() + j + 1);

                // calcula o custo da nova rota encontrada
                double novo_custo_total = 0.0;
                for (int k = 0; k < nova_rota.size() - 1; k++) {
                    novo_custo_total += matriz_adjacencia[nova_rota[k]][nova_rota[k + 1]];
                }
                novo_custo_total += matriz_adjacencia[nova_rota[nova_rota.size() - 1]][nova_rota[0]];

                // ver se o custo novo é melhor que o curto já existente
                if (novo_custo_total < custo_total) {
                    custo_total = novo_custo_total;
                    cout << "Rota 2-opt encontrou o novo custo: " << custo_total << endl;
                    cout << "------------------------------------- \n" << endl;
                    cout << "Cidades fixadas: " << cidades[rota[i]] << " e " << cidades[rota[j]] << endl;

                    melhoria = true;  // marca que uma melhoria foi encontrada
                    rota = nova_rota; // atualiza a rota para a nova rota melhorada
                }
            }
        }
    }

    // se nenhuma melhoria foi encontrada, sai do loop
    return rota;
}

int main() {
    recebe_cidades(7);
    // imprime_cidades();

    recebe_matriz(7);
    // imprime_matriz();

    // variaveis auxiliares
    vector<bool> visitado(cidades.size(), false); // vetor que indica cidades visitadas
    vector<int> rota; // vetor que armazena a rota atual
    rota.push_back(0); // começa na primeira cidade(angicos)

    // algoritmo de vizinho mais próximo
    double custo_total = 0.0;
    vizinho_mais_proximo(0, visitado, rota, custo_total);
    imprime_rota(rota);
    cout << "Custo total da rota: " << custo_total << endl;
    cout << "------------------------------------- \n" << endl;

    // Realiza busca local swap
    rota = busca_local_swap(rota, custo_total);
    imprime_rota(rota);

    // Realiza busca insertion
    // rota = busca_local_insertion(rota, custo_total);
    // imprime_rota(rota);

    // Realiza busca 2-opt
    // rota = busca_local_2opt(rota, custo_total);
    // imprime_rota(rota);

    return 0;
}