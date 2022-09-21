#include "ekg/ekg.hpp"

/*
 * Test ekg ui library.
 *
 * -- result A = Approved - No problems passed.
 * -- result B = Partial  - No problems but not totally done.
 * -- result F = Reproved - Problems or not working correct.
 *
 * All tests were ran in Linux.
 */
int main(void) {
    ekg::init();
    ekg::demo();
    ekg::quit();

    return 0;
}