/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#pragma once
#ifndef EKG_EVENT_H
#define EKG_EVENT_H

#include <iostream>
#include <SDL2/SDL.h>

/*
 * The popup event type.
 */
static uint32_t const EKG_POPUPEVENT = SDL_RegisterEvents(SDL_USEREVENT + 1);

/**
 * The main event struct with all events of UI.
 **/
struct ekg_event {
    uint32_t id = 0;
    uint16_t type = 0;

    char* text = nullptr;
    bool boolean = false;
    float value = 0;
};

#endif