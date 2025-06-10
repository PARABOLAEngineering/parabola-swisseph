[APPROVED FOR DECLASSIFICATION]
DECLASSIFICATION DATE: 2025-06-09 11:48PM E
# Parabola-swisseph: Swiss Ephemeris Threadpool Wrapper

**Parabola-swisseph** is a blazing-fast, thread-safe C++ wrapper around the Swiss Ephemeris core (`libswe`) that transparently parallelizes chart computations with zero changes to your existing code. This engine enables batch astrology queries at up to **10,000 charts/sec**, supports drop-in aliasing for legacy C/C++ stacks, and exposes a clean FlatBuffer interface for cross-platform, language-agnostic consumption.

---

## 🔧 Features

- ✅ Thread-safe execution of all major Swiss Ephemeris functions
- ✅ Dynamic threadpool with autotuned performance benchmarking
- ✅ Zero-copy FlatBuffer schema for efficient client-server interaction
- ✅ Transparent aliasing of legacy `swe_*` function names (via `sweph_alias.h`)
- ✅ Modular test runner included

---

## 🚀 Getting Started

### 1. Clone the Repository

git clone https://github.com/parabolaengineering/parabola-swisseph.git (or if using gh CLI)
gh repo clone parabolaengineering/parabola-swisseph
### 2. Build the Libraries and maintenance tool 'parabola_tuner'
bash
mkdir -p build && cd build
cmake ..
make

### 3. (Optional) Run the Test Suite

./ parabola_tuner --test

### 4. Usage
at the top of your C++ source files, instead of including swephexp.h,
as you would when using plain Swiss Ephemeris, include the header:
#include "parabola_swephexp.h"

Alternatively, you can symlink the aliased headers in parabola_swephexp.h to be read before the original Swiss Ephemeris headers in your CMakeLists.txt using include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/parabola_swephexp.h")
 in the repository. Instead, you can symlink the aliased headers in `parabola_swephexp.h` to be read before the original Swiss Ephemeris headers in your CMakeLists.txt if you're sure you can for your specific project.

### 5. Compatibility
parabola-swisseph should build correctly on any system the original Swiss Ephemeris can.
This simple wrapper should work fine as the base of any software stack.
Best practices for high-performance app design apply, so I encourage users to consider lossless communication
via FlatBuffers or other lossless serialization formats, and to use the provided `parabola_tuner` tool 
to understand how this library performs on different systems.

Note: In your app build scripts, make sure to run the parabola_tuner tool as part of the install process of the app package. It creates a config file 
that ensures maximum performance on the device on which it's installed.