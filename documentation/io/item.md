# Overview

You will read these topics:
- Item collector

# Item Collector

The EKG library contains an item collector named `ekg::item`, the item is a recursive vector collection. However, the item does not have direct iterator methods accessible.

The item contains four fields:
- `value` the display text of a UI widget.
- `state` the current state of a UI widget, such as `hovered`, or `selected`.
- `attr` the attribute of the item, sets the style and produces different features depending on the widget, such as `ekg::attr::box`, and `ekg::attr::separator`.
- `child_list` the children of the current item.

Setting a value to an item is simple: 

```cpp
ekg::item item("animals"); // constructor explicit
ekg::item item {"animals"}; // initialization-list implicit
ekg::item item = {"animals"}; // assign initialization-list implicit

ekg::item item {}; // initialization-list implicit
item.value = "animals"; // accessing the field, but not too recomendded
```

Item contains attr (attributes) flags and state flags, both manually configurable or automatically by setting the value component special chars.

Special chars:
- `\t` add automatically the attribute `ekg::attr::separator` to item.
- `\\` add automatically the attribute `ekg::attr::box` to item.

```cpp
ekg::item item {"Animals"};
item.set_value("\t\\Select All");

// insert a std::vector<std::string> to item.
item.insert({"\\Dogs"});
item.insert({"\\Cats"});
item.insert({"\\Humans"});
```

You can iterate over the item children using for loop:

```cpp
ekg::item item("animals");
item.insert({"dogs", "cats", "humans"})

for (ekg::item &items : item) {
	std::cout << items.value << std::endl;
}
```