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

#ifndef __Spinlock_include__
#define __Spinlock_include__


#include <cstdint>
#include "lib/async/Atomic.h"
#include "Lock.h"

/**
 * A simple spinlock implemented using test&set instructions.
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */
class Spinlock : public Lock {

public:

    Spinlock() noexcept;

    Spinlock(const Spinlock &other) = delete;

    Spinlock &operator=(const Spinlock &other) = delete;

    ~Spinlock() override = default;

    void acquire() override;

    bool tryAcquire() override;

    void release() override;

    bool isLocked() override;

private:

    static const uint8_t SPINLOCK_UNLOCK = 0x00;

    static const uint8_t SPINLOCK_LOCK = 0x01;

private:

    Atomic<uint8_t> lockVar;
};

#endif

