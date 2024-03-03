/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_CORE_H
#define EKG_CORE_H

#include <unordered_map>

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/service/handler.hpp"
#include "ekg/draw/font_renderer.hpp"
#include "ekg/service/theme.hpp"
#include "ekg/service/input.hpp"
#include "ekg/service/layout.hpp"
#include "ekg/util/io.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/gpu/api.hpp"

namespace ekg {
  extern struct current_hovered_state {
  public:
    int32_t id {};
    ekg::type type {};
    int32_t up {};
    int32_t down {};
    ekg::type down_type {};
    ekg::type up_type {};
    int32_t swap {};
    int32_t last {};
  } hovered;

  class runtime {
  private:
    static ekg::stack collect;
    static ekg::stack back;
    static ekg::stack front;
  private:
    std::unordered_map<int32_t, ekg::ui::abstract_widget*> widget_map {};
    std::unordered_map<int32_t, bool> processed_widget_map {};

    std::vector<ekg::ui::abstract_widget*> loaded_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> refresh_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> reload_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> synclayout_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> redraw_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> scissor_widget_list  {};
    std::vector<ekg::ui::abstract_widget*> update_widget_list {};

    ekg::ui::abstract *p_current_ui_container {};
    ekg::ui::abstract_widget *p_abs_activity_widget {};

    int32_t token_id {};
    int32_t widget_id_focused {};
    int32_t prev_widget_id_focused {};
    int32_t swap_widget_id_focused {};

    bool enable_high_priority_frequency {};
    bool should_re_batch_gui {};
  public:
    ekg::service::input service_input {};
    ekg::service::theme service_theme {};
    ekg::service::handler service_handler {};
    ekg::service::layout service_layout {};

    ekg::draw::font_renderer f_renderer_small {};
    ekg::draw::font_renderer f_renderer_normal {};
    ekg::draw::font_renderer f_renderer_big {};

    ekg::gpu::api *p_gpu_api {};
    ekg::gpu::allocator gpu_allocator {};
    ekg::timing ui_timing {};
    ekg::os::platform *p_os_platform {};
    ekg::os::io_event_serial io_event_serial {};
  protected:
    void prepare_tasks();

    void prepare_ui_env();

    void erase(int32_t id);    
  public:
    ekg::ui::abstract_widget *get_fast_widget_by_id(int32_t id);

    void gen_widget(ekg::ui::abstract *p_ui);

    void do_task_reload(ekg::ui::abstract_widget *widget);

    void do_task_synclayout(ekg::ui::abstract_widget *widget);

    void do_task_refresh(ekg::ui::abstract_widget *widget);

    void set_update_high_frequency(ekg::ui::abstract_widget *widget);

    void request_redraw_gui();

    void end_group_flag();

    void update_size_changed();

    void init();

    void quit();

    void process_event(SDL_Event &sdl_event);

    void process_update();

    void process_render();
  };
};

#endif