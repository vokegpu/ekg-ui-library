/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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