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

#ifndef __DiskInfo_include__
#define __DiskInfo_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "kernel/service/TimeService.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Shows information about a storage device.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class DiskInfo : public Command {

private:
    Kernel::StorageService *storageService = nullptr;
    Filesystem *fileSystem = nullptr;

public:
    /**
     * Default-constructor.
     */
    DiskInfo() = delete;

    /**
     * Copy-constructor.
     */
    DiskInfo(const DiskInfo &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit DiskInfo(Shell &shell);

    /**
     * Destructor.
     */
    ~DiskInfo() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif
