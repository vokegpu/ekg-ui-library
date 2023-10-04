# Contributing

Check how the widgets work, the UI class (not the widget) is reserved for the user only.  
The widgets class uses the UI as layer data.

Example:
```cpp
namespace ekg::ui {...

class drop_down_frame : public ekg::ui::abstract {
public:
  ...
};

// then you use the data into widget
class drop_down_frame_widget : public ekg::ui::abstract_widget {
public:
};

ekg::ui::drop_down_frame_widget *p_widget {new ...};
p_widget->ui = new ekg::ui::drop_down_frame();
```

# Code of Conduct

Note: Some old code maybe is not following the code of conduct, but it is being refactored.

```cpp
// everything in sneak_case
namespace ekg {} // always use namespace and ekg

// no smart pointers
// pointers start with prefix p_*
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
