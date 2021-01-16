#include <stdlib.h>
#include "boxCollision.h"

struct td_boxCollider {
    td_box hull;
    void *callbackData;
    void (*callbackFunction)(td_collision, void *);
};

td_boxCollider createBoxCollider(td_box hull) {
    td_boxCollider collider = malloc(sizeof(struct td_boxCollider));

    collider -> hull = hull;

    return collider;
}

void registerBoxColliderCallback(td_boxCollider collider, void *callbackFunction) {
    collider -> callbackFunction = callbackFunction;
}

td_collision checkCollision(td_boxCollider collider1, td_boxCollider collider2) {

}
