#include "integer.h"

Integer::Integer(int val)
{
    value = val;
}

int Integer::getValue() {
    return value;
}

void Integer::setValue(int val) {
    value = val;
}
