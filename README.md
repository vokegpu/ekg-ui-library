# Welcome 🐈

---

The purpose of EKG is to be fully customizable, smooth, and with many animations. Also, perform well with low pc specs. This is the new core version of EKG, a rewrite of the legacy core.  
If you want to know more about the EKG development progress I recommend you peek:
https://github.com/orgs/gamedevs-do-apocalipse/discussions/5

![Image Text](/splash/splash-shocase-3.0-light-theme.png?raw=true)
![Calc](/splash/splash-calc-showcase.png?raw=true)

https://github.com/gamedevs-do-apocalipse/gamedevs-do-apocalipse/assets/37088203/5f0cd227-e50d-495c-81a7-76e6155305eb
Background song: GUNNM OST 

# Get Started 🐈‍⬛

The EKG supports natively two APIs: OpenGL, and ~~Vulkan~~. Vulkan is not supported yet, but soon.

Be sure you compile together all these three libs:  
![GLEW](https://glew.sourceforge.net/)/![Vulkan](https://www.vulkan.org/), ![SDL2](https://www.libsdl.org/), and ![Freetype](https://freetype.org/).

The order of the linker is important too, be sure you link EKG first before the required libs.
![If you are on Android NDK](https://github.com/vokegpu/pompom), please replace GLEW with GLES3.

```cmake
# cmake example
add_executable(your-project-name ekg SDL2main SDL2 freetype glew32)
```
```makefile
cxx ... -lekg -lSDL2main -lSDL2 -lfreetype -lglew32
```

C++ compiler(s) support: GCC, MinGW32, MinGW64, Clang  
C++ std version: 17  
Library output path:   
`lib/win32/libekg.a` - Windows x86_x64  
`lib/linux/libekg.a` - Linux x86_x64  
`ANDROID_ABI/libekg.a` - Android armeabi-v7a arm64-v8a x86 x86_64  

# Example

EKG source code provides a demo application called Pompom under `test/build/OS` folder.

```c++
#include <ekg/ekg.hpp>

int32_t main(int32_t, char**) {
  // create SDL2 window and OpenGL context.
  //... init ekg.
  ekg::init(sdl_window, "./font.ttf");
  
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
      ekg::event(sdl_event);
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

![Image Text](https://github.com/vokegpu/ekg-ui-library/blob/version-core/splash/ekg-2-showcase-popup.png?raw=true)

```cpp
while (SDL_PollEvent(&sdlevent)) {
  ekg::event(sdl_event);

  if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
    auto main = ekg::popup("file", {"---Add", "Cut", "Copy", "---Paste", "---Select All", "Actions"});
    auto three = ekg::popup("file-add", {"Cube", "Plane", "Sphere", "Hexagon", "Hexagon"});
    auto hexagon = ekg::popup("file-add-hexagon", {"Tree D", "Plane", "Double Pairs Daggers"});
    auto game = ekg::popup("file-actions", {"Reload Clock", "Flush"});
  
    if (main != nullptr && three != nullptr) {
      main->append_linked("Add", three);
      main->append_linked("Actions", game);
      three->append_linked("Hexagon", hexagon);
    }
  }
}
```

There are more things, these steps are the official way to set up.

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

@VokeGpu 2023 All rights reserved.
