/*
** Modified by Kyonides Arkanthes (C) 2019
*/
//! \file Bitmap.hpp
//! Interface of the Bitmap class.

#pragma once

#include "Fwd.hpp"
#include "Color.hpp"
#include "GraphicsBase.hpp"
#include "Platform.hpp"
#include <string>
#include <vector>
#include "debugwriter.h"
namespace Gosu
{ //! Rectangular area of pixels, each represented by a Color value. Provides
  //! minimal drawing functionality and serves as a temporary holder for
  //! graphical resources which are usually turned into Images later.
  //! Has (expensive) value semantics.
  class Bitmap
  {
    unsigned w, h;

public:
    std::vector<Color> pixels;
    Bitmap() : w(0), h(0) {}
    Bitmap(unsigned w, unsigned h, Color c = Color::NONE)
    : w(w), h(h), pixels(w * h, c)
    {}

    unsigned width() const
    {
      return w;
    }

    unsigned height() const
    {
      return h;
    }

    void swap(Bitmap& other);
    void resize(unsigned width, unsigned height, Color c = Color::NONE);
    //! Returns the color at the specified position. x and y must be on the
    //! bitmap.
    Color get_pixel(unsigned x, unsigned y) const
    {
      return pixels[y * w + x];
    }
    //! Sets the pixel at the specified position to a color. x and y must
    //! be on the bitmap.
    void set_pixel(unsigned x, unsigned y, Color c)
    {
      pixels[y * w + x] = c;
    }
    //! This updates a pixel using the "over" alpha compositing operator, see:
    //! https://en.wikipedia.org/wiki/Alpha_compositing
    void blend_pixel(unsigned x, unsigned y, Color c);
    //! Inverts the current pixel, ignored if alpha value is 0.
    void invert_pixel(unsigned x, unsigned y);
    //! Inserts a bitmap at the given position. Parts of the inserted
    //! bitmap that would be outside of the target bitmap will be
    //! clipped away.
    void insert(const Bitmap& source, int x, int y);
    //! Inserts a portion of a bitmap at the given position. Parts of the
    //! inserted bitmap that would be outside of the target bitmap will be
    //! clipped away.
    void insert(const Bitmap& source, int x, int y, unsigned src_x, unsigned src_y,
        unsigned src_width, unsigned src_height);
    //! Direct access to the array of color values. May be useful for optimized
    //! OpenGL operations.
    const Color* data() const
    {
      return &pixels[0];
    }
    Color* data()
    {
      return &pixels[0];
    }
  };
  void enable_flip_h(bool flip_h);
  void enable_flip_y(bool flip_y);
  void enable_flip_h_y(bool flip_h, bool flip_y);
  void load_image_inverse_color(Bitmap& bitmap, const std::string& filename);
  //! Loads any supported image into a Bitmap.
  void load_image_file(Bitmap& bitmap, const std::string& filename);
  //! Loads any supported image into a Bitmap.
  void load_image_file(Bitmap& bitmap, Reader input);
  //! Saves a Bitmap to a file.
  void save_image_file(const Bitmap& bitmap, const std::string& filename);
  //! Saves a Bitmap to an arbitrary resource.
  void save_image_file(const Bitmap& bitmap, Writer writer,
    const std::string& format_hint = "png");
  void save_image_file(const std::string& filename, unsigned w, unsigned h, unsigned char*  data);
  //! Set the alpha value of all pixels which are equal to the color key
  //! to zero. Color values are adjusted so that no borders show up when
  //! the image is stretched or rotated.
  void apply_color_key(Bitmap& bitmap, Color key);
  //! The reverse of apply_color_key. Resets all fully transparent pixels by
  //! a background color, makes all other pixels fully opaque.
  void unapply_color_key(Bitmap& bitmap, Color background);
  void apply_border_flags(Bitmap& dest, const Bitmap& source, unsigned src_x, unsigned src_y,
    unsigned src_width, unsigned src_height, unsigned border_flags);
}
