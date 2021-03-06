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

#ifndef __VesaGraphics_include__
#define __VesaGraphics_include__

#include <cstdint>

#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "kernel/core/System.h"
#include "kernel/memory/manager/IOMemoryManager.h"

/**
 * Implementation of LinearFrameBuffer for graphics cards, that are compatible with the VBE-standard.
 * Compatible devices need to support at least VBE 2.0.
 */
class VesaGraphics : public LinearFrameBuffer {

public:
	/**
	 * Information about a VBE device.
	 * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
	 */
    struct VbeInfo {
		char 	    signature[4] = {'V', 'B', 'E', '2'};
		uint16_t 	version{};			// VBE version; high byte is major version, low byte is minor version
		uint32_t 	oem{};				// segment:offset pointer to OEM
		uint32_t 	capabilities{};		// bitfield that describes card capabilities
		uint16_t 	video_modes[2]{};	// segment:offset pointer to list of supported video modes
		uint16_t 	video_memory{};		// amount of video memory in 64KB blocks
		uint16_t 	software_rev{};		// software revision
		uint16_t 	vendor[2]{};		// segment:offset to card vendor string
		uint16_t 	product_name[2]{};	// segment:offset to card model name
		uint16_t 	product_rev[2]{};	// segment:offset pointer to product revision
		char reserved[222]{};			// reserved for future expansion
		char oem_data[256]{};			// OEM BIOSes store their strings in this area
	} __attribute__((packed)) ;

    /**
     * Information about a VBE graphics mode.
     * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
     */
	struct ModeInfo {
        uint16_t  attributes;
        uint8_t   winA,winB;
        uint16_t  granularity;
        uint16_t  winsize;
        uint16_t  segmentA, segmentB;
        uint16_t  realFctPtr[2];
        uint16_t  pitch;          // bytes per Scanline

        uint16_t  Xres, Yres;
        uint8_t   Wchar, Ychar, planes, bpp, banks;
        uint8_t   memory_model, bank_size, image_pages;
        uint8_t   reserved0;

        uint8_t   red_mask, red_position;
        uint8_t   green_mask, green_position;
        uint8_t   blue_mask, blue_position;
        uint8_t   rsv_mask, rsv_position;
        uint8_t   directcolor_attributes;

        uint32_t  physbase;       // address of the linear framebuffer
        uint32_t  reserved1;
        uint16_t  reserved2;

		bool operator!=(const ModeInfo &other) const {
            return Yres != other.Yres || Xres != other.Xres || bpp != other.bpp;
		};

    } __attribute__((packed));
	
	struct PaletteEntry {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t alignment;       // unused;
	} __attribute__((packed));

private:
    bool available = false;

	String vendorName = String();
	String deviceName = String();
	uint32_t videoMemorySize = 0;

    void *virtLfbAddress = nullptr;

	Util::ArrayList<LfbResolution> resolutions;

    static const uint16_t INVALID_MODE = 0xFFFF;
    static const constexpr char *NAME = "VesaGraphics";

private:
	/**
     * Overriding virtual function from LinearFrameBuffer.
     */
	bool setResolution(LinearFrameBuffer::LfbResolution resolution) override;

	/**
	 * Set the VBE-device to a given mode.
	 *
	 * @param mode The mode
	 * @return true, on success
	 */
    bool setMode(uint16_t mode);

    /**
     * Get information about the VBE device from the BIOS.
     */
    VbeInfo* getVbeInfo();

    /**
     * Get information about a specific VBE graphics mode from the BIOS.
     * @param mode The mode
     */
    ModeInfo* getModeInfo(uint16_t mode);

public:
    /**
     * Constructor.
     */
    VesaGraphics();

    /**
     * Copy-constructor.
     */
    VesaGraphics(const VesaGraphics &copy) = delete;

    /**
     * Destructor.
     */
    ~VesaGraphics() override = default;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    String getName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    bool isAvailable() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    Util::Array<LinearFrameBuffer::LfbResolution> getLfbResolutions() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	String getVendorName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	String getDeviceName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	uint32_t getVideoMemorySize() override;
};

#endif
