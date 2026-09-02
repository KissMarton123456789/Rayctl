# Rayctl
Why use Vulkan or OpenGL when you can manually shuffle raw 32-bit pixel integers across system memory like it's 1992?

#### Description
A software rendering engine built from scratch in C++20. No modern GPU pipelines, no fancy shaders—just pure Digital Differential Analysis (DDA), raw array indexing, and multithreaded CPU abuse.

#### Features
* 0% GPU Load: Your graphics card can continue sleeping.
* 100% CPU Load: Built-in room heater powered by [std::barrier](https://www.youtube.com/watch?v=QDia3e12czc) and math.
* Arch + Hyprland Native: Runs at 1,000 FPS strictly because I compile with -O3.
* Zero Fisheye Distortion: Thanks to vector math, not prayers.
