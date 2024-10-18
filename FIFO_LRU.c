//Equipo Doritos Nacho
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_FRAMES 4   // Número de frames (páginas físicas en memoria)
#define NUM_PAGES 10   // Número total de páginas virtuales

// Estructura para un frame de página en memoria física
typedef struct Frame {
    int page;           // Número de página almacenada en el frame (valor -1 si está vacío)
    bool valid;         // Indica si el frame está ocupado (true) o vacío (false)
    struct Frame *prev; // Puntero al frame previo (para lista doblemente enlazada)
    struct Frame *next; // Puntero al frame siguiente (para lista doblemente enlazada)
} Frame;

// Estructura para la lista de frames en memoria física
typedef struct FrameList {
    int numFrames;      // Número de frames actualmente ocupados
    Frame *head;        // Puntero al primer frame de la lista
    Frame *tail;        // Puntero al último frame de la lista
} FrameList;

// Función para crear un nuevo frame
Frame* createFrame() {
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    if (frame != NULL) {
        frame->page = -1;   // Inicialmente no hay página asignada
        frame->valid = false;
        frame->prev = NULL;
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
        frameList->tail = NULL;
    }
    return frameList;
}

// Función para mover un frame al frente de la lista (más recientemente usado)
void moveToFront(FrameList *frameList, Frame *frame) {
    if (frame == frameList->head) {
        return; // Ya está al frente
    }

    // Eliminar frame de su posición actual
    if (frame->prev != NULL) {
        frame->prev->next = frame->next;
    }
    if (frame->next != NULL) {
        frame->next->prev = frame->prev;
    }

    // Si era el último, actualizar el tail
    if (frame == frameList->tail) {
        frameList->tail = frame->prev;
    }

    // Insertar al frente
    frame->next = frameList->head;
    frame->prev = NULL;
    if (frameList->head != NULL) {
        frameList->head->prev = frame;
    }
    frameList->head = frame;

    // Si la lista estaba vacía, también actualizar el tail
    if (frameList->tail == NULL) {
        frameList->tail = frame;
    }
}

// Función para eliminar un frame de la lista (menos recientemente usado)
void removeFrame(FrameList *frameList, Frame *frame) {
    if (frame->prev != NULL) {
        frame->prev->next = frame->next;
    } else {
        frameList->head = frame->next;
    }
    if (frame->next != NULL) {
        frame->next->prev = frame->prev;
    } else {
        frameList->tail = frame->prev;
    }
    frameList->numFrames--;
    free(frame);
}

// Función para buscar un frame específico por número de página
Frame* findFrame(FrameList *frameList, int page) {
    Frame *current = frameList->head;
    while (current != NULL) {
        if (current->page == page) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Función para simular la carga de una página a memoria física utilizando LRU
void loadPage(FrameList *frameList, int page) {
    Frame *frame = findFrame(frameList, page);
    
    if (frame != NULL) {
        // Página ya está en memoria, moverla al frente (más recientemente usada)
        moveToFront(frameList, frame);
    } else {
        // Crear un nuevo frame
        frame = createFrame();
        frame->page = page;
        frame->valid = true;

        // Si la lista de frames ya está llena, eliminar el frame menos recientemente usado (tail)
        if (frameList->numFrames == NUM_FRAMES) {
            Frame *lruFrame = frameList->tail;
            removeFrame(frameList, lruFrame);
        }

        // Insertar el nuevo frame al frente
        moveToFront(frameList, frame);
        frameList->numFrames++;
    }
}

// Función para imprimir el estado actual de la lista de frames (solo para fines de depuración)
void printFrameList(FrameList *frameList) {
    printf("Estado actual de la lista de frames:\n");
    Frame *current = frameList->head;
    while (current != NULL) {
        printf("Página: %d, ", current->page);
        if (current->valid) {
            printf("Estado: Ocupado\n");
        } else {
            printf("Estado: Vacío\n");
        }
        current = current->next;
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
    printFrameList(frameList);  // Debería imprimir el estado actual de los frames

    // Intentar cargar otra página cuando todos los frames están ocupados
    loadPage(frameList, 5);
    printFrameList(frameList);  // Debería imprimir el estado actual después de la sustitución

    // Liberar la memoria utilizada por la lista de frames
    Frame *current = frameList->head;
    while (current != NULL) {
        Frame *next = current->next;
        free(current);
        current = next;
    }
    free(frameList);

    return 0;
}
