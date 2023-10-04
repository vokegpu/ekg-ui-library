# Overview

You will read these topics:
- Fundamentals
- Examples
- Methods
# Fundamentals

The EKG library contains an item collector named `ekg::item`, the item is a recursive vector collection. However, the item does not have direct iterator methods accessible.
# Examples

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

Item contains attr (attributes) flags and state flags, both manually configurable or automatically by setting the value component with special chars.

Special chars:
- `\t` add automatically the attribute `ekg::attr::separator` to the item.
- `\\` add automatically the attribute `ekg::attr::box` to the item.

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
item.insert({"dogs", "cats", "humans"});

for (ekg::item &items : item) {
	std::cout << items.value << std::endl;
}
```

# Methods

Set value with attributes check, use this instead of setting directly the field. 

```cpp
void set_value(std::string_view _value);
```

Initialize an item child without generating a copy.

```cpp
ekg::item &emplace_back();
```

Insert an item copy in the child list.

```cpp
void push_back(const ekg::item &item);
```

Insert an item by setting the value.

```cpp
void push_back(std::string_view item_value);
```

Insert a `std::vector<ekg::item>` child list.

```cpp
void insert(const std::vector<std::string> &item_value_list);
```

Get the child item by index position.

```cpp
ekg::item &at(uint64_t index);
```

Check if has item children.

```cpp
bool empty() const;
```

Get the size of the child list.

```cpp
uint64_t size() const;
```

Get begin iterator point.

```cpp
std::vector<ekg::item>::iterator begin();
```

Get end iterator point.

```cpp
std::vector<ekg::item>::iterator end();
```

Get const begin iterator point.

```cpp
std::vector<ekg::item>::const_iterator cbegin() const;
```

Get const end iterator point.

```cpp
std::vector<ekg::item>::const_iterator cend() const;
```