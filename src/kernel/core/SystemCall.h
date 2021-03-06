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

#ifndef __SystemCall_include__
#define __SystemCall_include__

#include <cstdint>
#include <lib/system/Call.h>
#include <kernel/log/Logger.h>
#include "kernel/interrupt/InterruptHandler.h"

namespace Kernel {

/**
 * System call interface.
 *
 * @author Filip Krakowski
 */
class SystemCall : public Kernel::InterruptHandler {

public:

    void trigger(Kernel::InterruptFrame &frame) override;

    static void registerSystemCall(Standard::System::Call::Code code, void(*func)(uint32_t paramCount, va_list params, Standard::System::Result *result));

    static int32_t atExit(void (*func)());

private:

    static Logger &log;

    static void (*systemCalls[256])(uint32_t paramCount, va_list params, Standard::System::Result *result);

};

}


#endif
