# EKG
EKG now support Windows/Linux, the project is totally built in a context SDL2 and OpenGL, be sure you have setup correctly.

In the code you look something like that:
```
/* Start of something. */
ekg_do_something();
/* End of something. */
```
I use this style code to find easy something in a big file (if you think, is easy to write invoke of... end of...).

The reason for EKG has all fields public in element UI (not interface accessible from user) is for stay avoid of wrong actions/events in widget.

EKG uses `SDL_image` `SDL_ttf` to handler font and images file.
For implement EKG in your project first you need to add all these libs.

# Information/help
You can read docs on:
