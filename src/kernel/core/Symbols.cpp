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

#include "kernel/memory/MemLayout.h"
#include "Management.h"
#include "lib/libc/printf.h"
#include "Symbols.h"

namespace Kernel {

Util::HashMap<String, Util::Address> Symbols::symbolTable(1129);

Util::HashMap<Util::Address, char *> Symbols::debugTable(1129);

Multiboot::ElfInfo Symbols::symbolInfo;

bool Symbols::initialized = false;

uint32_t pow(uint32_t base, uint32_t exponent) {

    if (exponent == 0) {
        return 1;
    }

    if (base == 0) {
        return 0;
    }

    uint32_t ret = base;

    for (uint32_t i = 1; i < exponent; i++) {
        ret *= base;
    }

    return ret;
}

uint32_t hextoint(const char *hexString) {

    uint32_t len = strlen(hexString);

    uint32_t ret = 0;

    char current;

    uint32_t num = 0;

    for (uint32_t i = len; i > 0; i--) {

        current = hexString[i - 1];

        if (current > 47 && current < 58) {
            num = current - '0';
        } else if (current > 64 && current < 71) {
            num = current - '7';
        } else if (current > 96 && current < 103) {
            num = current - 'W';
        } else {
            break;
        }

        ret += num * pow(16, len - i);
    }

    return ret;
}

uint32_t Symbols::get(const String &name) {

    if (symbolTable.containsKey((char *) name)) {

        return (uint32_t) symbolTable.get((char *) name);
    }

    return 0x0;
}

const char *Symbols::get(uint32_t eip) {

    if (!initialized) {

        return NO_INFORMATION;
    }

    while (!debugTable.containsKey(Util::Address(eip))) {

        eip--;
    }

    return debugTable.get(Util::Address(eip));
}

void Symbols::load(const ElfConstants::SectionHeader &sectionHeader) {

    uint32_t numEntries = sectionHeader.size / sectionHeader.entrySize;

    ElfConstants::SymbolEntry *entry = (ElfConstants::SymbolEntry *) PHYS2VIRT(sectionHeader.virtualAddress);

    ElfConstants::SectionHeader *stringSection = (ElfConstants::SectionHeader *) (symbolInfo.address +
                                                                                  sectionHeader.link *
                                                                                  symbolInfo.sectionSize);

    char *stringTable = (char *) PHYS2VIRT(stringSection->virtualAddress);

    for (uint32_t i = 0; i < numEntries; i++, entry++) {

        if (entry->value < KERNEL_START || entry->getSymbolType() == ElfConstants::SymbolType::SECTION) {

            continue;
        }

        char *symbol = &stringTable[entry->nameOffset];

        uint32_t address = entry->value;

        symbolTable.put(symbol, Util::Address(address));

        debugTable.put(Util::Address(address), symbol);
    }
}

void Symbols::initialize(const Multiboot::ElfInfo &elfInfo) {

    symbolInfo = elfInfo;

    ElfConstants::SectionHeader *sectionHeader = nullptr;

    for (uint32_t i = 0; i < symbolInfo.sectionCount; i++) {

        sectionHeader = (ElfConstants::SectionHeader *) (symbolInfo.address + i * symbolInfo.sectionSize);

        if (sectionHeader->type == ElfConstants::SectionHeaderType::SYMTAB) {

            load(*sectionHeader);
        }
    }

    initialized = true;
}

void Symbols::copy(const Multiboot::ElfInfo &elfInfo, uint8_t* &destination) {
    ElfConstants::SectionHeader *sectionHeader = nullptr;
    for (uint32_t i = 0; i < elfInfo.sectionCount; i++) {
        sectionHeader = (ElfConstants::SectionHeader *) (elfInfo.address + i * elfInfo.sectionSize);
        // only copy the sections that are not part of the loaded program
        if ((sectionHeader->virtualAddress & KERNEL_START) == 0) {
            // only copy symbols and strings, discard the rest
            if (sectionHeader->type == ElfConstants::SectionHeaderType::SYMTAB
            || sectionHeader->type == ElfConstants::SectionHeaderType::STRTAB) {
                memcpy(destination, (void*)sectionHeader->virtualAddress, sectionHeader->size);
                sectionHeader->virtualAddress = (elf32_addr) destination;
                destination += sectionHeader->size;
            } else {
                sectionHeader->virtualAddress = 0;
            }
        }
    }
}

bool Symbols::isInitialized() {

    return initialized;
}

}
