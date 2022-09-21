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
int main(int argv, char** argc) {
    SDL_Init(SDL_INIT_VIDEO);

    ekg::init();
    ekg::demo();
    ekg::quit();

    return 0;
}