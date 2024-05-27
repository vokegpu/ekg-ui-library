# Welcome 🐈

EKG purpose is simplicity way to create fency reitaned high-performance GUIs, always looking for low-specs.  
This readme is not updated. Soon.

[Check documentation here!](https://vokegpu.github.io/ekg-docs/)

---

![Image Text](/splash/splash-shocase-3.0-light-theme.png?raw=true)
![Calc](/splash/splash-calc-showcase.png?raw=true)

[showcase clip](https://github.com/gamedevs-do-apocalipse/gamedevs-do-apocalipse/assets/37088203/5f0cd227-e50d-495c-81a7-76e6155305eb)  
Background song: GUNNM OST 

Check the [development updates here](https://github.com/orgs/gamedevs-do-apocalipse/discussions/5).  
EKG is recognised as an open-source project by [JetBrains](https://www.jetbrains.com/) 🖤  
<a href="https://www.jetbrains.com"> <img src="https://resources.jetbrains.com/storage/products/company/brand/logos/jb_square.png" width="100" height="100" alt="JetBrains Black Box Logo logo."> <a/>

# Get Started 🐈‍⬛

The EKG supports natively two APIs: OpenGL, and ~~Vulkan~~. Vulkan is not supported yet, but soon.

Be sure you compile together all these three libs:  
[GLEW](https://glew.sourceforge.net/)/[Vulkan](https://www.vulkan.org/)/GLES3, [SDL2](https://www.libsdl.org/), and [Freetype](https://freetype.org/).

The order of the linker is important too, be sure you link EKG before the required libs.
[If you are on Android NDK](https://github.com/vokegpu/pompom), please replace GLEW with GLES3.

```
# cmake example
add_executable(your-project-name ekg SDL2main SDL2 freetype glew32)

# makefile example
cxx ... -lekg -lSDL2main -lSDL2 -lfreetype -lglew32
```

C++ compiler(s) support: GCC, MinGW32, MinGW64, Clang  
C++ std version: 17
  
Library output path:   
`lib/win32/libekg.a` Windows x86_x64  
`lib/linux/libekg.a` Linux x86_x64  
`ANDROID_ABI/libekg.a` Android armeabi-v7a arm64-v8a x86 x86_64  

# Example

EKG source code provides a demo application called Pompom under `test/build/OS` folder.

```c++
#include <ekg/ekg.hpp>

int32_t main(int32_t, char**) {
  // create SDL2 window and OpenGL context.
  //... init ekg.
  ekg::runtime ekg_runtime {};
  ekg::init(&ekg_runtime, sdl_window, "./font.ttf");
  
  // create GUI elements here or somewhere in the code, but once tick.
  ekg::frame("fps frame", {20, 20}, {200, 200});
  auto label_fps = ekg::label("fps", ekg::dock::fill)->set_text_align(ekg::dock::center);
  
  /*
   * mainloop of your application/game.
   */
  while (running) {
    // reduce CPU ticks and sync_flags with refresh rate...
    while (SDL_PollEvents(sdl_event)) {
      // handle the events here.
      // remember to poll ekg reset events:
      ekg::poll_event(sdl_event);
    }
    
    if (/* check timing each 1s to update display fps */) {
      display_fps = frames_elapsed;
      frames_elapsed = 0;
      label_fps->set_text(std::to_string(display_fps));
    }
    
    // remember to set delta time ticks of ekg.
    // this is very important to UI animations. 
    ekg::dt = 0.016f; // running 60 fps -> t = 1000 / 60 -> dt = t / 100;
    
    // the update section (it comes before render segment).
    ekg::update();
    
    // now you can clear the buffers using OpenGL and enable render section of ekg.
    // ...
    ekg::render();
  }
  
  return 0;
}
```

A simple example for creating textbox(s), buttons, and more widgets!

```cpp
auto frame = ekg::frame("lê rect", {20, 20}, {400, 400});

frame->set_drag(ekg::dock::top);
frame->set_resize(ekg::dock::left | ekg::dock::right | ekg::dock::bottom);

ekg::button("pompom click!!", ekg::dock::fill);
ekg::slider("pompok number!!", 0.34f, 0.11f, 0.934f, ekg::dock::fill | ekg::dock::next)->set_precision(4);
ekg::textbox("Lê textbox", "hiii", ekg::dock::fill | ekg::dock::next)->set_scaled_height(6);

ekg::label("RGB:", ekg::dock::next);

auto r = ekg::slider("red", 1.0f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);
auto g = ekg::slider("green", 0.69f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);
auto b = ekg::slider("blue", 1.0f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);

ekg::scroll("scroll minecraft");
ekg::popgroup();
```

![Image Text](/splash/clean-white-theme-showcase-code-6.png?raw=true)

Here how to create popup(s)!

```cpp
while (SDL_PollEvent(&sdlevent)) {
  ekg::task(sdl_event);

  if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
    // \t  add line separator
    auto main = ekg::popup("file", {"\tAdd", "Cut", "Copy", "\tPaste", "\tSelect All", "Actions"});
    auto three = ekg::popup("file-add", {"Cube", "Plane", "Sphere", "Hexagon", "Hexagon"});
    auto hexagon = ekg::popup("file-add-hexagon", {"Tree D", "Plane", "Double Pairs Daggers"});
    auto game = ekg::popup("file-actions", {"Reload Clock", "Flush"});
  
    if (main) { // not null
      main->link("Add", three);
      main->link("Actions", game);
      three->link("Hexagon", hexagon);
    }
  }
}
```

![Image Text](/splash/ekg-2-showcase-popup.png?raw=true)

---

# Widgets

At total, there is 40% of the widgets done:
- Button
- Checkbox
- Frame
- Label
- Popup
- Scroll
- Slider/ProgressBar
- Textbox/Entryfield

```
@copyright 2024 - Rina Wilk - VokeGpu
```
