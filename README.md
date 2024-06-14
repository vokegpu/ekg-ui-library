# 🐄 EKG 🐈

EKG is a reitaned UI-toolkit/UI library to create fancy high-perfmance GUIs, always looking for low-specs hardware.

Documentation: [here](https://vokegpu.github.io/ekg-docs/).  
Development updates: [here](https://github.com/orgs/gamedevs-do-apocalipse/discussions/5).

Please before cloning EKG, insert `--depth 1` flag, or you will clone almost 1gb (yeah this is bizzare, soon EKG MUST fix it filtering the repository).

![Image Text](/splash/showcase-cow.png?raw=true)

EKG is recognised as an open-source project by [JetBrains](https://www.jetbrains.com/) 🖤  
<a href="https://www.jetbrains.com"> <img src="https://resources.jetbrains.com/storage/products/company/brand/logos/jb_square.png" width="100" height="100" alt="JetBrains Black Box Logo logo."> <a/>

# Rendering & Platform Support

The graphics engine is implemented over a RHI-like (Rendering Hardware-Interface-like); supporting multi-API(s) (OpenGL, OpenGL ES3, Vulkan, etc). EKG implements some object-state based concepts, like in the initialization process and sampler creation, due the Khronos API high-performance Vulkan support.

| Name | Support |
| --- | --- |
| OpenGL | Stable, minimum version required 3.3 with [GLEW](https://glew.sourceforge.net/), [example project](https://github.com/vokegpu/ekg-demos/blob/SDL/src/applicationc.cpp) |
| OpenGL ES | Stable, minimum version required ES 3 with [NDK](https://github.com/android/ndk), [example project](https://github.com/vokegpu/pompom) |
| Vulkan | Not implemented |
| Metal | Not implemented, maybe implemented with [MoltenVK](https://github.com/KhronosGroup/MoltenVK) |
| DirectX 12 | Not implemented, perharps not even be implemented |
| DirectX 11 | Not implemented, perharps not even be implemented |

The OS (Android, Windows and Linux) platform (SDL, GLFW, Win32, Wayland, etc) support also used an interface.

| Name | Support | OS |
| --- | --- | --- |
| [SDL](https://www.libsdl.org/) | [Stable example code](https://github.com/vokegpu/ekg-demos/blob/SDL/src/application.cpp) | Linux, Windows & Android |
| [GLFW](https://www.glfw.org/) | [Stable example code](https://github.com/vokegpu/ekg-demos/blob/main/GLFW/src/application.cpp) | Linux & Windows |
| Win32 | Not implemeneted | ? |
| Wayland | Not implemented | ? |

I can not implement everything alone, so, if you want contribute writing these APIs and platform, be welcome.  
Check the [code of conduct](CODE-OF-CONDUCT.md) and 🖤 !

See more about [EKG-demos](https://github.com/vokegpu/ekg-demos/).

# Getting Started 🐈‍⬛

Initializing EKG for SDL and OpenGL is simple.

```cpp
#include <ekg/ekg.hpp>
#include <ekg/os/ekg_opengl.hpp>
#include <ekg/os/ekg_sdl.hpp>

int32_t main(int32_t, char**) {
  // initialize SDL window
  // initialize OpenGL context and GLEW

  /**
   * Runtime property is object-state based,
   * because of Vulkan support, but it does not require a large configutarion template.
   * 
   * The font rendering requires some explains:
   * - EKG font rendering is "maximum" optimized to fast rendering, so Kanjis or any char
   * far from 256 unicode value does not have a decent implementation.
   * - Only Emojis chars are implemented on font rendering.
   * 
   * Soon EKG should add Kanji and some chars support, but for now, only emoji and text is ok.
   * You can read more about how EKG font rendering works here: <yet no link> 
   **/
  ekg::runtime_property ekg_runtime_property {
    .p_font_path = "./local_font.ttf.otf",
    .p_font_path_emoji = "./local_optional_emoji_support.ttf.otf",
    .p_gpu_api = new ekg::os::opengl(), // check documentation for Vulkan initialization (not done yet :c)
    .p_os_platform = new ekg::os::sdl(p_sdl_win)
  };

  ekg::runtime ekg_runtime {};
  ekg::init(&ekg_runtime, ekg_runtime_property);

  // now EKG is initialized and done to be used.

  while (running_loop) {
    while (SDL_PollEvent(&sdl_event)) {
      /**
       * The EKG for SDL is different from other(s) platform implementations.
       * Each platform may perform with some "overheads", as in the case of GLFW.
       * GLFW does not have a loop-statement place like SDL to poll events.
       **/
      ekg::os::sdl_poll_event(&sdl_event);
    }

    /**
     * Note: These next functions are aside from platform interface implementation,
     * only events poll is necessary an interface.
     **/

    // The delta time (framerate interval time between new and old frame loop).
    ekg::ui::dt = 0.016f; // you need to calculate by yourself.

    /**
     * You are able to add multi-thread support to EKG
     * invoking this function on a different thread.
     * But I do not recommend it.
     **/
    ekg::update();

    // Now on rendering loop section you must invoke EKG render function.
    ekg::render();
  }
}
```

# UI Elements Creation and Configuration

The EKG is not an immediate UI library, then you need to build once widgets and tasks,  
you can create widgets on the application initialization or any other place at once.

Building widgets support builder-concept, so you have free to customize a lot the widget. 

```cpp

ekg::frame("meow", ekg::vec2(200, 200), ekg::vec2(200, 200))
  ->set_drag(ekg::dock::top)
  ->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

ekg::label("ekg sampler gui?", ekg::dock::fill);
ekg::button("click here bu", ekg::dock::fill | ekg::dock::next);

ekg::textbox("meow", "🐄 EKG 🐈", ekg::dock::fill | ekg::dock::next)
  ->set_scaled_height(3);

ekg::label("meow", ekg::dock::next);
ekg::slider("dragging the life", 0.0f, 0.0f, 666.0f, ekg::dock::fill)
  ->set_text_align(ekg::dock::center);

ekg::checkbox("checkbox like a 🐈‍⬛", true, ekg::dock::fill | ekg::dock::next);
ekg::scrollbar("scrollbar omg");

// not necessary unless you create a second frame that you do not want to dockenize on it.
ekg::pop_group(); 
```

![showcase-sampler-gui](/splash/ekg-sampler-gui.png?raw=true)

# Action Inputs and Tasks

The EKG is task-event-based then any widget-activity (activity here means activating the purpose of a widget, like a button press or a slider bar etc). You can link tasks to these activities using the `set_task` method in widget.

```cpp
ekg::button("inputs example meow", ekg::dock::fill)
  ->set_task(
    new ekg::task {
      .info = {
        .tag = "inputs example meow",
        .p_data = nullptr // you can pass any address ptr
      },
      .function = [](ekg::info &info) {
        auto p_void = info.p_data;
        auto p_ui = info.p_ui; // a `void *ptr` pointing to widget ui.

        std::cout << info.tag << std::endl;
      }
    },
    ekg::action::activity
  );
```

This example print outputs the name of task in terminal.

# Linking

* Warning: Instead of compiling everything and linking all required libs (Vulkan and OpenGL); soon EKG must implement a second native library code to separate GPU APIs implementations 
but for now EKG requires link the platform and API libraries together. 

First [download](https://github.com/vokegpu/ekg-ui-library/releases) the latest release and [FreeType2](https://freetype.org/).  
Let see for SDL2 and OpenGL with GLEW, note: the order link is important, be sure you are linking EKG first.

```bash
# cmake
add_executable(your-project-name ekg SDL2main SDL2 freetype glew32)

# make
cxx ... -lekg -lSDL2main -lSDL2 -lfreetype -lglew32
```

# Building

| Toolchain | Support | OS |
| --- | --- | --- |
| GCC | Stable | Arch & Ubuntu |
| MinGW | Stable | Windows |
| Clang | Untested | ? |

EKG used some std 17 features, so the minimum std version is 17, note: you must use [CMake](https://cmake.org/download/) with [Ninja](https://github.com/ninja-build/ninja) to generate makefiles.

If you can not compile because of `freetype not found`, insert flag `-DEKG_LINUX_NOT_FOUND_FREETYPE=1`; the CMake build file will force an include.

If you want to build test programs insert `-DEKG_ENABLE_TEST=1`.

```bash
cmake -S . -B ./cmake-build-debug -G Ninja
cmake --build ./cmake-build-debug
```
Build output folder tree:

* `./lib/windows/` for windows x86_64.
* `./lib/linux/` for any Linux kernel-based OS.
* `ANDROID_ABI/` Android armeabi-v7a arm64-v8a x86 x86_64.

---

@copyright 2024 - Rina Wilk - VokeGpu
