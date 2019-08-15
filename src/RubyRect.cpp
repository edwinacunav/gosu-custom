/*
** Kyonides Arkanthes <kyonides@gmail.com>
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

void rect_class_init()
{
  VALUE rect = rb_define_class("Rect", rb_cObject);
  rb_define_attr(rect, "x", 1, 1);
  rb_define_attr(rect, "y", 1, 1);
  rb_define_attr(rect, "width", 1, 1);
  rb_define_attr(rect, "height", 1, 1);
  rb_define_method(rect, "initialize", RUBY_METHOD_FUNC(rect_initialize), 4);
  rb_define_method(rect, "dimensions", RUBY_METHOD_FUNC(rect_dimensions), 0);
}
