/*
 * Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_USERTHREAD_H
#define HHUOS_USERTHREAD_H

#include "Thread.h"

namespace Kernel {

class UserThread : public Thread {

public:

    explicit UserThread(Process &process);

    UserThread(Process &process, const String &name);

    UserThread(Process &process, const String &name, uint8_t priority);

    UserThread(const Thread &copy) = delete;

    ~UserThread() override = default;

    Stack &getUserStack() override;

    Stack &getKernelStack() override;

    void run() override = 0;

private:

    void init();

    Stack kernelStack;

    Stack userStack;
};

}

#endif
