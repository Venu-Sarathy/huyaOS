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

#ifndef HHUOS_PARALLEL_COMMON_H
#define HHUOS_PARALLEL_COMMON_H

namespace Parallel {

/**
 * Starting addresses of the registers of each port.
 */
enum LptPort {
    LPT1 = 0x378,
    LPT2 = 0x278,
    LPT3 = 0x3bc
};

/**
 * Operating modes of an LPT-port.
 */
enum ParallelMode {
    SPP = 0,
    EPP = 1
};

}

#endif
