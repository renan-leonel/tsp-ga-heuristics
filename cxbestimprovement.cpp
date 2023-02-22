
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <time.h>
#include <fstream>
#include <sys/random.h>
#include <algorithm>
using namespace std;
// Consideracoes inicais:
// 1- A entrada eh recebida via terminal
// 2- Estruturas dinamicas sao usadas apenas em casos especificos. Ex: ler a entrada
// 3- A estrutura principal utilizada eh um array padrao do C de duas dimensoes
//  vetor[numero_de_vertices][2] => vetor[i][0] representa o valor de x na posicao i
//                               => vetor[i][1] representa o valor de y na posicao i
// 4- A execucao eh contralada pela constante TEMPO

int SIZE;
int POPULACAO = 20;
int MAXGERACOES = 100;
float TAXA_MUTACAO = 0.5;
int TENTATIVAS = 2;
int PARADA = 20;

// funcao para calcular a distancia euclidiana entre dois pontos
float pitagoras(float x1, float y1, float x2, float y2)
{
    return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

// Calcula a distancia de um ciclo(caminho)

float custo2(float **v, int *path)
{
    bool visited[SIZE] = {false};
    float d = 0;
    for (int i = 0; i < SIZE - 2; i++)
    {
        if (visited[i])
        {
            return -1;
        }
        else
        {
            visited[i] = true;
        }
        d += pitagoras(v[path[i]][0], v[path[i]][1], v[path[i + 1]][0], v[path[i + 1]][1]);
    }

    return d;
}

// Verifica qual dos movimentos de 2 vertices deve ser feito
float verificador(float **vertex, int a, int b, int size, int *f)
{
    int a2;
    if (a == 0)
    {
        a2 = size - 1;
    }
    else
    {
        a2 = a - 1;
    }
    int b2;
    if (b == size - 1)
    {
        b2 = 0;
    }
    else
    {

        b2 = b + 1;
    }

    if (b2 == a2)
        return 2;

    if (a == 0 and b == size - 1)
    {
        return 1;
    }
    // custo padrao
    float custo1 = pitagoras(vertex[f[a2]][0], vertex[f[a2]][1], vertex[f[a]][0], vertex[f[a]][1]);
    float custo2 = pitagoras(vertex[f[b]][0], vertex[f[b]][1], vertex[f[b2]][0], vertex[f[b2]][1]);
    float total = custo1 + custo2;
    // custo troca
    float custo3 = pitagoras(vertex[f[a]][0], vertex[f[a]][1], vertex[f[b2]][0], vertex[f[b2]][1]);
    float custo4 = pitagoras(vertex[f[a2]][0], vertex[f[a2]][1], vertex[f[b]][0], vertex[f[b]][1]);
    float total2 = custo3 + custo4;
    // se total2 < total == return < 0
    return total2 - total;
}

// Coloca a fatia do vetor[ini...fim] em ordem inversa
void troca_simples(int *tour, int i, int k, int size)
{
    for (; i < k; i++, k--)
    {
        int aux1;
        aux1 = tour[i];

        tour[i] = tour[k];

        tour[k] = aux1;
    }
}

// 2opt implementado com a estratégia de best improvement
void twoOpt(int size, float **v, int *f)
{
    bool improve = true;
    int best_i;
    int best_k;
    float diference;
    float best_diference;
    while (improve)
    {
        improve = false;

        for (int i = 0; i < size; i++)
        {
            // cout<< i <<" -";
            best_diference = 0;

            for (int k = i + 1; k < size; k++)
            {

                diference = verificador(v, i, k, size, f);
                if (diference < best_diference)
                {
                    best_i = i;
                    best_k = k;
                    best_diference = diference;
                }
            }

            if (best_diference < 0)
            {

                troca_simples(f, best_i, best_k, size);
                improve = true;
            }
        }
    }
}

// cria um vetor base para a geracao de cromossomos
vector<int> criarCaminhoInicial()
{
    vector<int> start_population;
    for (int i = 0; i < SIZE; i++)
    {
        start_population.push_back(i);
    }
    return start_population;
}

// cria solucoes aletorias para a primeira populacao
void gerarCromossomos(int **population, vector<int> vertex)
{
    std::srand(unsigned(std::time(0)));
    for (int i = 0; i < POPULACAO; i++)
    {
        vector<int> aux = vertex;
        std::random_shuffle(aux.begin(), aux.end());
        int *x = &aux[0];

        for (int k = 0; k < SIZE; k++)
        {
            population[i][k] = x[k];
        }
    }
}

// calcula o fitness de cada individuo
void calcularFitness(int **cromossomos, float *fitness, float **v)
{
    for (int i = 0; i < POPULACAO; i++)
    {
        float d = custo2(v, cromossomos[i]);
        // cout << fixed << d << endl;
        fitness[i] = (1 / d);
    }
}

// selecao de pais atraves da roleta
void roleta(float *fitness, int *pai1, int *pai2)
{
    float max_fit = 0;
    for (int i = 0; i < POPULACAO; i++)
    {
        // cout << fitness[i] << endl;
        max_fit += fitness[i];
    }

    vector<int> prob;
    for (int i = 0; i < POPULACAO; i++)
    {
        prob.push_back(ceil((fitness[i] / max_fit) * 100.0));
    }

    vector<int> roletaa;
    for (int i = 0; i < prob.size(); i++)
    {
        for (int k = 0; k < prob[i]; k++)
        {
            roletaa.push_back(i);
        }
    }
    int r1 = rand() % prob.size();
    int r2 = rand() % prob.size();

    while (roletaa[r1] == roletaa[r2])
    {
        r2 = rand() % prob.size();
    }

    *pai1 = roletaa[r1];
    *pai2 = roletaa[r2];
}

// mutacao simples de duas posicoes aleatorias
void mutacao(int *alvo)
{
    int r1 = rand() % SIZE;
    int r2 = rand() % SIZE;
    while (r1 == r2)
    {
        r2 = rand() % SIZE;
    }

    int aux = alvo[r1];
    alvo[r1] = alvo[r2];
    alvo[r2] = aux;
}

void cruzamentoCX(int *pai1, int *pai2, int *f1, int *f2)
{

    for (int i = 0; i < SIZE; i++)
    {
        f1[i] = pai2[i];
    }

    int index;
    index = 0;
    do
    {
        auto it = find(pai2, pai2 + SIZE, f1[index]);
        int idx = distance(pai2, it);
        f1[index] = pai1[index];
        index = idx;

    } while (index != 0);

    for (int i = 0; i < SIZE; i++)
    {
        f2[i] = pai1[i];
    }
    index = 0;
    do
    {
        auto it = find(pai1, pai1 + SIZE, f2[index]);
        int idx = distance(pai1, it);
        f1[index] = pai2[index];
        index = idx;

    } while (index != 0);
    // filho1 = f1;
    // filho2 = f2;
}

// procura o pior indivduo da populacao
int piorIndv(float *fitness)
{
    int idx = 0;
    float aux = fitness[0];

    for (int i = 0; i < POPULACAO; i++)
    {
        if (aux > fitness[i])
        {
            aux = fitness[i];
            idx = i;
        }
    }
    return idx;
}

// procura o melhor individuo da populacao
float melhorindividuo(int **population, float **tour)
{
    float c = custo2(tour, population[0]);
    int idx = 0;
    for (int i = 1; i < POPULACAO; i++)
    {
        float c2 = custo2(tour, population[i]);
        if (c2 < c)
        {
            c = c2;
            idx = i;
        }
    }
    cout << fixed << "melho da geracao: " << c << endl;
    // for (int i = 0; i < population[idx].size(); i++)
    // {
    //     cout << population[idx][i] << " => ";
    // }
    // cout << endl;
    return c;
}

int main()
{
    vector<string> vertices;
    vector<pair<float, float>> vertex;

    int tam;
    string entrada;
    int c = 0;
    while (getline(cin, entrada) and entrada != "EOF")
    {
        if (c > 5)
        {
            float x, y;
            int i;

            sscanf(entrada.c_str(), "%d %f %f", &i, &x, &y);
            pair<int, int> p;
            p.first = x;
            p.second = y;
            vertex.push_back(p);
        }
        c += 1;
    };
    tam = vertex.size();
    SIZE = vertex.size();

    float **v = (float **)malloc(tam * sizeof(float *));
    for (int i = 0; i < tam; i++)
    {
        v[i] = (float *)malloc(2 * sizeof(float));
    }

    for (int i = 0; i < tam; i++)
    {
        v[i][0] = vertex[i].first;
        v[i][1] = vertex[i].second;
    }

    int **population = (int **)malloc(POPULACAO * sizeof(int *));
    for (int i = 0; i < tam; i++)
    {
        population[i] = (int *)malloc(tam * sizeof(int));
    }

    vector<int> base = criarCaminhoInicial();
    int *caminhoinicial = &base[0];
    gerarCromossomos(population, base);

    int geracoes = 0;
    float custo_melhor = 0;
    int contador = 0;
    while (geracoes < MAXGERACOES and contador < PARADA)
    {

        cout << "Geracao: " << geracoes << endl;
        geracoes += 1;
        float *fitness = (float *)malloc(POPULACAO * sizeof(float *));
        calcularFitness(population, fitness, v);
        int idx_pai1, idx_pai2;
        roleta(fitness, &idx_pai1, &idx_pai2);
        int *filho1 = (int *)malloc(SIZE * sizeof(int *));
        int *filho2 = (int *)malloc(SIZE * sizeof(int *));
        cruzamentoCX(population[idx_pai1], population[idx_pai2], filho1, filho2);

        float mut = rand() / (double)RAND_MAX;
        if (mut <= TAXA_MUTACAO)
        {
            mutacao(filho1);
            mutacao(filho2);
        }
        int idx = piorIndv(fitness);
        float custof1, custof2, custopior;

        custof1 = custo2(v, filho1);
        custof2 = custo2(v, filho2);
        custopior = custo2(v, population[idx]);

        if (custof1 < custof2)
        {

            twoOpt(SIZE, v, filho1);
            float custo = custo2(v, filho1);
            if (custo < custopior)
            {
                population[idx] = filho1;
            }
        }
        else
        {

            twoOpt(SIZE, v, filho2);
            float custo = custo2(v, filho2);
            if (custo < custopior)
            {
                population[idx] = filho2;
            }
        }

        float melhor = melhorindividuo(population, v);
        if (melhor == custo_melhor)
        {
            contador++;
        }
        custo_melhor = melhor;
    }

    return 0;
}
