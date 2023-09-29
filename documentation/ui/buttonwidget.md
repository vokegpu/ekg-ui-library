# Overview

Here you will read these main topics:
- Fundamentals
- Examples
- Methods
# Fundamentals

The button widget works fundamentally simply, you have a text, and the alignment of text. Each button has its own optional `ekg::cpu::event` callback event, where you can add one lambda function.

The button widget contains two strings sets `set_text` and `set_tag`, `set_tag` is reserved for user purposes, while `set_text` is the visible text of the widget button.

# Examples

For creating a simple button with no callback event.
```cpp
auto p_button = ekg::button("cat", ekg::dock::fill | ekg::next);
```

The first parameter is the visible text of a button. The second parameter is the docking alignment in the frame widget.