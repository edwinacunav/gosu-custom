/*
** (C) 2019 Kyonides Arkanthes <kyonides@gmail.com>
**
** Usage:
**
** module CustomModule
**   module_accessor :my_method
**   module_accessor :some_method, :just_another_method, :"what_s_up"
** end
**
** It's the same as:
** class << self
*    attr_accessor :some_method, :just_another_method, :"what_s_up"
** end
**
** The same is valid for module_reader and module_writer
**
** Aliases: mod_reader, mod_writer, mod_accessor
*/

#include <ruby.h>
#include "Font.hpp"
#include "debugwriter.h"

static VALUE Color;

namespace Gosu
{
  void enable_flip_h(bool flip_h);
  void enable_flip_y(bool flip_y);
  void enable_flip_h_y(bool flip_h, bool flip_y);
  void draw_rect(double x, double y, double width, double height,
    Gosu::Color c, Gosu::ZPos z = 0, Gosu::AlphaMode mode = Gosu::AM_DEFAULT);
}

static VALUE hc_sym(const char* str)
{
  return rb_id2sym(rb_intern(str));
}

static VALUE hc_sym2(VALUE str)
{
  return rb_id2sym(rb_intern_str(str));
}

static VALUE get_hash_value(VALUE options, char *name)
{
  return rb_hash_delete(options, rb_id2sym(rb_intern(name)));
}

template<class C>
static void dispose(void *instance)
{
  if (!instance) return;
  delete static_cast<C*>(instance);
}

rb_data_type_t ColorType = { "Color", { 0, dispose<Gosu::Color>, 0, { 0, 0 } }, 0, 0, 0 };
rb_data_type_t FontType = { "Font", { 0, dispose<Gosu::Font>, 0, { 0, 0 } }, 0, 0, 0 };

static VALUE color_init(int argc, VALUE* args, VALUE self)
{
  unsigned alpha, red, green, blue;
  Gosu::Color *gc;
  if (argc == 0) {
    gc = new Gosu::Color();
  } else if (argc == 1) {
    gc = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(args[0]));
  } else {//->gl();//ULONG2NUM(args[0]);
    if (argc == 3) {
      alpha = 255;
      red   = RB_FIX2INT(args[0]);
      green = RB_FIX2INT(args[1]);
      blue  = RB_FIX2INT(args[2]);
    } else if (argc == 4) {
      alpha = RB_FIX2INT(args[0]);
      red   = RB_FIX2INT(args[1]);
      green = RB_FIX2INT(args[2]);
      blue  = RB_FIX2INT(args[3]);
    }
    gc = new Gosu::Color(alpha, red, green, blue);
  }
  RTYPEDDATA_DATA(self) = gc;
  return self;
}

static VALUE color_red(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->red());
}

static VALUE color_green(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->green());
}

static VALUE color_blue(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->blue());
}

static VALUE color_alpha(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->alpha());
}

static VALUE color_hue(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->hue());
}

static VALUE color_saturation(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->saturation());
}

static VALUE color_value(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->value());
}

static VALUE color_red_set(VALUE self, VALUE red)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_red(RB_FIX2INT(red));
  return RB_INT2FIX(color->red());
}

static VALUE color_green_set(VALUE self, VALUE green)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_green(RB_FIX2INT(green));
  return RB_INT2FIX(color->green());
}

static VALUE color_blue_set(VALUE self, VALUE blue)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_blue(RB_FIX2INT(blue));
  return RB_INT2FIX(color->blue());
}

static VALUE color_alpha_set(VALUE self, VALUE alpha)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_alpha(RB_FIX2INT(alpha));
  return RB_INT2FIX(color->alpha());
}

static VALUE color_hue_set(VALUE self, VALUE hue)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_hue(RB_FIX2INT(hue));
  return rb_float_new(color->hue());
}

static VALUE color_saturation_set(VALUE self, VALUE saturation)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_alpha(RB_FIX2INT(saturation));
  return rb_float_new(color->saturation());
}

static VALUE color_value_set(VALUE self, VALUE value)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->set_value(RB_FIX2INT(value));
  return rb_float_new(color->value());
}

