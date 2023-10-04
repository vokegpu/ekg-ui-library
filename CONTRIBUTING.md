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

Please follow the syntax and patterns:
```cpp
// everything in sneak_case
namespace ekg {} // always use namespace and ekg
```
