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

#include <kernel/memory/Paging.h>
#include "kernel/memory/MemLayout.h"
#include "kernel/core/Symbols.h"
#include "lib/file/tar/Archive.h"
#include "kernel/core/Management.h"
#include "lib/libc/printf.h"
#include "lib/libc/sprintf.h"
#include "kernel/bluescreen/BlueScreen.h"
#include "kernel/bluescreen/BlueScreenLfb.h"
#include "Structure.h"
#include "Constants.h"

extern "C" {
    extern char ___KERNEL_DATA_START__;
    extern char ___KERNEL_DATA_END__;
}

namespace Kernel::Multiboot {

Info Structure::info;

uint32_t Structure::customMemoryMapSize = 0;

uint32_t Structure::physReservedMemoryStart = UINT32_MAX;

uint32_t Structure::physReservedMemoryEnd = 0;

uint32_t Structure::kernelCopyLow = UINT32_MAX;

uint32_t Structure::kernelCopyHigh = 0;

MemoryMapEntry Structure::customMemoryMap[256];

Structure::MemoryBlock Structure::blockMap[256];

Util::ArrayList<MemoryMapEntry> Structure::memoryMap;

FrameBufferInfo Structure::frameBufferInfo;

Util::HashMap<String, ModuleInfo> Structure::modules;

Util::HashMap<String, String> Structure::kernelOptions;

extern "C" {
void readMemoryMap(Info *address);
}

void readMemoryMap(Info *address) {

    Structure::readMemoryMap(address);
}

void Structure::readMemoryMap(Info *address) {

    Info tmp = *address;

    MemoryMapEntry *memory = (MemoryMapEntry *) ((uint32_t) customMemoryMap - KERNEL_START);

    MemoryBlock *blocks = (MemoryBlock *) ((uint32_t) blockMap - KERNEL_START);

    uint32_t &mapSize = *((uint32_t *) ((uint32_t) &customMemoryMapSize - KERNEL_START));

    uint32_t kernelStart = (uint32_t) &___KERNEL_DATA_START__ - KERNEL_START;

    uint32_t kernelEnd = (uint32_t) &___KERNEL_DATA_END__ - KERNEL_START;

    memory[0] = {0x0, kernelStart, kernelEnd - kernelStart, MULTIBOOT_MEMORY_RESERVED };

    mapSize++;

    uint32_t memoryIndex = 1;

    ElfInfo &symbolInfo = tmp.symbols.elf;

    ElfConstants::SectionHeader *sectionHeader = nullptr;

    if (tmp.flags & *VIRT2PHYS(&MULTIBOOT_INFO_ELF_SHDR)) {

        for (uint32_t i = 0; i < symbolInfo.sectionCount; i++) {

            sectionHeader = (ElfConstants::SectionHeader *) (symbolInfo.address + i * symbolInfo.sectionSize);

            if (sectionHeader->virtualAddress == 0x0) {

                continue;
            }

            uint32_t startAddress = sectionHeader->virtualAddress < KERNEL_START ? sectionHeader->virtualAddress :
                                    sectionHeader->virtualAddress - KERNEL_START;

            memory[memoryIndex] = { 0x0, startAddress, sectionHeader->size, MULTIBOOT_MEMORY_RESERVED };

            memoryIndex++;

            mapSize++;
        }
    }

    if (tmp.flags & *VIRT2PHYS(&MULTIBOOT_INFO_MODS)) {

        auto *modInfo = (ModuleInfo *) tmp.moduleAddress;

        for (uint32_t i = 0; i < tmp.moduleCount; i++) {

            memory[memoryIndex] = {0x0, modInfo[i].start, modInfo[i].end - modInfo[i].start, MULTIBOOT_MEMORY_AVAILABLE };

            memoryIndex++;

            mapSize++;
        }
    }

    bool sorted;

    do {
        sorted = true;

        for (uint32_t i = 0; i < memoryIndex - 1; i++) {
            if (memory[i].address > memory[i + 1].address) {
                MemoryMapEntry help = memory[i];
                memory[i] = memory[i + 1];
                memory[i + 1] = help;

                sorted = false;
            }
        }
    } while (!sorted);

    uint32_t blockIndex = 0;
    blocks[blockIndex] = {static_cast<uint32_t>(memory[0].address), static_cast<uint32_t>(memory[0].length), 0};

    for (uint32_t i = 1; i < memoryIndex; i++) {

        if (memory[i].address > blocks[blockIndex].startAddress + blocks[blockIndex].lengthInBytes + PAGESIZE) {
            blocks[++blockIndex] = {static_cast<uint32_t>(memory[i].address), static_cast<uint32_t>(memory[i].length), 0};
        } else if (memory[i].address + memory[i].length > blocks[blockIndex].startAddress + blocks[blockIndex].lengthInBytes) {
            blocks[blockIndex].lengthInBytes = (memory[i].address + memory[i].length) - blocks[blockIndex].startAddress;
        }
    }

    uint32_t alignment = 4 * 1024 * 1024;
    for (uint32_t i = 0; i <= blockIndex; i++) {

        // Align start address down to the beginning of the 4MB page (uses integer division)
        uint64_t tmpAddress = blocks[i].startAddress;
        blocks[i].startAddress = (blocks[i].startAddress / alignment) * alignment;
        blocks[i].lengthInBytes += tmpAddress - blocks[i].startAddress;

        blocks[i].blockCount = blocks[i].lengthInBytes % alignment == 0 ? (blocks[i].lengthInBytes / alignment) :
                                        (blocks[i].lengthInBytes / alignment + 1);
    }

    uint32_t &maxAddress = *((uint32_t *) ((uint32_t) &physReservedMemoryEnd - KERNEL_START));

    uint32_t &minAddress = *((uint32_t *) ((uint32_t) &physReservedMemoryStart - KERNEL_START));

    for (uint32_t i = 0; i < memoryIndex; i++) {

        if (memory[i].address + memory[i].length > maxAddress) {

            maxAddress = memory[i].address + memory[i].length;
        }

        if (memory[i].address < minAddress) {

            minAddress = memory[i].address;
        }
    }
}

void Structure::init(Info *address) {

    info = *address;
}

void Structure::parse() {

    parseCommandLine();

    parseMemoryMap();

    parseSymbols();

    parseModules();

    parseFrameBufferInfo();
}

void Structure::parseCommandLine() {

    if (info.flags & MULTIBOOT_INFO_CMDLINE) {

        info.commandLine += KERNEL_START;

        Util::Array<String> options = String((char *) info.commandLine).split(" ");

        for (const String &option : options) {

            Util::Array<String> pair = option.split("=");

            if (pair.length() != 2) {

                continue;
            }

            kernelOptions.put(pair[0], pair[1]);
        }
    }
}

void Structure::parseMemoryMap() {

    if (info.flags & MULTIBOOT_INFO_MEM_MAP) {

        MemoryMapEntry *entry = (MemoryMapEntry *) (info.memoryMapAddress + KERNEL_START);

        uint32_t size = info.memoryMapLength / sizeof(MemoryMapEntry);

        for (uint32_t i = 0; i < size; i++) {

#if PRINT_MEMORY
            printf("0x%08x - 0x%08x : %u\n", (uint32_t) entry[i].address, (uint32_t) (entry[i].address + entry[i].length), entry[i].type);
#endif

            memoryMap.add(entry[i]);
        }

#if PRINT_MEMORY
        while(true);
#endif
    }
}

Util::Array<MemoryMapEntry> Structure::getMemoryMap() {

    if ((info.flags & MULTIBOOT_INFO_MEM_MAP) == 0x0) {
        return Util::Array<MemoryMapEntry>(0);
    }

    auto *entry = (MemoryMapEntry *) (info.memoryMapAddress + KERNEL_START);

    uint32_t size = info.memoryMapLength / sizeof(MemoryMapEntry);

    Util::Array<MemoryMapEntry> memoryMap(size);

    for (uint32_t i = 0; i < size; i++) {
        memoryMap[i] = entry[i];
    }

    return memoryMap;
}

FrameBufferInfo Structure::getFrameBufferInfo() {
    return frameBufferInfo;
}

void Structure::parseSymbols() {

    if (info.flags & MULTIBOOT_INFO_ELF_SHDR) {

        info.symbols.elf.address += KERNEL_START;

        Symbols::initialize(info.symbols.elf);
    }
}

void Structure::parseModules() {

    if (info.flags & MULTIBOOT_INFO_MODS) {

        info.moduleAddress += KERNEL_START;

        ModuleInfo *modInfo = (ModuleInfo *) info.moduleAddress;

        for (uint32_t i = 0; i < info.moduleCount; i++) {

            modInfo[i].string += KERNEL_START;

            modInfo[i].start += KERNEL_START;

            modInfo[i].end += KERNEL_START;

            modules.put(modInfo->string, *modInfo);
        }
    }
}

ModuleInfo Structure::getModule(const String &module) {

    if (isModuleLoaded(module)) {

        return modules.get(module);
    }

    return {0, 0, "unknown", 0};
}

bool Structure::isModuleLoaded(const String &module) {

    return modules.containsKey(module);
}

String Structure::getKernelOption(const String &key) {

    if (kernelOptions.containsKey(key)) {

        return kernelOptions.get(key);
    }

    return String();
}

void Structure::parseFrameBufferInfo() {

    if ((info.flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) && (info.framebufferBpp >= 8) && (info.framebufferType == 1)) {

        frameBufferInfo.address = Management::getInstance().mapIO(
                static_cast<uint32_t>(info.framebufferAddress), info.framebufferWidth * info.framebufferPitch);
        frameBufferInfo.width = static_cast<uint16_t>(info.framebufferWidth);
        frameBufferInfo.height = static_cast<uint16_t>(info.framebufferHeight);
        frameBufferInfo.bpp = info.framebufferBpp;
        frameBufferInfo.pitch = static_cast<uint16_t>(info.framebufferPitch);
        frameBufferInfo.type = info.framebufferType;

        BlueScreenLfb::fbInfo = frameBufferInfo;
    } else {

        frameBufferInfo.address = 0;
        frameBufferInfo.width = 0;
        frameBufferInfo.height = 0;
        frameBufferInfo.bpp = 0;
        frameBufferInfo.pitch = 0;
        frameBufferInfo.type = 0;
    }
}

}