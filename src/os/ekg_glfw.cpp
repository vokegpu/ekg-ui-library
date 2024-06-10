#include "ekg/os/ekg_glfw.hpp"
#include "ekg/ui/display.hpp"
#include "ekg/ekg.hpp"

ekg::os::glfw::glfw(GLFWwindow *p_glfw_win) {
  this->p_glfw_win = p_glfw_win;

  int32_t w {}, h {};

  /**
   * Sounds an unnecessary resize, but it is invoke a complete swapchain redo,
   * to fix the orthographic matrix neededs calc.
   */

  glfwGetWindowSize(this->p_glfw_win, &w, &h);
  glfwSetWindowSize(this->p_glfw_win, w--, h--);
  glfwSetWindowSize(this->p_glfw_win, w++, h++);
}

void ekg::os::glfw::init() {
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::arrow)]      = glfwCreateStandardCursor(GLFW_CURSOR_NORMAL);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::ibeam)]      = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::wait)]       = glfwCreateStandardCursor(GLFW_CURSOR_NORMAL);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::crosshair)]  = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::wait_arrow)] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::size_nwse)]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::size_nesw)]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::size_we)]    = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::size_ns)]    = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::size_all)]   = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::no)]         = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::hand)]       = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

void ekg::os::glfw::quit() {

}

void ekg::os::glfw::update_monitor_resolution() {
  const GLFWvidmode *p_glfw_vidmode {glfwGetVideoMode(glfwGetPrimaryMonitor())};

  this->monitor_resolution[0] = p_glfw_vidmode->width;
  this->monitor_resolution[1] = p_glfw_vidmode->height;

  this->update_cursor(ekg::system_cursor::arrow);
  ekg::cursor = ekg::system_cursor::arrow;

  glfwGetWindowSize(this->p_glfw_win, &ekg::ui::width, &ekg::ui::height);
}

void ekg::os::glfw::update_cursor(ekg::system_cursor system_cursor) {
  glfwSetCursor(this->p_glfw_win, this->loaded_system_cursor_list[static_cast<uint64_t>(system_cursor)]);
}

