#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#define SIZE 53

/*  DEFINICAO DAS ESTRUTURAS  */
//	HashTable
typedef struct NodeHashTable {
    struct DoubleLinkedList *list; // Alterado para a estrutura da DLL
} NodeHashTable;

NodeHashTable *hashTable[SIZE]; // Cria a tabela hash

//	DoubleLinkedList
typedef struct Node {
    char data[50];
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct DoubleLinkedList {
    Node* head;
    Node* tail;
    int size;
} DoubleLinkedList;


/*  DEFINICAO DAS FUNCOES  */

	/*  LIBERAR MEMORIA  */
// Criar funcao para liberar toda a memoria alocada na finalizacao
void freeList(DoubleLinkedList* list) {
    Node* current = list->head;
    Node* nextNode;

    // Percorre todos os nos da lista
    while (current != NULL) {
        nextNode = current->next; // Armazena o pr�ximo no
        free(current); // Libera a mem�ria do no atual
        current = nextNode; // Avanca para o pr�ximo no
    }

    // Libera a memoria da lista em si
    free(list);
}

void freeAll() {
    for (int i = 0; i < SIZE; i++) {
        if (hashTable[i] != NULL) {
            freeList(hashTable[i]->list);
            free(hashTable[i]);
        }
    }
}

	/*  DOUBLE LINKED LIST  */
// Fun��o para criar um novo n�
Node* createNode(const char *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->data, data); // Duplica a string
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Fun��o para inicializar a lista
DoubleLinkedList* createList() {
    DoubleLinkedList *list = (DoubleLinkedList *)malloc(sizeof(DoubleLinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0; // Inicializa o tamanho
    return list;
}

// Fun��o para inserir um novo n� no final da lista
void insertEnd(DoubleLinkedList *list, const char *data) {
    Node *newNode = createNode(data);
    if (list->head == NULL) { // Se a lista estiver vazia
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->prev = list->tail; // O novo n� aponta para o antigo tail
        list->tail->next = newNode;  // O antigo tail aponta para o novo n�
        list->tail = newNode;         // Atualiza o tail
    }
    list->size++; // Incrementa o tamanho
}

// Fun��o para remover um n� da lista
void removeNode(DoubleLinkedList *list, const char *data) {
    Node *current = list->head;
    while (current != NULL) {
        if (strcmp(current->data, data) == 0) {
            if (current->prev != NULL) {
                current->prev->next = current->next; // Ajusta o ponteiro do n� anterior
            } else {
                list->head = current->next; // Atualiza o head se o n� a ser removido for o primeiro
            }
            if (current->next != NULL) {
                current->next->prev = current->prev; // Ajusta o ponteiro do pr�ximo n�
            } else {
                list->tail = current->prev; // Atualiza o tail se o n� a ser removido for o �ltimo
            }
            free(current); // Libera a mem�ria do n�
            return;
        }
        current = current->next;
    }
    printf("O Elemento '%s' nao foi encontrado na lista.\n", data);
}


	/*  QUICK SORT  */
// Fun��o para particionar a lista
Node* partition(Node* low, Node* high) {
    char* pivot = high->data; // Usando o �ltimo elemento como piv�
    Node* i = low->prev; // O �ndice do menor elemento

    for (Node* j = low; j != high; j = j->next) {
        if (strcmp(j->data, pivot) < 0) {
            i = (i == NULL) ? low : i->next;
            char temp[50];
            strcpy(temp, i->data);
            strcpy(i->data, j->data);
            strcpy(j->data, temp);
        }
    }
    i = (i == NULL) ? low : i->next;
    char temp[50];
    strcpy(temp, i->data);
    strcpy(i->data, high->data);
    strcpy(high->data, temp);
    return i;
}

// Fun��o recursiva para implementar o QuickSort
void quickSortRec(Node* low, Node* high) {
    if (high != NULL && low != high && low != high->next) {
        Node* p = partition(low, high);
        quickSortRec(low, p->prev);
        quickSortRec(p->next, high);
    }
}

// Fun��o para executar o QuickSort na lista
void quickSort(DoubleLinkedList* list) {
    Node* tail = list->tail; // Obt�m o tail da lista
    quickSortRec(list->head, tail);
}



	/*  HASHTABLE  */
// Fun��o hash 
int hash(const char *key) {
	//Algoritmo DJB2 -- Por Daniel J. Bernstein
	unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; 
    	//Operador >> desloca o numero x bits para a esquerda
	}

    return hash % SIZE; // Retorna a soma m�dulo SIZE
}

// Adicionar um elemento na HashTable
void insert(char *key) {
    int index = hash(key);
    if (hashTable[index] == NULL) {
        hashTable[index] = (NodeHashTable *)malloc(sizeof(NodeHashTable));
        hashTable[index]->list = createList();
    }
    DoubleLinkedList *list = hashTable[index]->list;
    
    insertEnd(list, key);
}

//Funcao para adicionar nomes a tabela hash
void addListToHashTable(DoubleLinkedList* list){
	Node* current = list->head;
    while (current != NULL) {
    	insert(current->data);
        current = current->next;
    }
    freeList(list);
}

// Fun��o para buscar uma chave na lista
Node* search(DoubleLinkedList *list, const char *key) {
    Node *current = list->head;

    while (current != NULL) {
        if (strcmp(current->data, key) == 0) {
            return current; // Retorna o n� se encontrado
        }
        current = current->next;
    }
    return NULL; // Retorna NULL se n�o encontrado
}

// Modifica��o da fun��o search da tabela hash
void search(const char *key) {
    int index = hash(key);
    if (hashTable[index] != NULL) {
        DoubleLinkedList *list = hashTable[index]->list;
        Node *result = search(list, key);
        if (result != NULL) {
            printf("Elemento '%s' encontrado na tabela hash (index %d).\n", result->data, index);
        } else {
            printf("Elemento '%s' nao encontrado na tabela hash.\n", key);
        }
    } else {
        printf("Elemento '%s' nao encontrado na tabela hash.\n", key);
    }
}

// Fun��o para remover uma chave da lista
void removeKey(const char *key) {
    int index = hash(key);
    if (hashTable[index] != NULL) {
        DoubleLinkedList *list = hashTable[index]->list;
        Node *result = search(list, key);
        if (result != NULL) {
            removeNode(list, key); // Remove o n� da lista
            printf("Elemento '%s' removido da tabela hash (index %d).\n", key, index);
        } else {
            printf("Elemento '%s' nao encontrado na tabela hash, nao pode ser removido.\n", key);
        }
    } else {
        printf("Elemento '%s' nao encontrado na tabela hash, nao pode ser removido.\n", key);
    }
}



	/*  LEITURA  */
// Leitura do arquivo
// Fun��o para ler dados de um arquivo e inserir na HashTable
DoubleLinkedList* readFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }
    
    DoubleLinkedList *list = createList();
    
    printf("Lendo documento %s...\n", filename);
    char buffer[50]; // Ajuste o tamanho conforme necess�rio
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove a nova linha
        insertEnd(list, buffer); // Adiciona � lista
    }

    fclose(file);
    return list;
}



	/*	FUNCOES EXTRAS 	*/
	
