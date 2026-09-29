# Future Optimizations & Engineering Roadmap

This document outlines the architectural specifications, mathematical formulations, and engineering roadmap for the C fractal engine.

---

## 1. Engine & Memory Lifecycle Robustness (Immediate Hygiene)

### Motivation
Before extending the engine with additional workloads and mathematical models, ensure clean resource management and zero memory leaks.

### Specifications
1. **Buffer Lifecycle Parity**:
   - In `engine_init_mt()`, both `pixel_buffer` (`uint32_t*`) and `iteration_buffer` (`float*`) are dynamically allocated.
   - `engine_cleanup()` must cleanly free both buffers:
     ```c
     free(engine->pixel_buffer);
     free(engine->iteration_buffer);
     ```
2. **Modular File Cleanliness**:
   - Audit empty or redundant header/source stubs (`engine_utils.h`, `renderer_cpu.h`) and consolidate helper macros and prototypes.

---

## 2. Polymorphic Fractal Core & New Fractal Types

### Motivation
The current design defines a `FractalInterface` in `fractal.h`, but `main.c` hardcodes the Mandelbrot implementation. Abstracting the active fractal allows runtime switching (e.g., via number keys or UI shortcuts) across diverse mathematical dynamical systems.

### Mathematical Formulations & Implementations

#### A. Generalized Julia Sets
- **Formula**:
  $$z_{n+1} = z_n^2 + c$$
  Where $c \in \mathbb{C}$ is a fixed complex parameter, and the initial point $z_0 = x + i y$ is mapped from the screen coordinates.
- **Escape Condition**: $|z_n|^2 = Re(z_n)^2 + Im(z_n)^2 > 4.0$ (or custom radius).
- **Interactive Coupling**: Allow interactive binding of $c$ to mouse coordinates when hovering over the Mandelbrot set.

#### B. The Burning Ship Fractal
- **Formula**:
  $$z_{n+1} = (|Re(z_n)| + i|Im(z_n)|)^2 + c$$
  Expanded into real arithmetic:
  $$x_{n+1} = x_n^2 - y_n^2 + x_0$$
  $$y_{n+1} = 2 |x_n y_n| + y_0$$
- **Characteristics**: Non-analytic mapping creating distinctive towering masts and asymmetric flame structures.

