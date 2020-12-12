#include <math.h>
#include "../../IO/logger.h"
#include "tuple.h"

td_tuple productTuple(td_tuple a, td_tuple b) {
    return (td_tuple) { a.x * b.x, b.x * b.y };
}

td_tuple multiplyTuple(float multiple, td_tuple tuple) {
    return (td_tuple) { multiple * tuple.x, multiple * tuple.y };
}

td_tuple divideTuple(td_tuple a, td_tuple b) {
    return (td_tuple) { a.x / b.x, a.y / b.y };
}

td_tuple addTuple(td_tuple a, td_tuple b) {
    return (td_tuple) { a.x + b.x, a.y + b.y };
}

td_tuple subtractTuple(td_tuple a, td_tuple b) {
    return (td_tuple) { a.x - b.x, a.y - b.y };
}

td_tuple normaliseTuple(td_tuple tuple) {
    float hyp = sqrt(tuple.x * tuple.x + tuple.y * tuple.y);
    return (td_tuple) { tuple.x / hyp, tuple.y / hyp };
}

void printTuple(td_tuple tuple) {
    logInfo("{ %f, %f }\n", tuple.x, tuple.y);
}
