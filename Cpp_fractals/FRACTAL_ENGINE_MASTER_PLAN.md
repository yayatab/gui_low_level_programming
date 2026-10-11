# C++23 Fractal Engine: Architectural Master Plan

This document details the complete end-to-end engineering roadmap for building a high-performance fractal engine in **C++23**, evolving from a **CPU multi-threaded software renderer** to a **hardware-accelerated Apple Metal engine**, supporting multiple dynamical systems and interactive parameter control.

---

## Architecture Overview

The system is designed with strict separation between **Mathematics/Domain**, **Render Hardware Interface (RHI)**, and **Application/Windowing**.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        Application & UI Layer                          │
│     - SDL3 Window Management & Input (Pan, Cursor-Centric Zoom)        │
│     - Dear ImGui Parameter Panel (Fractal Switcher, Palette Tuning)    │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                             Domain Layer                               │
│  - Viewport: Affine projection Screen(px, py) <-> Complex(x, y)        │
│  - Fractal Models: Mandelbrot, Julia, Burning Ship, Newton-Raphson     │
│  - Palette Engine: Cosine gradient generation, smooth coloring math    │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ passes SceneState & Viewport
┌───────────────────────────────────▼────────────────────────────────────┐
│               Render Hardware Interface (RHI) Abstraction              │
│                           <<interface>> IRenderer                      │
│   + init(SDL_Window*)                                                  │
│   + render(const SceneState& state)                                    │
│   + resize(uint32_t width, uint32_t height)                            │
│   + shutdown()                                                         │
└───────────────────────────▲──────────────────────────▲─────────────────┘
                            │                          │
           ┌────────────────┴────────┐        ┌────────┴─────────────────┐
           │       CPURenderer       │        │       MetalRenderer      │
           │ - Multi-threaded CPU    │        │ - Metal Compute/Fragment │
           │ - ARM NEON SIMD         │        │ - MSL Shaders            │
           │ - SDL Streaming Texture │        │ - CAMetalLayer / MTLLayer│
           └─────────────────────────┘        └──────────────────────────┘
```

---

## Phase 1: Project Setup & CPU Multi-Threaded Engine (C++23)

### 1.1 CMake & Dependency Architecture
- Target **C++23** (`CMAKE_CXX_STANDARD 23`).
- Use CMake `FetchContent` to manage dependencies:
  - **SDL3**: Windowing, input polling, event pump, raw native window handles.
  - **GLM**: Vector and matrix math matching shader semantics.
- Replace legacy SFML configuration in `Cpp_fractals/CMakeLists.txt`.

### 1.2 Viewport & Affine Coordinate Mapping
- Maps screen space $[0, W) \times [0, H)$ to complex coordinate space $\mathbb{C}$:
  $$c_x = \text{center}_x + \left(\frac{p_x - \frac{W}{2}}{\frac{H}{2}}\right) \cdot \frac{1}{\text{zoom}}$$
  $$c_y = \text{center}_y - \left(\frac{p_y - \frac{H}{2}}{\frac{H}{2}}\right) \cdot \frac{1}{\text{zoom}}$$
- **Cursor-Centric Zoom**:
  When zooming by factor $k$ at mouse position $(p_x, p_y)$, the complex point $c_{\text{mouse}}$ under the cursor must remain invariant:
  $$\text{center}_{\text{new}} = c_{\text{mouse}} + (\text{center}_{\text{old}} - c_{\text{mouse}}) \cdot \frac{1}{k}$$

### 1.3 Mandelbrot Mathematics & Escape Time
- Iteration: $z_{n+1} = z_n^2 + c$, where $z_0 = 0$, $c = x + iy$.
- **Cardioid & Period-2 Bulb Early Exit**:
  Avoids computing interior points:
  $$q = \left(x - \frac{1}{4}\right)^2 + y^2$$
  If $q \left(q + \left(x - \frac{1}{4}\right)\right) < \frac{1}{4} y^2$ (main cardioid) or $(x + 1)^2 + y^2 < \frac{1}{16}$ (period-2 bulb), the point is guaranteed in the set $\rightarrow$ return `max_iterations`.
- **Continuous (Smooth) Coloring Formula**:
  Eliminates discrete iteration banding:
  $$\nu = n + 1 - \frac{\ln(\ln(|z_n|))}{\ln 2}$$

### 1.4 Modern Multi-Threaded Rasterization
- Pixel buffer: Contiguous memory storing 32-bit RGBA values (`std::vector<uint32_t>`).
- Row-level parallelization across CPU cores using `std::jthread` and work slicing to maximize cache efficiency.

### 1.5 Presentation via SDL3
- Create an `SDL_Renderer` and streaming `SDL_Texture` (`SDL_PIXELFORMAT_RGBA32`, `SDL_TEXTUREACCESS_STREAMING`).
- Upload the buffer each frame via `SDL_UpdateTexture` and render to screen.

---

## Phase 2: Polymorphic Fractal Domain & Palette System

### 2.1 Polymorphic Fractal Abstraction
Create a modern C++23 interface or concept for fractals:
```cpp
struct FractalPointResult {
    float iteration; // Smooth iteration or convergence metric
    int root_id;     // Used by Newton-Raphson roots
    bool escaped;
};
```

### 2.2 Implementing the 4 Dynamical Systems
1. **Mandelbrot Set**:
   - $z_{n+1} = z_n^2 + c$, $z_0 = 0$.
2. **Julia Sets**:
   - $z_{n+1} = z_n^2 + c_0$, where $c_0 \in \mathbb{C}$ is a fixed parameter, $z_0 = \text{screen point}$.
   - Allows interactive coupling: hovering over Mandelbrot sets $c_0$ for Julia!
3. **Burning Ship**:
   - Non-analytic mapping: $x_{n+1} = x_n^2 - y_n^2 + c_x$, $y_{n+1} = 2 |x_n y_n| + c_y$.
4. **Newton-Raphson Fractals**:
   - Root-finding on $f(z) = z^3 - 1 = 0$:
     $$z_{n+1} = z_n - \frac{z_n^3 - 1}{3 z_n^2} = \frac{2 z_n^3 + 1}{3 z_n^2}$$
   - Convergence test: $|z_{n+1} - z_n| < \epsilon$.
   - Coloring corresponds to both the converged root and the step count.

### 2.3 Procedural Cosine Palette Engine
- Utilizes the Inigo Quilez cosine gradient formula:
  $$\text{color}(t) = a + b \cdot \cos(2\pi (c \cdot t + d))$$
  Where $a, b, c, d \in \mathbb{R}^3$ allow defining any color ramp using only 12 float parameters.

---

## Phase 3: GUI, Controls & Parameter Tuning (Dear ImGui)

### 3.1 ImGui Integration
- Fetch Dear ImGui via `FetchContent`.
- Hook Dear ImGui into SDL3 event processing (`ImGui_ImplSDL3_ProcessEvent`).
- Render UI overlays for real-time interaction.

### 3.2 Feature Controls
- **Fractal Selector**: Dropdown to switch between Mandelbrot, Julia, Burning Ship, and Newton.
- **Dynamic Controls**:
  - Max iteration slider (10 to 5,000+).
  - Palette color pickers and frequency sliders.
  - Coordinate inspector (center $x, y$, current zoom depth, frame time, FPS).
  - Julia constant $(c_r, c_i)$ sliders or interactive cursor capture.

---

## Phase 4: Render Hardware Interface (RHI) Abstraction

### 4.1 The `IRenderer` Interface
Formalize backend switching so the application code remains completely untouched:

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual bool init(SDL_Window* window) = 0;
    virtual void render(const SceneState& state) = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual void shutdown() = 0;
};
```

