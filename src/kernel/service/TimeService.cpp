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

#include "kernel/core/SystemCall.h"
#include "TimeService.h"

namespace Kernel {

TimeService::TimeService(TimeProvider &provider) : provider(provider) {
    rtc = new Rtc();
}

uint32_t TimeService::getSystemTime() {
    return provider.getMillis();
}

void TimeService::msleep(uint32_t ms) {
    unsigned long st = getSystemTime();

    while (true) {
        if (getSystemTime() > (st + ms)) {
            break;
        }

        Standard::System::Result result{};
        Standard::System::Call::execute(Standard::System::Call::SCHEDULER_YIELD, result, 0);
    }
}

}