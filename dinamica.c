

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MAX_USER 32
#define MAX_DOC 48


typedef enum { NORMAL = 0, URGENTE = 1 } Prioridad_t;
typedef enum { EN_COLA = 0, IMPRIMIENDO = 1, COMPLETADO = 2, CANCELADO = 3 } Estado_t;

typedef struct {
    int id;                   
    char usuario[MAX_USER];   
    char documento[MAX_DOC];  
    int paginas_total;        
    int paginas_restantes;    
    int copias;               
    Prioridad_t prioridad;    
    Estado_t estado;          
    int ms_por_pagina;        
} PrintJob_t;

typedef struct Node_t {
    PrintJob_t job;
    struct Node_t* next;
} Node_t;


typedef struct {
    Node_t* head; // Frente de la cola
    Node_t* tail; // Final de la cola
    int size;     // Cantidad de elementos
} QueueDynamic_t;



void qd_init(QueueDynamic_t* q);
int qd_is_empty(const QueueDynamic_t* q);
int qd_enqueue(QueueDynamic_t* q, PrintJob_t job);
int qd_peek(const QueueDynamic_t* q, PrintJob_t* out);
int qd_dequeue(QueueDynamic_t* q, PrintJob_t* out);
void qd_destroy(QueueDynamic_t* q);
void qd_print(const QueueDynamic_t* q);

void vaciar_buffer();




int main() {
    QueueDynamic_t cola;
    qd_init(&cola); 
    
    int opcion;
    int contador_id = 1; 
    
    do {
        printf("\n=== SIMULADOR DE COLA DINAMICA ===\n");
        printf("1. Agregar trabajo (enqueue)\n");
        printf("2. Ver siguiente trabajo (peek)\n");
        printf("3. Procesar trabajo (dequeue)\n");
        printf("4. Listar cola\n");
        printf("5. Salir\n");
        printf("Selecciona una opcion: ");
        
        if (scanf("%d", &opcion) != 1) {
            vaciar_buffer();
            opcion = 0; 
        }
        vaciar_buffer();

        switch (opcion) {
            case 1: { 
                PrintJob_t nuevo_trabajo;
                nuevo_trabajo.id = contador_id;
                
                printf("Usuario: ");
                scanf("%31s", nuevo_trabajo.usuario);
                vaciar_buffer();
                
                printf("Documento: ");
                scanf("%47s", nuevo_trabajo.documento);
                vaciar_buffer();
                
                printf("Paginas: ");
                scanf("%d", &nuevo_trabajo.paginas_total);
                vaciar_buffer();
                
                nuevo_trabajo.paginas_restantes = nuevo_trabajo.paginas_total;
                nuevo_trabajo.copias = 1;
                nuevo_trabajo.prioridad = NORMAL;
                nuevo_trabajo.estado = EN_COLA;
                nuevo_trabajo.ms_por_pagina = 300;
                
                if (qd_enqueue(&cola, nuevo_trabajo)) {
                    printf("[+] Agregado: id=%d usuario=%s doc=%s pags=%d\n", 
                           nuevo_trabajo.id, nuevo_trabajo.usuario, 
                           nuevo_trabajo.documento, nuevo_trabajo.paginas_total);
                    contador_id++;
                } else {
                    printf("[!] Error: No se pudo asignar memoria para el trabajo.\n");
                }
                break;
            }
            case 2: { 
                PrintJob_t siguiente;
                if (qd_peek(&cola, &siguiente)) {
                    printf("[i] Siguiente en fila: id=%d %s %s %d pags\n", 
                           siguiente.id, siguiente.usuario, 
                           siguiente.documento, siguiente.paginas_total);
                } else {
                    printf("[-] La cola esta vacia.\n");
                }
                break;
            }
            case 3: { 
                PrintJob_t procesado;
                if (qd_dequeue(&cola, &procesado)) {
                    printf("[v] Procesado: id=%d %s %s %d pags\n", 
                           procesado.id, procesado.usuario, 
                           procesado.documento, procesado.paginas_total);
                } else {
                    printf("[-] La cola esta vacia.\n");
                }
                break;
            }
            case 4: { 
                qd_print(&cola);
                break;
            }
            case 5:
                printf("Saliendo y liberando memoria...\n");
                qd_destroy(&cola); 
                break;
            default:
                printf("[!] Opcion no valida.\n");
        }
    } while (opcion != 5);

    return 0;
}




void qd_init(QueueDynamic_t* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

int qd_is_empty(const QueueDynamic_t* q) {
    return q->size == 0; 
}

int qd_enqueue(QueueDynamic_t* q, PrintJob_t job) {
   
    Node_t* new_node = (Node_t*)malloc(sizeof(Node_t));
    if (new_node == NULL) {
        return 0; 
    }
    
    
    new_node->job = job;
    new_node->next = NULL; 
    
    
    if (qd_is_empty(q)) {
        q->head = new_node; 
        q->tail = new_node;
    } else {
        q->tail->next = new_node; 
        q->tail = new_node;       
    }
    
    q->size++;
    return 1;
}

int qd_peek(const QueueDynamic_t* q, PrintJob_t* out) {
    if (qd_is_empty(q)) return 0;
    
    *out = q->head->job;
    return 1;
}

int qd_dequeue(QueueDynamic_t* q, PrintJob_t* out) {
    if (qd_is_empty(q)) return 0;
    
   
    Node_t* temp = q->head;
    
    *out = temp->job;
    
    q->head = q->head->next;
    
    
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    
    free(temp);
    
    q->size--;
    return 1;
}

void qd_destroy(QueueDynamic_t* q) {
    PrintJob_t temp_job;
    int count = 0;
    
    while (!qd_is_empty(q)) {
        qd_dequeue(q, &temp_job);
        count++;
    }
    printf("[i] Se liberaron %d trabajos pendientes en la memoria.\n", count);
}

void qd_print(const QueueDynamic_t* q) {
    if (qd_is_empty(q)) {
        printf("La cola esta vacia.\n");
        return;
    }
    
    printf("--- ESTADO DE LA COLA (%d trabajos pendientes) ---\n", q->size);
    Node_t* current = q->head;
    int pos = 1;
    while (current != NULL) {
        printf("[%d] id=%d %s %s %d pags\n", 
               pos, current->job.id, current->job.usuario, 
               current->job.documento, current->job.paginas_total);
        current = current->next;
        pos++;
    }
    printf("--------------------------------------------------\n");
}

void vaciar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}