/* swevid_loader.h */
#ifndef SWEVID_LOADER_H
#define SWEVID_LOADER_H

#include <cstddef>
#include <cstdint>
#include <string>

// Global pointers for the memory-mapped swevid
extern void* swevid_data;
extern size_t swevid_size;

// Load a .swevid (fake media) file into memory
bool load_swevid(const std::string& path);

// Override for Swiss Ephemeris file reads
extern "C" int read_swe_file(const char* fname, char* outbuf, int offset, int length);

#endif // SWEVID_LOADER_H