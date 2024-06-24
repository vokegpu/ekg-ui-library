#include "ekg/ekg.hpp"
#include "ekg/layout/scale.hpp"

float ekg::layout::scale_factor {1.0f};

void ekg::layout::scale_calculate() {
  ekg::vec2 monitor_resolution {ekg::ui::scale};
  ekg::vec2 input_resolution {static_cast<float>(ekg::ui::width), static_cast<float>(ekg::ui::height)};

  if (ekg::ui::auto_scale) {
    ekg::core->p_os_platform->update_monitor_resolution();
    monitor_resolution.x = static_cast<float>(ekg::core->p_os_platform->monitor_resolution[0]);
    monitor_resolution.y = static_cast<float>(ekg::core->p_os_platform->monitor_resolution[1]);
    ekg::ui::scale = {1920.0f, 1080.0f};
    input_resolution = monitor_resolution;
  }

  /**
   * The scale is step-based, each step change the scale, e.g:
   * scale percent interval = 25
   * scale percent = 100 (scale GUI resolution == window size)
   *
   * scale percent in:
   * 0   == 0
   * 25  == 1
   * 50  == 2
   * 75  == 3
   * 100 == 4
   *
   * Then it is divided by 4 (4 is the maximum value)
   * e.g: 2/4 = 0.5f --> 3/4 = 0.75f
   **/

  float base_scale {
    ekg::ui::scale.x * ekg::ui::scale.y
  };

  float monitor_scale {
    monitor_resolution.x * monitor_resolution.y
  };

  float monitor_factor {
    monitor_scale / base_scale
  };

  float monitor_scale_percent {
    monitor_factor * 100.0f
  };
  
  float factor {
    ((input_resolution.x * input_resolution.y) / base_scale) * 100.0f
  };

  factor = (
    roundf(factor / ekg::ui::scale_interval) / (monitor_scale_percent / ekg::ui::scale_interval)
  );

  ekg::layout::scale_factor = ekg_clamp(factor, 0.5f, 2.0f);
}