#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// struct dos nos da lista encadeada de frequencias e dos nos da arvore
typedef struct tlist {
    char caracter;
    int frequencia;
    struct tlist *esquerda, *direita, *next;
} No;

// struct da lista de frequencias e dos nos da arvore
typedef struct lista {
    No *first;
    int tamanho;
} Lista;

// struct para construir a tabela de codigos
typedef struct tupla {
    char caracter;
    char codigo[100]; 
} Codigo;

// funcao para iniciar a lista
void iniciaLista(Lista *lista) {
    lista->first = NULL;
    lista->tamanho = 0;
}

// funcao para inserir nos na lista de frequencias
void inserirNo(Lista *lista, char caracter) {
    No *aux;
    No *novoNo = malloc(sizeof(No));
    novoNo->caracter = caracter;
    novoNo->frequencia = 1;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    novoNo->next = NULL;

    // insere o no no inicio da list se ela estiver vazia
    if (lista->first == NULL) {
        lista->first = novoNo;
    } else {
        // se a lista nao estiver vazia insere no final
        aux = lista->first;
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = novoNo;
    }
    lista->tamanho++;
}

// funcao para excluir da lista os nos da lista de frequencia e montar a arvore de huffman
No* removerNo(Lista *lista) {
    No *aux = NULL;

    if (lista->first) {
        aux = lista->first;
        lista->first = aux->next;
        aux->next = NULL;
        lista->tamanho--;
    }

    return aux;
}

// funcao para contar as frequencias dos caracteres
void contarFrequencias(Lista *lista, char *texto) {
    No *aux;
    // percorre o texto
    for (int i = 0; i < strlen(texto); i++) {
        // flag para indicar se e a primeira vez do caracter
        // se nao for a primeira vez, incrementa a frequencia
        int flag = 0;
        for (aux = lista->first; aux != NULL; aux = aux->next) {
            if (texto[i] == aux->caracter) {
                aux->frequencia++;
                flag = 1;
                break;
            }
        }
        // se for a primeira vez, insere novo no com o caractere
        if (!flag)
            inserirNo(lista, texto[i]);
    }
}


// funcao bubblesort para ordenar a lista encadeada
void bubbleSort(Lista *lista) {
    if (lista->first == NULL) 
        return;

    int trocado;
    No *aux;
    No *prev = NULL;

    do {
        trocado = 0;
        aux = lista->first;

        while (aux->next != prev) {
            if (aux->frequencia > aux->next->frequencia || (aux->frequencia == aux->next->frequencia && aux->caracter > aux->next->caracter)) {
                // troca os nos
                char tempCaracter = aux->caracter;
                int tempFrequencia = aux->frequencia;

                aux->caracter = aux->next->caracter;
                aux->frequencia = aux->next->frequencia;

                aux->next->caracter = tempCaracter;
                aux->next->frequencia = tempFrequencia;

                trocado = 1;
            }
            aux = aux->next;
        }
        prev = aux;
    } while (trocado);
}

// funcao para inserir novo no na arvore, na ordem correta
void inserirNoArvore(Lista *lista, No *no) {
    No *aux, *prev;

    // se a lista estiver vazia insere o no como primeiro
    if (lista->first == NULL) {
        lista->first = no;
    } else {
        aux = lista->first;
        prev = NULL;

        // encontra a posicao correta para inserir o no
        while (aux != NULL && (aux->frequencia < no->frequencia ||
               (aux->frequencia == no->frequencia && aux->caracter < no->caracter))) {
            prev = aux;
            aux = aux->next;
        }

        // insere no comeoo
        if (prev == NULL) {  
            no->next = lista->first;
            lista->first = no;
        } 
        else {  
            prev->next = no;
            no->next = aux;
        }
    }

    lista->tamanho++;
}


// funcao para criar a arvore de huffman 
No* criarArvore(Lista *lista) {
    while (lista->tamanho > 1) {
        No *primeiro = removerNo(lista);
        No *segundo = removerNo(lista);
        No *novo = malloc(sizeof(No));

        if (novo) {
            novo->caracter = '\0';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esquerda = primeiro;
            novo->direita = segundo;
            novo->next = NULL;
            inserirNoArvore(lista, novo);
        } else {
            printf("\nErro ao alocar memoria na criacao da arvore de huffman\n");
            break;
        }
    }
    return lista->first;
}


// funcao para calcular a altura da arvore
int alturaArvore(No *raiz) {
    int esquerda, direita;

    if (raiz == NULL)
        return 0;

    esquerda = alturaArvore(raiz->esquerda) + 1;
    direita = alturaArvore(raiz->direita) + 1;

    if (esquerda > direita) {
        return esquerda;
    }
    else {
        return direita;
    }
}

// funcao para criar a tabela de codigos a partir da lista de frequencias 
// preenche com os caracteres da lista e o codigo com string vazia
void criarTabelaChar(Lista *listaFrequencias, Codigo *tabela, int tamanho) {
    No *aux = listaFrequencias->first;

    for (int i = 0; i < tamanho; i++) {
        tabela[i].caracter = aux->caracter;
        strcpy(tabela[i].codigo, ""); 
        aux = aux->next;
    }
}

// funcao para encontrar o indice de um caracter na tabela de codigos
int encontrarIndice(Codigo *tabela, int tamanho, char caracter) {
    for (int i = 0; i < tamanho; i++) {
        if (tabela[i].caracter == caracter) {
            return i;
        }
    }
    return -1;   
}

void imprimirTabelaCodigos(Codigo *tabela, int tamanho) {
    for (int i = tamanho-1; i >=0; i--) {
        printf("Caracter: %c Codigo: %s\n", tabela[i].caracter, tabela[i].codigo);
    }
}

// funcao para preencher a tabela de codigos
void preencheTabelaCodigos(Codigo *tabelaCodigos, No *raiz, char *caminho, int profundidade) {

    if (raiz == NULL)
        return;

    // quando chegar em uma folha, adiciona o codigo do caracter na tabela de codigos
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        int indice = encontrarIndice(tabelaCodigos, profundidade, raiz->caracter);

        if (indice == -1) {
            printf("Erro ao encontrar o indice do caracter %c\n", raiz->caracter);
            return;
        }

        strcpy(tabelaCodigos[indice].codigo, caminho);
    } 
    // enquanto nao chegar em uma folha, percorre a arvore e armazena o caminho em uma string
    else {
        if (raiz->esquerda != NULL) {
            char caminhoEsquerda[100];
            strcpy(caminhoEsquerda, caminho);
            strcat(caminhoEsquerda, "0");
            preencheTabelaCodigos(tabelaCodigos, raiz->esquerda, caminhoEsquerda, profundidade);
        }
        if (raiz->direita != NULL) {
            char caminhoDireita[100];
            strcpy(caminhoDireita, caminho);
            strcat(caminhoDireita, "1");
            preencheTabelaCodigos(tabelaCodigos, raiz->direita, caminhoDireita, profundidade);
        }
    }
}

// funcao para codificar o texto usando a tabela de codigos
void codificar(Codigo *tabela, int tamanho, char *texto, char *textoCodificado) {
    textoCodificado[0] = '\0'; 
    // percorre o texto original, procura cada caracter na tabela de codigos e concatena o codigo correspondente na strind codificada
    for (int i = 0; i < strlen(texto); i++) {
        int indice = encontrarIndice(tabela, tamanho, texto[i]);

        if (indice == -1) {
            printf("Erro ao encontrar o indice do caracter %c\n", texto[i]);
            return;
        }

        strcat(textoCodificado, tabela[indice].codigo);
    }
}

// funcao para decodificar o texto codificado usando a arvore de huffman
void decodificar(No *raiz, char *textoCodificado, char *textoDecodificado) {
    No *aux = raiz;
    int posicao = 0;

    //percorre o texto codificado e percorre a arvore de huffman
    for (int i = 0; i < strlen(textoCodificado); i++) {
        if (textoCodificado[i] == '0') {
            aux = aux->esquerda;
        } else if (textoCodificado[i] == '1') {
            aux = aux->direita;
        }

        // quando chegar na folha, adiciona o caracter na string do texto decodificado
        if (aux->esquerda == NULL && aux->direita == NULL) {
            textoDecodificado[posicao++] = aux->caracter;
            aux = raiz;
        }
    }
    textoDecodificado[posicao] = '\0';
}


// funcao para limpar a arvore
void limparArvore(No *raiz) {

    if (raiz == NULL)
        return;

    limparArvore(raiz->esquerda);
    limparArvore(raiz->direita);
    free(raiz);
}


int main() {

    // ler o texto do arquivo txt
    FILE *amostra = fopen("amostra.txt", "r");
    if (!amostra) {
        printf("Erro ao abrir o arquivo amostra.txt\n");
        return 1;
    }

    fseek(amostra, 0, SEEK_END);
    long tamanhoArquivo = ftell(amostra);
    fseek(amostra, 0, SEEK_SET);

    char *texto = malloc(tamanhoArquivo + 1);
    if (!texto) {
        printf("Erro ao alocar memoria para o texto\n");
        fclose(amostra);
        return 1;
    }

    fread(texto, 1, tamanhoArquivo, amostra);
    texto[tamanhoArquivo] = '\0';
    fclose(amostra);    
    
    // contar frequencia de cada caractere
    Lista listaFrequencias;
    iniciaLista(&listaFrequencias);
    contarFrequencias(&listaFrequencias, texto);

    // ordenar a lista de frequencias
    bubbleSort(&listaFrequencias);
    
    // iniciar preenchimento da tabela de codigos
    int tamanhoTabela = listaFrequencias.tamanho;
    Codigo codigos[tamanhoTabela];
    criarTabelaChar(&listaFrequencias, codigos, tamanhoTabela);
    
    // montar a arvore de huffman
    No *arvore = criarArvore(&listaFrequencias);

    // preencher a tabela de codigos
    char caminho[100] = "";
    preencheTabelaCodigos(codigos, arvore, caminho, tamanhoTabela);
    // imprimirTabelaCodigos(codigos, tamanhoTabela);
    
    // codificar o texto original
    char textoCodificado[1000];
    codificar(codigos, tamanhoTabela, texto, textoCodificado);
    // printf("\nTexto codificado: %s\n", textoCodificado);

    // decodificar o texto codificado
    char textoDecodificado[1000];
    decodificar(arvore, textoCodificado, textoDecodificado);
    // printf("\nTexto decodificado: %s\n", textoDecodificado);
    
    // salvando o texto codificado em um arquivo
    FILE *codificadoFile = fopen("codificado.txt", "w");
        if(!codificadoFile){
            printf("Erro ao abrir o arquivo codificado.txt\n");
        return 1;
    }
    fprintf(codificadoFile, "%s", textoCodificado);
    fclose(codificadoFile);

    // salvando o texto decodificado em um arquivo
    FILE *decodificadoFile = fopen("decodificado.txt", "w");
    if(!decodificadoFile){
        printf("Erro ao abrir o arquivo decodificado.txt\n");
        return 1;
    }
    fprintf(decodificadoFile, "%s\n", textoDecodificado);
    fclose(decodificadoFile);

    // liberar memoria
    limparArvore(arvore);

    return 0;
}
