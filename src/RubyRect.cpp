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

void rect_class_init()
{
  VALUE rect = rb_define_class("Rect", rb_cObject);
  rb_define_attr(rect, "x", 1, 1);
  rb_define_attr(rect, "y", 1, 1);
  rb_define_attr(rect, "width", 1, 1);
  rb_define_attr(rect, "height", 1, 1);
  rb_define_method(rect, "initialize", RUBY_METHOD_FUNC(rect_initialize), 4);
  rb_define_method(rect, "dimensions", RUBY_METHOD_FUNC(rect_dimensions), 0);
  VALUE view = rb_define_class("Viewport", rect);
  rb_define_attr(rect, "horizontal", 1, 1);
  rb_define_attr(rect, "vertical", 1, 1);
  rb_define_method(view, "initialize", RUBY_METHOD_FUNC(viewport_initialize), 4);
  rb_define_method(view, "visible?", RUBY_METHOD_FUNC(viewport_is_visible), 2);
}
