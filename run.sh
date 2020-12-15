#!/bin/bash

readonly CONST_QEMU_BIN_I386="qemu-system-i386"
readonly CONST_QEMU_BIN_X86_64="qemu-system-x86_64"
readonly CONST_QEMU_MACHINE_PC="pc"
readonly CONST_QEMU_MACHINE_PC_KVM="pc,accel=kvm,kernel-irqchip=off"
readonly CONST_QEMU_BIOS_PC=""
readonly CONST_QEMU_BIOS_EFI="/usr/share/edk2-ovmf/ia32/OVMF_CODE.fd"
readonly CONST_QEMU_RAM="256M"
readonly CONST_QEMU_ARGS="
    -boot d
    -cdrom hhuOS.iso
    -vga std
    -monitor stdio
    -cpu pentium,+sse,+sse2
    -rtc base=localtime,clock=host
    -drive index=0,if=floppy
    -drive format=raw,file=hdd0.img,if=none,id=disk0
    -device ich9-ahci,id=ahci
    -device ide-hd,drive=disk0,bus=ahci.0
    -netdev user,id=eth0,hostfwd=tcp::8821-:8821
    -device e1000,netdev=eth0
    -object filter-dump,id=filter0,netdev=eth0,file=eth0.dump"
readonly CONST_QEMU_OLD_AUDIO_ARGS="
  -soundhw pcspk
  -device sb16,irq=10,dma=1
"
readonly CONST_QEMU_NEW_AUDIO_ARGS="
  -audiodev alsa,id=alsa
  -machine pcspk-audiodev=alsa
  -device sb16,irq=10,dma=1,audiodev=alsa
"

QEMU_BIN="${CONST_QEMU_BIN_I386}"
QEMU_MACHINE="${CONST_QEMU_MACHINE_PC}"
QEMU_BIOS="${CONST_QEMU_BIOS_PC}"
QEMU_RAM="${CONST_QEMU_RAM}"
QEMU_ARGS="${CONST_QEMU_ARGS}"

QEMU_GDB_PORT=""

version_lt() {
  test "$(printf "%s\n" "$@" | sort -V | tr ' ' '\n' | head -n 1)" != "$2"
}

get_audio_parameters() {
  qemu_version=$(${QEMU_BIN} --version | head -n 1 | cut -c 23-)

  if version_lt "$qemu_version" "5.0.0"; then
    printf "%s" "${CONST_QEMU_OLD_AUDIO_ARGS}"
    return
  fi

  printf "%s" "${CONST_QEMU_NEW_AUDIO_ARGS}"
}

check_file() {
  local file=$1

  if [ ! -f "$file" ]; then
    printf "File '%s' does not exist! Did you run build.sh?\\n" "${file}"
    exit 1
  fi
}

parse_architecture() {
  local architecture=$1

  if [ "${architecture}" == "i386" ] || [ "${architecture}" == "x86" ]; then
    QEMU_BIN="${CONST_QEMU_BIN_I386}"
  elif [ "${architecture}" == "x86_64" ] || [ "${architecture}" == "x64" ]; then
    QEMU_BIN="${CONST_QEMU_BIN_X86_64}"
  else
    printf "Invalid architecture '%s'!\\n" "${architecture}"
    exit 1
  fi
}

parse_machine() {
  local machine=$1

  if [ "${machine}" == "pc" ]; then
    QEMU_MACHINE="${CONST_QEMU_MACHINE_PC}"
  elif [ "${machine}" == "pc-kvm" ]; then
    QEMU_MACHINE="${CONST_QEMU_MACHINE_PC_KVM}"
  else
    printf "Invalid machine '%s'!\\n" "${machine}"
    exit 1
  fi
}

parse_bios() {
  local bios=$1

  if [ "${bios}" == "bios" ] || [ "${bios}" == "default" ]; then
    QEMU_BIOS="${CONST_QEMU_BIOS_PC}"
  elif [ "${bios}" == "efi" ]; then
    QEMU_BIOS="${CONST_QEMU_BIOS_EFI}"
  else
    QEMU_BIOS="${bios}"
  fi
}

parse_ram() {
  local memory=$1

  QEMU_RAM="${memory}"
}

parse_debug() {
  local port=$1

  echo "set architecture i386
      set disassembly-flavor intel
      break *(debug_label - 0xC0000000)
      target remote 127.0.0.1:${port}
      continue" >/tmp/gdbcommands."$(id -u)"

  QEMU_GDB_PORT="${port}"
}

start_gdb() {
  local port=$1

  gdb -x /tmp/gdbcommands.$(id -u) "loader/boot/hhuOS.bin"
  
  exit $?
}

print_usage() {
  printf "Usage: ./run.sh [OPTION...]
    Available options:
    -a, --architecture
        Set the architecture, which qemu should emulate ([i386,x86] | [x86_64,x64]) (Default: i386)
    -m, --machine
        Set the machine profile, which qemu should emulate ([pc] | [pc-kvm]) (Defualt: pc)
    -b, --bios
        Set the bios, which qemu should use ([bios,default] | [efi] | [/path/to/bios.file]) (Default: bios)
    -r, --ram
        Set the amount of ram, which qemu should use (e.g. 256, 1G, ...) (Default: 256M)
    -d, --debug
        Set the port, on which qemu should listen for GDB clients (default: disabled)
    -h, --help
        Show this help message\\n"
}

parse_args() {
  while [ "${1}" != "" ]; do
    local arg=$1
    local val=$2

    case $arg in
    -a | --architecture)
      parse_architecture "$val"
      ;;
    -m | --machine)
      parse_machine "$val"
      ;;
    -b | --bios)
      parse_bios "$val"
      ;;
    -r | --ram)
      parse_ram "$val"
      ;;
    -d | --debug)
      parse_debug "$val"
      ;;
    -g | --gdb)
      start_gdb
      ;;
    -h | --help)
      print_usage
      exit 0
      ;;
    *)
      printf "Unknown option '%s'\\n" "${arg}"
      print_usage
      exit 1
      ;;
    esac
    shift 2
  done
}

run_qemu() {
  local command="${QEMU_BIN}"

  if [ -n "${QEMU_MACHINE}" ]; then
    command="${command} -machine ${QEMU_MACHINE}"
  fi

  if [ -n "${QEMU_BIOS}" ]; then
    command="${command} -bios ${QEMU_BIOS}"
  fi

  command="${command} -m ${QEMU_RAM}"

  command="${command} ${QEMU_ARGS}"

  if [ -n "${QEMU_GDB_PORT}" ]; then
    $command -gdb tcp::"${QEMU_GDB_PORT}" -S &
  else
    $command
  fi
}

check_file hhuOS.iso
check_file hdd0.img

parse_args "$@"

QEMU_ARGS="${QEMU_ARGS} $(get_audio_parameters)"

run_qemu
