#include <iostream>
#include "process_handler.h"

using namespace ljstack;

int main() {
    ProcessHandler handler(19159);
    handler.attach();
    int *ptr = (int *)(void *)(0x000055c9ac9dd014);
    int ret = handler.geto(ptr);
    handler.detach();
    printf("%d\n", ret);
    return 0;
}