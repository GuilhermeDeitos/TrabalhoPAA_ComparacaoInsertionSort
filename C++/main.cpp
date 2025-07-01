#include <stdio.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

void gerarNumerosAleatorios(int v, vector<int>& lista) {
    srand(time(0));
    int menorValor = INT_MAX;
    for(int i=0; i<v; i++){
        lista.push_back(rand () % 100000 + 1);
        // Atualizar o menor valor encontrado
        if(lista[i] < menorValor) {
            menorValor = lista[i];
        }
    }
    // Salvar os números gerados em um arquivo
    FILE *file = fopen("numeros.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }
    cout << "Menor valor gerado: " << menorValor << endl;
}

void insertionSort(vector<int>& lista) {
    int aux = lista.size();
    for(int i = 1; i < aux; i++){
        int chave = lista[i];
        int j = i - 1;
        while(j>=0 && lista[j] > chave){
            lista[j + 1] = lista[j];
            j--;
        }
        lista[j + 1] = chave;
    }  
    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }
}

void insertionSortRange(vector<int>& lista, int start, int end) {
    for(int i = start + 1; i < end; i++){
        int chave = lista[i];
        int j = i - 1;
        while(j >= start && lista[j] > chave){
            lista[j + 1] = lista[j];
            j--;
        }
        lista[j + 1] = chave;
    }
}

// Função auxiliar para fazer merge de dois ranges ordenados
void mergeRanges(vector<int>& lista, int left, int mid, int right) {
    vector<int> temp(right - left);
    int i = left, j = mid, k = 0;
    // Enquanto houver elementos em ambos os ranges
    while(i < mid && j < right) {
        // Comparar os elementos e adicionar o menor ao vetor temporário
        // Se o elemento do primeiro range for menor ou igual ao do segundo
        if(lista[i] <= lista[j]) {
            // Adicionar o elemento do primeiro range
            temp[k++] = lista[i++];
        } else {
            // Caso contrário, adicionar o elemento do segundo range
            temp[k++] = lista[j++];
        }
    }
    
    // Adicionar os elementos restantes do primeiro range, se houver
    while(i < mid) temp[k++] = lista[i++];
    // Adicionar os elementos restantes do segundo range, se houver
    while(j < right) temp[k++] = lista[j++];
    
    // Copiar os elementos do vetor temporário de volta para o vetor original
    for(int i = 0; i < k; i++) {
        lista[left + i] = temp[i];
    }
}

void insertionSortMultithread(vector<int>& lista, int numThreads) {
    int size = lista.size();
    int chunkSize = size / numThreads;
    vector<thread> threads;
    
    // Criar threads para ordenar cada chunk
    for(int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? size : (i + 1) * chunkSize;
        threads.push_back(thread(insertionSortRange, ref(lista), start, end));
    }
    
    // Aguardar todas as threads terminarem
    for(auto& t : threads) {
        t.join();
    }
    
    // Merge dos chunks ordenados 
    for(int gap = chunkSize; gap < size; gap *= 2) {
        for(int i = 0; i < size; i += gap * 2) {
            int mid = min(i + gap, size);
            int end = min(i + gap * 2, size);
            if(mid < end) {
                mergeRanges(lista, i, mid, end);
            }
        }
    }
    
    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_multithread.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }
}

void encontrarMenorValorArquivo(const string& nomeArquivo, int& menorValor) {
    FILE *file = fopen(nomeArquivo.c_str(), "r");
    if (file == NULL) {
        cout << "Erro ao abrir o arquivo." << endl;
        return;
    }
    
    int valor;
    menorValor = INT_MAX; // Inicializa com o maior valor possível

    while(fscanf(file, "%d", &valor) != EOF) {
        if(valor < menorValor) {
            menorValor = valor;
        }
    }
    
    fclose(file);
}

int main(){
    int v;
    cout << "Digite o número de elementos: ";
    cin >> v;
    
    // Gerar dados uma única vez e fazer cópias para cada teste
    vector<int> dadosOriginais;
    gerarNumerosAleatorios(v, dadosOriginais);
    
    // Sequencial
    vector<int> listaSequencial = dadosOriginais; // Cópia dos dados
    cout << "Iniciando a ordenação sequencial..." << endl;
    clock_t inicio = clock();
    insertionSort(listaSequencial);
    clock_t fim = clock();
    double tempoSequencial = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Ordenação sequencial concluída em " << tempoSequencial << " segundos." << endl;
    
    // Multithread
    cout << "Iniciando a ordenação multithread..." << endl;
    int numThreads;
    int maxThreads = thread::hardware_concurrency();
    cout << "Número máximo de threads disponíveis: " << maxThreads << endl;
    cout << "Digite o número de threads a serem usadas: ";
    cin >> numThreads;
    
    vector<int> listaMultithread = dadosOriginais; // Cópia dos dados
    inicio = clock();
    insertionSortMultithread(listaMultithread, numThreads); 
    fim = clock();
    double tempoMultithread = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Ordenação multithread concluída em " << tempoMultithread << " segundos." << endl;
    
    // Comparação de performance
    cout << "\n=== COMPARAÇÃO ===" << endl;
    cout << "Tempo sequencial: " << tempoSequencial << "s" << endl;
    cout << "Tempo multithread: " << tempoMultithread << "s" << endl;
    if(tempoSequencial > 0) {
        double speedup = tempoSequencial / tempoMultithread;
        cout << "Speedup: " << speedup << "x" << endl;
    }

    return 0;
}