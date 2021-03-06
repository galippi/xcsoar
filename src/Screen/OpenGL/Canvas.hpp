/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_OPENGL_CANVAS_HPP
#define XCSOAR_SCREEN_OPENGL_CANVAS_HPP

#include "Util/NonCopyable.hpp"
#include "Math/fixed.hpp"
#include "Math/Angle.hpp"
#include "Screen/Brush.hpp"
#include "Screen/Font.hpp"
#include "Screen/Pen.hpp"
#include "Screen/OpenGL/Color.hpp"
#include "Screen/OpenGL/Point.hpp"
#include "Compiler.h"

#include <assert.h>
#include <tchar.h>

#ifdef ANDROID
#include <GLES/gl.h>
#else
#include <SDL/SDL_opengl.h>
#endif

class GLTexture;

/**
 * Base drawable canvas class
 * 
 */
class Canvas : private NonCopyable {
  friend class SubCanvas;

protected:
  int x_offset, y_offset;
  unsigned width, height;

  Pen pen;
  Brush brush;
  TTF_Font *font;
  Color text_color, background_color;
  enum {
    OPAQUE, TRANSPARENT
  } background_mode;

public:
  Canvas()
    :x_offset(0), y_offset(0), width(0), height(0),
     font(NULL), background_mode(OPAQUE) {}
  Canvas(unsigned _width, unsigned _height)
    :width(_width), height(_height),
     font(NULL), background_mode(OPAQUE) {}

  void set(unsigned _width, unsigned _height) {
    width = _width;
    height = _height;
  }

protected:
  /**
   * Returns true if the outline should be drawn after the area has
   * been filled.  As an optimization, this function returns false if
   * brush and pen share the same color.
   */
  bool pen_over_brush() const {
    return pen.defined() &&
      (brush.is_hollow() || brush.get_color() != pen.get_color());
  }

public:
  bool defined() const {
    return true;
  }

  unsigned get_width() const {
    return width;
  }

  unsigned get_height() const {
    return height;
  }

  void null_pen() {
    pen = Pen(0, Color::BLACK);
  }

  void white_pen() {
    pen = Pen(1, Color::WHITE);
  }

  void black_pen() {
    pen = Pen(1, Color::BLACK);
  }

  void hollow_brush() {
    brush.reset();
  }

  void white_brush() {
    brush = Brush(Color::WHITE);
  }

  void black_brush() {
    brush = Brush(Color::BLACK);
  }

  void select(const Pen &_pen) {
    pen = _pen;
  }

  void select(const Brush &_brush) {
    brush = _brush;
  }

  void select(const Font &_font) {
    font = _font.native();
  }

  void set_text_color(const Color c) {
    text_color = c;
  }

  Color get_text_color() const {
    return text_color;
  }

  void set_background_color(const Color c) {
    background_color = c;
  }

  Color get_background_color() const {
    return background_color;
  }

  void background_opaque() {
    background_mode = OPAQUE;
  }

  void background_transparent() {
    background_mode = TRANSPARENT;
  }

  void outline_rectangle(int left, int top, int right, int bottom,
                         Color color) {
    pen.get_color().set();

    const GLvalue v[] = {
      left, top,
      right, top,
      right, bottom,
      left, bottom,
    };
    glVertexPointer(2, GL_VALUE, 0, v);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
  }

  void rectangle(int left, int top, int right, int bottom) {
    fill_rectangle(left, top, right, bottom, brush);

    if (pen_over_brush())
      outline_rectangle(left, top, right, bottom, pen.get_color());
  }

  void fill_rectangle(int left, int top, int right, int bottom,
                      const Color color);

  void fill_rectangle(int left, int top, int right, int bottom,
                      const Brush &brush) {
    if (brush.is_hollow())
      return;

    fill_rectangle(left, top, right, bottom, brush.get_color());
  }

  void fill_rectangle(const RECT &rc, const Color color) {
    fill_rectangle(rc.left, rc.top, rc.right, rc.bottom, color);
  }

  void fill_rectangle(const RECT rc, const Brush &brush) {
    fill_rectangle(rc.left, rc.top, rc.right, rc.bottom, brush);
  }

  void clear() {
    rectangle(0, 0, get_width(), get_height());
  }

  void clear(const Color color) {
    fill_rectangle(0, 0, get_width(), get_height(), color);
  }

  void clear(const Brush &brush) {
    fill_rectangle(0, 0, get_width(), get_height(), brush);
  }

  void clear_white() {
    clear(Color::WHITE);
  }

  void round_rectangle(int left, int top, int right, int bottom,
                       unsigned ellipse_width, unsigned ellipse_height) {
    rectangle(left, top, right, bottom); // XXX
  }

