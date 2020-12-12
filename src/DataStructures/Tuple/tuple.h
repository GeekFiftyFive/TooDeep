#ifndef TUPLE
#define TUPLE

typedef struct {
    float x;
    float y;
} td_tuple;

td_tuple productTuple(td_tuple, td_tuple);

td_tuple multiplyTuple(float, td_tuple);

td_tuple divideTuple(td_tuple, td_tuple);

td_tuple addTuple(td_tuple, td_tuple);

td_tuple subtractTuple(td_tuple, td_tuple);

td_tuple normaliseTuple(td_tuple);

void printTuple(td_tuple);

#endif