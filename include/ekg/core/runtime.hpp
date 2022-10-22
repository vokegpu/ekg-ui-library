#ifndef EKG_CORE_H
#define EKG_CORE_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/service/handler.hpp"
#include "ekg/draw/font.hpp"
#include "ekg/service/theme.hpp"
#include "ekg/service/input.hpp"
#include "ekg/core/feature.hpp"
#include "ekg/service/layout.hpp"
#include "ekg/util/util_ui.hpp"

namespace ekg {
    /* The swap maps for prevent copies of hashes and vector. */
    struct swap {
        static ekg::stack collect;
        static ekg::stack back;
        static ekg::stack front;
        static std::vector<ekg::ui::abstract_widget*> buffer;

        static void refresh();
    };

    /* The main runtime for run ekg. */
    class runtime : public ekg::feature {
    private:
        SDL_Window* root {};

        /* Widget env lists and maps for tasks. */

        std::vector<ekg::ui::abstract_widget*> loaded_widget_list {};
        std::vector<ekg::ui::abstract*> loaded_interface_list {};

        std::vector<ekg::ui::abstract_widget*> to_reload_widgets {};
        std::vector<ekg::ui::abstract_widget*> to_refresh_widgets {};
        std::vector<ekg::ui::abstract_widget*> to_reset_widgets {};
        std::vector<ekg::ui::abstract_widget*> to_sync_layout_widgets {};

        std::map<uint32_t, ekg::ui::abstract_widget*> widgets_map {};
        std::map<uint32_t, ekg::ui::abstract> interfaces_map {};
        std::map<uint32_t, bool> processed_widgets_map {};

        /* Core services and instances. */

        ekg::ui::abstract_widget* current_bind_group {};
        ekg::gpu::allocator allocator {};

        ekg::draw::font_renderer f_renderer_small {};
        ekg::draw::font_renderer f_renderer_normal {};
        ekg::draw::font_renderer f_renderer_big {};

        ekg::service::input input_manager {};
        ekg::service::theme theme_manager {};
        ekg::service::handler handler {};
        ekg::service::layout layout_manager {};

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
        void sync_layout_widget(ekg::ui::abstract_widget* widget);
        void pop_group();

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif