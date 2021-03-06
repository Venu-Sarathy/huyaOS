/*
 * Copyright (C) 2018/19 Thiemo Urselmann
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#ifndef HHUOS_DESCRIPTORS_H
#define HHUOS_DESCRIPTORS_H

#include "lib/util/ArrayList.h"

/**
 * This class is a special kind of list.
 * Its main purpose is to keep track of the current
 * used descriptor in a descriptor block and making it available to the caller.
 * @tparam T Type of descriptor used.
 */
template <class T>
class Descriptors {
public:
    explicit Descriptors(Util::ArrayList<T> *descriptors);
    ~Descriptors() = default;

    /**
     * Retrieve the current descriptor.
     * @return The current descriptor pointed to by an index-value.
     */
    T current();


    /**
     * Advances the list-index up to the last element.
     * If it is the last index in the list, setNext wraps the index to 0 and starts
     * from the beginning.
     */
    void setNext();

private:
    /**
     * The descriptor-block to be wrapped.
     */
    Util::ArrayList<T> *descriptors;

    /**
     * The index of the current descriptor in progress.
     */
    uint32_t index;
};

template<class T>
Descriptors<T>::Descriptors(Util::ArrayList<T> *descriptors) : descriptors(descriptors), index(0) {
}


template<class T>
T Descriptors<T>::current() {
    return descriptors->get(index);
}

template<class T>
void Descriptors<T>::setNext() {
    index = (index + 1) % descriptors->size();
}




#endif //HHUOS_DESCRIPTORS_H
