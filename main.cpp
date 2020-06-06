#include <iostream>
#include "utils.h"
#include "process_handler.h"

using namespace ljstack;

int main() {
    ProcessHandler handler(8831);
    int a = handler.attach();
    if (a < 0) return -1;
    int *ptr = (int *)(void *)(0x000055f07b475014);
    int ret = handler.geto(ptr);
    a = handler.detach();
    if (a < 0) return -1;
    LOG_OUT("%d", ret);
    return 0;
}