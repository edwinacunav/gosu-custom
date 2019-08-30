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

namespace Gosu
{
  void enable_flip_h(bool flip_h);
  void enable_flip_y(bool flip_y);
  void enable_flip_h_y(bool flip_h, bool flip_y);
}

static VALUE get_hash_value(VALUE options, char *name)
{
  return rb_hash_delete(options, rb_id2sym(rb_intern(name)));
}

static void font_dispose(void *instance)
{
  delete static_cast<Gosu::Font*>(instance);
}

rb_data_type_t FontType = { "TempFont", { 0, font_dispose, 0, { 0, 0 } }, 0, 0, 0 };

static VALUE font_init(int argc, VALUE* options, VALUE self)
{
  unsigned font_flags = 0, height = 22;
  if (!options) rb_raise(rb_eArgError, "wrong number of arguments (provided 0, expected 1..3");
  std::string font_name = Gosu::default_font_name();
  VALUE name = options[0];
  if (!RB_NIL_P(name) && RSTRING_LEN(name) > 0) font_name = StringValueCStr(name);
  if (!RB_NIL_P(options[1])) height = RB_FIX2INT(options[1]);
  VALUE hash = options[2];
  if (RB_TYPE_P(hash, T_HASH)) {
    rb_p(rb_str_new_cstr("Received TempFont Options"));
    if (get_hash_value(hash, "bold"))// == Qtrue)
      font_flags |= Gosu::FF_BOLD;
    if (get_hash_value(hash, "italic"))
      font_flags |= Gosu::FF_ITALIC;
    if (get_hash_value(hash, "underline"))
      font_flags |= Gosu::FF_UNDERLINE;
    if (get_hash_value(hash, "flip_h"))// == Qtrue)
      Gosu::enable_flip_h(true);
    if (get_hash_value(hash, "flip_y"))// == Qtrue)
      Gosu::enable_flip_y(true);
    /*if (get_hash_value(hash, "invert") == Qtrue) {
      normal_color = 0;
      std::string fn = StringValueCStr(source);
      Gosu::load_image_inverse_color(bmp, fn);
    }*/
  }
  Gosu::Font *f = new Gosu::Font(height, font_name, font_flags);
  RTYPEDDATA_DATA(self) = f;
  return self;
}

static VALUE font_name(VALUE self) {
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  return !font ? rb_str_new_cstr("") : rb_str_new_cstr(font->name().c_str());
}

static VALUE font_height(VALUE self) {
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  return !font ? RB_INT2FIX(0) : RB_INT2FIX(font->height());
}

static VALUE font_flags(VALUE self) {
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  return !font ? Qnil : font->flags();
}

static VALUE font_markup_width(VALUE self, VALUE markup)
{
  Gosu::Font *font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return RB_INT2FIX(0);
  std::string text = StringValueCStr(markup);
  return DBL2NUM(font->markup_width(text));
}

/*static VALUE font_draw_markup(int argc, VALUE *argv, VALUE self) {
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  std::string *arg2 = 0 ;
  double arg3 ;
  double arg4 ;
  Gosu::ZPos arg5 ;
  double arg6 = (double) 1 ;
  double arg7 = (double) 1 ;
  Gosu::Color arg8 = (Gosu::Color) Gosu::Color::WHITE ;
  Gosu::AlphaMode arg9 = (Gosu::AlphaMode) Gosu::AM_DEFAULT ;
  void *argp1 = 0 ;
  int res1 = 0 ;
  int res2 = SWIG_OLDOBJ ;
  double val3 ;
  int ecode3 = 0 ;
  double val4 ;
  int ecode4 = 0 ;
  double val5 ;
  int ecode5 = 0 ;
  double val6 ;
  int ecode6 = 0 ;
  double val7 ;
  int ecode7 = 0 ;
  if ((argc < 4) || (argc > 8))
    rb_raise(rb_eArgError, "wrong # of arguments(%d for 4)",argc);// SWIG_fail;
  res1 = SWIG_ConvertPtr(self, &argp1, Gosu_Font, 0 | 0);
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), Ruby_Format_TypeError("", "Gosu::Font", "draw_markup", 1, self));
  arg1 = reinterpret_cast< Gosu::Font * >(argp1);
  std::string *ptr = (std::string *)0;
  res2 = SWIG_AsPtr_std_string(argv[0], &ptr);
  if (!SWIG_IsOK(res2))
    SWIG_exception_fail(SWIG_ArgError(res2), Ruby_Format_TypeError("", "String", "draw_markup", 2, argv[0]));
  if (!ptr) {
    SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "std::string const &","draw_markup", 2, argv[0])); 
  }
  arg2 = ptr;
  ecode3 = SWIG_AsVal_double(argv[1], &val3);
  if (!SWIG_IsOK(ecode3))
    SWIG_exception_fail(SWIG_ArgError(ecode3), Ruby_Format_TypeError("", "Float", "draw_markup", 3, argv[1]));
  arg3 = static_cast< double >(val3);
  ecode4 = SWIG_AsVal_double(argv[2], &val4);
  if (!SWIG_IsOK(ecode4))
    SWIG_exception_fail(SWIG_ArgError(ecode4), Ruby_Format_TypeError("", "Float", "draw_markup", 4, argv[2]));
  arg4 = static_cast< double >(val4);
  ecode5 = SWIG_AsVal_double(argv[3], &val5);
  if (!SWIG_IsOK(ecode5))
    SWIG_exception_fail(SWIG_ArgError(ecode5), Ruby_Format_TypeError("", "Gosu::ZPos", "draw_markup", 5, argv[3]));
  arg5 = static_cast< Gosu::ZPos >(val5);
  if (argc > 4) {
    ecode6 = SWIG_AsVal_double(argv[4], &val6);
    if (!SWIG_IsOK(ecode6)) {
      SWIG_exception_fail(SWIG_ArgError(ecode6), Ruby_Format_TypeError("", "Float", "draw_markup", 6, argv[4]));
    arg6 = static_cast< double >(val6);
  }
  if (argc > 5) {
    ecode7 = SWIG_AsVal_double(argv[5], &val7);
    if (!SWIG_IsOK(ecode7))
      SWIG_exception_fail(SWIG_ArgError(ecode7), Ruby_Format_TypeError("", "Float", "draw_markup", 7, argv[5]));
    arg7 = static_cast< double >(val7);
  }
  if (argc > 6) {
    if (TYPE(argv[6]) == T_FIXNUM || TYPE(argv[6]) == T_BIGNUM) {
      arg8 = Gosu::Color(NUM2ULONG(argv[6]));
    } else {
      void* ptr;
      int res = SWIG_ConvertPtr(argv[6], &ptr, SWIGTYPE_p_Gosu__Color, 0);
      if (!SWIG_IsOK(res)) {
        SWIG_exception_fail(SWIG_ValueError, "invalid value");
      } else if (ptr == nullptr) {
        SWIG_exception_fail(SWIG_ValueError, "invalid null reference of type Gosu::Color");
      } else {
        arg8 = *reinterpret_cast<Gosu::Color*>(ptr);
      }
    }
  }
  if (argc > 7) {
    const char* cstr = Gosu::cstr_from_symbol(argv[7]);
    if (!strcmp(cstr, "default")) {
      arg9 = Gosu::AM_DEFAULT;
    } else if (!strcmp(cstr, "add") || !strcmp(cstr, "additive")) {
      arg9 = Gosu::AM_ADD;
    } else if (!strcmp(cstr, "multiply")) {
      arg9 = Gosu::AM_MULTIPLY;
    } else {
      SWIG_exception_fail(SWIG_ValueError, "invalid alpha mode (expected one of :default, :add, "
        ":multiply)");
    }
  }
  try {
    ((Gosu::Font const *)arg1)->draw_markup((std::string const &)*arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  if (SWIG_IsNewObj(res2)) delete arg2;
  return Qnil;
}

static VALUE font_draw_markup_rel(int argc, VALUE *argv, VALUE self) {
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  std::string *arg2 = 0 ;
  double arg3 ;
  double arg4 ;
  Gosu::ZPos arg5 ;
  double arg6 ;
  double arg7 ;
  double arg8 = (double) 1 ;
  double arg9 = (double) 1 ;
  Gosu::Color arg10 = (Gosu::Color) Gosu::Color::WHITE ;
  Gosu::AlphaMode arg11 = (Gosu::AlphaMode) Gosu::AM_DEFAULT ;
  void *argp1 = 0 ;
  int res1 = 0 ;
  int res2 = SWIG_OLDOBJ ;
  double val3 ;
  int ecode3 = 0 ;
  double val4 ;
  int ecode4 = 0 ;
  double val5 ;
  int ecode5 = 0 ;
  double val6 ;
  int ecode6 = 0 ;
  double val7 ;
  int ecode7 = 0 ;
  double val8 ;
  int ecode8 = 0 ;
  double val9 ;
  int ecode9 = 0 ;
  if ((argc < 6) || (argc > 10))
    rb_raise(rb_eArgError, "wrong # of arguments(%d for 6)",argc);
  res1 = SWIG_ConvertPtr(self, &argp1,SWIGTYPE_p_Gosu__Font, 0 |  0 );
  if (!SWIG_IsOK(res1))
    SWIG_exception_fail(SWIG_ArgError(res1), Ruby_Format_TypeError( "", "Gosu::Font const *","draw_markup_rel", 1, self ));
  arg1 = reinterpret_cast< Gosu::Font * >(argp1);
  std::string *ptr = (std::string *)0;
  res2 = SWIG_AsPtr_std_string(argv[0], &ptr);
  if (!SWIG_IsOK(res2))
    SWIG_exception_fail(SWIG_ArgError(res2), Ruby_Format_TypeError( "", "std::string const &","draw_markup_rel", 2, argv[0] ));
  if (!ptr)
    SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "std::string const &","draw_markup_rel", 2, argv[0]));
  arg2 = ptr;
  ecode3 = SWIG_AsVal_double(argv[1], &val3);
  if (!SWIG_IsOK(ecode3))
    SWIG_exception_fail(SWIG_ArgError(ecode3), Ruby_Format_TypeError( "", "double","draw_markup_rel", 3, argv[1]));
  arg3 = static_cast< double >(val3);
  ecode4 = SWIG_AsVal_double(argv[2], &val4);
  if (!SWIG_IsOK(ecode4))
    SWIG_exception_fail(SWIG_ArgError(ecode4), Ruby_Format_TypeError( "", "double","draw_markup_rel", 4, argv[2]));
  arg4 = static_cast< double >(val4);
  ecode5 = SWIG_AsVal_double(argv[3], &val5);
  if (!SWIG_IsOK(ecode5))
    SWIG_exception_fail(SWIG_ArgError(ecode5), Ruby_Format_TypeError( "", "Gosu::ZPos","draw_markup_rel", 5, argv[3]));
  arg5 = static_cast< Gosu::ZPos >(val5);
  ecode6 = SWIG_AsVal_double(argv[4], &val6);
  if (!SWIG_IsOK(ecode6))
    SWIG_exception_fail(SWIG_ArgError(ecode6), Ruby_Format_TypeError( "", "double","draw_markup_rel", 6, argv[4]));
  arg6 = static_cast< double >(val6);
  ecode7 = SWIG_AsVal_double(argv[5], &val7);
  if (!SWIG_IsOK(ecode7))
    SWIG_exception_fail(SWIG_ArgError(ecode7), Ruby_Format_TypeError( "", "double","draw_markup_rel", 7, argv[5]));
  arg7 = static_cast< double >(val7);
  if (argc > 6) {
    ecode8 = SWIG_AsVal_double(argv[6], &val8);
    if (!SWIG_IsOK(ecode8))
      SWIG_exception_fail(SWIG_ArgError(ecode8), Ruby_Format_TypeError( "", "double","draw_markup_rel", 8, argv[6]));
    arg8 = static_cast< double >(val8);
  }
  if (argc > 7) {
    ecode9 = SWIG_AsVal_double(argv[7], &val9);
    if (!SWIG_IsOK(ecode9))
      SWIG_exception_fail(SWIG_ArgError(ecode9), Ruby_Format_TypeError( "", "double","draw_markup_rel", 9, argv[7]));
    arg9 = static_cast< double >(val9);
  }
  if (argc > 8) {
    if (TYPE(argv[8]) == T_FIXNUM || TYPE(argv[8]) == T_BIGNUM) {
      arg10 = Gosu::Color(NUM2ULONG(argv[8]));
    } else {
      void* ptr;
      int res = SWIG_ConvertPtr(argv[8], &ptr, SWIGTYPE_p_Gosu__Color, 0);
      if (!SWIG_IsOK(res)) {
        SWIG_exception_fail(SWIG_ValueError, "invalid value");
      } else if (ptr == nullptr) {
        SWIG_exception_fail(SWIG_ValueError, "invalid null reference of type Gosu::Color");
      } else {
        arg10 = *reinterpret_cast<Gosu::Color*>(ptr);
      }
    }
  }
  if (argc > 9) {
    const char* cstr = Gosu::cstr_from_symbol(argv[9]);
    if (!strcmp(cstr, "default")) {
      arg11 = Gosu::AM_DEFAULT;
    }
    else if (!strcmp(cstr, "add") || !strcmp(cstr, "additive")) {
      arg11 = Gosu::AM_ADD;
    }
    else if (!strcmp(cstr, "multiply")) {
      arg11 = Gosu::AM_MULTIPLY;
    }
    else {
      SWIG_exception_fail(SWIG_ValueError, "invalid alpha mode (expected one of :default, :add, "
        ":multiply)");
    }
  }
  try {
    ((Gosu::Font const *)arg1)->draw_markup_rel((std::string const &)*arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11);
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  if (SWIG_IsNewObj(res2)) delete arg2;
  return Qnil;
}

static VALUE font_set_image(VALUE self, VALUE first, VALUE second) {
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  std::string arg2;
  Gosu::Image *arg3 = 0;
  void *argp1 = 0, *argp3;
  int res1 = 0, res3 = 0;
  res1 = SWIG_ConvertPtr(self, &argp1,SWIGTYPE_p_Gosu__Font, 0 |  0 );
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), Ruby_Format_TypeError( "", "Gosu::Font *","set_image", 1, self )); 
  }
  arg1 = reinterpret_cast< Gosu::Font * >(argp1);
  std::string *ptr = (std::string *)0;
  int res = SWIG_AsPtr_std_string(argv[0], &ptr);
  if (!SWIG_IsOK(res) || !ptr)
    SWIG_exception_fail(SWIG_ArgError((ptr ? res : SWIG_TypeError)), Ruby_Format_TypeError( "", "std::string","set_image", 2, argv[0]));
    arg2 = *ptr;
    if (SWIG_IsNewObj(res)) delete ptr;
  }
  res3 = SWIG_ConvertPtr(argv[1], &argp3, SWIGTYPE_p_Gosu__Image,  0 );
  if (!SWIG_IsOK(res3))
    SWIG_exception_fail(SWIG_ArgError(res3), Ruby_Format_TypeError( "", "Gosu::Image const &","set_image", 3, argv[1]));
  if (!argp3)
    SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "Gosu::Image const &","set_image", 3, argv[1]));
  arg3 = reinterpret_cast< Gosu::Image * >(argp3);
  try {
    (arg1)->set_image(arg2,(Gosu::Image const &)*arg3);
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  return Qnil;
}

static VALUE font_s0(VALUE self, VALUE one, VALUE two, VALUE three) {
  Gosu::Window *arg1 = 0;
  std::string *arg2 = 0;
  int arg3;
  void *argp1 = 0;
  int res1 = 0;
  int res2 = SWIG_OLDOBJ;
  int val3;
  int ecode3 = 0;
  const char *classname SWIGUNUSED = "Gosu::Font";
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  res1 = SWIG_ConvertPtr(argv[0], &argp1, SWIGTYPE_p_Gosu__Window, 0);
  if (!SWIG_IsOK(res1))
    SWIG_exception_fail(SWIG_ArgError(res1), Ruby_Format_TypeError( "", "Gosu::Window &","Font", 1, argv[0] ));
  if (!argp1)
    SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "Gosu::Window &","Font", 1, argv[0]));
  arg1 = reinterpret_cast< Gosu::Window * >(argp1);
  {
    std::string *ptr = (std::string *)0;
    res2 = SWIG_AsPtr_std_string(argv[1], &ptr);
    if (!SWIG_IsOK(res2)) {
      SWIG_exception_fail(SWIG_ArgError(res2), Ruby_Format_TypeError( "", "std::string const &","Font", 2, argv[1] ));
    }
    if (!ptr) {
      SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "std::string const &","Font", 2, argv[1]));
    }
    arg2 = ptr;
  }
  ecode3 = SWIG_AsVal_int(argv[2], &val3);
  if (!SWIG_IsOK(ecode3)) {
    SWIG_exception_fail(SWIG_ArgError(ecode3), Ruby_Format_TypeError( "", "int","Font", 3, argv[2] ));
  }
  arg3 = static_cast< int >(val3);
  {
    try {
      result = (Gosu::Font *)new_Gosu_Font__SWIG_0(*arg1,(std::string const &)*arg2,arg3);
      DATA_PTR(self) = result;
      SWIG_RubyAddTracking(result, self);
    }
    catch (const std::exception& e) {
      SWIG_exception(SWIG_RuntimeError, e.what());
    }
  }
  if (SWIG_IsNewObj(res2)) delete arg2;
  return self;
}

static VALUE font__s1(int argc, VALUE *argv, VALUE self) {
  int arg1;
  VALUE arg2 = (VALUE) 0;
  int val1;
  int ecode1 = 0;
  const char *classname SWIGUNUSED = "Gosu::Font";
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  if ((argc < 1) || (argc > 2))
    rb_raise(rb_eArgError, "wrong # of arguments(%d for 1)",argc);
  ecode1 = SWIG_AsVal_int(argv[0], &val1);
  if (!SWIG_IsOK(ecode1)) {
    SWIG_exception_fail(SWIG_ArgError(ecode1), Ruby_Format_TypeError( "", "int","Font", 1, argv[0] ));
  } 
  arg1 = static_cast< int >(val1);
  if (argc > 1) {
    arg2 = argv[1];
  }
  try {
    result = (Gosu::Font *)new_Gosu_Font__SWIG_1(arg1,arg2);
    DATA_PTR(self) = result;
    SWIG_RubyAddTracking(result, self);
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  return self;
}

static VALUE font_new(int nargs, VALUE *args, VALUE self) {
  int argc;
  VALUE argv[3];
  int ii;
  argc = nargs;
  if (argc > 3) SWIG_fail;
  for (ii = 0; (ii < argc); ++ii) {
    argv[ii] = args[ii];
  }
  if ((argc >= 1) && (argc <= 2)) {
    int _v;
    {
      int res = SWIG_AsVal_int(argv[0], NULL);
      _v = SWIG_CheckState(res);
    }
    if (_v) {
      if (argc <= 1) {
        return _wrap_new_Font__SWIG_1(nargs, args, self);
      }
      _v = (argv[1] != 0);
      if (_v) {
        return _wrap_new_Font__SWIG_1(nargs, args, self);
      }
    }
  }
  if (argc == 3) {
    int _v;
    void *vptr = 0;
    int res = SWIG_ConvertPtr(argv[0], &vptr, SWIGTYPE_p_Gosu__Window, 0);
    _v = SWIG_CheckState(res);
    if (_v) {
      int res = SWIG_AsPtr_std_string(argv[1], (std::string**)(0));
      _v = SWIG_CheckState(res);
      if (_v) {
        {
          int res = SWIG_AsVal_int(argv[2], NULL);
          _v = SWIG_CheckState(res);
        }
        if (_v) {
          return _wrap_new_Font__SWIG_0(nargs, args, self);
        }
      }
    }
  }
fail:
  Ruby_Format_OverloadedError( argc, 3, "Font.new", 
    "    Font.new(Gosu::Window &window, std::string const &font_name, int height)\n"
    "    Font.new(int height, VALUE options)\n");
}


static VALUE font_markup_width(VALUE self, VALUE val)
{
  Gosu::Font font = static_cast<Gosu::Font*>(RTYPEDDATA_DATA(self));
  if (!font) return Qnil;
  std::string *arg2 = 0;
  void *argp1 = 0;
  int res1 = 0;
  int res2 = SWIG_OLDOBJ;
  double val3;
  int ecode3 = 0;
  double result;
  VALUE vresult = Qnil;
  res1 = SWIG_ConvertPtr(self, &argp1, SWIGTYPE_p_Gosu__Font, 0 | 0);
  if (!SWIG_IsOK(res1))
    SWIG_exception_fail(SWIG_ArgError(res1), Ruby_Format_TypeError("", "Gosu::Font", "markup_width", 1, self));
  arg1 = reinterpret_cast< Gosu::Font * >(argp1);
  VALUE str;
  if ( rb_obj_is_instance_of(val, rb_cFloat) || rb_obj_is_instance_of(val, rb_cInteger) ) {
    VALUE any2str = val;
    str = rb_any_to_s(val);
  } else {
    str = val;
  }
  std::string *ptr = (std::string *)0;
  res2 = SWIG_AsPtr_std_string(str, &ptr);
  if (!SWIG_IsOK(res2))
    SWIG_exception_fail(SWIG_ArgError(res2), Ruby_Format_TypeError( "", "std::string const &","markup_width", 2, str));
  if (!ptr)
    SWIG_exception_fail(SWIG_ValueError, Ruby_Format_TypeError("invalid null reference ", "std::string const &","markup_width", 2, str));
  arg2 = ptr;
  // if (argc > 1) { ecode3 = SWIG_AsVal_double(argv[1], &val3);
  //  if (!SWIG_IsOK(ecode3)) {
  //    SWIG_exception_fail(SWIG_ArgError(ecode3), Ruby_Format_TypeError("", "Float", "markup_width", 3, argv[1])); } 
  //  arg3 = static_cast< double >(val3);}
  try {
    result = (double)Gosu_Font_markup_width(arg1, (std::string const &)*arg2);//,arg3);
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  return DBL2NUM(result);
}*/

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

template<rb_data_type_t *FontType>
static VALUE font_allocate(VALUE klass)
{
  return rb_data_typed_object_wrap(klass, 0, FontType);
}

void ruby_ext_init()
{
  VALUE font = rb_define_class("TempFont", rb_cObject);
  rb_define_alloc_func(font, font_allocate<&FontType>);
  rb_define_method(font, "initialize", RMF(font_init), -1);
  rb_define_method(font, "name", RMF(font_name), 0);
  rb_define_method(font, "height", RMF(font_height), 0);
  rb_define_method(font, "flags", RMF(font_flags), 0);
  rb_define_method(font, "markup_width", RMF(font_markup_width), 1);
  rb_define_method(font, "text_width", RMF(font_markup_width), 1);
  /*rb_define_method(font, "draw_markup", RMF(font_draw_markup), -1);
  rb_define_method(font, "draw_markup_rel", RMF(font_draw_markup_rel), -1);
  rb_define_method(font, "set_image", RMF(font_set_image), -1);
  rb_define_method(font, "draw_text", RMF(font_draw_markup), -1);
  rb_define_method(font, "draw_text_rel", RMF(font_draw_markup_rel), -1);*/
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