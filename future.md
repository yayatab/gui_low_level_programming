# Future Optimizations & Features

## Precomputed Look-Up Table (LUT) for Palettes

### Motivation
Evaluating analytical palette functions (such as 3 $\cos$ operations per pixel or multi-stop linear interpolation searches) on every pixel of a high-resolution frame (e.g., $1280 \times 720 \approx 921{,}600$ pixels, or $1920 \times 1080 \approx 2{,}073{,}600$ pixels) consumes significant CPU cycles.

A **Precomputed Look-Up Table (LUT)** samples the chosen color gradient once into a contiguous array, turning color evaluation in the inner rendering loop into a single, $O(1)$ L1-cache-friendly memory read.

---

### Design & Architecture

1. **Table Sizing**:
   - Use a power-of-two size (e.g., $N = 1024$ or $N = 2048$ entries).
   - A table of 1024 32-bit integers (`uint32_t`) requires only $4\,\text{KB}$ of memory, fitting entirely within the CPU's L1 data cache (typically $32\,\text{KB}$ to $128\,\text{KB}$).

2. **Fast Wrapping**:
   - Power-of-two sizes allow replacing modulo arithmetic (`%`) or `fmodf` with a bitwise AND mask:
     $$\text{index} = (\text{uint32\_t})(t \cdot 1024.0\text{f}) \ \& \ 1023$$

3. **LUT Generation**:
   - When a palette or gradient profile is initialized or modified:
     ```c
     #define LUT_SIZE 1024
     #define LUT_MASK (LUT_SIZE - 1)

     typedef struct {
         uint32_t colors[LUT_SIZE];
     } PaletteLUT;

     void palette_lut_generate(PaletteLUT *lut, /* palette generator function or params */) {
         for (int i = 0; i < LUT_SIZE; i++) {
             float t = (float)i / (float)LUT_SIZE;
             lut->colors[i] = evaluate_palette(t);
         }
     }
     ```

4. **Fast Sampling in Inner Loop**:
   - In `render_fractal_row`:
     ```c
     if (iterations >= max_iter) {
         return 0xFF000000; // Black for set interior
     }
     uint32_t lut_idx = (uint32_t)((iterations * frequency + phase) * LUT_SIZE) & LUT_MASK;
     uint32_t pixel_color = lut->colors[lut_idx];
     ```

5. **Dynamic Palette Swapping**:
   - Multiple LUT instances (e.g. `fire_lut`, `ocean_lut`, `rainbow_lut`) can be precomputed at application startup and hot-swapped during runtime without re-allocation.
