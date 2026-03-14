

#include <stdio.h>
#include <string.h>

#define MAX_JOBS 10
#define MAX_USER 32
#define MAX_DOC 48



typedef enum { NORMAL = 0, URGENTE = 1 } Prioridad_t;
typedef enum { EN_COLA = 0, IMPRIMIENDO = 1, COMPLETADO = 2, CANCELADO = 3 } Estado_t;

typedef struct {
    int id;                   // Autoincremental
    char usuario[MAX_USER];   // Quien imprime
    char documento[MAX_DOC];  // Nombre del documento
    int paginas_total;        // Total de paginas
    int paginas_restantes;    // Para simular progreso
    int copias;               // Opcional (>=1)
    Prioridad_t prioridad;    // normal/urgente
    Estado_t estado;          // en cola/imprimiendo/...
    int ms_por_pagina;        // Opcional para simulacion
} PrintJob_t;

typedef struct {
    PrintJob_t data[MAX_JOBS]; // Arreglo fijo
    int size;                  // Cantidad actual de elementos
} QueueStatic_t;




void qs_init(QueueStatic_t* q);
int qs_is_empty(const QueueStatic_t* q);
int qs_is_full(const QueueStatic_t* q);
int qs_enqueue(QueueStatic_t* q, PrintJob_t job);
int qs_peek(const QueueStatic_t* q, PrintJob_t* out);
int qs_dequeue(QueueStatic_t* q, PrintJob_t* out);
void qs_print(const QueueStatic_t* q);


void vaciar_buffer();




int main() {
    QueueStatic_t cola;
    qs_init(&cola);
    
    int opcion;
    int contador_id = 1; 
    
    do {
        printf("\n=== SIMULADOR DE COLA DE IMPRESION ===\n");
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
                if (qs_is_full(&cola)) {
                    printf("[!] Error: La cola de impresion esta llena (Max %d).\n", MAX_JOBS);
                    break;
                }
                
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
                
                if (qs_enqueue(&cola, nuevo_trabajo)) {
                    printf("[+] Agregado: id=%d usuario=%s doc=%s pags=%d\n", 
                           nuevo_trabajo.id, nuevo_trabajo.usuario, 
                           nuevo_trabajo.documento, nuevo_trabajo.paginas_total);
                    contador_id++;
                }
                break;
            }
            case 2: { 
                PrintJob_t siguiente;
                if (qs_peek(&cola, &siguiente)) {
                    printf("[i] Siguiente en fila: id=%d %s %s %d pags\n", 
                           siguiente.id, siguiente.usuario, 
                           siguiente.documento, siguiente.paginas_total);
                } else {
                    printf("[-] La cola esta vacia, no hay siguiente trabajo.\n");
                }
                break;
            }
            case 3: { 
                PrintJob_t procesado;
                if (qs_dequeue(&cola, &procesado)) {
                    printf("[v] Procesado: id=%d %s %s %d pags\n", 
                           procesado.id, procesado.usuario, 
                           procesado.documento, procesado.paginas_total);
                } else {
                    printf("[-] La cola esta vacia, no hay trabajos por procesar.\n");
                }
                break;
            }
            case 4: { 
                qs_print(&cola);
                break;
            }
            case 5:
                printf("Saliendo del simulador...\n");
                break;
            default:
                printf("[!] Opcion no valida. Intenta de nuevo.\n");
        }
    } while (opcion != 5);

    return 0;
}




void qs_init(QueueStatic_t* q) {
    q->size = 0;
}

int qs_is_empty(const QueueStatic_t* q) {
    return q->size == 0;
}

int qs_is_full(const QueueStatic_t* q) {
    return q->size == MAX_JOBS;
}

int qs_enqueue(QueueStatic_t* q, PrintJob_t job) {
    if (qs_is_full(q)) return 0; 
    
    q->data[q->size] = job;
    q->size++;
    return 1;
}

int qs_peek(const QueueStatic_t* q, PrintJob_t* out) {
    if (qs_is_empty(q)) return 0;
    
    *out = q->data[0]; 
    return 1;
}

int qs_dequeue(QueueStatic_t* q, PrintJob_t* out) {
    if (qs_is_empty(q)) return 0; 
    
    *out = q->data[0]; 
    
    
    for (int i = 1; i < q->size; i++) {
        q->data[i - 1] = q->data[i]; 
    }
    
    q->size--;
    return 1;
}

void qs_print(const QueueStatic_t* q) {
    if (qs_is_empty(q)) {
        printf("La cola esta vacia.\n");
        return;
    }
    
    printf("--- ESTADO DE LA COLA (%d/%d) ---\n", q->size, MAX_JOBS);
    for (int i = 0; i < q->size; i++) {
        printf("[%d] id=%d %s %s %d pags\n", 
               i+1, q->data[i].id, q->data[i].usuario, 
               q->data[i].documento, q->data[i].paginas_total);
    }
    printf("---------------------------------\n");
}


void vaciar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}