### 4.2 Decoupling Scene State
A compact POD struct `SceneState` encapsulates everything needed to render a frame:
- Viewport bounds (`dvec2 center`, `double zoom`, `uint32_t width, height`).
- Active fractal enum and fractal-specific constants.
- Palette parameters ($a, b, c, d$ vectors and scale).

---

## Phase 5: Apple Metal GPU Acceleration

### 5.1 Why Metal on Apple Silicon?
- **Zero Overhead**: Direct native Apple driver path (no OpenGL 4.1 translation layer or deprecation warnings).
- **Unified Memory Architecture (UMA)**: CPU and GPU share the same physical RAM pool, enabling zero-copy buffer sharing.
- **Compute Shaders**: First-class support for `kernel` functions executing across threadgroups.

### 5.2 Metal Integration with SDL3
- Retrieve native NSWindow handle from SDL3 (`SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, ...)`).
- Attach a `CAMetalLayer` as the window's backing layer.
- Initialize `id<MTLDevice>`, `id<MTLCommandQueue>`, and pipeline states via Objective-C++ (`.mm`) or the modern [metal-cpp](https://developer.apple.com/metal/cpp/) official C++ headers from Apple.

### 5.3 Metal Shading Language (MSL) Implementation
Two rendering strategies:
1. **Compute Pipeline (`kernel void compute_fractal(...)`)**:
   - Each thread computes one pixel $(x, y)$ in parallel.
   - Writes directly to a `texture2d<float, access::write>`.
   - Blits / presents the texture to the `CAMetalDrawable`.
2. **Fragment Pipeline**:
   - Draw a full-screen quad (2 triangles), computing the fractal in `fragment float4 fractal_frag(...)`.

### 5.4 High-Precision Deep Zoom on GPU
- Apple Silicon GPUs excel at 32-bit floats.
- At deep zooms ($> 10^7$), single-precision floating-point numbers run out of mantissa bits, causing visual pixelation.
- We will plan an extension for **Emulated Double Precision (Double-Single / `ds_float`)** on MSL to allow deep zooming without artifacting.

---

## Phase 6: SIMD Vectorization & Comparative Benchmarking

### 6.1 CPU SIMD Acceleration (ARM NEON)
- For the CPU backend, implement 2-wide `float64x2_t` or 4-wide `float32x4_t` ARM NEON vectorization in C++23.
- Compare scalar CPU vs. SIMD CPU vs. Metal GPU compute.

### 6.2 Benchmarking & Telemetry
- Overlay performance metrics:
  - Frame time (ms), Frames Per Second (FPS).
  - Effective Megapixels computed per second.
  - Energy & memory footprint.

---

## Implementation Progression Matrix

| Phase | Milestone | Primary Technologies | Key Deliverable |
| :--- | :--- | :--- | :--- |
| **1** | CPU Engine & SDL3 | C++23, SDL3, GLM | Interactive CPU Mandelbrot with pan/zoom |
| **2** | Multiple Fractals & Palettes | C++23 Concepts / Interfaces | Julia, Burning Ship, Newton & smooth gradients |
| **3** | ImGui Integration | Dear ImGui, SDL3 | Live UI parameter controls & fractal switcher |
| **4** | RHI Abstraction | C++23 Interfaces | Clean `IRenderer` decoupling CPU from GPU |
| **5** | Apple Metal Acceleration | Metal-cpp / MSL | 60+ FPS GPU-accelerated rendering |
| **6** | Deep Zoom & NEON SIMD | ARM NEON, MSL double-float | Deep zoom without precision loss, performance telemetry |
