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
    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_bucket.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }
}

void bucketSortMultithread(vector<int>& lista, int numBuckets, int numThreads) {
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

    // Salvar a lista ordenada em um arquivo
    FILE *file = fopen("numeros_ordenados_bucket_multithread.txt", "w");
    if (file != NULL) {
        for (const auto& num : lista) {
            fprintf(file, "%d\n", num);
        }
        fclose(file);
    }
}

int main(){
int qntdElementos;
    cout << "Digite o número de elementos: ";
    cin >> qntdElementos;
    
    // Gerar dados uma única vez e fazer cópias para cada teste
    vector<int> dadosOriginais;
    gerarNumerosAleatorios(qntdElementos, dadosOriginais);

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

    // Bucket Sort Sequencial
    cout << "Iniciando o Bucket Sort sequencial..." << endl;
    vector<int> listaBucketSequencial = dadosOriginais; // Cópia dos dados
    inicio = clock();
    int numBuckets; // Número de buckets já definido: 10, 100 e 1000
    cout << "Digite o número de buckets: ";
    cin >> numBuckets;
    bucketSort(listaBucketSequencial, numBuckets);
    fim = clock();
    double tempoBucketSequencial = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Bucket Sort sequencial concluído em " << tempoBucketSequencial << " segundos." << endl;

    // Bucket Sort Multithread
    cout << "Iniciando o Bucket Sort multithread..." << endl;
    vector<int> listaBucketMultithread = dadosOriginais; // Cópia dos dados
    inicio = clock();
    bucketSortMultithread(listaBucketMultithread, numBuckets, numThreads);
    fim = clock();
    double tempoBucketMultithread = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "Bucket Sort multithread concluído em " << tempoBucketMultithread << " segundos." << endl;
    
    // Salvar em um csv o tempo de execução de cada método
    FILE *file = fopen("tempos_execucao.csv", "w");
    if (file != NULL) {
        fprintf(file, "Quantidade de elementos: %d\n", qntdElementos);
        fprintf(file, "Número de threads: %d\n", numThreads);
        fprintf(file, "Número de buckets: %d\n", numBuckets);
        fprintf(file, "=== TEMPOS DE EXECUÇÃO ===\n");
        fprintf(file, "Metodo,Tempo (segundos)\n");
        fprintf(file, "Insertion Sort Sequencial,%.6f\n", tempoSequencial);
        fprintf(file, "Insertion Sort Multithread,%.6f\n", tempoMultithread);
        fprintf(file, "Bucket Sort Sequencial,%.6f\n", tempoBucketSequencial);
        fprintf(file, "Bucket Sort Multithread,%.6f\n", tempoBucketMultithread);
        fclose(file);
        cout << "Tempos de execução salvos em tempos_execucao.csv." << endl;
    } else {
        cout << "Erro ao salvar os tempos de execução." << endl;
    }   

    // Comparação de performance
    cout << "\n=== COMPARAÇÃO ===" << endl;
    cout << "Tempo de ordenação sequencial: " << tempoSequencial << " segundos." << endl;
    cout << "Tempo de ordenação multithread: " << tempoMultithread << " segundos." << endl;
    cout << "Tempo de Bucket Sort sequencial: " << tempoBucketSequencial << " segundos." << endl;
    cout << "Tempo de Bucket Sort multithread: " << tempoBucketMultithread << " segundos." << endl;
    cout << "Melhor método: " 
         << (tempoSequencial < tempoMultithread ? "Sequencial" : "Multithread") 
         << " para Insertion Sort." << endl;
    cout << "Melhor método: "
         << (tempoBucketSequencial < tempoBucketMultithread ? "Sequencial" : "Multithread")
         << " para Bucket Sort." << endl;   

    return 0;
}