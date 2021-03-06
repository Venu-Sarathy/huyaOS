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

#ifndef __StdOutAppender_include__
#define __StdOutAppender_include__


#include "lib/string/String.h"
#include "Appender.h"

namespace Kernel {

class StdOutAppender : public Appender {

public:

    explicit StdOutAppender();

    StdOutAppender(const StdOutAppender &other) = delete;

    StdOutAppender &operator=(const StdOutAppender &other) = delete;

    ~StdOutAppender() override = default;

    void append(const String &message) override;
};

}

#endif
