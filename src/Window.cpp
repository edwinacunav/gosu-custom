#include "Platform.hpp"
#if !defined(GOSU_IS_IPHONE)

#include "Gosu.hpp"
#include "GraphicsImpl.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <time.h>
#include <sys/time.h>
#include "debugwriter.h"
using namespace std;

namespace Gosu
{
  namespace FPS
  {
    void register_frame();
  }

  static void throw_sdl_error(const string& operation)
  {
    const char* error = SDL_GetError();
    throw runtime_error(operation + ": " + (error ? error : "(unknown error)"));
  }

  static void cleanup();

  SDL_Window* shared_window()
  {
    static SDL_Window* window = nullptr;
    if (window == nullptr) {
      if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw_sdl_error("Could not initialize SDL Video");
      atexit(cleanup);
      Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;
#if SDL_VERSION_ATLEAST(2, 0, 1)
      flags |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif
      window =
        SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64, 64, flags);
      if (window == nullptr)
        throw_sdl_error("Could not create window");
      SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    }
    return window;
  }

  static SDL_GLContext shared_gl_context()
  {
    static SDL_GLContext context = nullptr;
    if (context == nullptr) {
#ifdef GOSU_IS_OPENGLES
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
      context = SDL_GL_CreateContext(shared_window());
      if (context == nullptr)
        throw_sdl_error("Could not create OpenGL context");
    }
    return context;
  }

  void ensure_current_context()
  {
    SDL_GL_MakeCurrent(shared_window(), shared_gl_context());
  }

  static void cleanup()
  {
    SDL_GL_DeleteContext(shared_gl_context());
    SDL_DestroyWindow(shared_window());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}

struct Gosu::Window::Impl
{
  bool fullscreen;
  bool block_fullscreen;
  bool show_cursor;
  double update_interval;
  // A single `bool open` is not good enough to support the tick() method: When close() is called
  // from outside the window's call graph, the next call to tick() must return false (transition
  // from CLOSING to CLOSED), but the call after that must return show the window again
  // (transition from CLOSED to OPEN).
  enum { CLOSED, OPEN, CLOSING } state = CLOSED;
  unique_ptr<Graphics> graphics;
  unique_ptr<Input> input;
};

Gosu::Window::Window(unsigned width, unsigned height, bool fullscreen, double update_interval)
: pimpl(new Impl)
{
  pimpl->block_fullscreen = false;
  pimpl->show_cursor = false;
  // Even in fullscreen mode, temporarily show the window in windowed mode to centre it.
  // This ensures that the window will be centred correctly when exiting fullscreen mode.
  // Fixes https://github.com/gosu/gosu/issues/369
  // (This will implicitly create graphics() and input(), and make the OpenGL context current.)
  resize(width, height, false);
  SDL_SetWindowPosition(shared_window(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  // Really enable fullscreen if desired.
  resize(width, height, fullscreen);
  SDL_GL_SetSwapInterval(1);
  pimpl->update_interval = update_interval;
  input().on_button_down = [this](Button button) { button_down(button); };
  input().on_button_up   = [this](Button button) { button_up(button); };
}

Gosu::Window::~Window()
{
  SDL_HideWindow(shared_window());
}

unsigned Gosu::Window::width() const
{
  return graphics().width();
}

unsigned Gosu::Window::height() const
{
  return graphics().height();
}

bool Gosu::Window::fullscreen() const
{
  return pimpl->fullscreen;
}

bool Gosu::Window::block_fullscreen() const
{
  return pimpl->block_fullscreen;
}

bool Gosu::Window::show_cursor() const
{
  return pimpl->show_cursor;
}

bool Gosu::Window::set_block_fullscreen(bool boolean)
{
  pimpl->block_fullscreen = boolean;
  return boolean;
}

bool Gosu::Window::set_show_cursor(bool boolean)
{
  pimpl->show_cursor = boolean;
  return boolean;
}

void Gosu::Window::resize(unsigned width, unsigned height, bool fullscreen)
{
  pimpl->fullscreen = fullscreen;
  int actual_width = width;
  int actual_height = height;
  double scale_factor = 1.0;
  double black_bar_width = 0;
  double black_bar_height = 0;
  if (fullscreen && !pimpl->block_fullscreen) {
    actual_width  = Gosu::screen_width(this);
    actual_height = Gosu::screen_height(this);
    double scale_x = 1.0 * actual_width / width;
    double scale_y = 1.0 * actual_height / height;
    scale_factor = min(scale_x, scale_y);
    if (scale_x < scale_y)
      black_bar_height = (actual_height / scale_x - height) / 2;
    else if (scale_y < scale_x)
      black_bar_width = (actual_width / scale_y - width) / 2;
  }
  else {
    double max_width  = Gosu::available_width(this);
    double max_height = Gosu::available_height(this);
    if (width > max_width || height > max_height) {
      scale_factor = min(max_width / width, max_height / height);
      actual_width  = width  * scale_factor;
      actual_height = height * scale_factor;
    }
  }
  SDL_SetWindowFullscreen(shared_window(), fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  SDL_SetWindowSize(shared_window(), actual_width, actual_height);
#if SDL_VERSION_ATLEAST(2, 0, 1)
  SDL_GL_GetDrawableSize(shared_window(), &actual_width, &actual_height);
#endif
  ensure_current_context();
  if (!pimpl->graphics)
    pimpl->graphics.reset(new Graphics(actual_width, actual_height));
  else
    pimpl->graphics->set_physical_resolution(actual_width, actual_height);
  pimpl->graphics->set_resolution(width, height, black_bar_width, black_bar_height);
  if (!pimpl->input) pimpl->input.reset(new Input(shared_window()));
  pimpl->input->set_mouse_factors(1 / scale_factor, 1 / scale_factor,
                                  black_bar_width, black_bar_height);
}

double Gosu::Window::update_interval() const
{
  return pimpl->update_interval;
}

void Gosu::Window::set_update_interval(double update_interval)
{
  pimpl->update_interval = update_interval;
}

string Gosu::Window::caption() const
{
  const char* title = SDL_GetWindowTitle(shared_window());
  return title ? title : "";
}

void Gosu::Window::set_caption(const string& caption)
{
  SDL_SetWindowTitle(shared_window(), caption.c_str());
}

void Gosu::Window::show()
{
  unsigned long time_before_tick = milliseconds();
  while (tick()) {
    // Sleep to keep this loop from eating 100% CPU.
    unsigned long tick_time = milliseconds() - time_before_tick;
    if (tick_time < update_interval()) {
      sleep(update_interval() - tick_time);
    }
    time_before_tick = milliseconds();
  }
  pimpl->state = Impl::CLOSED;
}

bool Gosu::Window::tick()
{
  if (pimpl->state == Impl::CLOSING) {
    pimpl->state = Impl::CLOSED;
    return false;
  }
  if (pimpl->state == Impl::CLOSED) {
    SDL_ShowWindow(shared_window());
    pimpl->state = Impl::OPEN;
    // SDL_GL_GetDrawableSize returns different values before and after showing the window.
    // -> When first showing the window, update the physical size of Graphics (=glViewport).
    // Fixes https://github.com/gosu/gosu/issues/318
    int width, height;
    SDL_GL_GetDrawableSize(shared_window(), &width, &height);
    graphics().set_physical_resolution(width, height);
  }
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    #ifdef GOSU_IS_MAC
    // Workaround for https://github.com/gosu/gosu/issues/458
    // "Resize" the window to its current dimensions after it is shown.
    // Otherwise it will be black on macOS 10.14 (Mojave) until the user moves it around.
    // TODO: Since this affects `brew install supertux` as well, maybe file an SDL bug?
    case SDL_WINDOWEVENT: {
      if (e.window.event == SDL_WINDOWEVENT_SHOWN)
        resize(this->width(), this->height(), fullscreen());
      break;
    }
    #endif
    case SDL_QUIT: {
      close();
      break;
    }
    case SDL_DROPFILE: {
      char* dropped_filedir = e.drop.file;
      if (dropped_filedir == nullptr) break;
      drop(string(dropped_filedir));
      SDL_free(dropped_filedir);
      break;
    }
    default: {
      input().feed_sdl_event(&e);
      break;
    }
    }
  }
  Song::update();
  input().update();
  update();
  SDL_ShowCursor(pimpl->show_cursor);//(needs_cursor());
  if (needs_redraw()) {
    ensure_current_context();
    graphics().frame([&] {
        draw();
        FPS::register_frame();
    });
    SDL_GL_SwapWindow(shared_window());
  }
  if (pimpl->state == Impl::CLOSING)
    pimpl->state = Impl::CLOSED;
  return pimpl->state == Impl::OPEN;
}

void Gosu::Window::save_screenshot(const std::string &format)
{
  time_t rt = time(NULL);
  char filename[120];
  size_t surface_len = width() * height() * sizeof(uint8_t);//surface->format->BytesPerPixel
  unsigned char *pixels = new unsigned char[surface_len];
  tm *tmp = localtime(&rt);
  sprintf(filename, "Screenshots/shot_%d-%02d-%02d_%02d%02d%02d.%s", tmp->tm_year+1900,
      tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec, format.c_str());
  SDL_Surface* surface = SDL_GetWindowSurface(shared_window());
  SDL_LockSurface(surface);
  if (!surface) {
    printf("Error: No valid surface could be found.");
    SDL_FreeSurface(surface);
    return;
  }
  //Gosu::save_image_file(filename, width(), height(), pixels);
  //SDL_FreeSurface(surface);
  //if (format == "bmp") {
    //if ( IMG_SaveBMP(surface, filename) != 0 )
    //  Debug() << "Freeing BMP surface after failure";
  if (format == "jpg") {
    if ( IMG_SaveJPG(surface, filename, 95) != 0 )
      Debug() << "Freeing JPG surface after failure";
  } else {
    if ( IMG_SavePNG(surface, filename) != 0 )
      Debug() << "Freeing PNG surface after failure";
  }//
  delete[] pixels;
  Debug() << "Sent data to Bitmap";
  //Bitmap bmp;
  //bmp.resize(width(), height());
  //Gosu::Color *bytes = bmp.data();
  //memcpy(bytes, pixels, surface_len);
  //Gosu::save_image_file(bmp, filename);
}

void Gosu::Window::close()
{
  pimpl->state = Impl::CLOSING;
  SDL_HideWindow(shared_window());
}

void Gosu::Window::button_down(Button button)
{
  bool toggle_fullscreen;
  // Default shortcuts for toggling fullscreen mode, see: https://github.com/gosu/gosu/issues/361
#ifdef GOSU_IS_MAC
  // cmd+F and cmd+ctrl+F are both common shortcuts for toggling fullscreen mode on macOS.
  toggle_fullscreen = button == KB_F &&
      (Input::down(KB_LEFT_META) || Input::down(KB_RIGHT_META)) &&
      !Input::down(KB_LEFT_SHIFT) && !Input::down(KB_RIGHT_SHIFT) &&
      !Input::down(KB_LEFT_ALT) && !Input::down(KB_RIGHT_ALT);
#else
  // Alt+Enter and Alt+Return toggle fullscreen mode on all other platforms.
  toggle_fullscreen = (button == KB_RETURN || button == KB_ENTER) &&
      (Input::down(KB_LEFT_ALT)     || Input::down(KB_RIGHT_ALT)) &&
      !Input::down(KB_LEFT_CONTROL) && !Input::down(KB_RIGHT_CONTROL) &&
      !Input::down(KB_LEFT_META)    && !Input::down(KB_RIGHT_META) &&
      !Input::down(KB_LEFT_SHIFT)   && !Input::down(KB_RIGHT_SHIFT);
#endif
  // F11 is supported as a shortcut for fullscreen mode on all platforms.
  if (!toggle_fullscreen && button == KB_F11 &&
      !Input::down(KB_LEFT_ALT)     && !Input::down(KB_RIGHT_ALT) &&
      !Input::down(KB_LEFT_CONTROL) && !Input::down(KB_RIGHT_CONTROL) &&
      !Input::down(KB_LEFT_META)    && !Input::down(KB_RIGHT_META) &&
      !Input::down(KB_LEFT_SHIFT)   && !Input::down(KB_RIGHT_SHIFT)) {
      toggle_fullscreen = true;
  }
  if (toggle_fullscreen) resize(width(), height(), !fullscreen());
}

const Gosu::Graphics& Gosu::Window::graphics() const
{
  return *pimpl->graphics;
}

Gosu::Graphics& Gosu::Window::graphics()
{
  return *pimpl->graphics;
}

const Gosu::Input& Gosu::Window::input() const
{
  return *pimpl->input;
}

Gosu::Input& Gosu::Window::input()
{
  return *pimpl->input;
}

#endif