static VALUE color_invert(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  Gosu::Color *gc = new Gosu::Color(color->invert());
  VALUE rc = rb_obj_alloc(Color);
  RTYPEDDATA_DATA(rc) = gc;
  return rc;
}

static VALUE color_invert_bang(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  color->inverted();
  RTYPEDDATA_DATA(self) = color;
  return self;
}

static VALUE color_gl(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  return RB_INT2FIX(color->gl());
}

static VALUE color_dup(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  Gosu::Color *gc = new Gosu::Color(*color);
  VALUE cdup = rb_obj_alloc(Color);
  RTYPEDDATA_DATA(cdup) = gc;
  return cdup;
}

static VALUE color_inspect(VALUE self)
{
  Gosu::Color *color = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(self));
  if (!color) return Qnil;
  char buffer[64];
  sprintf(buffer, "#<Color:ARGB=0x%02x_%02x_%02x_%02x>",
    color->alpha(), color->red(), color->green(), color->blue());
  return rb_str_new_cstr(buffer);
}

static VALUE font_init(int argc, VALUE* options, VALUE self)
{
  unsigned font_flags = 0, height = 22;
  if (!options) rb_raise(rb_eArgError, "wrong number of arguments (provided 0, expected 1..3");
  std::string font_name = Gosu::default_font_name();
  VALUE zero = RB_INT2FIX(0), n = RB_INT2FIX(255);
  rb_iv_set(self, "@text", rb_str_new_cstr(""));
  rb_iv_set(self, "@height", RB_INT2FIX(22));
  rb_iv_set(self, "@base_x", zero);
  rb_iv_set(self, "@x", zero);
  rb_iv_set(self, "@y", zero);
  rb_iv_set(self, "@z", RB_INT2FIX(1));
  rb_iv_set(self, "@rel_x", rb_float_new(0.0));
  rb_iv_set(self, "@rel_y", rb_float_new(0.0));
  rb_iv_set(self, "@scale_x", RB_INT2FIX(1));
  rb_iv_set(self, "@scale_y", RB_INT2FIX(1));
  rb_iv_set(self, "@width", RB_INT2FIX(120));
  rb_iv_set(self, "@max_width", RB_INT2FIX(120));
  rb_iv_set(self, "@side", hc_sym("left"));
  VALUE argb[4] = { n, n, n, n };
  VALUE c = rb_obj_alloc(Color);
  rb_iv_set(self, "@color", color_init(4, argb, c));
  rb_iv_set(self, "@mode", zero);
  VALUE name = options[0];
  if (RB_TYPE_P(name, T_STRING) && rb_str_strlen(name) > 0)
    font_name = StringValueCStr(name);
  if (!RB_NIL_P(options[1]))
    height = RB_FIX2INT(options[1]);
  VALUE hash = options[2];
  if (RB_TYPE_P(hash, T_HASH)) {
    if (get_hash_value(hash, "bold"))
      font_flags |= Gosu::FF_BOLD;
    if (get_hash_value(hash, "italic"))
      font_flags |= Gosu::FF_ITALIC;
    if (get_hash_value(hash, "underline"))
      font_flags |= Gosu::FF_UNDERLINE;
    VALUE max = get_hash_value(hash, "max");
    if (RB_TYPE_P(max, T_FIXNUM))
      rb_iv_set(self, "@max_width", max);
    if (get_hash_value(hash, "flip_h"))
      Gosu::enable_flip_h(true);
    if (get_hash_value(hash, "flip_y"))
      Gosu::enable_flip_y(true);
    VALUE clr = get_hash_value(hash, "c");
    if (RB_NIL_P(clr))
      clr = get_hash_value(hash, "color");
    if (!RB_NIL_P(clr))
      rb_iv_set(self, "@color", clr);
    /*if (get_hash_value(hash, "invert")) {
      normal_color = 0;std::string fn = StringValueCStr(source);
      Gosu::load_image_inverse_color(bmp, fn); }*/
  }
  Gosu::Font *f = new Gosu::Font(height, font_name, font_flags);
  RTYPEDDATA_DATA(self) = f;
  f->set_visible(true);
  return self;
}

double font_text_width_double(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font || RSTRING_LEN(rb_iv_get(self, "@text")) == 0) return 0;
  return font->set_text_width();
}

