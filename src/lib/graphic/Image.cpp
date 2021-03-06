/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "kernel/service/GraphicsService.h"
#include "kernel/core/System.h"
#include "Image.h"

extern "C" {
#include "lib/libc/math.h"
}

void Image::draw(uint16_t x, uint16_t y) const {
    auto *lfb = Kernel::System::getService<Kernel::GraphicsService>()->getLinearFrameBuffer();

    for(uint16_t i = 0; i < height; i++) {
        for(uint16_t j = 0; j < width; j++) {
            lfb->drawPixel(x + j, y + i, pixelBuf[i * width + j]);
        }
    }
}

uint32_t Image::getHeight() const {
    return height;
}

uint32_t Image::getWidth() const {
    return width;
}

uint8_t Image::getColorDepth() const {
    return depth;
}

void Image::scaleUp(uint8_t factor) {
    if(factor <= 1) {
        return;
    }

    uint32_t newHeight = height * factor;
    uint32_t newWidth = width * factor;

    auto *scaledPixelBuf = new Color[newHeight * newWidth];

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            const Color &currentPixel = pixelBuf[i * width + j];

            for(uint8_t k = 0; k < factor; k++) {
                for(uint8_t l = 0; l < factor; l++) {
                    scaledPixelBuf[(i * factor + k) * newWidth + j * factor + l] = Color(currentPixel);
                }
            }
        }
    }

    delete pixelBuf;

    height = newHeight;
    width = newWidth;
    pixelBuf = scaledPixelBuf;
}

void Image::scaleDown(uint8_t factor) {
    if(factor <= 1) {
        return;
    }

    uint32_t newHeight = height / factor;
    uint32_t newWidth = width / factor;

    auto *scaledPixelBuf = new Color[newHeight * newWidth];

    for(uint32_t i = 0; i < newHeight; i++) {
        for(uint32_t j = 0; j < newWidth; j++) {
            float red = 0;
            float blue = 0;
            float green = 0;
            float alpha = 0;

            for(uint8_t k = 0; k < factor; k++) {
                for(uint8_t l = 0; l < factor; l++) {
                    const Color &currentPixel = pixelBuf[(i * factor + k) * width + j * factor + l];

                    red += currentPixel.getRed();
                    blue += currentPixel.getBlue();
                    green += currentPixel.getGreen();
                    alpha += currentPixel.getAlpha();
                }
            }

            scaledPixelBuf[i * newWidth + j] = Color(static_cast<uint8_t>(red / pow(factor, 2)),
                                                  static_cast<uint8_t>(green / pow(factor, 2)),
                                                  static_cast<uint8_t>(blue / pow(factor, 2)),
                                                  static_cast<uint8_t>(alpha / pow(factor, 2)));
        }
    }

    delete pixelBuf;

    height = newHeight;
    width = newWidth;
    pixelBuf = scaledPixelBuf;
}
