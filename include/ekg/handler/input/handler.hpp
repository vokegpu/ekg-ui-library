#ifndef EKG_HANDLER_INPUT_HANDLER_HPP
#define EKG_HANDLER_INPUT_HANDLER_HPP

#include <vector>
#include <unordered_map>
#include <array>

#include "ekg/handler/input.hpp"
#include "ekg/io/timing.hpp"

namespace ekg::handler {
  class input {
  protected:
    // @TODO: change from unordored map to linear query
  
    std::unordered_map<std::string, std::vector<bool*>> input_bindings_map {};
    std::unordered_map<std::string, ekg::input_bind_t> input_bind_map {};
    std::unordered_map<std::string, bool> input_map {};
    std::array<char[8], 7> special_keys {};

    std::vector<std::string> special_keys_unit_pressed {};
    std::vector<std::string> double_click_mouse_buttons_pressed {};
    std::vector<std::string> input_released_list {};
    std::vector<bool*> just_fired_input_bind {};

    bool finger_hold_event {};
    bool finger_swipe_event {};
    bool is_special_keys_released {};

    ekg::timing_t double_interact {};
    ekg::timing_t last_time_wheel_was_fired {};
    ekg::timing_t cooldown_wheel {};

    // prevent too many unoptmized use of string allocations
    std::string key_name {};
    std::string string_builder {};

    ekg::input_bind_function_t input_bind_listener_function {};
  public:
    ekg::input_info_t input {};
  protected:
    void complete_with_units(
      std::string &string_builder,
      std::string_view key_name
    );

    bool contains_unit(
      std::string_view label
    );

    bool is_special_key(
      int32_t sdl_key_code
    );
  public:
    void init();
    void quit();
    void poll_event();
    void update();

    void insert_input_bind(
      std::string_view tag,
      std::string_view input
    );

    void erase_input_bind(
      std::string_view tag,
      std::string_view input_to_erase
    );

    void erase_input_bind(
      std::string_view tag
    );

    void set_input_bind_state(
      std::string_view tag,
      bool state
    );

    bool get_input_bind_state(
      std::string_view tag
    );

    void set_input_state(
      std::string_view input,
      bool state
    );

    bool get_input_state(
      std::string_view tag
    );
    
    void set_input_bind_listener(
      ekg::input_bind_function_t input_bind_listener_function
    );
  };
}

#endif
