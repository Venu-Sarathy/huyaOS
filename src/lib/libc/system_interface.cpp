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

#include "kernel/core/Management.h"
#include "printf.h"
#include "system_interface.h"

void *sys_alloc_mem(uint32_t size, uint32_t alignment) {
    return new(alignment) char[size];
}

void sys_free_mem(void *ptr) {
    delete (char*) ptr;
}

void *sys_realloc_mem(void *ptr, size_t size, size_t alignment) {
    return Kernel::Management::getInstance().realloc(ptr, size, alignment);
}