  void raised_edge(RECT &rc) {
    Pen bright(1, Color(240, 240, 240));
    select(bright);
    two_lines(rc.left, rc.bottom - 2, rc.left, rc.top,
              rc.right - 2, rc.top);

    Pen dark(1, Color(128, 128, 128));
    select(dark);
    two_lines(rc.left + 1, rc.bottom - 1, rc.right - 1, rc.bottom - 1,
              rc.right - 1, rc.top + 1);

    ++rc.left;
    ++rc.top;
    --rc.right;
    --rc.bottom;
  }

  void polyline(const RasterPoint *points, unsigned num_points);
  void polygon(const RasterPoint *points, unsigned num_points);

  void autoclip_polygon(const RasterPoint *lppt, unsigned cPoints) {
    // XXX clip
    polygon(lppt, cPoints);
  }

  void autoclip_polyline(const RasterPoint *lppt, unsigned cPoints) {
    // XXX clip
    polyline(lppt, cPoints);
  }

  void line(int ax, int ay, int bx, int by) {
    pen.get_color().set();

    const GLvalue v[] = { ax, ay, bx, by };
    glVertexPointer(2, GL_VALUE, 0, v);
    glDrawArrays(GL_LINE_STRIP, 0, 2);
  }

  void line(const RasterPoint a, const RasterPoint b) {
    line(a.x, a.y, b.x, b.y);
  }

  void two_lines(int ax, int ay, int bx, int by, int cx, int cy);
  void two_lines(const RasterPoint a, const RasterPoint b,
                 const RasterPoint c);

  void circle(int x, int y, unsigned radius);

  void segment(int x, int y, unsigned radius,
               Angle start, Angle end, bool horizon=false);

  void draw_focus(RECT rc) {
    // XXX
  }

  void draw_button(RECT rc, bool down);

  gcc_pure
  const SIZE text_size(const TCHAR *text, size_t length) const;

  gcc_pure
  const SIZE text_size(const TCHAR *text) const;

  gcc_pure
  unsigned text_width(const TCHAR *text) const {
    return text_size(text).cx;
  }

  gcc_pure
  unsigned text_height(const TCHAR *text) const {
    return font != NULL ? TTF_FontHeight(font) : 0;
  }

  void text(int x, int y, const TCHAR *text);
  void text(int x, int y, const TCHAR *text, size_t length);

  void text_opaque(int x, int y, const RECT &rc, const TCHAR *text);

  void text_clipped(int x, int y, const RECT &rc, const TCHAR *text) {
    // XXX
    this->text(x, y, text);
  }

  void text_clipped(int x, int y, unsigned width, const TCHAR *text) {
    // XXX
    this->text(x, y, text);
  }

  void formatted_text(RECT *rc, const TCHAR *text, unsigned format);

  /**
   * Draws a texture.  The caller is responsible for binding it and
   * enabling GL_TEXTURE_2D.
   */
  void stretch(int dest_x, int dest_y,
               unsigned dest_width, unsigned dest_height,
               const GLTexture &texture,
               int src_x, int src_y,
               unsigned src_width, unsigned src_height);

  void stretch(int dest_x, int dest_y,
               unsigned dest_width, unsigned dest_height,
               const GLTexture &texture);


  void copy(int dest_x, int dest_y,
            unsigned dest_width, unsigned dest_height,
            const Bitmap &src, int src_x, int src_y);
  void copy(const Bitmap &src);

  void stretch_transparent(const Bitmap &src, Color key);

  void stretch(int dest_x, int dest_y,
               unsigned dest_width, unsigned dest_height,
               const Bitmap &src,
               int src_x, int src_y,
               unsigned src_width, unsigned src_height);
  void stretch(int dest_x, int dest_y,
               unsigned dest_width, unsigned dest_height,
               const Bitmap &src);

  void stretch(const Bitmap &src) {
    stretch(0, 0, width, height, src);
  }

  void copy_or(int dest_x, int dest_y,
               unsigned dest_width, unsigned dest_height,
               const Bitmap &src, int src_x, int src_y);

  void copy_or(const Bitmap &src) {
    copy_or(0, 0, get_width(), get_height(), src, 0, 0);
  }

  void copy_and(int dest_x, int dest_y,
                unsigned dest_width, unsigned dest_height,
                const Bitmap &src, int src_x, int src_y);

  void copy_and(const Bitmap &src) {
    copy_and(0, 0, get_width(), get_height(), src, 0, 0);
  }

  void scale_copy(int dest_x, int dest_y,
                  const Bitmap &src,
                  int src_x, int src_y,
                  unsigned src_width, unsigned src_height);
};

#endif
