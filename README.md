The official runtime ekg ui library source code.

---

The purpose of ekg is being fully customizable, smooth and with many animations. Also perform good in low pc specs. This is the new core version of ekg, rewrite of legacy core. 

# Running

C++ compiler(s) support: GNU  
Library output path:   
`lib/win32/libekg.a` - Windows 32/64bits  
`lib/linux/libekg.a` - Linux x86_x64

# Setup

First move `include/` to compiler include path or set in cmake/makefile/other project directory, then you need to link the library.  
Note that you need to link all dependencies that come with ekg: ![SDL2](https://www.libsdl.org/), ![GLEW](https://glew.sourceforge.net/) and ![FreeType](http://freetype.org/).

```c++
#include <ekg/ekg.hpp>

int32_t main(int32_t, char**) {
  // create SDL2 window and OpenGL context.
  //... init ekg.
  ekg::init(sdl_window);
  
  // create GUI elements here or somewhere in the code, but once tick.
  ekg::frame("fps frame", {20, 20}, {200, 200});
  auto label_fps = ekg::label("fps", ekg::dock::left | ekg::dock::top);
  
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
    ekg::dt = 0.16f; // running 60 fps -> t = 1000 / 60 -> dt = t / 100;
    
    // the update section (it comes before render segment).
    ekg::update();
    
    // now you can clear the buffers using OpenGL and enable render section of ekg.
    // ...
    ekg::render();
  }
  
  return 0;
}
```

```cpp  
for (int i {}; i < 1; i++) {
  auto frame {ekg::frame("hello", {80, 80}, {200, 200})};
  frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);
  frame->set_drag(ekg::dock::top);

  ekg::label("DeathWishes.com", ekg::dock::left | ekg::dock::top);
  ekg::checkbox("", ekg::dock::left | ekg::dock::top)->set_box_align(ekg::dock::center | ekg::dock::left);
  ekg::button("Register", ekg::dock::left | ekg::dock::top | ekg::dock::next);
  ekg::button("Login", ekg::dock::left | ekg::dock::top | ekg::dock::next);
  ekg::button("User List", ekg::dock::left | ekg::dock::top | ekg::dock::next);
  ekg::button("Exit", ekg::dock::left | ekg::dock::top | ekg::dock::next)->set_callback(new ekg::cpu::event {"exit-callback", nullptr, [](void *pdata) {
    SDL_Event custom_sdl_event {};
    custom_sdl_event.type = SDL_QUIT;
    SDL_PushEvent(&custom_sdl_event);
  }});
}
```

![Image Text](https://github.com/vokegpu/ekg-ui-library/blob/version-core/splash/splash-showcase-4-1.0.0.png?raw=true)

There is more things, theses steps are the official way to setup.

# Services

The theme store the current colors scheme loaded, you can use the default or load one theme file. One of important objectives is make the UI looks pretty and smooth in mobile devices.

Input manager handle all bindings of dimension element, you can tag combinations of different inputs.  
When pressing units keybinds (special keys + random keyboard key) you need to write in the correct pattern: ctrl -> shift -> tab; E.g "lshift+tab+b"; l (left) r (right).  
```c++
ekg::input::bind("custom-tag-a", "mouse-left");
ekg::input::bind("custom-tag-a", "mouse-left-double");
ekg::input::bind("custom-tag-b", "r");
ekg::input::bind("custom-tag-b", "lctrl+a"); // also be sure you are keybinding in correct pattern.
ekg::input::bind("custom-tag-c", "r");
ekg::input::bind("hii!! :))", "mouse-5-double"); // multiples buttons of mouse.

// ... any place of your code:
if (ekg::input::pressed("custom-tag-a")) {
  ekg::log("you pressed a!");
}

if (ekg::input::pressed("custom-tag-b")) {
  ekg::log("you pressed b!");
}

if (ekg::input::pressed("custom-tag-c")) {
  ekg::log("you pressed a!");
}

if (ekg::input::pressed("hii!! :))")) {
  ekg::log("hii!");
}
```

# Buffering

`ekg` library automatically sets the OpenGL API, if your project is running in NDK the ekg sets to OpenGL ES, for high performance applications sets OpenGL 4 or 3.

All buffers into gpu are handled and optimized by `ekg::gpu::allocator`, allocator protects your gpu from multiples buffers dispatch, two buffers based interact with high shading communication making a partial instanced rendering.

---

@VokeGpu 2023 All rights reserved.
