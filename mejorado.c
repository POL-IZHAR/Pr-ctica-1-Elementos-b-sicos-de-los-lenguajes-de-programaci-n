
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN system("cls")
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define CLEAR_SCREEN system("clear")
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

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
    Node_t* head; 
    Node_t* tail; 
    int size;     
} QueueDynamic_t;


void qd_init(QueueDynamic_t* q);
int qd_is_empty(const QueueDynamic_t* q);
int qd_enqueue_priority(QueueDynamic_t* q, PrintJob_t job); 
int qd_peek(const QueueDynamic_t* q, PrintJob_t* out);
int qd_dequeue(QueueDynamic_t* q, PrintJob_t* out);
void qd_destroy(QueueDynamic_t* q);
void qd_print(const QueueDynamic_t* q);


void simular_impresion(QueueDynamic_t* q, int* stats_trabajos, int* stats_paginas);
void vaciar_buffer();
void pausar_pantalla();


int main() {
    QueueDynamic_t cola;
    qd_init(&cola); 
    
    int opcion;
    int contador_id = 1; 
    
    
    int total_trabajos_completados = 0;
    int total_paginas_impresas = 0;
    
    do {
        CLEAR_SCREEN;
        printf("\n=== SIMULADOR DE COLA DE IMPRESION (SESION 3) ===\n");
        printf("1. Agregar trabajo (NORMAL)\n");
        printf("2. Agregar trabajo (URGENTE)\n");
        printf("3. Ver siguiente trabajo (peek)\n");
        printf("4. Listar cola\n");
        printf("5. SIMULAR IMPRESION DE TODA LA COLA\n");
        printf("6. Salir\n");
        printf("Selecciona una opcion: ");
        
        if (scanf("%d", &opcion) != 1) {
            vaciar_buffer();
            opcion = 0; 
        }
        vaciar_buffer();

        switch (opcion) {
            case 1:
            case 2: { 
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
                nuevo_trabajo.estado = EN_COLA;
                nuevo_trabajo.ms_por_pagina = 300; 
                
                
                nuevo_trabajo.prioridad = (opcion == 2) ? URGENTE : NORMAL;
                
                if (qd_enqueue_priority(&cola, nuevo_trabajo)) {
                    printf("[+] Agregado %s: id=%d usuario=%s doc=%s pags=%d\n", 
                           (opcion == 2) ? "[URGENTE]" : "[NORMAL]",
                           nuevo_trabajo.id, nuevo_trabajo.usuario, 
                           nuevo_trabajo.documento, nuevo_trabajo.paginas_total);
                    contador_id++;
                } else {
                    printf("[!] Error: No se pudo asignar memoria.\n");
                }
                pausar_pantalla();
                break;
            }
            case 3: { // PEEK
                PrintJob_t siguiente;
                if (qd_peek(&cola, &siguiente)) {
                    printf("[i] Siguiente en fila: id=%d %s %s %d pags (Prioridad: %s)\n", 
                           siguiente.id, siguiente.usuario, siguiente.documento, siguiente.paginas_total,
                           siguiente.prioridad == URGENTE ? "URGENTE" : "NORMAL");
                } else {
                    printf("[-] La cola esta vacia.\n");
                }
                pausar_pantalla();
                break;
            }
            case 4: { // LISTAR
                qd_print(&cola);
                pausar_pantalla();
                break;
            }
            case 5: { 
                simular_impresion(&cola, &total_trabajos_completados, &total_paginas_impresas);
                pausar_pantalla();
                break;
            }
            case 6:
                printf("Saliendo y liberando memoria...\n");
                qd_destroy(&cola);
                break;
            default:
                printf("[!] Opcion no valida.\n");
                pausar_pantalla();
        }
    } while (opcion != 6);

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


int qd_enqueue_priority(QueueDynamic_t* q, PrintJob_t job) {
    Node_t* new_node = (Node_t*)malloc(sizeof(Node_t));
    if (new_node == NULL) return 0; 
    
    new_node->job = job;
    new_node->next = NULL; 
    
    if (qd_is_empty(q)) {
        q->head = new_node; 
        q->tail = new_node;
    } 
    else if (job.prioridad == NORMAL) {
        
        q->tail->next = new_node; 
        q->tail = new_node;
    } 
    else { 
        if (q->head->job.prioridad == NORMAL) {
            new_node->next = q->head;
            q->head = new_node;
        } else {

            Node_t* current = q->head;
            while (current->next != NULL && current->next->job.prioridad == URGENTE) {
                current = current->next;
            }
            new_node->next = current->next;
            current->next = new_node;
            
            
            if (new_node->next == NULL) {
                q->tail = new_node;
            }
        }
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
    
    if (q->head == NULL) q->tail = NULL;
    
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
    printf("[i] Se liberaron %d trabajos pendientes.\n", count);
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
        printf("[%d] id=%d %s %s %d pags [%s]\n", 
               pos, current->job.id, current->job.usuario, 
               current->job.documento, current->job.paginas_total,
               current->job.prioridad == URGENTE ? "URGENTE" : "NORMAL");
        current = current->next;
        pos++;
    }
    printf("--------------------------------------------------\n");
}


void simular_impresion(QueueDynamic_t* q, int* stats_trabajos, int* stats_paginas) {
    if (qd_is_empty(q)) {
        printf("[-] La cola esta vacia. No hay nada que imprimir.\n");
        return;
    }

    PrintJob_t trabajo_actual;
    
    while (qd_dequeue(q, &trabajo_actual)) {
        trabajo_actual.estado = IMPRIMIENDO; 

        CLEAR_SCREEN;
        printf("=========================================\n");
        printf("          IMPRESORA EN PROCESO           \n");
        printf("=========================================\n");
        printf("-> Procesando ID: %d\n", trabajo_actual.id);
        printf("-> Usuario: %s\n", trabajo_actual.usuario);
        printf("-> Documento: %s\n", trabajo_actual.documento);
        printf("-> Estado: IMPRIMIENDO...\n");
        printf("-----------------------------------------\n");

        
        while (trabajo_actual.paginas_restantes > 0) {
            int pagina_actual = trabajo_actual.paginas_total - trabajo_actual.paginas_restantes + 1;
            
            
            printf("[");
            for(int i=0; i<pagina_actual; i++) printf("#");
            for(int i=0; i<(trabajo_actual.paginas_total - pagina_actual); i++) printf("-");
            printf("] Pagina %d de %d\n", pagina_actual, trabajo_actual.paginas_total);
            
            SLEEP_MS(trabajo_actual.ms_por_pagina); 
            
            trabajo_actual.paginas_restantes--;
            (*stats_paginas)++; 
        }

        trabajo_actual.estado = COMPLETADO; 
        (*stats_trabajos)++;
    
        printf("\n[OK] Trabajo ID=%d COMPLETADO exitosamente!\n", trabajo_actual.id);
        SLEEP_MS(1500); 
    }

    CLEAR_SCREEN;
    printf("=========================================\n");
    printf("        SIMULACION FINALIZADA            \n");
    printf("=========================================\n");
    printf("ESTADISTICAS GLOBALES:\n");
    printf("- Trabajos completados hoy: %d\n", *stats_trabajos);
    printf("- Total de paginas impresas: %d\n", *stats_paginas);
    printf("=========================================\n");
}

void vaciar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar_pantalla() {
    printf("\nPresiona ENTER para continuar...");
    vaciar_buffer();
}