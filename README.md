# learning vulkan with hello triangle

vulkan is a bitch

- Win32 main (lol) with windows programming
- Header and source files split (why would you want to put everything in one main file?)

## components

- Instances
- Validation layers
- Swapchains 
- Queues
- Imageviews Framebuffers
- Pipeline
- Render pass (this will be replaced in favor of dynamic rendering)
- Command buffers
- Binary swapchains (those will be replaced in favor of timeline swapchains) and fences

## notes

there is so much technical debt in naming things, structuring the project, and making project decisions.

## references
- [vulkan-tutorial *(literally 7 years already)*](https://vulkan-tutorial.com)
- [Official vulkan-tutorial](https://docs.vulkan.org/tutorial/latest)

## building

- build with MSYS2 `(mingw-w64-x86_64)` because the makefile has `pkg-config`. otherwise, your environment should have `pkg-config` installed.
- get the vulkan SDK for vulkan headers and validation layers
