#include <stdlib.h>
#include "boxCollision.h"
#include "../IO/logger.h"

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

void registerBoxColliderCallbackData(td_boxCollider collider, void *data) {
    collider -> callbackData = data;
}

bool checkCollision(td_boxCollider collider1, td_boxCollider collider2) {
    td_box col1Box = collider1 -> hull;
    td_box col2Box = collider2 -> hull;
    td_tuple col1Corner; // collider1 bottom right corner
    td_tuple col2Corner; // collider2 bottom right corner

    col1Corner.x = col1Box.x + col1Box.w;
    col1Corner.y = col1Box.y - col1Box.h;

    col2Corner.x = col2Box.x + col2Box.w;
    col2Corner.y = col2Box.y - col2Box.h;

    td_tuple intrusion;

    //Calculate X Intrusion
    if(col1Corner.x > col2Box.x && col1Corner.x < col2Corner.x)
        intrusion.x = col1Corner.x - col2Box.x;
    else if(col1Box.x > col2Box.x && col1Box.x < col2Corner.x)
        intrusion.x = col1Box.x - col2Corner.x;
    else {
        if(col2Corner.x > col1Box.x && col2Corner.x < col1Corner.x)
            intrusion.x = col1Corner.x - col2Box.x;
        else if(col2Box.x > col1Box.x && col2Box.x < col1Corner.x)
            intrusion.x = col1Box.x - col2Corner.x;
        else intrusion.x = 0;
    }

    //Calculate Y Intrusion
    if(col1Corner.y < col2Box.y && col1Corner.y > col2Corner.y)
        intrusion.y = col1Corner.y - col2Box.y;
    else if(col1Box.y < col2Box.y && col1Box.y > col2Corner.y)
        intrusion.y = col1Box.y - col2Corner.y;
    else {
        if(col2Corner.y < col1Box.y && col2Corner.y > col1Corner.y)
            intrusion.y = col1Corner.y - col2Box.y;
        else if(col2Box.y < col1Box.y && col2Box.y > col1Corner.y)
            intrusion.y = col1Box.y - col2Corner.y;
        else intrusion.y = 0;
    }

    td_collision collision = { intrusion };

    if(intrusion.x && intrusion.y) {
        if(collider1 -> callbackFunction) {
            collision.hull = col1Box;
            collision.collidingHull = col2Box;
            collider1 -> callbackFunction(collision, collider1 -> callbackData);
        }

        if(collider2 -> callbackFunction) {
            collision.hull = col2Box;
            collision.collidingHull = col1Box;
            collider2 -> callbackFunction(collision, collider2 -> callbackData);
        }

        return true;
    }

    return false;
}

td_tuple getBoxColliderPosition(td_boxCollider collider) {
    return (td_tuple) { collider -> hull.x, collider -> hull.y };
}

void setBoxColliderPosition(td_boxCollider collider, td_tuple position) {
    collider -> hull.x = position.x;
    collider -> hull.y = position.y;
}
