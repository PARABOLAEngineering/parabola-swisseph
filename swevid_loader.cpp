
/* swevid_loader.cpp */
#include "swevid_loader.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>

void* swevid_data = nullptr;
size_t swevid_size = 0;

bool load_swevid(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open swevid");
        return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror("fstat swevid");
        close(fd);
        return false;
    }

    swevid_size = static_cast<size_t>(sb.st_size);
    swevid_data = mmap(nullptr, swevid_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (swevid_data == MAP_FAILED) {
        perror("mmap swevid");
        swevid_data = nullptr;
        return false;
    }

    return true;
}

extern "C" int read_swe_file(const char* fname, char* outbuf, int offset, int length) {
    // If not loaded, fallback
    if (!swevid_data) {
        return -1;
    }

    // Enforce .swevid extension for our loader
    std::string fstr(fname);
    if (fstr.size() < 7 || fstr.substr(fstr.size() - 7) != ".swevid") {
        return -1;
    }

    size_t uoffset = static_cast<size_t>(offset);
    size_t ulength = static_cast<size_t>(length);
    if (uoffset + ulength > swevid_size) {
        return -2;
    }

    std::memcpy(outbuf, static_cast<uint8_t*>(swevid_data) + uoffset, ulength);
    return 0;
}