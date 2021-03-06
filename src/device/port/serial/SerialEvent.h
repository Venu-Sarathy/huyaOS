#ifndef HHUOS_SERIALEVENT_H
#define HHUOS_SERIALEVENT_H

#include "device/port/PortEvent.h"
#include "Common.h"

namespace Serial {

template<ComPort port>
class SerialEvent : public PortEvent {

public:

    SerialEvent();

    explicit SerialEvent(char c);

    SerialEvent(const SerialEvent &other);

    SerialEvent &operator=(const SerialEvent &other);

    ~SerialEvent() override = default;

    String getType() const override;

    char getChar() override;

private:

    char c;

    static const constexpr char *NAME_1 = "Com1Event";
    static const constexpr char *NAME_2 = "Com2Event";
    static const constexpr char *NAME_3 = "Com3Event";
    static const constexpr char *NAME_4 = "Com4Event";

};

template<ComPort port>
SerialEvent<port>::SerialEvent() : c(0) {

}

template <ComPort port>
SerialEvent<port>::SerialEvent(char c) : c(c) {

}

template<ComPort port>
SerialEvent<port>::SerialEvent(const SerialEvent &other) : c(other.c) {
}

template<ComPort port>
SerialEvent<port> &SerialEvent<port>::operator=(const SerialEvent &other) {
    if (&other == this) {
        return *this;
    }

    c = other.c;

    return *this;
}

template <ComPort port>
String SerialEvent<port>::getType() const {
    switch(port) {
        case COM1 :
            return NAME_1;
        case COM2 :
            return NAME_2;
        case COM3 :
            return NAME_3;
        case COM4 :
            return NAME_4;
    }
}

template <ComPort port>
char SerialEvent<port>::getChar() {
    return c;
}

}

#endif
