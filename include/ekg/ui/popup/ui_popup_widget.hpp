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

#ifndef EKG_UI_POPUP_WIDGET_H
#define EKG_UI_POPUP_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg::ui {
	class popup_widget : public ekg::ui::abstract_widget {
	protected:
        struct element {
            ekg::rect rect_text {};
            ekg::rect rect_bound {};
            bool separator {};
        };
    public:
        std::vector<ekg::ui::popup_widget::element> element_list {};
        float scissor_opened_height {};
        float separator_offset {};
        uint64_t elapsed_animation_ticks {};
        bool parent_id_popup_opened {false};

        int32_t hovered_element {};
        int32_t popup_opened {-1};
        int32_t top_level_popup {};
    public:
        bool is_hovering_any_popup(int32_t top_level);
        void unset_visible_all_sub_popup();
        void get_popup_path(std::string &path);

        void on_destroy() override;
        void on_reload() override;
        void on_pre_event(SDL_Event&) override;
        void on_event(SDL_Event&) override;
        void on_post_event(SDL_Event&) override;
        void on_update() override;
        void on_draw_refresh() override;
	};
}

#endif