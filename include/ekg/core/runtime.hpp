#ifndef EKG_CORE_H
#define EKG_CORE_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/service/handler.hpp"
#include "ekg/draw/font.hpp"
#include "ekg/service/theme.hpp"
#include "ekg/service/input.hpp"
#include "ekg/core/feature.hpp"
#include <map>

namespace ekg {
    /* The swap maps for prevent copies of hashes and vector. */
    struct swap {
        static std::map<uint32_t, ekg::ui::abstract_widget*> fast;
        static std::map<uint32_t, ekg::ui::abstract_widget*> continuous;
        static std::map<uint32_t, ekg::ui::abstract_widget*> target;
        static std::vector<ekg::ui::abstract_widget*> buffer;

        static void refresh();
    };

    /* The main runtime for run ekg. */
    class runtime : public ekg::feature {
    private:
        SDL_Window* root {};

        /* Widget env lists and maps for tasks. */

        std::vector<ekg::ui::abstract_widget*> list_widget {};
        std::vector<ekg::ui::abstract*> list_interface {};

        std::vector<ekg::ui::abstract_widget*> list_update_widget {};
        std::vector<ekg::ui::abstract_widget*> list_refresh_widget {};
        std::vector<ekg::ui::abstract_widget*> list_reset_widget {};

        std::map<uint32_t, ekg::ui::abstract_widget*> map_widget {};
        std::map<uint32_t, ekg::ui::abstract> map_interface {};

        /* Core services and instances. */

        ekg::gpu::allocator allocator {};

        ekg::draw::font_renderer f_renderer_small {};
        ekg::draw::font_renderer f_renderer_normal {};
        ekg::draw::font_renderer f_renderer_big {};

        ekg::service::input input_manager {};
        ekg::service::theme theme_manager {};
        ekg::service::handler handler {};

        /* Tokens for use in creation of elements. */

        uint32_t token_id {};
        uint32_t widget_id_focused {};
        uint32_t prev_widget_id_focused {};
        uint32_t swap_widget_id_focused {};

        uint32_t widget_id_pressed_focused {};
        uint32_t widget_id_released_focused {};

        /* Security methods of ekg. */

        void prepare_tasks();
        void prepare_ui_env();
    public:
        void set_root(SDL_Window* sdl_win_root);
        SDL_Window* get_root();

        ekg::gpu::allocator &get_gpu_allocator();

        ekg::draw::font_renderer &get_f_renderer_small();
        ekg::draw::font_renderer &get_f_renderer_normal();
        ekg::draw::font_renderer &get_f_renderer_big();

        ekg::service::input &get_service_input();
        ekg::service::theme &get_service_theme();
        ekg::service::handler &get_service_handler();

        ekg::ui::abstract_widget* get_fast_widget_by_id(uint32_t id);
        void reload_widget(ekg::ui::abstract_widget* widget);
        void create_ui(ekg::ui::abstract* ui);
        void reset_widget(ekg::ui::abstract_widget* widget);

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif