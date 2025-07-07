#include <stdio.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

void gerarNumerosAleatorios(int qntElementos, vector<int>& lista) {
    srand(time(0));
    int menorValor = INT_MAX;
    for(int i=0; i<qntElementos; i++){
        lista.push_back(rand () % 1000000 + 1);
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
    clock_t inicio = clock();
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
    clock_t fim = clock();
    double tempo = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Insertion Sort concluído em " << tempo << " segundos." << endl;
    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }

    FILE *f = fopen("tempos_execucao.csv", "a");
    if (f != NULL) {
        fprintf(f, "Insertion Sort,%.6f\n", tempo);
        fclose(f);
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
    clock_t inicio = clock();
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
    clock_t fim = clock();
    double tempo = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Insertion Sort multithread concluído em " << tempo << " segundos." << endl;

    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_multithread.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }

    FILE *f = fopen("tempos_execucao.csv", "a");
    if (f != NULL) {
        fprintf(f, "Insertion Sort multithread,%.6f\n", tempo);
        fclose(f);
    }
}

void separarBuckets(const vector<int>& lista, vector<vector<int>>& buckets, int numBuckets) {
    int maxValue = *max_element(lista.begin(), lista.end());
    int minValue = *min_element(lista.begin(), lista.end());
    int range = maxValue - minValue + 1;
    int bucketSize = range / numBuckets;

    for(int num : lista) {
        int bucketIndex = (num - minValue) / bucketSize;
        if(bucketIndex >= numBuckets) bucketIndex = numBuckets - 1; // Garantir que não exceda o número de buckets
        buckets[bucketIndex].push_back(num);
    }
}

void bucketSort(vector<int>& lista, int numBuckets) {
    clock_t inicio = clock();
    vector<vector<int>> buckets(numBuckets);
    
    // Separar os números em buckets
    separarBuckets(lista, buckets, numBuckets);
    
    // Ordenar cada bucket usando insertion sort
    for(auto& bucket : buckets) {
        insertionSort(bucket);
    }
    
    // Concatenar os buckets de volta na lista original
    lista.clear();
    for(const auto& bucket : buckets) {
        lista.insert(lista.end(), bucket.begin(), bucket.end());
    }
    clock_t fim = clock();
    double tempo = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Bucket Sort concluído em " << tempo << " segundos." << endl;
    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_bucket.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }

    FILE *f = fopen("tempos_execucao.csv", "a");
    if (f != NULL) {
        fprintf(f, "Bucket Sort,%.6f\n", tempo);
        fclose(f);
    }
}

void bucketSortMultithread(vector<int>& lista, int numBuckets, int numThreads) {
    clock_t inicio = clock();
    vector<vector<int>> buckets(numBuckets);
    
    // Separar os números em buckets
    separarBuckets(lista, buckets, numBuckets);
    
    vector<thread> threads;
    
    // Limitar o número de threads ao valor especificado
    int threadsToUse = min(numThreads, numBuckets);
    int bucketsPerThread = numBuckets / threadsToUse;
    
    // Ordenar buckets usando o número limitado de threads
    for(int t = 0; t < threadsToUse; t++) {
        int startBucket = t * bucketsPerThread;
        int endBucket = (t == threadsToUse - 1) ? numBuckets : (t + 1) * bucketsPerThread;
        
        threads.push_back(thread([&buckets, startBucket, endBucket]() {
            for(int i = startBucket; i < endBucket; i++) {
                insertionSort(buckets[i]);
            }
        }));
    }
    
    // Aguardar todas as threads terminarem
    for(auto& t : threads) {
        t.join();
    }
    
    // Concatenar os buckets de volta na lista original
    lista.clear();
    for(const auto& bucket : buckets) {
        lista.insert(lista.end(), bucket.begin(), bucket.end());
    }
    clock_t fim = clock();
    double tempo = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Bucket Sort multithread concluído em " << tempo << " segundos." << endl;

    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_bucket_multithread.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }

    FILE *f = fopen("tempos_execucao.csv", "a");
    if (f != NULL) {
        fprintf(f, "Bucket Sort multithread,%.6f\n", tempo);
        fclose(f);
    }
}

void lerArquivo(const string& nomeArquivo, vector<int>& lista) {
    // Arrumar para funcionar na estrutura de pastas colocada
}

int main(){
    // Gerar dados uma única vez e fazer cópias para cada teste
    vector<int> dadosOriginais;
    
    /*
 
    AJUSTAR PARA FUNCIONAR E FACILITAR NA HORA DA EXECUÇÃO DOS TESTES
    string nomeArquivo;
    lerArquivo(nomeArquivo, dadosOriginais); 
    
    */
    
    
    // Sequencial
    vector<int> listaSequencial = dadosOriginais; // Cópia dos dados
    cout << "Iniciando a ordenação sequencial..." << endl;
    insertionSort(listaSequencial);
    
    // Multithread
    cout << "Iniciando a ordenação multithread..." << endl;
    int numThreads;
    int maxThreads = thread::hardware_concurrency();
    cout << "Número máximo de threads disponíveis: " << maxThreads << endl;
    cout << "Digite o número de threads a serem usadas: ";
    cin >> numThreads;
    vector<int> listaMultithread = dadosOriginais; // Cópia dos dados
    insertionSortMultithread(listaMultithread, numThreads); 
    
    // Bucket Sort Sequencial
    cout << "Iniciando o Bucket Sort sequencial..." << endl;
    vector<int> listaBucketSequencial = dadosOriginais; // Cópia dos dados
    int numBuckets = 10; // Valor padrão para o número de buckets
    bucketSort(listaBucketSequencial, numBuckets);

    // Bucket Sort Multithread
    cout << "Iniciando o Bucket Sort multithread..." << endl;
    vector<int> listaBucketMultithread = dadosOriginais; // Cópia dos dados
    bucketSortMultithread(listaBucketMultithread, numBuckets, numThreads);
    
    return 0;
}