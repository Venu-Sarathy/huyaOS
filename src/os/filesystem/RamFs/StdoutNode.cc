#include "StdoutNode.h"
#include "kernel/Kernel.h"

StdoutNode::StdoutNode() : VirtualNode("stdout", FsNode::CHAR_FILE) {
    stdStreamService = Kernel::getService<StdStreamService>();
}

uint64_t StdoutNode::getLength() {
    return 0;
}

uint64_t StdoutNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    return numBytes;
}

uint64_t StdoutNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stdout = stdStreamService->getStdout();
    
    stdout->writeBytes(buf, numBytes);
    stdout->flush();

    return numBytes;
}