//Calcular o desvio-Padrao para analise de distibuicao da tabela
double calcularDesvioPadrao() {
    int totalNodes = 0;
    int totalRecords = 0;
    double mean, variance = 0.0;

    // Contar o n�mero total de registros
    for (int i = 0; i < SIZE; i++) {
        if (hashTable[i] != NULL && hashTable[i]->list != NULL) {
            totalRecords += hashTable[i]->list->size; // Adiciona o tamanho da lista
        }
    }

    // Calcular a m�dia
    if (totalRecords > 0) {
        mean = (double)totalRecords / SIZE; // A m�dia deve ser o total de registros dividido pelo n�mero de �ndices

        // Calcular a vari�ncia
        for (int i = 0; i < SIZE; i++) {
            if (hashTable[i] != NULL && hashTable[i]->list != NULL) {
                double diff = hashTable[i]->list->size - mean;
                variance += diff * diff; // Soma dos quadrados das diferen�as
            }
        }
        variance /= SIZE; // Divide pela quantidade de �ndices
    } else {
        return 0.0; // Se n�o houver registros, a vari�ncia � 0
    }

    return sqrt(variance);
}
	
// Fun��o para imprimir a lista
void printList(DoubleLinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        printf(" %s <->", current->data);
        current = current->next;
    }
    printf(" NULL \n");
}

