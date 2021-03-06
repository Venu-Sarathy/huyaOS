#include "kernel/core/System.h"
#include <cstdint>
#include "kernel/core/Management.h"
#include "Uhci.h"

Kernel::Logger &Uhci::log = Kernel::Logger::get("UHCI");

Uhci::~Uhci() {
    delete usbCommandPort;
    delete usbStatusPort;
    delete usbInterruptEnablePort;
    delete frameNumberPort;
    delete frameListBaseAddressPort;
    delete startOfFrameModifyPort;
    delete statusControlPort1;
    delete statusControlPort2;

    Kernel::Management::getInstance().freeIO(frameList);
}

void Uhci::setup(const Pci::Device &device) {
    log.trace("Initializing UHCI controller");

    uint32_t baseAddress = Pci::readDoubleWord(device.bus, device.device, device.function, Pci::PCI_HEADER_BAR4) & 0x0000ffe0;
    uint32_t usbVersion = Pci::readWord(device.bus, device.device, device.function, 0x60);

    log.info(usbVersion == 0x10 ? "Controller is compliant with USB version 1.0" :
                                   "Controller is compliant with a USB version prior to 1.0");
    log.info("IO base address: 0x%08x", baseAddress);

    timeService = Kernel::System::getService<Kernel::TimeService>();

    usbCommandPort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_COMMAND_REGISTER));
    usbStatusPort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_STATUS_REGISTER));
    usbInterruptEnablePort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_INTERRUPT_REGISTER));
    frameNumberPort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::FRAME_NUMBER_REGISTER));
    frameListBaseAddressPort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::FRAME_LIST_BASE_ADDRESS_REGISTER));
    startOfFrameModifyPort = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::START_OF_FRAME_MODIFY_REGISTER));
    statusControlPort1 = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::PORT_1_STATUS_CONTROL_REGISTER));
    statusControlPort2 = new IoPort(static_cast<uint16_t>(baseAddress + IORegisterOffset::PORT_2_STATUS_CONTROL_REGISTER));

    frameList = static_cast<FrameListPointer *>(Kernel::Management::getInstance().mapIO(1024 * sizeof(FrameListPointer)));

    memset(frameList, 0, 1024 * sizeof(FrameListPointer));

    for(uint32_t i = 0; i < 1024; i++) {
        frameList[i].terminate = 1;
    }

    Pci::enableBusMaster(device.bus, device.device, device.function);
    Pci::enableIoSpace(device.bus, device.device, device.function);

    stopHostController();

    resetHostController();

    disableInterrupts();

    //Reset command and status registers
    usbCommandPort->outw(0x00);
    statusControlPort1->outw(0x00);
    statusControlPort2->outw(0x00);

    // Set SOF Timing to 1 ms
    startOfFrameModifyPort->outb(0x40);

    // Set maximum packet size to 64 byte
    usbCommandPort->outb(MAX_PACKET);

    // Set frame list address
    frameNumberPort->outb(0x00);
    frameListBaseAddressPort->outdw(reinterpret_cast<uint32_t>(Kernel::Management::getInstance().getPhysicalAddress(frameList)));

    startHostController();

    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::CONFIGURE_FLAG);

    resetPort(1);
    resetPort(2);

    if(statusControlPort1->inw() & UsbPortControlStatus::CURRENT_CONNECT_STATUS) {
        log.info("Device detected on port 1");
    }

    if(statusControlPort2->inw() & UsbPortControlStatus::CURRENT_CONNECT_STATUS) {
        log.info("Device detected on port 2");
    }

    log.trace("Finished initializing UHCI controller");
}

void Uhci::resetHostController() {
    log.trace("Resetting Host Controller");

    // Send reset command
    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::HOST_CONTROLLER_RESET);

    // Wait for the controller to reset itself
    while(usbCommandPort->inw() & UsbCommand::HOST_CONTROLLER_RESET);

    log.trace("Successfully reset Host Controller");
}

void Uhci::startHostController() {
    log.trace("Starting Host Controller");

    // Send start command
    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::RUN_STOP);

    log.trace("Successfully started Host Controller");
}

void Uhci::stopHostController() {
    log.trace("Stopping Host Controller");

    // Send stop command
    usbCommandPort->outw(usbCommandPort->inw() & ~UsbCommand::RUN_STOP);

    // Wait for the controller to finish processing the current transaction and stop
    while(!(usbStatusPort->inw() & UsbStatus::HOST_CONTROLLER_HALTED));

    log.trace("Successfully stopped Host Controller");
}

void Uhci::disableInterrupts() {
    usbInterruptEnablePort->outw(0x00);
}

void Uhci::enableInterrupt(Uhci::UsbInterrupt interrupt) {
    usbInterruptEnablePort->outw(usbInterruptEnablePort->inw() | interrupt);
}

void Uhci::resetPort(uint8_t portNum) {
    if(portNum != 1 && portNum != 2) {
        log.warn("Trying to reset an undefined port (%u)! Aborting...", portNum);

        return;
    }

    log.trace("Resetting port %u", portNum);

    IoPort *port = portNum == 1 ? statusControlPort1 : statusControlPort2;

    // Clear change flags
    port->outw(port->inw() | UsbPortControlStatus::CONNECT_STATUS_CHANGE | UsbPortControlStatus::PORT_ENABLED_DISABLED_CHANGE);

    // Reset port
    port->outw(port->inw() | UsbPortControlStatus::PORT_RESET);
    timeService->msleep(100);

    port->outw(port->inw() & ~UsbPortControlStatus::PORT_RESET);
    timeService->msleep(10);

    log.trace("Successfully reset port %u", portNum);
}

void Uhci::trigger(Kernel::InterruptFrame &frame) {
    log.debug("INTERRUPT");
}