static VALUE font_get_text_width(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font || RSTRING_LEN(rb_iv_get(self, "@text")) == 0) return RB_INT2FIX(0);
  VALUE w = rb_float_new(font->set_text_width());
  return rb_iv_set(self, "@width", w);
}

static VALUE font_text_width(VALUE self, VALUE text)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return RB_INT2FIX(0);
  std::string ctext = StringValueCStr(text);
  VALUE w = rb_float_new(font->text_width(ctext));
  return rb_iv_set(self, "@width", w);
}

static VALUE font_markup_width(VALUE self, VALUE markup)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return RB_INT2FIX(0);
  std::string text = StringValueCStr(markup);
  VALUE w = rb_float_new(font->markup_width(text));
  return rb_iv_set(self, "@width", w);
}

static VALUE font_update_align(VALUE self)
{
  VALUE final_x, side = rb_iv_get(self, "@side");
  int b = RB_FIX2INT(rb_iv_get(self, "@base_x"));
  int m = RB_FIX2INT(rb_iv_get(self, "@max_width"));
  double w = font_text_width_double(self);
  if (side == hc_sym("left")) return Qnil;
  if (side == hc_sym("center"))
    final_x = RB_INT2FIX(b + (m - w) / 2);
  else if (side == hc_sym("right"))
    final_x = RB_INT2FIX(b + m - w);
  rb_iv_set(self, "@width", w);
  rb_iv_set(self, "@x", final_x);
  return Qnil;
}

static VALUE font_xy_side(VALUE self, VALUE x, VALUE y, VALUE side)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  rb_iv_set(self, "@base_x", x);
  rb_iv_set(self, "@x", x);
  rb_iv_set(self, "@y", y);
  if (!RB_NIL_P(side)) rb_iv_set(self, "@side", side);
  font_update_align(self);
  return rb_ary_new3(3, x, y, side);
}

static VALUE font_xyz_side(VALUE self, VALUE x, VALUE y, VALUE z, VALUE side)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  rb_iv_set(self, "@base_x", x);
  rb_iv_set(self, "@x", x);
  rb_iv_set(self, "@y", y);
  rb_iv_set(self, "@z", z);
  if (!RB_NIL_P(side)) rb_iv_set(self, "@side", side);
  font_update_align(self);
  return rb_ary_new3(4, x, y, z, side);
}

static VALUE font_x_set(VALUE self, VALUE nx)
{
  rb_iv_set(self, "@base_x", nx);
  bool same = rb_iv_get(self, "@side") == hc_sym("left");
  same ? rb_iv_set(self, "@x", nx) : font_update_align(self);
  return rb_iv_get(self, "@x");
}

static VALUE font_xy(VALUE self, VALUE x, VALUE y)
{
  return font_xy_side(self, x, y, Qnil);
}

static VALUE font_xyz(VALUE self, VALUE x, VALUE y, VALUE z)
{
  return font_xyz_side(self, x, y, z, Qnil);
}

static VALUE font_scale_xy(VALUE self, VALUE x, VALUE y)
{
  rb_iv_set(self, "@scale_x", x);
  rb_iv_set(self, "@scale_y", y);
  return rb_ary_new3(2, x, y);
}

static VALUE font_name(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  const char* name = !font ? "" : font->name().c_str();
  return rb_str_new_cstr(name);
}

static VALUE font_text(VALUE self)
{
  return rb_iv_get(self, "@text");
}

static VALUE font_color(VALUE self)
{
  return rb_iv_get(self, "@color");
}

static VALUE font_mode(VALUE self)
{
  VALUE mode = rb_iv_get(self, "@mode");
  if (mode == RB_FIX2INT(0)) return rb_str_new_cstr("default");
  if (mode == RB_FIX2INT(1)) return rb_str_new_cstr("add");
  if (mode == RB_FIX2INT(2)) return rb_str_new_cstr("multiply");
}

static VALUE font_visible(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  return font->visible() ? Qtrue : Qfalse;
}

