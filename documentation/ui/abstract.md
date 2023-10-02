# Overview

Here you will read these topics:
- Fundamentals
- Methods
# Fundamentals

The abstract widget is not used for the user-programmer part and is not recommended for any purpose. Abstract widget is used by the runtime core as a polymorphism heritage base for handling everything. 
# Methods

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