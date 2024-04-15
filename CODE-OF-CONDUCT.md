# Overview

- [Packages](#Packages)
- [Code](#Code)
- [UI](#UI)
- [Immutable UI](#Immutable-UI)
- [Lacunes](#Lacunes)

### Packages

All headers must be inside of `include`; headers-definition are allowed but must be under [1]
some strictly requirements. 

The source code definitions must be inside of `src`, follow the package path, and filename case.

```
include ->
 packages
  declarations
src ->
 packages
  sources
```

Each new package is a namespace, except [2] the `core` and `util` [4];  
declarations must follow a namespace path `ekg::package::something`.

### Code

Note: Some old code may not follow the code of conduct, but it is being refactored.

```cpp
// everything in sneak_case
namespace ekg {} // always use namespace and ekg

// no smart pointers
// pointers start with prefix p_* (except `ekg::core`, check lacune [3])
int32_t *p_int {};

// also always use initialization-list implicit {}
int32_t number {5}; // check lacune [1]

// always use cpp keywords for prevent implicit cast and other behaviors
static_cast<t>

// ifndef, no pragma once
#ifndef EKG_PACKAGE_HEADER_NAME_H
#define EKG_PACKAGE_HEADER_NAME_H

#endif
```

### UI

Creating UI elements requires two importants objects: `ekg::ui::abstract` and `ekg::ui::abstract_widget`.  
The `::abstract` does not has access to the `::abstract_widget`; only `::abstract_widget` contains  
access to the `ekg::ui::abstract`.

```cpp
// include/ui/abstract/ui_abstract.hpp
class abstract_widget {
public:
  ekg::ui::abstract *p_ui {};
};
```

E.g. creating a drop down frame UI element:
- Each setter method, must return the element instance (buildable pattern)

```cpp
// include/ui/frame ui_drop_down_frame.hpp
namespace ekg::ui {...
class drop_down_frame : public ekg::ui::abstract {
public:
  ...

  // buildable pattern, just add at end of function `return this`.
  ekg::ui::drop_down_frame *set_something(foo bar);
};

// include/ui/frame/ui_drop_down_frame_widget.hpp
// then you use the data into widget
class drop_down_frame_widget : public ekg::ui::abstract_widget {
public:
};
```

Registering the widget requires two fundamental steps: 
- Insert the type at enum `ekg::type::drop_down_frame`
- Implement the generator at `ekg::runtime::gen_widget`

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

Letting accessible to the user-programmer, after the widget is registered, must be in `ekg.hpp`:

```cpp
// include/ekg.hpp

// if this UI element is dockable by a container element,
// pre-initialize with `ekg::dock::none`.

/**
 * Docks go here.
 */
ekg::ui::drop_down_frame ekg::drop_down_frame(..., ekg::dock dock = ekg::dock::none);

// src/ekg.cpp
ekg::ui::drop_down_frame ekg::drop_down_frame(...) {
  ekg::ui::drop_down_frame *p_ui {new ekg::ui::drop_down_frame()};

  p_ui->unsafe_set_type(ekg::type::drop_down_frame());
  ekg::core->gen_widget(p_ui);

  p_ui->set_something(something_from_param);
  ...

  return p_ui;
}

```

### Immutable UI

```cpp
// ekg/ekg.cpp
ekg::ui::label *imut_label(std::string_view text) {
  ekg::ui::label *p_ui {new ekg::ui::label()};
  
  p_ui->unsafe_set_immutable(true);
  p_ui->set_text(text);

  return p_ui;
}
```

# Lacunes

- [1] The variable, function and method MUST follow: template, and default initialization value as-a-config.
- [2] The `ekg::core` and `ekg/util/*` is user-programmer accessible, a fundamental part of code that do not require EKG techniques concepts. 
- [3] The `ekg::core` is the unique PTR wich does not has `p_` prefix. The reason, pure design, as lacune [2].
- [4] The `ekg/util/*` package has the liberty of creating any namespace extensions without declare `ekg::util` namespace, e.g. `ekg/util/io.hpp` with an extension of `ekg::input::`.
