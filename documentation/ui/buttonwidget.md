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

### Button

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

Set text [alignment](../layout/layoutmask.md) dock, `center` `left` `right` `top` `bottom`.

```cpp
ekg::ui::button *set_text_align(uint16_t dock);
```

Get text alignment dock.

```cpp
uint16_t get_text_align();
```

### Abstract

Set tag for reserved purposes.

```cpp
ekg::ui::abstract *set_tag(std::string_view tag);
```

Get the tag for reserved purposes.

```cpp
std::string_view get_tag();
```

Add a child to the widget.

```cpp
ekg::ui::abstract *add_child(int32_t id);
```

Get the widget child id list.

```cpp
std::vector<int32_t> &get_child_id_list();
```

Remove a child widget from the mother widget.

```cpp
ekg::ui::abstract *remove_child(int32_t id);
```

Set the widget ID.

```cpp
ekg::ui::abstract *set_id(int32_t id);
```

Get the widget ID.

```cpp
int32_t get_id();
```

Set parent (mother) widget ID.

```cpp
ekg::ui::abstract *set_parent_id(int32_t parent_id);
```

Get parent (mother) widget ID.

```cpp
int32_t get_parent_id();
```

Set the alive state of the widget.

```cpp
ekg::ui::abstract *set_alive(bool is_alive);
```

Get the alive state.

```cpp
bool is_alive();
```

Destroy the widget.

```cpp
void destroy();
```

Set the state of the widget, `visible` `invisible`.

```cpp
ekg::ui::abstract *set_state(const ekg::state &_state);
```

Get the state of the widget, `visible` `invisible`.

```cpp
ekg::state get_state();
```

Set the type of widget, `abstract` `frame` `button` `label` `slider` `slider2d` `checkbox` `textbox` `combobox` `listbox` `tab` `popup` `scroll`.

```cpp
ekg::ui::abstract *set_type(const ekg::type &_type);
```

Get the type of widget, `abstract` `frame` `button` `label` `slider` `slider2d` `checkbox` `textbox` `combobox` `listbox` `tab` `popup` `scroll`.

```cpp
ekg::type get_type();
```

Get the place [dock](../layout/layoutdock.md), `fill` `next` `none` `right` `bottom`.

```cpp
uint16_t get_place_dock();
```

Get the sync flags reference.

```cpp
uint16_t &get_sync();
```

Reset the UI data front-end with the widget back-end.

```cpp
void reset();
```

Access to absolute widget back-end rectangle.

```cpp
ekg::rect &widget();
```

Get the UI data front-end rectangle.

```cpp
ekg::rect &ui();
```

Set scaled height layout in [font](../draw/fontrenderer.md) size (multiplied by).

```cpp
ekg::ui::abstract *set_scaled_height_layout(int32_t scaled_size);
```

Check if the widget has a parent (mother widget).

```cpp
bool has_parent();
```

Check if the widget has children.

```cpp
bool has_children();
```