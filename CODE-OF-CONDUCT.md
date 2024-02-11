# Contributing

All headers must be inside of `include`; headers-definition are allowed but must be under [1]
some strictly requirements. 

The source code definitions must be inside of `src`, and must follow the package path, and filename case.

```
include ->
 packages
  declarations
src ->
 packages
  sources
```

Each new package is a namespace, except the `core` and `util` [2];  
declarations must follow a namespace path `ekg::package::something`.

Note: Some old code may not follow the code of conduct, but it is being refactored.

```cpp
// everything in sneak_case
namespace ekg {} // always use namespace and ekg

// no smart pointers
// pointers start with prefix p_* (except `ekg::core`, check lacunes [3])
int32_t *p_int {};

// also always use initialization-list implicit {}
int32_t number {5};

// always use cpp keywords for prevent implicit cast and other behaviors
static_cast<t>

// ifndef, no pragma once
#ifndef EKG_PACKAGE_HEADER_NAME_H
#define EKG_PACKAGE_HEADER_NAME_H

#endif
```

### UI elements

Creating UI elements requires two importants objects, `ekg::ui::abstract` and `ekg::ui::abstract_widget`.  
The `ekg::ui::abstract` does not has access to the `ekg::ui::abstract_widget`; only `ekg::ui::abstract_widget` contains  
contains access to the `ekg::ui::abstract`. E.g. creating a drop down frame UI element:

```cpp
// include/ui/frame ui_drop_down_frame.hpp
namespace ekg::ui {...

class drop_down_frame : public ekg::ui::abstract {
public:
  ...
};

// include/ui/frame/ui_drop_down_frame_widget.hpp
// then you use the data into widget
class drop_down_frame_widget : public ekg::ui::abstract_widget {
public:
};
```

Initially you create the `ekg::ui::abstract`, which gonna be accessible to  
the user-programmer (the programmer who uses the lib). Then, you need to implement the `ekg::ui::abstract_widget` registration,  
in `ekg::runtime::gen_widget`, and the type in enum struct `ekg::type::drop_down_frame`.

```cpp
// include/ui/abstract/ui_abstract.hpp
namespace ekg {
  enum class type {
    ...,
    ...,
    drop_down_frame, // sneak_case, not SCREAM_SNEAK_CASE
    ...,
  }

// src/core/runtime.cpp
void ekg::runtime::gen_widget(ekg::ui::abstract *p_ui) {
  switch (p_ui->get_type()) {
    case ekg::type::drop_down_frame: {
      ekg::ui::drop_down_frame_widget *p_widget {new ekg::ui::drop_down_frame_widget()};
      p_widget->p_data = p_ui;
      p_widget_created = p_widget;
      append_group = true; // check if you want append directly to the frame
      break;
    }

    case ... {
      ...
      break;
    }

    ...
  }
}
```

# Lacunes
- [1] The variable, function and method MUST follow: template, and default initialization value as-a-config.
- [2] The `ekg::core` and `ekg::util` is user-programmer accessible, a fundamental part of code that do not require EKG techniques concepts. 
- [3] The `ekg::core` is the unique PTR wich does not has `p_` prefix. The reason, pure design, as lacune [2].