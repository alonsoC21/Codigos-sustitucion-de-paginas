#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_FRAMES 4   // Número de frames (páginas físicas en memoria)
#define NUM_PAGES 10   // Número total de páginas virtuales

// Estructura para un frame de página en memoria física
typedef struct Frame {
    int page;           // Número de página almacenada en el frame
    bool valid;         // Indica si el frame está ocupado (true) o vacío (false)
    bool referenced;    // Bit de referencia para el algoritmo Clock
    struct Frame *next; // Puntero al frame siguiente (para lista circular)
} Frame;

// Estructura para la lista de frames en memoria física
typedef struct FrameList {
    int numFrames;      // Número de frames actualmente ocupados
    Frame *head;        // Puntero al primer frame de la lista
    Frame *current;     // Puntero al frame actual (para el algoritmo Clock)
} FrameList;

// Función para crear un nuevo frame
Frame* createFrame() {
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    if (frame != NULL) {
        frame->page = -1;      // Inicialmente no hay página asignada
        frame->valid = false;
        frame->referenced = false; // Inicialmente, el bit de referencia está en 0
        frame->next = NULL;
    }
    return frame;
}

// Función para inicializar la lista de frames en memoria física
FrameList* createFrameList() {
    FrameList *frameList = (FrameList *)malloc(sizeof(FrameList));
    if (frameList != NULL) {
        frameList->numFrames = 0;
        frameList->head = NULL;
        frameList->current = NULL;
    }
    return frameList;
}

// Función para insertar un frame al final de la lista (Clock)
void insertFrame(FrameList *frameList, Frame *frame) {
    if (frameList->head == NULL) {
        // Lista vacía
        frameList->head = frame;
        frame->next = frame; // Forma un ciclo
    } else {
        Frame *tail = frameList->head;
        while (tail->next != frameList->head) {
            tail = tail->next;
        }
        tail->next = frame; // Inserta al final
        frame->next = frameList->head; // Mantiene el ciclo
    }
    frameList->numFrames++;
}

// Función para simular la carga de una página a memoria física utilizando el algoritmo Clock
void loadPage(FrameList *frameList, int page) {
    Frame *current = frameList->head;

    // Buscar si la página ya está en memoria
    while (current != NULL) {
        if (current->page == page) {
            current->referenced = true; // Actualiza el bit de referencia
            return;
        }
        current = current->next;
        if (current == frameList->head) break; // Vuelve al inicio si es un ciclo
    }

    // Si la página no está en memoria, hay que cargarla
    Frame *newFrame = createFrame();
    newFrame->page = page;
    newFrame->valid = true;

    if (frameList->numFrames < NUM_FRAMES) {
        insertFrame(frameList, newFrame); // Insertar el nuevo frame si hay espacio
    } else {
        // Reemplazar la página usando el algoritmo Clock
        while (true) {
            if (frameList->current == NULL) {
                frameList->current = frameList->head; // Comienza desde la cabeza
            }

            if (frameList->current->referenced == false) {
                // Reemplaza esta página
                printf("Reemplazando página: %d\n", frameList->current->page);
                frameList->current->page = newFrame->page; // Reemplazar
                frameList->current->valid = true; // Establece como ocupado
                frameList->current->referenced = true; // Establece el bit de referencia
                free(newFrame); // Liberar el nuevo frame ya que se usó el existente
                return;
            } else {
                // Resetear el bit de referencia y mover al siguiente frame
                frameList->current->referenced = false;
                frameList->current = frameList->current->next;
            }
        }
    }
}

// Función para imprimir el estado actual de la lista de frames (solo para fines de depuración)
void printFrameList(FrameList *frameList) {
    printf("Estado actual de la lista de frames:\n");
    Frame *current = frameList->head;
    if (current != NULL) {
        do {
            printf("Página: %d, ", current->page);
            printf("Estado: %s, ", current->valid ? "Ocupado" : "Vacío");
            printf("Referencia: %s\n", current->referenced ? "1" : "0");
            current = current->next;
        } while (current != frameList->head);
    }
    printf("\n");
}

int main() {
    FrameList *frameList = createFrameList();

    // Simular la carga de varias páginas a memoria física
    loadPage(frameList, 1);
    loadPage(frameList, 2);
    loadPage(frameList, 3);
    loadPage(frameList, 4);
    printFrameList(frameList);  // Imprimir estado inicial de los frames

    // Intentar cargar otras páginas cuando todos los frames están ocupados
    loadPage(frameList, 5);
    printFrameList(frameList);  // Imprimir estado después de la sustitución

    // Liberar la memoria utilizada por la lista de frames
    Frame *current = frameList->head;
    if (current != NULL) {
        do {
            Frame *next = current->next;
            free(current);
            current = next;
        } while (current != frameList->head);
    }
    free(frameList);

    return 0;
}