#### C. Newton-Raphson Fractals
- **Formula**: Finding roots of complex polynomials (e.g., $f(z) = z^3 - 1 = 0$) using Newton's method:
  $$z_{n+1} = z_n - \frac{f(z_n)}{f'(z_n)} = z_n - \frac{z_n^3 - 1}{3 z_n^2} = \frac{2 z_n^3 + 1}{3 z_n^2}$$
- **Termination Criteria**:
  - Stop when $|z_{n+1} - z_n| < \epsilon$ (convergence to a root).
  - Stop when $n \ge \text{max\_iter}$ (failure to converge).
- **Coloring**: Map color to both the specific root converged to ($\{1, e^{i 2\pi/3}, e^{i 4\pi/3}\}$) and the number of steps required.

### Architecture
- Define an array of registered fractals:
  ```c
  static FractalInterface FRACTALS[] = {
      mandelbrot_interface,
      julia_interface,
      burning_ship_interface,
      newton_interface
  };
  ```
- Store active fractal index in `Engine` or application context.
- Support fractal-specific custom parameter structs passed via `const void* custom_params`.

---

## 3. SIMD Vectorization (ARM NEON / AVX2)

### Motivation
The escape calculation is embarrassingly parallel and computationally bound by floating-point arithmetic. Multi-threading distributes rows across CPU cores, but each core executes scalar operations. Modern CPUs feature vector registers capable of performing SIMD (Single Instruction, Multiple Data) operations across multiple complex coordinates simultaneously.

### Architecture & Vector Width
- **Apple Silicon (ARM64 NEON)**:
  - 128-bit vector registers (`float64x2_t` for 2 doubles, or `float32x4_t` for 4 single-precision floats).
  - High zoom depths benefit from double precision, while standard navigation is significantly accelerated by single precision.
- **x86_64 (AVX2 / FMA)**:
  - 256-bit vector registers (`__m256d` for 4 doubles, or `__m256` for 8 floats).

### Vectorized Escape Loop Design (ARM NEON `float64x2_t`)
1. **Coordinate Packing**:
   Load two adjacent horizontal pixels $x_0, x_1$ into a vector register `v_cr`, and duplicate the row coordinate $y$ into `v_ci`.
2. **Parallel Arithmetic**:
   ```c
   // z_r2 = z_r * z_r; z_i2 = z_i * z_i;
   float64x2_t z_r2 = vmulq_f64(z_r, z_r);
   float64x2_t z_i2 = vmulq_f64(z_i, z_i);

   // mag_sq = z_r2 + z_i2
   float64x2_t mag_sq = vaddq_f64(z_r2, z_i2);

   // mask = mag_sq <= escape_radius_sq
   uint64x2_t active_mask = vcleq_f64(mag_sq, v_escape_radius_sq);

   // z_i = 2 * z_r * z_i + c_i
   z_i = vaddq_f64(vmulq_f64(v_two, vmulq_f64(z_r, z_i)), v_ci);

   // z_r = z_r2 - z_i2 + c_r
   z_r = vaddq_f64(vsubq_f64(z_r2, z_i2), v_cr);
   ```
3. **Loop Termination**:
   Accumulate iteration counts only for active lanes. Break out early when all vector lanes have escaped (`vmaxvq_u32` or bitwise reduction equals 0).

---

## 4. Algorithmic Optimization: Mariani–Silver Boundary Tracing

### Motivation
Interior regions of fractals (such as the Mandelbrot cardioid or black interior bodies) take the full `max_iter` cycles per pixel if not caught by analytical checks. Furthermore, large exterior basins take uniform iteration counts.

### The Mariani–Silver Algorithm
1. Subdivide the screen into rectangular blocks (e.g., $32 \times 32$ or $16 \times 16$ pixels).
2. Compute only the 1-pixel perimeter of each block.
3. **Uniformity Test**:
   - If every pixel along the boundary escaped at the exact same iteration count $K$, then by the Maximum Modulus Principle (for exterior regions) and topological connectedness, the entire interior can be filled with $K$ without computing interior points.
4. **Subdivision**:
   - If the boundary pixels differ, subdivide the rectangle into 4 quadrants recursively down to a base tile size (e.g., $4 \times 4$), where individual pixels are computed directly.

---

## 5. Dynamic Detail, Controls & Telemetry

### A. Adaptive Iterations
- As the user zooms in towards fine boundary filaments, the required iterations to resolve features scale logarithmically with zoom level:
  $$\text{max\_iter} \propto A + B \cdot \log_{10}(\text{zoom})$$
- Provide hotkeys (`[` and `]`) for manual iteration overrides, with an automatic scaling mode.

### B. Runtime Telemetry HUD
- Track timing metrics using high-resolution timers (`SDL_GetPerformanceCounter()`):
  - Escape calculation time (ms).
  - Palette colorization time (ms).
  - SDL texture upload and presentation time (ms).
  - Effective FPS.
- Format and display telemetry via window title or minimal bitmap font overlay:
  ```text
  FPS: 120 | Compute: 4.2ms | Colorize: 0.8ms | Zoom: 1.4e6 | Iter: 1000
  ```

---

## 6. Precomputed Look-Up Table (LUT) for Palettes (Implemented)

*Completed in commit `9745ffc` and decoupled in `97f654a`.*
- Employs 1024-entry power-of-two look-up tables (`uint32_t`) mapped to L1 cache.
- Replaces runtime trigonometric calculations with fast bitwise mask indexing:
  $$\text{index} = (\text{uint32\_t})(t \cdot 1024.0\text{f}) \ \& \ 1023$$
- Allows fluid $60\text{+} \text{ FPS}$ color-cycling animations completely decoupled from fractal recalculation.

## 7. Unit Tests
