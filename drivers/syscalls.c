#include <sys/types.h>

// Proporcionado por el linker (debe estar definido en el archivo de enlace)
extern char _end; // Final de la sección .bss
static char *heap_end;

// Implementación de _sbrk
void *_sbrk(ptrdiff_t incr) {
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end; // Inicializa el heap al final de .bss
    }

    prev_heap_end = heap_end;

    // Ajusta el heap
    heap_end += incr;

    // Retorna el inicio previo del heap
    return (void *)prev_heap_end;
}