//Funcao para imprimir a HashTable
void printHashTable(){
    for (int i = 0; i < SIZE; i++) {
    if (hashTable[i] != NULL && hashTable[i]->list != NULL) {
        printf("\nIndex %d: ", i);
        Node* current = hashTable[i]->list->head;
        while (current != NULL) {
            printf("%s <-> ", current->data);
            current = current->next;
        }
        printf("NULL\n");
    } else {
        printf("\nIndex %d: (vazio)\n", i);
    }
    }
}

//Funcao para imprimir as infos da hashTable
void printInfoHashTable(){
	for (int i = 0; i < SIZE; i++) {
	    if (hashTable[i] != NULL && hashTable[i]->list != NULL) {
	        printf("Index %d: %d Registros\n", i, hashTable[i]->list->size);
	    } else {
	        printf("Index %d: (vazio)\n", i);
	    }
    }
    printf("\nDESVIO PADRAO CALCULADO PARA A HASH TABLE: %.2f\n", calcularDesvioPadrao());
}

// Fun��o para imprimir o histograma da HashTable
void printHistogram() {
    printf("\nHistograma da HashTable:\n");
    for (int i = 0; i < SIZE; i++) {
        if (hashTable[i] != NULL && hashTable[i]->list != NULL) {
            int count = hashTable[i]->list->size;
            int scaledCount = count / 100; // Escala: 1 asterisco para cada 100 registros
            
            printf("Index %2d: ", i);
            for (int j = 0; j < scaledCount; j++) {
                printf("*");
            }
            printf(" (%d)\n", count);
        } else {
            printf("Index %2d: (vazio)\n", i);
        }
    }
}

/*  EXECUCAO DA FUNCAO MAIN  */
main() {
    printf("INICIANDO PROGRAMA...\n");
    
    // Ler o arquivo com nomes
    DoubleLinkedList *list = readFromFile("nomes.txt"); // l� e retorna lista
    
    // Ordenar a lista
    quickSort(list);
    
    // Adicionar lista para hashTable
    addListToHashTable(list);
    
    // Imprimir informa��es da HashTable
    printInfoHashTable();
    getch();
    
    for (;;) {
        int op;
        system("cls");

        printf("Digite a opcao desejada:\n");
        printf("1 - Inserir nova Chave\n");
        printf("2 - Buscar Chave\n");
        printf("3 - Remover Chave\n");
        printf("4 - Imprimir Hash Table \n");
        printf("5 - Imprimir informacoes da Hash Table \n");
        printf("6 - Imprimir histograma da HashTable \n");
        printf("0 - Finalizar\n");
        scanf("%i", &op);
        
        if (op == 0) {
            break;
        }
        
        if (op == 1) {
            char buffer[50];
            printf("Digite a chave a ser inserida na HashTable: ");
            scanf("%s", buffer);
            insert(buffer);
            printf("Chave '%s' inserida na HashTable (index %d).\n", buffer, hash(buffer));
            getch(); 
            
        } else if (op == 2) {
            char buffer[50];
            printf("Digite a chave a ser buscada: ");
            scanf("%s", buffer);
            search(buffer); 
            
            getch(); 
            
        } else if (op == 3) {
            char buffer[50];
            printf("Digite a chave a ser removida: ");
            scanf("%s", buffer);
            removeKey(buffer); 
            
            getch(); 
            
        } else if (op == 4) {
            printHashTable();
            getch(); 
            
        } else if (op == 5) {
            printInfoHashTable();
            getch(); 
        }else if(op == 6){
        	printHistogram();
        	getch();
		}else{
			printf("opcao invalida!");
			getch();
		}
    }
    
    printf("FINALIZANDO PROGRAMA...\n");
    freeList(list);
    freeAll();
    
}
