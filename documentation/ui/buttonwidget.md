# Overview

Here you will read these main topics:
- Fundamentals
- Examples
- Methods
# Fundamentals

The button widget works fundamentally simply, you have a text, and the alignment of text. Each button has its own optional `ekg::cpu::event` callback event [task](../core/eventtask.md), where you can add one lambda function.

The button widget contains two strings sets `set_text` and `set_tag`, `set_tag` is reserved for user purposes, while `set_text` is the visible text of the widget button.

# Examples

For creating a simple button with no callback event.

```cpp
auto p_button = ekg::button("cat", ekg::dock::fill | ekg::next);
```

The first parameter is the visible text of a button. The second parameter is the docking alignment in the frame widget.

To create a callback button, with [tasks](../core/eventtask.md).

```cpp
p_button->set_callback(new ekg::cpu::event("cat", nullptr, [](void *p_callback) {
	ekg::log() << "Cat!";
}));
```

This callback event is not deleted after execution, because it is batched, which means that all clicks will execute this [task](../core/eventtask.md).

# Methods

#### Button

Set the [font](../draw/fontrenderer.md) size of button text, `ekg::font::small` `ekg::font::normal` `ekg::font::big`.

```cpp
ekg::ui::button *set_font_size(ekg::font font);
```

Get the [font](../draw/fontrenderer.md) size of button text, `ekg::font::small` `ekg::font::normal` `ekg::font::big`.

```cpp
ekg::font get_font_size();
```

Set [dock](../layout/layoutdock.md) place, `fill` `next` `none` `right` `bottom`.

```cpp
ekg::ui::button *set_place(uint16_t dock);
```

Set dimension width in pixels.

```cpp
ekg::ui::button *set_width(float w);
```

Get dimension width in pixels.

```cpp
float get_width();
```

Set dimension height in [font](../draw/fontrenderer.md) size scale (multiplied by).

```cpp
ekg::ui::button *set_scaled_height(int32_t h);
```

Get dimension height in [font](../draw/fontrenderer.md) size scale (multiplied by).

```cpp
int32_t get_scaled_height();
```

Get dimension height in pixels.

```cpp
float get_height();
```

Set callback event [task](../core/eventtask.md).

```cpp
ekg::ui::button *set_callback(ekg::cpu::event *p_callback);
```

Get callback event [task](../core/eventtask.md).

```cpp
ekg::cpu::event *get_callback();
```

Set display text.

```cpp
ekg::ui::button *set_text(std::string_view text);
```

Get display text.

```cpp
std::string_view get_text();
```

Set button state pressed/callback.

```cpp
ekg::ui::button *set_value(bool state);
```

Get button state.

```cpp
bool get_value();
```

Set text alignment dock.

```cpp
ekg::ui::button *set_text_align(uint16_t dock);
```

Get text alignment dock.

```cpp
uint16_t get_text_align();
```

#### Abstract

Set tag for reserved purposes.

```cpp
ekg::ui::abstract *set_tag(std::string_view tag);
```

Get the tag for reserved purposes.

```cpp
std::string_view get_tag();
```

