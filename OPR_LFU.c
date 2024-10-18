#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_FRAMES 4   // Número de frames (páginas físicas en memoria)
#define NUM_PAGES 10   // Número total de páginas virtuales

// Estructura para un frame de página en memoria física
typedef struct Frame {
    int page;           // Número de página almacenada en el frame
    bool valid;         // Indica si el frame está ocupado (true) o vacío (false)
    int frequency;      // Contador de frecuencia de acceso a la página
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
        frame->page = -1;     // Inicialmente no hay página asignada
        frame->valid = false;
        frame->frequency = 0; // Inicialmente la frecuencia es 0
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

// Función para insertar un frame al final de la lista
void insertFrame(FrameList *frameList, Frame *frame) {
    if (frameList->head == NULL) {
        // Lista vacía
        frameList->head = frame;
        frameList->tail = frame;
    } else {
        // Insertar al final de la lista
        frameList->tail->next = frame;
        frame->prev = frameList->tail;
        frameList->tail = frame;
    }
    frameList->numFrames++;
}

// Función para eliminar un frame de la lista
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

// Función para simular la carga de una página a memoria física utilizando el algoritmo LFU
void loadPage(FrameList *frameList, int page) {
    Frame *existingFrame = findFrame(frameList, page);
    if (existingFrame != NULL) {
        // Si la página ya está en memoria, incrementar su frecuencia
        existingFrame->frequency++;
        return;
    }

    // Si la lista de frames ya está llena, determinar la página LFU a reemplazar
    if (frameList->numFrames == NUM_FRAMES) {
        Frame *lfuFrame = frameList->head;
        Frame *current = frameList->head->next;

        // Encontrar el frame con la menor frecuencia
        while (current != NULL) {
            if (current->frequency < lfuFrame->frequency) {
                lfuFrame = current;
            }
            current = current->next;
        }

        // Remover el frame LFU encontrado
        removeFrame(frameList, lfuFrame);
    }

    // Crear un nuevo frame para la nueva página
    Frame *newFrame = createFrame();
    newFrame->page = page;
    newFrame->valid = true;
    newFrame->frequency = 1; // Inicializar frecuencia a 1

    // Insertar el nuevo frame en la lista de frames
    insertFrame(frameList, newFrame);
}

// Función para imprimir el estado actual de la lista de frames (solo para fines de depuración)
void printFrameList(FrameList *frameList) {
    printf("Estado actual de la lista de frames:\n");
    Frame *current = frameList->head;
    while (current != NULL) {
        printf("Página: %d, ", current->page);
        printf("Estado: %s, ", current->valid ? "Ocupado" : "Vacío");
        printf("Frecuencia: %d\n", current->frequency);
        current = current->next;
    }
    printf("\n");
}

int main() {
    FrameList *frameList = createFrameList();

    // Simular el orden de accesos a las páginas (simplificado)
    int futureAccess[NUM_PAGES] = {1, 2, 3, 4, 5, 1, 2, 1, 3, 4};

    // Simular la carga de páginas a memoria física utilizando el algoritmo LFU
    for (int i = 0; i < NUM_PAGES; ++i) {
        loadPage(frameList, futureAccess[i]);
        printFrameList(frameList);
    }

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
