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

#include "kernel/core/System.h"
#include "kernel/log/Logger.h"
#include "StorageService.h"
#include "device/storage/device/Partition.h"
#include "EventBus.h"

namespace Kernel {

Logger &StorageService::log = Logger::get("STORAGE");

StorageDevice *StorageService::getDevice(const String &name) {
    if (!devices.containsKey(name)) {
        return nullptr;
    }

    return devices.get(name);
}

void StorageService::registerDevice(StorageDevice *device) {
    auto *eventBus = System::getService<EventBus>();

    devices.put(device->getName(), device);

    log.info("Registering device: %s", (char *) device->getName());

    Util::SmartPointer<Event> event(new StorageAddEvent(device));
    eventBus->publish(event);

    //Scan device for partitions and register them as well
    Util::Array<StorageDevice::PartitionInfo> partitions = device->readPartitionTable();

    for (StorageDevice::PartitionInfo info : partitions) {

        String partitionName(device->getName() + "p" + String::valueOf(info.number, 10));

        StorageDevice *partition = new Partition(device, info.startSector, info.sectorCount, info.systemId,
                                                 partitionName);
        registerDevice(partition);
    }
}

void StorageService::removeDevice(const String &name) {
    auto *eventBus = System::getService<EventBus>();

    Util::Array<String> deviceNames = devices.keySet();
    for (const String &currentName : deviceNames) {
        if (currentName.beginsWith(name)) {
            log.info("Removing device: %s", (char *) currentName);

            devices.remove(currentName);

            Util::SmartPointer<Event> event(new StorageRemoveEvent(currentName));
            eventBus->publish(event);
        }
    }
}

}
