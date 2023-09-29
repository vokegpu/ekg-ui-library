# Overview

Here you will read these main topics:
- Fundamentals
- Docking
- Widgets

# Fundamentals

The layout mask or docking system, is an internal feature of the layout service, layout mask is a mask of positions between rectangles of a widget. The purpose of having layout mask is to make flexibility widgets.

# Docking

The acceptable docking positions are: `center, left, right, top, bottom`
Each docking direction has it own behavior.

# Widgets

All widgets use the layout mask feature, normally you can set by `set_*_align`.
e.g [button widget](documentation/ui/Button Widget.md) align method:

```cpp
p_button->set_text_align(ekg::dock::center);
```