# Overview

Here you will read these main topics:
- Fundamentals
- Docking
- Widgets

# Fundamentals

The layout mask or docking system, is an internal feature of the layout service, A layout mask is a mask of positions between rectangles of a widget. The purpose of having a layout mask is to make flexible widgets.
# Docking

The acceptable docking positions are: `center, left, right, top, bottom`
Each docking direction has it is own behaviour.
# Widgets

All widgets use the layout mask feature, normally you can set by `set_*_align`.
e.g. [button widget](button.md) align method:

```cpp
p_button->set_text_align(ekg::dock::center);
```
