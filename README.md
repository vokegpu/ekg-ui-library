The official runtime ekg widget library.

---

This is the new core version of ekg, rewrite of legacy core.
Images bellow showcasing the current state of ekg.  

![Image Text](https://github.com/ekg-ui-library/ekg/blob/version-core/splash/splash-showcase-1-1.0.0.png.png?raw=true)
# Running

The current version of ekg only support the MinGW64 compiler.  
For use the library on your project, download the repo or release, then you add include and lib files:  
`lib/win32/libekg.a` - Windows 32/64bits  
`lib/linux/libekg.a` - Linux x86_x64  

# About

The purpose of ekg is being fully customizable, smooth and with many animations. Also perform good in low pc specs.  

`ekg` library automatically sets the OpenGL API, if your project is running in NDK the ekg sets to OpenGL ES, for high performance applications sets OpenGL 4 or 3.

# Hardware

All buffers swap into GPU are totally handled and optimized by `ekg::gpu::allocator`, it protects your gpu from multiples buffers dispatch, it only uses two buffers and implement high shading communication making a partial instanced rendering.

The scale manager works with dynamic offsets, for better interactions it automatically sets the scale of min_offset and button sizes (if auto-scale mode is enabled), other service of scale manager is set widgets position based on root display, small screens like smartphones, reduce the bounds interaction between mother/master parent and others widgets.

# Author

Hi I am rina the author (a), I do many commits sometime because I have a notebook and I need to handle the job a lot of times...
The current version state of ekg depends on all of my job, sometime I do not have motivation to code on it, but yes... I love to do GUIs and UIs library!
