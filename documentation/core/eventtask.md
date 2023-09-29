# Overview

Here you will read these topics:
- Runtime
- Execution
- GUI
# Runtime

The event system and handler service exist for two different purposes, the handler is a task manager to works with callback events (tasks) and executes the main tasks of the EKG runtime core.
The event system exists to release UI events. 

# Execution

The structure of a task is simple:
```cpp
ekg::cpu::event task {
	.p_tag = "task event cat 1",
	.p_callback = nullptr,
	.function = [](void *p_callback) {};
};
```

For dispatching a task is simple, reference or copy `ekg::core->service_handler.generate()`
```cpp
auto &task = ekg::core->service_handler.generate();
task.p_tag = "cat";
task.p_callback = nullptr;
task.function = [](void *p_callback) {};

// it is already dispatched :)!
```