#include "greeting.h"
#include <stdio.h>

int main() {
    struct greeting g = {42, 0, "Alexander"};
    greet(&g);
}
