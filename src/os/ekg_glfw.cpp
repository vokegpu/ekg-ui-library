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
  this->loaded_system_cursor_list[static_cast<uint64_t>(ekg::system_cursor::no)]         = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
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
    default:
      name = glfwGetKeyName(key.key, key.scancode);
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
  serialized.text_input = glfwGetKeyName(codepoint, 0);

  ekg::poll_io_event = true;
  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event();
  ekg::poll_io_event = false;
}

void ekg::os::glfw_key_callback(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};

  switch (action) {
  case GLFW_PRESS:
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

  switch (ekg::poll_io_event) {
    case true:
      ekg::cursor = ekg::system_cursor::arrow;
      ekg::core->process_event();
      ekg::poll_io_event = false;
      break;
  }
}

void ekg::os::glfw_mouse_button_callback(int32_t button, int32_t action, int32_t mods) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};

  switch (action) {
  case GLFW_PRESS:
    serialized.event_type = ekg::platform_event_type::mouse_button_up;
    serialized.mouse_button = button;
    ekg::poll_io_event = true;
    break;

  case GLFW_RELEASE:
    serialized.event_type = ekg::platform_event_type::mouse_button_up;
    serialized.mouse_button = button;
    ekg::poll_io_event = true;
    break;
  }

  switch (ekg::poll_io_event) {
    case true:
      ekg::cursor = ekg::system_cursor::arrow;
      ekg::core->process_event();
      ekg::poll_io_event = false;
      break;
  }
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