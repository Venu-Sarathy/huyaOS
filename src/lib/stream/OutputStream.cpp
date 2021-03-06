/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
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

#include "OutputStream.h"


OutputStream::OutputStream() {
    base = 10;
}

void OutputStream::writeBytes(char *data, uint64_t len) {
    for(uint64_t i = 0; i < len; i++) {
        put(data[i]);
    }
}

OutputStream& OutputStream::operator << (char c) {
    put(c);
    return *this;
}

OutputStream& OutputStream::operator << (unsigned char c) {
    return *this << (char) c;
}

OutputStream& OutputStream::operator << (char* string) {
    char* pos = string;
    while (*pos) {
        put (*pos);
        pos++;
    }
    return *this;
}

OutputStream& OutputStream::operator << (const char* string) {
    return *this << (char *) string;
}

OutputStream& OutputStream::operator << (short ival) {
    return *this << (long) ival;
}

OutputStream& OutputStream::operator << (unsigned short ival) {
    return *this << (unsigned long) ival;
}

OutputStream& OutputStream::operator << (int ival) {
    return *this << (long) ival;
}

OutputStream& OutputStream::operator << (unsigned int ival) {
    return *this << (unsigned long) ival;
}

OutputStream& OutputStream::operator << (long ival) {
    if (ival < 0) {
        put('-');
        ival = -ival;
    }

    return *this << (unsigned long) ival;
}

OutputStream& OutputStream::operator << (unsigned long ival) {
    unsigned long div;
    char digit;

    if (base == 8)
        put ('0');
    else if (base == 16) {
        put ('0');
        put ('x');
    }

    for (div = 1; ival/div >= (unsigned long) base; div *= base);

    for (; div > 0; div /= (unsigned long) base) {
        digit = static_cast<char>(ival / div);
        if (digit < 10)
                put ('0' + digit);
        else
                put (static_cast<char>('A' + digit - 10));
        ival %= div;
    }
    return *this;
}

OutputStream& OutputStream::operator << (void* ptr) {
    int oldbase = base;
    base = 16;
    *this << (unsigned long) ptr;
    base = oldbase;
    return *this;
 }

OutputStream& OutputStream::operator << (OutputStream& (*f) (OutputStream&)) {
    return f(*this);
}

OutputStream& endl (OutputStream& os) {
    os << '\n';
    os.flush ();
    return os;
}

OutputStream& bin (OutputStream& os) {
    os.base = 2;
    return os;
}

OutputStream& oct (OutputStream& os) {
    os.base = 8;
    return os;
}

OutputStream& dec (OutputStream& os) {
    os.base = 10;
    return os;
}

OutputStream& hex (OutputStream& os) {
    os.base = 16;
    return os;
}
