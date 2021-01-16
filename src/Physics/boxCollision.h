#ifndef TD_BOX_COLLISION
#define TD_BOX_COLLISION

#include <stdbool.h>
#include "../DataStructures/Box/box.h"
#include "../DataStructures/Tuple/tuple.h"

typedef enum {
    TD_CRNR_TOP,    // top left corner
    TD_CRNR_BOTTOM, // bottom right corner
    TD_INTR_NONE
} td_intrudingCorner;

typedef struct td_boxCollider *td_boxCollider;

td_boxCollider createBoxCollider(td_box);

void registerBoxColliderCallback(td_boxCollider, void *);

void registerBoxColliderCallbackData(td_boxCollider, void *);

bool checkCollision(td_boxCollider, td_boxCollider);

td_tuple getBoxColliderPosition(td_boxCollider);

void setBoxColliderPosition(td_boxCollider, td_tuple);

#endif
