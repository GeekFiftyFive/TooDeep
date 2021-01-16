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
    collider -> callbackData = NULL;
    collider -> callbackFunction = NULL;

    return collider;
}

void registerBoxColliderCallback(td_boxCollider collider, void *callbackFunction) {
    collider -> callbackFunction = callbackFunction;
}

void registerCallbackData(td_boxCollider collider, void *data) {
    collider -> callbackData = data;
}

bool checkCollision(td_boxCollider collider1, td_boxCollider collider2) {
    td_box col1Box = collider1 -> hull;
    td_box col2Box = collider2 -> hull;
    td_tuple col1Corner; // collider1 bottom right corner
    td_tuple col2Corner; // collider2 bottom right corner

    col1Corner.x = col1Box.x + col1Box.w;
    col1Corner.y = col1Box.y + col1Box.h;

    col2Corner.x = col2Box.x + col2Box.w;
    col2Corner.y = col2Box.y + col2Box.h;

    // TODO: Make this into a function instead of repeating
    if(col1Corner.x > col2Box.x &&
       col1Corner.y > col2Box.y &&
       col1Corner.x < col2Corner.x &&
       col1Corner.y < col2Corner.y) {
        // Collider 1 bottrom right corner intersected

        if(collider1 -> callbackFunction) {
            td_tuple amount = (td_tuple) {
                col1Corner.x - col2Box.x,
                col1Corner.y - col2Box.y
            };

            td_collision collision = (td_collision) {
                amount,
                TD_CRNR_BOTTOM
            };
        }

        if(collider2 -> callbackFunction) {
            td_tuple amount = (td_tuple) {
                col2Box.x - col1Corner.x,
                col2Box.y - col1Corner.y
            };

            td_collision collision = (td_collision) {
                amount,
                TD_CRNR_TOP
            };
        }

        return true;
    }

    if(col2Corner.x > col1Box.x &&
       col2Corner.y > col1Box.y &&
       col2Corner.x < col1Corner.x &&
       col2Corner.y < col1Corner.y) {
        // Collider 2 bottrom right corner intersected

        if(collider1 -> callbackFunction) {
            td_tuple amount = (td_tuple) {
                col2Corner.x - col1Box.x,
                col2Corner.y - col1Box.y
            };

            td_collision collision = (td_collision) {
                amount,
                TD_CRNR_TOP
            };
        }

        if(collider2 -> callbackFunction) {
            td_tuple amount = (td_tuple) {
                col1Box.x - col2Corner.x,
                col1Box.y - col2Corner.y
            };

            td_collision collision = (td_collision) {
                amount,
                TD_CRNR_BOTTOM
            };
        }

        return true;
    }

    return false;
}
