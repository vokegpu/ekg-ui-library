#include "ekg/ekg.hpp"

void ekg::dispatch(ekg::cpu::event* event) {
    ekg::core->get_service_handler().dispatch(event);
}

void ekg::dispatch(const ekg::env &env) {
    ekg::core->get_service_handler().task((uint32_t) env);
}