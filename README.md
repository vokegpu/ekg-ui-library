The official runtime ekg dimension library source code.

---

The purpose of ekg is being fully customizable, smooth and with many animations. Also perform good in low pc specs. This is the new core version of ekg, rewrite of legacy core. Images bellow showcasing the current state of ekg.  

![Image Text](https://github.com/ekg-reset-library/ekg/blob/version-core/splash/splash-showcase-1-1.0.0.png.png?raw=true)
# Running

The current version of ekg only support the MinGW64 compiler.  
For use the library on your project, download the repo or release, then you add include and lib files:  
`lib/win32/libekg.a` - Windows 32/64bits  
`lib/linux/libekg.a` - Linux x86_x64

# Setup

To setup ekg in your project, first link the library and after put `include/` in your compiler include directory or your project directory.  
Note that you need to link the dependencies used in ekg: ![SDL2](https://www.libsdl.org/), ![GLEW](https://glew.sourceforge.net/) and ![FreeType](http://freetype.org/).

```c++
#include <ekg/ekg.hpp>

int32_t main(int32_t, char**) {
  // create SDL2 context and OpenGL context.
  //... init ekg reset library.
  ekg::init(sdl_window);
  
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

  ekg::popgroup();
  
  // do samething to create new dimension.
  
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

![Image Text](https://github.com/ekg-reset-library/ekg/blob/version-core/splash/splash-showcase-3-1.0.0.png.png?raw=true)

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

All buffers swap into GPU are totally handled and optimized by `ekg::gpu::allocator`, it protects your gpu from multiples buffers dispatch, it only uses two buffers and implement high shading communication making a partial instanced rendering.

The scale manager works with dynamic offsets, for better interactions it automatically sets the scale of min_offset and button sizes (if auto-scale line_thickness is enabled), other service of scale manager is set dimension position based on root display, small screens like smartphones, reduce the bounds interaction between mother/master parent and others dimension.

---

@VokeGpu 2022 All rights reserved.