void ekg::os::glfw::get_key_name(io_key &key, std::string &name) {
  switch (key.key) {
    case GLFW_KEY_LEFT_CONTROL:
      name = "lctrl";
      break;
    case GLFW_KEY_RIGHT_CONTROL:
      name = "rctrl";
      break;
    case GLFW_KEY_LEFT_SHIFT:
      name = "lshift";
      break;
    case GLFW_KEY_RIGHT_SHIFT:
      name = "rshift";
      break;
    case GLFW_KEY_LEFT_ALT:
      name = "alt";
      break;
    case GLFW_KEY_RIGHT_ALT:
      name = "altgr";
      break;
    case GLFW_KEY_TAB:
      name = "tab";
      break;
    case GLFW_KEY_ESCAPE:
      name = "ESCAPE";
      break;
    case GLFW_KEY_ENTER:
      name = "return";
      break;
    case GLFW_KEY_BACKSPACE:
      name = "BACKSPACE";
      break;
    case GLFW_KEY_INSERT:
      name = "INSERT";
      break;
    case GLFW_KEY_DELETE:
      name = "DELETE";
      break;
    case GLFW_KEY_RIGHT:
      name = "RIGHT";
      break;
    case GLFW_KEY_LEFT:
      name = "LEFT";
      break;
    case GLFW_KEY_DOWN:
      name = "DOWN";
      break;
    case GLFW_KEY_UP:
      name = "UP";
      break;
    case GLFW_KEY_PAGE_UP:
      name = "PAGE_UP";
      break;
    case GLFW_KEY_PAGE_DOWN:
      name = "PAGE_DOWN";
      break;
    case GLFW_KEY_HOME:
      name = "HOME";
      break;
    case GLFW_KEY_END:
      name = "END";
      break;
    case GLFW_KEY_CAPS_LOCK:
      name = "CAPS_LOCK";
      break;
    case GLFW_KEY_SCROLL_LOCK:
      name = "SCROLL_LOCK";
      break;
    case GLFW_KEY_NUM_LOCK:
      name = "NUM_LOCK";
      break;
    case GLFW_KEY_PRINT_SCREEN:
      name = "PRINT_SCREEN";
      break;
    case GLFW_KEY_PAUSE:
      name = "PAUSE";
      break;
    case GLFW_KEY_F1:
      name = "F1";
      break;
    case GLFW_KEY_F2:
      name = "F2";
      break;
    case GLFW_KEY_F3:
      name = "F3";
      break;
    case GLFW_KEY_F4:
      name = "F4";
      break;
    case GLFW_KEY_F5:
      name = "F5";
      break;
    case GLFW_KEY_F6:
      name = "F6";
      break;
    case GLFW_KEY_F7:
      name = "F7";
      break;
    case GLFW_KEY_F8:
      name = "F8";
      break;
    case GLFW_KEY_F9:
      name = "F9";
      break;
    case GLFW_KEY_F10:
      name = "F10";
      break;
    case GLFW_KEY_F11:
      name = "F11";
      break;
    case GLFW_KEY_F12:
      name = "F12";
      break;
    case GLFW_KEY_F13:
      name = "F13";
      break;
    case GLFW_KEY_F14:
      name = "F14";
      break;
    case GLFW_KEY_F15:
      name = "F15";
      break;
    case GLFW_KEY_F16:
      name = "F16";
      break;
    case GLFW_KEY_F17:
      name = "F17";
      break;
    case GLFW_KEY_F18:
      name = "F18";
      break;
    case GLFW_KEY_F19:
      name = "F19";
      break;
    case GLFW_KEY_F20:
      name = "F20";
      break;
    case GLFW_KEY_F21:
      name = "F21";
      break;
    case GLFW_KEY_F22:
      name = "F22";
      break;
    case GLFW_KEY_F23:
      name = "F23";
      break;
    case GLFW_KEY_F24:
      name = "F24";
      break;
    case GLFW_KEY_F25:
      name = "F25";
      break;
    case GLFW_KEY_KP_0:
      name = "KP_0";
      break;
    case GLFW_KEY_KP_1:
      name = "KP_1";
      break;
    case GLFW_KEY_KP_2:
      name = "KP_2";
      break;
    case GLFW_KEY_KP_3:
      name = "KP_3";
      break;
    case GLFW_KEY_KP_4:
      name = "KP_4";
      break;
    case GLFW_KEY_KP_5:
      name = "KP_5";
      break;
    case GLFW_KEY_KP_6:
      name = "KP_6";
      break;
    case GLFW_KEY_KP_7:
      name = "KP_7";
      break;
    case GLFW_KEY_KP_8:
      name = "KP_8";
      break;
    case GLFW_KEY_KP_9:
      name = "KP_9";
      break;
    case GLFW_KEY_KP_DECIMAL:
      name = "KP_DECIMAL";
      break;
    case GLFW_KEY_KP_DIVIDE:
      name = "KP_DIVIDE";
      break;
    case GLFW_KEY_KP_MULTIPLY:
      name = "KP_MULTIPLY";
      break;
    case GLFW_KEY_KP_SUBTRACT:
      name = "KP_SUBTRACT";
      break;
    case GLFW_KEY_KP_ADD:
      name = "KP_ADD";
      break;
    case GLFW_KEY_KP_ENTER:
      name = "return";
      break;
    case GLFW_KEY_KP_EQUAL:
      name = "KP_EQUAL";
      break;
    case GLFW_KEY_LEFT_SUPER:
      name = "LEFT_SUPER";
      break;
    case GLFW_KEY_RIGHT_SUPER:
      name = "RIGHT_SUPER";
      break;
    case GLFW_KEY_MENU:
      name = "MENU";
      break;
    default:
      const char *p_key_name {glfwGetKeyName(key.key, key.scancode)};
      if (p_key_name) {
        name = glfwGetKeyName(key.key, key.scancode);
      }
      break;
  }
}

