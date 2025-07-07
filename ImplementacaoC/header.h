#include <vector>

void insertionSort(vector<int> arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // Move elements of arr[0..i-1], that are greater than key,
        // to one position ahead of their current position
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

//Método para separar os Buckets para o Bucket Sort
vector<vector<int>> separarBuckets(vector<int> arr) {
    vector<vector<int>> buckets;
    //Encontrar maior e menor valor para definir o range
    int max_val = arr[0];
    int min_val = arr[0];
    for (int i = 1; i < arr.size(); i++) {
        if (arr[i] > max_val) max_val = arr[i];
        if (arr[i] < min_val) min_val = arr[i];
    }
    // Calcular o range
    double range = (double)(max_val - min_val) / arr.size();
    // Criar os buckets
    int num_buckets = arr.size();
    buckets.resize(num_buckets);

    // Distribuir os elementos nos buckets
    for (int i = 0; i < arr.size(); i++) {
        int bucket_index = (arr[i] - min_val) / range;
        if (bucket_index >= num_buckets) bucket_index = num_buckets - 1; // Garantir que não exceda o índice
        buckets[bucket_index].push_back(arr[i]);
    }
    
    return buckets
}

void bucketSort(vector<int> arr) {
    vector<vector<int>> buckets = separarBuckets(arr);
    int num_buckets = buckets.size();
    
    // Ordenar cada bucket e concatenar os resultados
    arr.clear();
    for (int i = 0; i < num_buckets; i++) {
        insertionSort(buckets[i], buckets[i].size());
        arr.insert(arr.end(), buckets[i].begin(), buckets[i].end());
    }  
}

// função para abrir o arquivo e ler os números
vector<int> lerNumerosDeArquivo(const string& nome_arquivo) {
    vector<int> numeros;
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return numeros;
    }
    
    int numero;
    while (arquivo >> numero) {
        numeros.push_back(numero);
    }
    
    arquivo.close();
    return numeros;
}

// função para escrever os números ordenados em um arquivo csv contendo quantidade de numeros ordenados, o tempo de execução e o método (Insertion Sort com ou sem bucketsort) e técnica de threading
void criar_arquivo_saida(const string& nome_arquivo, const vector<int>& numeros, double tempo_execucao, const string& metodo, bool multithreading) {
    ofstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao criar o arquivo: " << nome_arquivo << endl;
        return;
    }
    
    arquivo << numeros.size() << "," << tempo_execucao << "," << metodo << "," << (multithreading ? "Sim" : "Não") << "\n";
    
    arquivo.close();
}

// função para executar o algoritmo de ordenação e medir o tempo de execução
void executar_algoritmo(const string& nome_arquivo, const string& metodo, bool multithreading) {
    vector<int> numeros = lerNumerosDeArquivo(nome_arquivo);
    if (numeros.empty()) {
        cerr << "Nenhum número encontrado no arquivo." << endl;
        return;
    }
    
    auto inicio = chrono::high_resolution_clock::now();
    
    if (metodo == "Insertion Sort") {
        insertionSort(numeros, numeros.size());
    } else if (metodo == "Bucket Sort") {
        bucketSort(numeros);
    } else {
        cerr << "Método de ordenação desconhecido: " << metodo << endl;
        return;
    }
    
    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;
    
    string nome_arquivo_saida = "../Arquivos/output/saida_" + metodo + (multithreading ? "_multithreading" : "_singlethreading") + ".csv";
    criar_arquivo_saida(nome_arquivo_saida, numeros, duracao.count(), metodo, multithreading);
}
