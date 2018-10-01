#include "resource.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

static std::map<std::string, std::string> cached_resources;

FILE* get_resource_file(const char* resource) {
  FILE* f = std::fopen(resource, "rb");
  if (f == nullptr) {
    throw std::runtime_error("Failed to open file:" + std::string(resource));
  }
  return f;
}

void* get_resource_mem(const char* resource, int* length) {
  auto it = cached_resources.find(resource);
  if (it == cached_resources.end()) {
    std::ifstream in(resource, std::ifstream::binary);

    if (!in.is_open()) {
      throw std::runtime_error("Failed to open file: " + std::string(resource));
    }

    std::string content;
    char buffer[2048];

    for (;;) {
      in.read(buffer, sizeof(buffer));
      if (in.gcount() == 0) {
        break;
      }
      content.append(buffer, in.gcount());
    }
    it = cached_resources.emplace(resource, std::move(content)).first;
  }

  *length = static_cast<int>(it->second.size());
  return it->second.data();
}