static VALUE font_text_set(VALUE self, VALUE txt)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  if (RB_NIL_P(txt) || RB_TYPE_P(txt, T_FIXNUM))
    txt = rb_funcall(txt, rb_intern("to_s"), 0);
  std::string str = StringValueCStr(txt);
  font->set_text(str);
  font_get_text_width(self);
  if (rb_iv_get(self, "@side") != hc_sym("left")) font_update_align(self);
  return rb_iv_set(self, "@text", txt);
}

static VALUE font_height_set(VALUE self, VALUE height)
{
  if (!RB_TYPE_P(height, T_FIXNUM)) return rb_iv_get(self, "@height");
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return RB_INT2FIX(0);
  font->height();
  return rb_iv_set(self, "@height", height);
}

static VALUE font_color_set(VALUE self, VALUE color)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  VALUE old_color = rb_iv_get(self, "@color");
  if (RB_NIL_P(color)) return old_color;
  Gosu::Color *c = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(color));
  font->set_color(c);//RTYPEDDATA_DATA(color) = c;
  return rb_iv_set(self, "@color", color);
}

static VALUE font_mode_set(VALUE self, VALUE mode)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  if (hc_sym("add") == mode) {
    rb_iv_set(self, "@mode", RB_INT2FIX(1));
    return mode;
  } else if (hc_sym("multiply") == mode) {
    rb_iv_set(self, "@mode", RB_INT2FIX(2));
    return mode;
  }
  rb_iv_set(self, "@mode", RB_INT2FIX(0));
  return hc_sym("default");
}

static VALUE font_visible_set(VALUE self, VALUE boolean)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  font->set_visible(boolean == Qtrue || boolean == RB_FIX2INT(1));
  return boolean;
}

static VALUE font_flags(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  return !font ? Qnil : font->flags();
}

static VALUE font_draw_markup(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font || !font->visible()) return Qnil;
  VALUE rstr = rb_iv_get(self, "@text");
  std::string markup = StringValueCStr(rstr);
  double x = NUM2DBL(rb_iv_get(self, "@x"));
  double y = NUM2DBL(rb_iv_get(self, "@y"));
  double z = NUM2DBL(rb_iv_get(self, "@z"));
  double sx = NUM2DBL(rb_iv_get(self, "@scale_x"));
  double sy = NUM2DBL(rb_iv_get(self, "@scale_y"));
  VALUE rc = rb_iv_get(self, "@color");
  Gosu::Color *c = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(rc));
  Gosu::AlphaMode mode = (Gosu::AlphaMode)RB_FIX2INT(rb_iv_get(self, "@mode"));
  font->draw_markup(markup, x, y, z, sx, sy, *c, mode);
  return Qnil;
}

static VALUE font_draw_markup_rel(VALUE self)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  VALUE rstr = rb_iv_get(self, "@text");
  std::string markup = StringValueCStr(rstr);
  double x = NUM2DBL(rb_iv_get(self, "@x"));
  double y = NUM2DBL(rb_iv_get(self, "@y"));
  double z = NUM2DBL(rb_iv_get(self, "@z"));
  double rx = NUM2DBL(rb_iv_get(self, "@rel_x"));
  double ry = NUM2DBL(rb_iv_get(self, "@rel_y"));
  double sx = NUM2DBL(rb_iv_get(self, "@scale_x"));
  double sy = NUM2DBL(rb_iv_get(self, "@scale_y"));
  VALUE rc = rb_iv_get(self, "@color");
  Gosu::Color *c = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(rc));
  Gosu::AlphaMode mode = (Gosu::AlphaMode)RB_FIX2INT(rb_iv_get(self, "@mode"));
  font->draw_markup_rel(markup, x, y, z, rx, ry, sx, sy, *c, mode);
  return Qnil;
}

static VALUE font_set_image(VALUE self, VALUE str, VALUE img) {
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  std::string cs = StringValueCStr(str);
  Gosu::Image *gimg = static_cast<Gosu::Image*>(RTYPEDDATA_DATA(img));
  font->set_image(cs, (Gosu::Image const &)*gimg);
  return Qnil;
}

static VALUE rect_initialize(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h)
{
  rb_iv_set(self, "@x", x);
  rb_iv_set(self, "@y", y);
  rb_iv_set(self, "@width", w);
  rb_iv_set(self, "@height", h);
  return self;
}

