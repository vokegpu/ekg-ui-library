/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
*/

#ifndef EKG_DRAW_H
#define EKG_DRAW_H

#include "ekg/util/geometry.hpp"
#include <vector>
#include <iostream>

namespace ekg {
    enum drawmode {
        filled = 0, circle = -1, outline = 1
    };

    namespace draw {
        struct box {
            std::string text {};
            ekg::rect rect {};
            int32_t id {};
        };

        class immediate {
        protected:
            std::vector<ekg::draw::box> loaded_box_list {};
            int32_t token_id {};
        public:
            void send_popup(ekg::draw::box&);
        };

        void rect(const ekg::rect&, const ekg::vec4&, int32_t = 0);
        void rect(float, float, float, float, const ekg::vec4&, int32_t = 0);

        void sync_scissor(ekg::rect&, int32_t);
        void bind_scissor(int32_t);
        void bind_off_scissor();
        bool is_visible(int32_t, ekg::vec4&);
        void get_visible(int32_t, ekg::rect&);
    }
}

#endif