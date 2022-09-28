The official runtime ekg ui library.

---

This is the new core verion of ekg, rewrite of legacy core.

# Running

The current version of ekg only support the MinGW64 compiller.  
For use in Windows 32/64bits: `lib/win32/libekg.a`  
For use in Linux x86_x64: `lib/linux/libekg.a`  

# About

For NDK you will use OpenGL ES 2 or 3, for high performance applications you will use OpenGL 4 or 3, for now there is only OpenGL 3/4.

The ekg has two modes: hybrid and immediate; The hybrid mode joint immediate mode with buidable mode - If widget is created at once tick, the current mode of widget is WBO (widget built once), else WBI (widget built immedate).

