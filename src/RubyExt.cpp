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

static VALUE hc_sym(const char* str)
{
  return rb_id2sym(rb_intern(str));
}

static VALUE hc_sym2(VALUE str)
{
  return rb_id2sym(rb_intern_str(str));
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

void ruby_ext_init()
{
  VALUE rect = rb_define_class("Rect", rb_cObject);
  rb_define_attr(rect, "x", 1, 1);
  rb_define_attr(rect, "y", 1, 1);
  rb_define_attr(rect, "width", 1, 1);
  rb_define_attr(rect, "height", 1, 1);
  rb_define_method(rect, "initialize", RMF(rect_initialize), 4);
  rb_define_method(rect, "dimensions", RMF(rect_dimensions), 0);
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