#ifndef HELL_PRIVATE_H
#define HELL_PRIVATE_H

#define	MAX_EDIT_LINE	256
#define MAX_QUEUE_EVENTS 64

#include "types.h"

typedef enum {
    HELL_WINDOW_XCB_TYPE,
    HELL_WINDOW_WIN32_TYPE
} Hell_WindowType;

typedef struct Hell_Window {
    unsigned int    width;
    unsigned int    height;
    Hell_WindowID   id;
    Hell_WindowType type;
    void*           typeSpecificData;
} Hell_Window;

#endif /* end of include guard: HELL_PRIVATE_H */
