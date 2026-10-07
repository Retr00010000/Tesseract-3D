# 🧊 Tesseract 3D

[![Language](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/Graphics-OpenGL%203.3%20Core-green.svg)](https://www.opengl.org/)
[![Windowing](https://img.shields.io/badge/Library-GLFW%203.3.8-orange.svg)](https://www.glfw.org/)
[![Math](https://img.shields.io/badge/Math-GLM%200.9.9-blueviolet.svg)](https://github.com/g-truc/glm)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg)](#)

An interactive, real-time 3D simulation of a rotating Tesseract hypercube engineered from scratch in **C++20** and modern **OpenGL 3.3 (Core Profile)**. Features custom GLSL shaders, a custom memory-based animated GIF texture engine (`AnimatedGifTexture`), full real-time mouse drag orientation with gyroscopic spin, a dynamic looping cosmic space background (`space.gif`), an animated 90-frame hypercube skin (`Tess.gif`), and complete portable toolchain support via **MinGW-w64 (`g++`)**.

---

## 📸 In-Game Preview


<p align="center">
  <img width="792" height="790" alt="Screenshot 2026-10-07 220836" src="https://github.com/user-attachments/assets/fff03aa1-050b-4ec0-91ea-8dce766f210c" />
</p>

*Real-time interactive 3D Tesseract hypercube rendered in OpenGL Core Profile with dynamic animated GIF cube texturing, responsive mouse dragging, and deep-space cosmic background.*

---

## 🕹️ Controls & Mouse Interaction

| Action / Interaction | Input | Description |
| :--- | :--- | :--- |
| **3D Free-Orbit Drag** | <kbd>Left Mouse Click</kbd> + <kbd>Drag</kbd> | Interactively rotates the cube around the X and Y axes with smooth sensitivity |
| **Release Orientation** | <kbd>Release Click</kbd> | Retains user-defined orientation while resuming gyroscopic spin |
| **Continuous Spin** | *Automatic* | 60 FPS gyroscopic rotation along diagonal axis `(0.5, 1.0, 0.0)` |
| **Exit Application** | <kbd>Alt</kbd> + <kbd>F4</kbd> or <kbd>✕</kbd> | Cleanly releases OpenGL buffers, terminates GLFW, and exits |

---

## ⚙️ How the Application Runs

### 1. 🔄 The Render Loop & Layered Passes
* **Dual-Pass Rendering Pipeline:**
  1. **Background Space Pass (Depth Test Disabled):** Renders a full-screen quad (`bgVertices`) mapped with the looping 60-frame cosmic background (`space.gif`). Disabling `GL_DEPTH_TEST` ensures the background always resides behind all 3D geometry without writing to the depth buffer.
  2. **3D Cube Pass (Depth Test Enabled):** Re-enables `GL_DEPTH_TEST` with `GL_LESS` depth comparison. Calculates composite transformation matrices ($P \times V \times M$) and draws the 36-index (12 triangles) cube textured with the active frame of `Tess.gif`.
* **Framerate & Delta-Timing:**
  * Uses `glfwGetTime()` to drive continuous, smooth angular increments at 60 FPS (`autoRotation += 0.8f`).
  * Double-buffered presentation via `glfwSwapBuffers` guarantees flicker-free rendering with VSync support.

### 2. 🌌 Animated GIF Texture Engine (`AnimatedGifTexture`)
* **In-Memory Decoding:** Unlike static image loaders, `AnimatedGifTexture` loads multi-frame animated GIFs using `stbi_load_gif_from_memory`:
  ```cpp
  stbi_load_gif_from_memory(buffer.data(), (int)buffer.size(), &delays, &width, &height, &totalFrames, &comp, 4);
  ```
* **Dynamic GPU Texture Array:**
  * Generates an array of OpenGL texture IDs (`glGenTextures(totalFrames, ...)`).
  * Automatically stores per-frame millisecond delays (`frameDelaysMs`) and computes the cumulative duration `totalDurationMs`.
  * Uploads all individual frames to dedicated GPU texture buffers with linear filtering and mipmaps.
* **Real-Time Frame Binding:**
  * On every draw call, `Bind(glfwGetTime(), GL_TEXTURE0)` computes elapsed milliseconds modulo total loop duration and instantly binds the corresponding frame texture ID.
  * Applied seamlessly to both the rotating **Tesseract cube** (`assets/textures/Tess.gif`) and the **cosmic backdrop** (`assets/textures/space.gif`).

### 3. 📐 Mathematical Projection & 3D Matrices (GLM)

#### **Model Matrix ($M$)**
Combines user-interactive pitch and yaw rotations derived from mouse drag delta offsets with the continuous automatic gyroscopic spin:

```math
\mathbf{M} = \mathbf{R}_{\text{auto}}(\theta_{\text{auto}}, (0.5, 1.0, 0.0)) \times \mathbf{R}_y(\theta_{\text{yaw}}) \times \mathbf{R}_x(\theta_{\text{pitch}})
```

#### **View Matrix ($V$)**
Positions the virtual camera back along the Z-axis at $(0.0, 0.0, -2.5)$ using `glm::translate`:

```math
\mathbf{V} = \text{glm::translate}(\mathbf{I}, (0.0, 0.0, -2.5))
```

#### **Projection Matrix ($P$)**
Applies perspective projection with a $45^\circ$ Field of View (FOV), 1:1 aspect ratio ($800 \times 800$), a near clipping plane of $0.1$, and a far clipping plane of $100.0$:

```math
\mathbf{P} = \text{glm::perspective}(\text{radians}(45.0^\circ), 1.0, 0.1, 100.0)
```

#### **GLSL Shader Pipeline**
The vertex shader ([`default.vert`](assets/shaders/default.vert)) multiplies object-space vertex positions by the Model-View-Projection (MVP) matrix product to compute clip-space coordinates:

```math
\mathbf{v}_{\text{clip}} = \mathbf{P} \times \mathbf{V} \times \mathbf{M} \times \begin{bmatrix} x \\ y \\ z \\ 1.0 \end{bmatrix}
```

---

## ✨ Features & Architecture

* **Decoupled from Visual Studio:** Completely ported from MSBuild (`.vcxproj` / `.slnx`) to standard **MinGW-w64 (`g++`)** and PowerShell automation.
* **Dual Animated GIF Pipeline:** Both background and cube surfaces are driven by synchronized, multi-frame animated GIFs.
* **Interactive 3D Mouse Controls:** Real-time drag rotation enables full examination of the hypercube geometry from any angle.
* **Smart Asset Resolver:** Engine loaders automatically resolve assets from `./assets/`, `../assets/`, or flat directories for resilient execution from any working directory.
* **Clean Modern OpenGL Architecture:** All GPU buffers are encapsulated into reusable object-oriented abstractions (`VAO`, `VBO`, `EBO`, `Shader`, `Texture`, `AnimatedGifTexture`).

---

## 🗂️ Project Structure

```text
Tesseract/
├── assets/
│   ├── gameplay.png              # In-game preview screenshot
│   ├── shaders/
│   │   ├── default.vert          # Vertex shader for 3D perspective & 2D background
│   │   └── default.frag          # Texture sampler fragment shader
│   └── textures/
│       ├── space.gif             # 60-frame animated cosmic space background
│       └── Tess.gif              # 90-frame animated Tesseract hypercube texture
├── Dependencies/
│   ├── GLAD/                     # OpenGL 3.3+ function loader (glad.c & headers)
│   ├── GLFW/                     # Cross-platform windowing, context & input (MinGW-w64)
│   │   ├── include/              # GLFW API C/C++ headers
│   │   └── lib-mingw-w64/        # Static MinGW archive (libglfw3.a)
│   ├── glm/                      # OpenGL Mathematics (matrix math, transformations)
│   └── stb/                      # stb_image header-only image & GIF decoding library
├── source/
│   ├── Tesseract.cpp             # Main application entry point, render loop & mouse interaction
│   └── engine/                   # Reusable Modern OpenGL abstractions & texture engines
│       ├── AnimatedGifTexture.h  # Animated GIF texture loader header
│       ├── AnimatedGifTexture.cpp# Multi-frame animated GIF texture decoder & player
│       ├── EBO.h / EBO.cpp       # Element Buffer Object wrapper
│       ├── VAO.h / VAO.cpp       # Vertex Array Object wrapper
│       ├── VBO.h / VBO.cpp       # Vertex Buffer Object wrapper
│       ├── ShaderClass.h / .cpp  # GLSL shader program compilation & linking
│       ├── Texture.h / Texture.cpp # Static OpenGL 2D texture generation & binding
│       ├── glad.c                # GLAD OpenGL loader source
│       └── stb.cpp               # stb_image implementation translation unit
├── .vscode/                      # VS Code tasks, debug launcher & C++ Intellisense configuration
├── .gitignore                    # Git tracking ignore rules
├── build.ps1                     # Automated build, link, asset copy, and run script
└── README.md                     # Project documentation
```

---

## 🚀 How to Build and Run

### 📋 Prerequisites
* **Operating System:** Windows 10 or Windows 11 (64-bit)
* **Compiler:** MinGW-w64 (`g++` with C++20 support)
  * Easily installed via WinGet:
    ```powershell
    winget install BrechtSanders.WinLibs.POSIX.UCRT
    ```
  * Or verify with: `g++ --version`
* **Shell:** PowerShell 5.1+

---

### Option 1: Automated PowerShell Script (Recommended)

Run the included automated build script from the project root:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

* Automatically detects `g++` from system `PATH` or WinGet directories.
* Statically links MinGW runtime libraries (`-static -static-libgcc -static-libstdc++`) for maximum portability.
* Compiles all engine units, copies the `assets/` directory to `Debug/`, and immediately launches `Tesseract.exe`.

> **Compile only (without launching):**
> ```powershell
> powershell -ExecutionPolicy Bypass -File .\build.ps1 -NoRun
> ```

---

### Option 2: Visual Studio Code

1. Open the `Tesseract` folder in **Visual Studio Code**.
2. Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd> and select **Run Tesseract** (or **Build Tesseract**).
3. Alternatively, open the **Run & Debug** panel (<kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>D</kbd>) and select **Play Tesseract** (<kbd>F5</kbd>).

---

### Option 3: Manual Command-Line Build (MinGW g++)

You can compile the executable manually using PowerShell:

```powershell
g++ -std=c++20 -mwindows -static -static-libgcc -static-libstdc++ `
  source/Tesseract.cpp `
  source/engine/glad.c `
  source/engine/AnimatedGifTexture.cpp `
  source/engine/EBO.cpp `
  source/engine/ShaderClass.cpp `
  source/engine/stb.cpp `
  source/engine/Texture.cpp `
  source/engine/VAO.cpp `
  source/engine/VBO.cpp `
  -Isource `
  -Isource/engine `
  -IDependencies `
  -IDependencies/GLAD/include `
  -IDependencies/GLFW/include `
  -IDependencies/stb `
  -LDependencies/GLFW/lib-mingw-w64 `
  -lglfw3 -lopengl32 -lgdi32 `
  -o Debug/Tesseract.exe

# Copy assets and launch
Copy-Item -Path "assets" -Destination "Debug" -Recurse -Force
.\Debug\Tesseract.exe
```

---

### Option 4: Direct Precompiled Execution

If the project has already been built:

```powershell
.\Debug\Tesseract.exe
```

---

## 🛠️ Troubleshooting & Notes

* **Windows Smart App Control (SAC) / Defender Notice:**
  On modern Windows 11 systems, Smart App Control or local execution policies may block freshly compiled, unsigned `.exe` files. If blocked:
  1. Open **Windows Settings** → **Privacy & Security** → **Windows Security**.
  2. Click **App & browser control** → **Smart App Control settings**.
  3. Set to **Off** or enable **Developer Mode** in **System → For Developers**.
* **Missing Assets / Textures at Launch:**
  The application includes a smart path resolver in `AnimatedGifTexture` and `ShaderClass` that automatically searches `./assets/`, `../assets/`, and subfolders. Always ensure `assets/` is present either in the project root or copied alongside the executable in `Debug/`.
* **Header Ordering Notice:**
  If modifying the source code, always make sure `<glad/glad.h>` is included **before** `<GLFW/glfw3.h>` to avoid OpenGL header redefinition conflicts.
