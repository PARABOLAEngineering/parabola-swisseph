[APPROVED FOR DECLASSIFICATION]
DECLASSIFICATION DATE: 2025-06-09 11:48PM E
# Parabola-swisseph: Swiss Ephemeris Threadpool Wrapper

**Parabola-swisseph** is a blazing-fast, thread-safe C++ wrapper around the Swiss Ephemeris core (`libswe`) that transparently parallelizes chart computations with zero changes to your existing code. This engine enables batch astronomy and astrology queries at up to **10,000 charts/sec**, 
supports built-in aliasing of legacy `swe_*` functions, and provides a dynamic threadpool with autotuned performance benchmarking.

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
#run inside your project folder, or create a specific folder for dependencies, such as /deps or /include
git clone https://github.com/parabolaengineering/parabola-swisseph.git (or if using gh CLI)
gh repo clone parabolaengineering/parabola-swisseph

### 2. Build the Libraries and testing/tuning tool 'parabola_tuner'
# Run this inside the swisseph folder, NOT your project root. #

mkdir -p build
cmake ./build
make

### 3. (Optional) Run the Test Suite

./ parabola_tuner --test

### 4. Usage
You should be able to use this library as a drop-in replacement for the original Swiss Ephemeris in your projects out of the box.
This is because swephexp.h, the main header file of the original Swiss Ephemeris, has been aliased against `parabola_swephexp.h` in this modified library.
This header provides the same interface as the original Swiss Ephemeris, 
but with added thread safety and performance optimizations.
If it doesn't work out of the box, you can use the `sweph_alias.h` header file to alias the original Swiss Ephemeris functions to their wrapped versions manually.
At the top of your C++ source files, instead of #include <swephexp.h>, #include <parabola_swephexp.h>.

### 5. Compatibility
parabola-swisseph should build correctly on any system the original Swiss Ephemeris can.
This simple wrapper should work fine with any software stack.
Best practices for high-performance app design apply, so I encourage users to consider lossless communication
via FlatBuffers or other lossless serialization formats, and to use the provided `parabola_tuner` tool to understand how this library performs on different systems.

Note: In your app build scripts, make sure to run the parabola_tuner tool as part of the install process of your app's app package (.exe, .apk, .ipa, .deb, .app etc). 
It creates a config file that will be used by the library to optimize its performance for the specific device.
This will ensure that the library is configured with the optimal threadpool size and other parameters to ensure maximum performance on the app's specific home device.