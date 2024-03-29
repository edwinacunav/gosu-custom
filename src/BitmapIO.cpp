/*
** Modified by Kyonides Arkanthes (C) 2019
*/
#include "Bitmap.hpp"
#include "IO.hpp"
#include "Platform.hpp"
#include "Utility.hpp"
#include <cstring>
#include <stdexcept>
#include "debugwriter.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_LINEAR

// Disable comma warnings in stb headers.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomma"
#endif

#include "stb_image.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

using namespace std;

namespace
{
  int read_callback(void* user, char* data, int size)
  {
    Gosu::Reader* reader = static_cast<Gosu::Reader*>(user);
    size_t remaining = reader->resource().size() - reader->position();
    size_t actual_size = (size < remaining ? size : remaining);
    reader->read(data, actual_size);
    return static_cast<int>(actual_size);
  }

  void skip_callback(void* user, int n)
  {
    Gosu::Reader* reader = static_cast<Gosu::Reader*>(user);
    reader->set_position(reader->position() + n);
  }

  int eof_callback(void* user)
  {
    Gosu::Reader* reader = static_cast<Gosu::Reader*>(user);
    return reader->position() == reader->resource().size();
  }

  bool is_bmp(Gosu::Reader reader)
  {
    size_t remaining = reader.resource().size() - reader.position();
    if (remaining < 2) return false;
    char magic_bytes[2];
    reader.read(magic_bytes, sizeof magic_bytes);
    return magic_bytes[0] == 'B' && magic_bytes[1] == 'M';
  }
}

void Gosu::enable_flip_h(bool flip_h)
{
  stbi_kyon_set_flip_horizontally_on_load(flip_h);
}

void Gosu::enable_flip_y(bool flip_y)
{
  stbi_set_flip_vertically_on_load(flip_y);
}

void Gosu::enable_flip_h_y(bool flip_h, bool flip_y)
{
  stbi_kyon_set_flip_horizontally_on_load(flip_h);
  stbi_set_flip_vertically_on_load(flip_y);
}

void Gosu::load_image_inverse_color(Gosu::Bitmap& bitmap, const string& filename)
{
  load_image_file(bitmap, filename);
  int w = bitmap.width(), h = bitmap.height();
  for (int x = 0; x < w; x++)
    for (int y = 0; y < h; y++)
      bitmap.invert_pixel(x, y);
}

void Gosu::load_image_file(Gosu::Bitmap& bitmap, const string& filename)
{
  Buffer buffer;
  load_file(buffer, filename);
  load_image_file(bitmap, buffer.front_reader());
}

void Gosu::load_image_file(Gosu::Bitmap& bitmap, Reader input)
{
  bool needs_color_key = is_bmp(input);
  stbi_io_callbacks callbacks;
  callbacks.read = read_callback;
  callbacks.skip = skip_callback;
  callbacks.eof = eof_callback;
  int x, y, n;
  stbi_uc* bytes = stbi_load_from_callbacks(&callbacks, &input, &x, &y, &n, STBI_rgb_alpha);
  if (bytes == nullptr) {
// TODO - stbi_failure_reason is not thread safe. It should be wrapped in a mutex.
    throw runtime_error("Cannot load image: " + string(stbi_failure_reason()));
  }
  bitmap.resize(x, y);
  memcpy(bitmap.data(), bytes, x * y * sizeof(Gosu::Color));
  stbi_image_free(bytes);
  if (needs_color_key) apply_color_key(bitmap, Gosu::Color::FUCHSIA);
}
// Disable comma warnings in stb headers.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomma"
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

void Gosu::save_image_file(const Gosu::Bitmap& bitmap, const string& filename)
{
  int ok;
  if (has_extension(filename, "bmp"))
    ok = stbi_write_bmp(filename.c_str(), bitmap.width(), bitmap.height(), 4, bitmap.data());
  else if (has_extension(filename, "tga"))
    ok = stbi_write_tga(filename.c_str(), bitmap.width(), bitmap.height(), 4, bitmap.data());
  else if (has_extension(filename, "jpg") || has_extension(filename, "jpeg"))
    ok = stbi_write_jpg(filename.c_str(), bitmap.width(), bitmap.height(), 4, bitmap.data(), 95);
  else
    ok = stbi_write_png(filename.c_str(), bitmap.width(), bitmap.height(), 4, bitmap.data(), 0);
  if (ok == 0) throw runtime_error("Could not save image data to file: " + filename);
}

static void stbi_write_to_writer(void* context, void* data, int size)
{
  reinterpret_cast<Gosu::Writer*>(context)->write(data, size);
}

void Gosu::save_image_file(const Gosu::Bitmap& bitmap, Gosu::Writer writer,
                           const string& format_hint)
{
  int ok;
  if (has_extension(format_hint, "bmp")) {
    ok = stbi_write_bmp_to_func(stbi_write_to_writer, &writer, bitmap.width(), bitmap.height(),
      4, bitmap.data());
  } else if (has_extension(format_hint, "tga")) {
    stbi_write_tga_with_rle = 0;
    ok = stbi_write_tga_to_func(stbi_write_to_writer, &writer, bitmap.width(), bitmap.height(),
      4, bitmap.data());
  } else if (has_extension(format_hint, "jpg") || has_extension(format_hint, "jpeg")) {
    ok = stbi_write_jpg_to_func(stbi_write_to_writer, &writer, bitmap.width(), bitmap.height(),
      4, bitmap.data(), 95);
  } else {
    ok = stbi_write_png_to_func(stbi_write_to_writer, &writer, bitmap.width(), bitmap.height(),
      4, bitmap.data(), 0);
  }
  if (ok > 0) return;
  throw runtime_error("Could not save image data to memory (format hint = '" + format_hint + "'");
}

void Gosu::save_image_file(const string& filename, unsigned w, unsigned h, unsigned char* data)
{
  int ok;
  if (has_extension(filename, "bmp"))
    ok = stbi_write_bmp(filename.c_str(), w, h, 4, data);
  else if (has_extension(filename, "tga"))
    ok = stbi_write_tga(filename.c_str(), w, h, 4, data);
  else if (has_extension(filename, "jpg") || has_extension(filename, "jpeg"))
    ok = stbi_write_jpg(filename.c_str(), w, h, 4, data, 95);
  else
    ok = stbi_write_png(filename.c_str(), w, h, 4, data, 0);
  if (ok == 0) throw runtime_error("Could not save image data to file: " + filename);
}