static VALUE rect_dimensions(VALUE self)
{
  VALUE ary = rb_ary_new3(2, rb_iv_get(self, "@x"), rb_iv_get(self, "@y"));
  rb_ary_push(ary, rb_iv_get(self, "@width"));
  rb_ary_push(ary, rb_iv_get(self, "@height"));
  return ary;
}

static VALUE drawrect_initialize(int argc, VALUE* args, VALUE self)
{
  if (argc < 4)
    rb_raise(rb_eArgError, "wrong number of arguments (given %s, expected 4..7)", argc);
  rb_call_super(4, args);
  VALUE color, mode, zero = RB_INT2FIX(0);
  VALUE z = argc < 5 ? zero : args[4];
  if (argc < 6) {
    Gosu::Color *gc = new Gosu::Color(255, 255, 255, 255);
    color = rb_obj_alloc(Color);
    RTYPEDDATA_DATA(color) = gc;
  } else {
    color = args[5];
    mode = argc < 7 ? zero : args[6];
  }
  rb_iv_set(self, "@color", color);
  rb_iv_set(self, "@z", z);
  rb_iv_set(self, "@mode", mode);
  return self;
}

static VALUE drawrect_color_set(VALUE self, VALUE color)
{
  VALUE old_color = rb_iv_get(self, "@color");
  if (RB_NIL_P(color)) return old_color;
  Gosu::Color *c = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(color));
  RTYPEDDATA_DATA(color) = c;
  return rb_iv_set(self, "@color", color);
}

static VALUE drawrect_mode_set(VALUE self, VALUE mode)
{
  if (hc_sym("add") == mode) {
    rb_iv_set(self, "@mode", RB_INT2FIX(1));
    return mode;
  } else if (hc_sym("multiply") == mode) {
    rb_iv_set(self, "@mode", RB_INT2FIX(2));
    return mode;
  }
  rb_iv_set(self, "@mode", RB_INT2FIX(0));
  return hc_sym("default");
}

static VALUE drawrect_draw(VALUE self) {
  double x = rb_num2dbl(rb_iv_get(self, "@x"));
  double y = rb_num2dbl(rb_iv_get(self, "@y"));
  double w = rb_num2dbl(rb_iv_get(self, "@width"));
  double h = rb_num2dbl(rb_iv_get(self, "@height"));
  double z = rb_num2dbl(rb_iv_get(self, "@z"));
  VALUE rc = rb_iv_get(self, "@color");
  Gosu::Color *c = static_cast<Gosu::Color*>(RTYPEDDATA_DATA(rc));
  Gosu::AlphaMode mode = (Gosu::AlphaMode)RB_FIX2INT(rb_iv_get(self, "@mode"));
  Gosu::draw_rect(x, y, w, h, *c, z, mode);
  return Qnil;
}

static VALUE viewport_initialize(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h)
{
  VALUE param[4] = { x, y, w, h };
  rb_call_super(4, param);
  VALUE xw = rb_funcall(x, rb_intern("+"), 1, w);
  VALUE yh = rb_funcall(y, rb_intern("+"), 1, h);
  rb_iv_set(self, "@horizontal", rb_range_new(x, xw, 1));
  rb_iv_set(self, "@vertical", rb_range_new(y, yh, 1));
  return self;
}

static VALUE viewport_is_visible(VALUE self, VALUE nx, VALUE ny)
{
  VALUE side = rb_iv_get(self, "@horizontal");
  if (!rb_funcall(side, rb_intern("include?"), 1, nx)) return Qfalse;
  side = rb_iv_get(self, "@vertical");
  return rb_funcall(side, rb_intern("include?"), 1, ny);
}

static VALUE module_reader(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_reader is exclusive for modules!");
  VALUE args[argc], basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    rb_define_attr(self, func_name, 1, 0);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(argc, argv, meth);
  return Qnil;
}

static VALUE module_writer(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_writer is exclusive for modules!");
  VALUE basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    new_str = rb_str_plus(basic_str, rb_str_new_cstr("="));
    argv[n] = hc_sym2(new_str);
    rb_define_attr(self, func_name, 0, 1);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(argc, argv, meth);
  return Qnil;
}

