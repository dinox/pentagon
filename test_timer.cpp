#include "timer.h"

int main() {
    Timer t(100);
    t.start();
    for (int i = 0; i < 1000; i++)
        asm ("");
    t.stop();
    t.print_cycles();
    return 0;
}
