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

#ifndef __deprecated_String_include__
#define __deprecated_String_include__

#include <cstdint>
#include <cstdarg>
#include "lib/util/Array.h"
#include "lib/stream/OutputStream.h"

/**
 * @author Filip Krakowski
 */
class String {

public:

    String() noexcept;

    String(const char string[]) noexcept;

    String(char character) noexcept;

    String(const String &other) noexcept;

    ~String();

    uint32_t hashCode() const;

    uint32_t length() const;

    uint32_t indexOf(char character, uint32_t start = 0) const;

    uint32_t indexOf(const String &other, uint32_t start = 0) const;

    bool isEmpty() const;

    String substring(uint32_t begin, uint32_t end) const;

    String strip();

    Util::Array<String> split(const String &delimiter, uint32_t limit = 0) const;

    String remove(const String &string) const;

    String removeAll(const String &string) const;

    bool beginsWith(const String &string) const;

    bool endsWith(const String &string) const;

    static String valueOf(bool value);

    static String valueOf(int32_t value, uint8_t radix, bool sign = true);

    static String valueOf(int32_t value, uint8_t radix, uint8_t padding, bool sign = true);

    static String join(const String &separator, const Util::Array<String> &elements);

    static String format(const char *fmt, ...);

    static String vformat(const char *fmt, va_list args);

    static bool isAlpha(char c);

    String toUpperCase();

    String toLowerCase();

    bool operator==(const String &other) const;

    bool operator!=(const String &other) const;

    String &operator=(const String &other);

    String &operator+=(const String &other);

    friend String operator+(const String &first, const String &second);

    friend String operator+(const String &first, char second);

    friend String operator+(const String &first, const char *second);

    friend String operator+(char first, const String &string);

    friend String operator+(const char *first, const String &second);

    friend OutputStream &operator<<(OutputStream &outStream, const String &string);

    char operator[](uint32_t index) const;

    char &operator[](uint32_t index);

    explicit operator char*() const;

    explicit operator uint32_t() const;

private:

    static const uint8_t CASE_OFFSET = 32;

    uint32_t len;

    char *buffer;

    friend class Logger;
};


#endif