static VALUE module_accessor(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_accessor is exclusive for modules!");
  int m, count = argc * 2;
  VALUE args[count], basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    m = n * 2;
    args[m] = argv[n];
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    rb_define_attr(self, func_name, 1, 1);
    new_str = rb_str_plus(basic_str, rb_str_new_cstr("="));
    args[m + 1] = hc_sym2(new_str);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(count, args, meth);
  return Qnil;
}

#define RMF(func) ((VALUE (*)(ANYARGS))(func))

template<rb_data_type_t *rb_Type>
static VALUE allocate(VALUE klass)
{
  return rb_data_typed_object_wrap(klass, 0, rb_Type);
}

void ruby_ext_init()
{
  VALUE mGosu = rb_define_module("Gosu");
  rb_define_const(mGosu, "MAJOR_VERSION", RB_INT2FIX(0));
  rb_define_const(mGosu, "MINOR_VERSION", RB_INT2FIX(15));
  rb_define_const(mGosu, "POINT_VERSION", RB_INT2FIX(0));
  VALUE str = rb_str_new_cstr("Julian Raschke, Jan Lücker, Kyonides Arkanthes and others");
  rb_define_const(mGosu, "CONTRIBUTORS", str);
  //Color = rb_define_class_under(mGosu, "Color", rb_cObject);
  Color = rb_define_class("Color", rb_cObject);
  rb_define_alloc_func(Color, allocate<&ColorType>);
  rb_define_method(Color, "initialize", RMF(color_init), -1);
  rb_define_const(Color, "GL_FORMAT", RB_INT2FIX(Gosu::Color::GL_FORMAT));
  //rb_define_singleton_method(Color, "from_hsv", RMF(color_from_hsv), -1);
  //rb_define_singleton_method(Color, "from_ahsv", RMF(color_from_ahsv), -1);
  rb_define_method(Color, "red", RMF(color_red), 0);
  rb_define_method(Color, "green", RMF(color_green), 0);
  rb_define_method(Color, "blue", RMF(color_blue), 0);
  rb_define_method(Color, "alpha", RMF(color_alpha), 0);
  rb_define_method(Color, "hue", RMF(color_hue), 0);
  rb_define_method(Color, "saturation", RMF(color_saturation), 0);
  rb_define_method(Color, "value", RMF(color_value), 0);
  rb_define_method(Color, "red=", RMF(color_red_set), 1);
  rb_define_method(Color, "green=", RMF(color_green_set), 1);
  rb_define_method(Color, "blue=", RMF(color_blue_set), 1);
  rb_define_method(Color, "alpha=", RMF(color_alpha_set), 1);
  rb_define_method(Color, "hue=", RMF(color_hue_set), 1);
  rb_define_method(Color, "saturation=", RMF(color_saturation_set), 1);
  rb_define_method(Color, "value=", RMF(color_value_set), 1);
  rb_define_method(Color, "invert", RMF(color_invert), 0);
  rb_define_method(Color, "invert!", RMF(color_invert_bang), 0);
  //rb_define_method(Color, "bgr", RMF(color_bgr), 0);
  //rb_define_method(Color, "abgr", RMF(color_abgr), 0);
  rb_define_method(Color, "gl", RMF(color_gl), 0);
  //rb_define_singleton_method(Color, "rgb", RMF(color_rgb), 0);
  //rb_define_singleton_method(Color, "rgba", RMF(color_rgba), 0);
  //rb_define_singleton_method(Color, "argb", RMF(color_argb), 0);
  rb_define_method(Color, "dup", RMF(color_dup), 0);
  rb_define_method(Color, "inspect", RMF(color_inspect), 0);
  //rb_define_method(Color, "==", RMF(color_equal), 1);
  VALUE font = rb_define_class("Font", rb_cObject);
  rb_define_alloc_func(font, allocate<&FontType>);
  rb_define_attr(font, "base_x", 1, 0);
  rb_define_attr(font, "x", 1, 0);
  rb_define_attr(font, "y", 1, 1);
  rb_define_attr(font, "z", 1, 1);
  rb_define_attr(font, "scale_x", 1, 1);
  rb_define_attr(font, "scale_y", 1, 1);
  rb_define_attr(font, "width", 1, 0);
  rb_define_attr(font, "height", 1, 0);
  rb_define_attr(font, "max_width", 1, 1);
  rb_define_method(font, "initialize", RMF(font_init), -1);
  rb_define_method(font, "x=", RMF(font_x_set), 1);
  rb_define_method(font, "xy_side", RMF(font_xy_side), 3);
  rb_define_method(font, "xyz_side", RMF(font_xyz_side), 4);
  rb_define_method(font, "xy", RMF(font_xy), 2);
  rb_define_method(font, "xyz", RMF(font_xyz), 3);
  rb_define_method(font, "scale_xy", RMF(font_scale_xy), 2);
  rb_define_method(font, "update_align", RMF(font_update_align), 0);
  rb_define_method(font, "name", RMF(font_name), 0);
  rb_define_method(font, "text", RMF(font_text), 0);
  rb_define_method(font, "color", RMF(font_color), 0);
  rb_define_method(font, "mode", RMF(font_mode), 0);
  rb_define_method(font, "visible", RMF(font_visible), 0);
  rb_define_method(font, "text=", RMF(font_text_set), 1);
  rb_define_method(font, "height=", RMF(font_height_set), 1);
  rb_define_method(font, "color=", RMF(font_color_set), 1);
  rb_define_method(font, "mode=", RMF(font_mode_set), 1);
  rb_define_method(font, "visible=", RMF(font_visible_set), 1);
  rb_define_method(font, "flags", RMF(font_flags), 0);
  rb_define_method(font, "markup_width", RMF(font_markup_width), 1);
  rb_define_method(font, "compute_width", RMF(font_get_text_width), 0);
  rb_define_method(font, "text_width", RMF(font_text_width), 1);
  rb_define_method(font, "draw_text", RMF(font_draw_markup), 0);
  rb_define_method(font, "draw_markup", RMF(font_draw_markup), 0);
  rb_define_method(font, "draw_text_rel", RMF(font_draw_markup_rel), 0);
  rb_define_method(font, "draw_markup_rel", RMF(font_draw_markup_rel), 0);
  rb_define_method(font, "set_image", RMF(font_set_image), -1);
  VALUE rect = rb_define_class("Rect", rb_cObject);
  rb_define_attr(rect, "x", 1, 1);
  rb_define_attr(rect, "y", 1, 1);
  rb_define_attr(rect, "width", 1, 1);
  rb_define_attr(rect, "height", 1, 1);
  rb_define_method(rect, "initialize", RMF(rect_initialize), 4);
  rb_define_method(rect, "dimensions", RMF(rect_dimensions), 0);
  VALUE drawrect = rb_define_class("DrawRect", rect);
  rb_define_attr(drawrect, "z", 1, 1);
  rb_define_attr(drawrect, "color", 1, 0);
  rb_define_attr(drawrect, "mode", 1, 0);
  rb_define_method(drawrect, "initialize", RMF(drawrect_initialize), -1);
  rb_define_method(drawrect, "color=", RMF(drawrect_color_set), 1);
  rb_define_method(drawrect, "mode=", RMF(drawrect_mode_set), 1);
  rb_define_method(drawrect, "draw", RMF(drawrect_draw), 0);
  VALUE view = rb_define_class("Viewport", rect);
  rb_define_attr(rect, "horizontal", 1, 1);
  rb_define_attr(rect, "vertical", 1, 1);
  rb_define_method(view, "initialize", RMF(viewport_initialize), 4);
  rb_define_method(view, "visible?", RMF(viewport_is_visible), 2);
  rb_define_method(rb_cModule, "module_reader", RMF(module_reader), -1);
  rb_define_method(rb_cModule, "module_writer", RMF(module_writer), -1);
  rb_define_method(rb_cModule, "module_accessor", RMF(module_accessor), -1);
  rb_define_method(rb_cModule, "mod_reader", RMF(module_reader), -1);
  rb_define_method(rb_cModule, "mod_writer", RMF(module_writer), -1);
  rb_define_method(rb_cModule, "mod_accessor", RMF(module_accessor), -1);
}