#ifndef TD_BOX_COLLISION
#define TD_BOX_COLLISION

#include "../DataStructures/Box/box.h"

typedef enum {
    TD_INTR_LEFT,
    TD_INTR_RIGHT,
    TD_INTR_TOP,
    TD_INTR_BOTTOM,
    TD_INTR_NONE
} td_intrusionDirection;

typedef struct td_boxCollider *td_boxCollider;

typedef struct td_collision {
    float amount;
    td_intrusionDirection direction;
} td_collision;

td_boxCollider createBoxCollider(td_box);

void registerBoxColliderCallback(td_boxCollider, void *);

td_collision checkCollision(td_boxCollider, td_boxCollider);

#endif
