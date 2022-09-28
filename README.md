The official runtime ekg ui library.

---

This is the new core verion of ekg, rewrite of legacy core.

# Running

The current version of ekg only support the MinGW64 compiller.  

For use the library on your project, donwload the repo or release, then you add include and lib files:  
`lib/win32/libekg.a` - Windows 32/64bits  
`lib/linux/libekg.a` - Linux x86_x64  

# About

The purpose of ekg is be fully customizable, smooth and with many animations. Also perform good in low pc specs.  

`ekg` library automaticallly sets the OpenGL API, if your project is running in NDK the ekg sets to OpenGL ES, for high performance applications sets OpenGL 4 or 3.

The ekg has two modes: hybrid and immediate; The hybrid mode joint immediate mode with buidable mode - If widget is created at once tick, the current mode of widget is WBO (widget built once), else WBI (widget built immedate).

# Hardware

All buffers allocated into GPU are totally handled and optimized by `ekg::gpu::allocator`, it protects your gpu from multiples buffers dispatch, it only use one VBO and implement high shading communcation making a parcial instanced rendering.

The scale manager works with dynamic offsets, for better interactions it automatically sets the scale of offset and button sizes (if auto-scale mode is enabled), other service of scale manager is set widgets position based on root display, small screens like smartphones, reduce the bounds interaction beetwen master (mommy/mother/me) parent and others widgets.
