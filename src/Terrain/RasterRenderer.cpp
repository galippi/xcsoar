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

#include "Terrain/RasterRenderer.hpp"
#include "Terrain/RasterMap.hpp"
#include "Math/Earth.hpp"
#include "Math/FastMath.h"
#include "Screen/Ramp.hpp"
#include "Screen/Layout.hpp"
#include "WindowProjection.hpp"
#include "Asset.hpp"

#include <assert.h>

static inline unsigned
MIX(unsigned x, unsigned y, unsigned i)
{
  return (x * i + y * ((1 << 7) - i)) >> 7;
}

inline void
TerrainShading(const short illum, BYTE &r, BYTE &g, BYTE &b)
{
  char x;
  if (illum < 0) {
    // shadow to blue
    x = min(63, -illum);
    r = MIX(0,r,x);
    g = MIX(0,g,x);
    b = MIX(64,b,x);
  } else if (illum > 0) {
    // highlight to yellow
    x = min(32, illum / 2);
    r = MIX(255,r,x);
    g = MIX(255,g,x);
    b = MIX(16,b,x);
  }
}

// map scale is approximately 2 points on the grid
// therefore, want one to one mapping if mapscale is 0.5
// there are approx 30 pixels in mapscale
// 240/QUANTISATION_PIXELS resolution = 6 pixels per terrain
// (mapscale/30)  km/pixels
//        0.250   km/terrain
// (0.25*30/mapscale) pixels/terrain
//  mapscale/(0.25*30)
//  mapscale/7.5 terrain units/pixel
//
// this is for TerrainInfo.StepSize = 0.0025;
RasterRenderer::RasterRenderer()
  :quantisation_pixels(2),
   image(NULL)
{
  // scale quantisation_pixels so resolution is not too high on large displays
  if (is_embedded())
    quantisation_pixels = Layout::FastScale(quantisation_pixels);
}


RasterRenderer::~RasterRenderer()
{
  delete image;
}

void
RasterRenderer::ScanMap(const RasterMap &map, const WindowProjection &projection)
{
  unsigned x = projection.GetScreenWidth() / 2;
  unsigned y = projection.GetScreenHeight() / 2;
  GeoPoint Gmid = projection.ScreenToGeo(x, y);

  pixel_size = fixed_sqrt_half *
    Distance(Gmid,
             projection.ScreenToGeo(x + quantisation_pixels,
                                      y + quantisation_pixels));

  // set resolution

  fixed map_pixel_size = map.pixel_distance(Gmid, 1);
  quantisation_effective = (int)ceil(map_pixel_size / pixel_size);

  height_matrix.Fill(map, projection, quantisation_pixels,
                     pixel_size * 3 < map_pixel_size * 2);
}

void
RasterRenderer::GenerateImage(bool is_terrain, bool do_shading,
                              unsigned height_scale,
                              int contrast, int brightness,
                              const Angle sunazimuth)
{
  if (image == NULL ||
      height_matrix.get_width() > image->GetWidth() ||
      height_matrix.get_height() > image->GetHeight()) {
    delete image;
    image = new RawBitmap(height_matrix.get_width(),
                          height_matrix.get_height());
  }

  if (quantisation_effective > min(height_matrix.get_width(),
                                   height_matrix.get_height()) / 4)
    do_shading = false;

  if (do_shading)
    GenerateSlopeImage(is_terrain, height_scale, contrast, brightness,
                       sunazimuth);
  else
    GenerateUnshadedImage(is_terrain, height_scale);
}

void
RasterRenderer::GenerateUnshadedImage(bool is_terrain, unsigned height_scale)
{
  const int min_height = is_terrain
    ? min(1000, (int)height_matrix.get_minimum()) : 0;
  const int height_factor = is_terrain
    ? max(2000, (int)height_matrix.get_maximum()) - min_height : 0;

  const short *src = height_matrix.GetData();
  const BGRColor *oColorBuf = color_table + 64 * 256;
  BGRColor *dest = image->GetTopRow();

  for (unsigned y = height_matrix.get_height(); y > 0; --y) {
    BGRColor *p = dest;
    dest = image->GetNextRow(dest);

    for (unsigned x = height_matrix.get_width(); x > 0; --x) {
      short h = *src++;
      if (gcc_likely(!RasterBuffer::is_special(h))) {
        h = height_factor > 0
          ? (h - min_height) * 254 / height_factor
          : min(254, h >> height_scale);
        *p++ = oColorBuf[h];
      } else if (!RasterBuffer::is_water(h)) {
        // we're in the water, so look up the color for water
        *p++ = oColorBuf[255];
      } else {
        /* outside the terrain file bounds: white background */
        *p++ = BGRColor(0xff, 0xff, 0xff);
      }
    }
  }
}

