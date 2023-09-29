
Here you will read these main topics:
- Fundamentals
- Examples
- Methods

# Fundamentals

The listbox (list box) widget can be fundamentally used to create different selectable and recursive items.

The recursive item system works like a node system, each item of a category can have a sub-item.
You can create a select box item setting the attribute of the item.

# Examples

```cpp
auto p_listbox = ekg::listbox("cow", ekg::dock::fill | ekg::dock::next);

p_listbox["animals"].set_text("Animals");
p_listbox["plants"].set_text("Plants");
p_listbox["objects"].set_text("Objects");

auto &animals = p_listbox["animals"];

animals["rina"].set_text("Cow");
animals["rina"].set_value(true);
animals["rina"].set_box_texture(0);
animals["rina"].set_attr(ekg::attribute::box); // select box
animals["rina"].set_attr_box_callable_when(animals.get_id()); // select all

animals["mrmeow"].set_text("Cat");
animals["mrmeow"].set_attr(ekg::attribute::box);
animals["mrmeow"].set_attr_box_callable_when(animals.get_id()); // select all
```