void ekg::os::glfw::get_special_key(io_key &key, ekg::special_key &special_key) {
  switch (key.key) {
    case GLFW_KEY_LEFT_CONTROL:
      special_key = ekg::special_key::left_ctrl;
      break;
    case GLFW_KEY_RIGHT_CONTROL:
      special_key = ekg::special_key::right_ctrl;
      break;
    case GLFW_KEY_LEFT_SHIFT:
      special_key = ekg::special_key::left_shift;
      break;
    case GLFW_KEY_RIGHT_SHIFT:
      special_key = ekg::special_key::right_shift;
      break;
    case GLFW_KEY_LEFT_ALT:
      special_key = ekg::special_key::left_alt;
      break;
    case GLFW_KEY_RIGHT_ALT:
      special_key = ekg::special_key::right_alt;
      break;
    case GLFW_KEY_TAB:
      special_key = ekg::special_key::tab;
      break;
    default:
      special_key = ekg::special_key::unknown;
      break;
  }
}

const char *ekg::os::glfw::get_clipboard_text() {
  return glfwGetClipboardString(this->p_glfw_win);
}

void ekg::os::glfw::set_clipboard_text(const char *p_text) {
  glfwSetClipboardString(this->p_glfw_win, p_text);

}

bool ekg::os::glfw::has_clipboard_text() {
  return glfwGetClipboardString(this->p_glfw_win) != NULL;
}

uint64_t ekg::os::glfw::get_ticks() {
  return static_cast<uint64_t>(glfwGetTime() * 1000);
}

void ekg::os::glfw_window_size_callback(int32_t w, int32_t h) {
  ekg::ui::width = w;
  ekg::ui::height = h;
        
  ekg::core->p_gpu_api->update_viewport(ekg::ui::width, ekg::ui::height);
  ekg::core->update_size_changed();
}

void ekg::os::glfw_scroll_callback(double dx, double dy) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};
  serialized.event_type = ekg::platform_event_type::mouse_wheel;
  serialized.mouse_wheel_x = static_cast<int32_t>(dx);
  serialized.mouse_wheel_y = static_cast<int32_t>(dy);
  serialized.mouse_wheel_precise_x = dx;
  serialized.mouse_wheel_precise_y = dy;

  ekg::poll_io_event = true;
  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}

void ekg::os::glfw_char_callback(uint32_t codepoint) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};
  serialized.event_type = ekg::platform_event_type::text_input;

  // idk it seems pretty much a workaround, predictable crash if codepoint
  // is larger than 127 (overflow)
  const char c [1] {static_cast<char>(codepoint)};
  serialized.text_input = (c);

  ekg::poll_io_event = true;
  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}

void ekg::os::glfw_key_callback(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};
  std::string a {};

  switch (action) {
  case GLFW_PRESS:
    serialized.event_type = ekg::platform_event_type::key_down;
    serialized.key.key = key;
    serialized.key.scancode = scancode;
    ekg::poll_io_event = true;
    break;
  
  case GLFW_REPEAT:
    serialized.event_type = ekg::platform_event_type::key_down;
    serialized.key.key = key;
    serialized.key.scancode = scancode;
    ekg::poll_io_event = true;
    break;

  case GLFW_RELEASE:
    serialized.event_type = ekg::platform_event_type::key_up;
    serialized.key.key = key;
    serialized.key.scancode = scancode;
    ekg::poll_io_event = true;
    break;
  }

  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}

void ekg::os::glfw_mouse_button_callback(int32_t button, int32_t action, int32_t mods) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};

  /**
   * The mouse button number on GLFW is different from SDL2,
   * but EKG is mainly SDL-based; then the `mouse_button`
   * will convert to SDL code sequences:
   * 
   * (converted) (glfw original button code)
   * 1 0
   * 2 2
   * 3 1
   * 4 3
   * 5 4
   * ... 
   **/

  switch (action) {
  case GLFW_PRESS:
    serialized.event_type = ekg::platform_event_type::mouse_button_down;
    serialized.mouse_button = (1 + (button == 1) + button - (1 * (button == 2)));
    ekg::poll_io_event = true;

    break;

  case GLFW_RELEASE:
    serialized.event_type = ekg::platform_event_type::mouse_button_up;
    serialized.mouse_button = (1 + (button == 1) + button - (1 * (button == 2)));
    ekg::poll_io_event = true;
    break;
  }

  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}

void ekg::os::glfw_cursor_pos_callback(double x, double y) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};
  serialized.event_type = ekg::platform_event_type::mouse_motion;
  serialized.mouse_motion_x = static_cast<float>(x);
  serialized.mouse_motion_y = static_cast<float>(y);

  ekg::poll_io_event = true;
  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}