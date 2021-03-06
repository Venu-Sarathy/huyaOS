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

#include "lib/math/Random.h"
#include "device/graphic/text/TextDriver.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/GraphicsService.h"
#include "Loop.h"

#include "kernel/core/System.h"

Spinlock Loop::printLock;

Loop::Loop(uint32_t id, uint8_t priority) : KernelThread(String::format("Loop%u", id), priority) {
    myID = id;
}

void Loop::run () {
    auto *stream = (Kernel::System::getService<Kernel::GraphicsService>())->getTextDriver();
    auto *timeService = Kernel::System::getService<Kernel::TimeService>();

    for (uint32_t i = 0; isRunning; i++) {
        printLock.acquire();

        String string = String::format("Loop[%u]: %u", myID, i);

        stream->setpos (21, static_cast<uint16_t>(10 + 2 * myID));
        stream->puts((char *) string, string.length(), Colors::HHU_GRAY, Colors::BLACK);

        printLock.release();

        timeService->msleep(10);
    }
}