// JMW: if zoomed right in (e.g. one unit is larger than terrain
// grid), then increase the step size to be equal to the terrain
// grid for purposes of calculating slope, to avoid shading problems
// (gridding of display) This is why quantisation_effective is used instead of 1
// previously.  for large zoom levels, quantisation_effective=1
void
RasterRenderer::GenerateSlopeImage(bool is_terrain, unsigned height_scale,
                                   int contrast,
                                   const int sx, const int sy, const int sz)
{
  RECT border;
  border.left = quantisation_effective;
  border.top = quantisation_effective;
  border.right = height_matrix.get_width() - quantisation_effective;
  border.bottom = height_matrix.get_height() - quantisation_effective;

  const unsigned height_slope_factor = max(1, (int)pixel_size);

  const int min_height = is_terrain
    ? min(1000, (int)height_matrix.get_minimum()) : 0;
  const int height_factor = is_terrain
    ? max(2000, (int)height_matrix.get_maximum()) - min_height : 0;

  const short *src = height_matrix.GetData();
  const BGRColor *oColorBuf = color_table + 64 * 256;
  BGRColor *dest = image->GetTopRow();

  for (unsigned y = 0; y < height_matrix.get_height(); ++y) {
    const unsigned row_plus_index = y < (unsigned)border.bottom
      ? quantisation_effective
      : height_matrix.get_height() - 1 - y;
    const unsigned row_plus_offset = height_matrix.get_width() * row_plus_index;

    const unsigned row_minus_index = y >= quantisation_effective
      ? quantisation_effective : y;
    const unsigned row_minus_offset = height_matrix.get_width() * row_minus_index;

    const unsigned p31 = row_plus_index + row_minus_index;

    BGRColor *p = dest;
    dest = image->GetNextRow(dest);

    for (unsigned x = 0; x < height_matrix.get_width(); ++x, ++src) {
      short h = *src;
      if (gcc_likely(!RasterBuffer::is_special(h))) {
        h = height_factor > 0
          ? (h - min_height) * 254 / height_factor
          : min(254, h >> height_scale);

        // no need to calculate slope if undefined height or sea level

        // Y direction
        assert(src - row_minus_offset >= height_matrix.GetData());
        assert(src + row_plus_offset >= height_matrix.GetData());
        assert(src - row_minus_offset < height_matrix.GetDataEnd());
        assert(src + row_plus_offset < height_matrix.GetDataEnd());

        // X direction

        const unsigned column_plus_index = x < (unsigned)border.right
          ? quantisation_effective
          : height_matrix.get_width() - 1 - x;
        const unsigned column_minus_index = x >= (unsigned)border.left
          ? quantisation_effective : x;

        assert(src - column_minus_index >= height_matrix.GetData());
        assert(src + column_plus_index >= height_matrix.GetData());
        assert(src - column_minus_index < height_matrix.GetDataEnd());
        assert(src + column_plus_index < height_matrix.GetDataEnd());

        short h_above = src[-(int)row_minus_offset];
        short h_below = src[row_plus_offset];
        short h_left = src[-(int)column_minus_index];
        short h_right = src[column_plus_index];

        if (gcc_unlikely(RasterBuffer::is_special(h_above) ||
                         RasterBuffer::is_special(h_below) ||
                         RasterBuffer::is_special(h_left) ||
                         RasterBuffer::is_special(h_right))) {
          /* some "special" terrain value surrounding us (water or
             invalid), skip slope calculation */
          *p++ = oColorBuf[h];
          continue;
        }

        const int p32 = h_above - h_below;
        const int p22 = h_right - h_left;

        const unsigned p20 = column_plus_index + column_minus_index;

        const int dd0 = p22 * p31;
        const int dd1 = p20 * p32;
        const int dd2 = p20 * p31 * height_slope_factor;
        const int mag = (dd0 * dd0 + dd1 * dd1 + dd2 * dd2);
        const int num = (dd2 * sz + dd0 * sx + dd1 * sy);
#ifdef FIXED_MATH
        const int sval = num / (int)isqrt4(mag);
#else
        const int sval = num / (int)sqrt((fixed)mag);
#endif
        int sindex = (sval - sz) * contrast / 128;
        if (gcc_unlikely(sindex < -64))
          sindex = -64;
        if (gcc_unlikely(sindex > 63))
          sindex = 63;
        *p++ = oColorBuf[h + 256*sindex];
      } else if (RasterBuffer::is_water(h)) {
        // we're in the water, so look up the color for water
        *p++ = oColorBuf[255];
      } else {
        /* outside the terrain file bounds: white background */
        *p++ = BGRColor(0xff, 0xff, 0xff);
      }
    }
  }
}

void
RasterRenderer::GenerateSlopeImage(bool is_terrain, unsigned height_scale,
                                   int contrast, int brightness,
                                   const Angle sunazimuth)
{
  const Angle fudgeelevation =
    Angle::degrees(fixed(10.0 + 80.0 * brightness / 255.0));

  const int sx = (int)(255 * fudgeelevation.fastcosine() * -sunazimuth.fastsine());
  const int sy = (int)(255 * fudgeelevation.fastcosine() * -sunazimuth.fastcosine());
  const int sz = (int)(255 * fudgeelevation.fastsine());

  GenerateSlopeImage(is_terrain, height_scale, contrast,
                     sx, sy, sz);
}

void
RasterRenderer::ColorTable(const COLORRAMP *color_ramp, bool do_water,
                           unsigned height_scale, int interp_levels)
{
  for (int i = 0; i < 256; i++) {
    for (int mag = -64; mag < 64; mag++) {
      BYTE r, g, b;
      if (i == 255) {
        if (do_water) {
          // water colours
          r = 85;
          g = 160;
          b = 255;
        } else {
          r = 255;
          g = 255;
          b = 255;

          // ColorRampLookup(0, r, g, b,
          // Color_ramp, NUM_COLOR_RAMP_LEVELS, interp_levels);
        }
      } else {
        Color color =  ColorRampLookup(i << height_scale, color_ramp,
                                       NUM_COLOR_RAMP_LEVELS, interp_levels);
        r = color.red();
        g = color.green();
        b = color.blue();

        TerrainShading(mag, r, g, b);
      }

      color_table[i + (mag + 64) * 256] = BGRColor(r, g, b);
    }
  }
}
