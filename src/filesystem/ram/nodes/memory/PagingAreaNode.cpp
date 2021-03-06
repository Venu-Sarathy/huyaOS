#include "PagingAreaNode.h"

PagingAreaNode::PagingAreaNode() : MemoryNode("pagingarea", FsNode::REGULAR_FILE) {

}

void PagingAreaNode::getValues() {
    uint32_t free = Kernel::Management::getInstance().getPagingAreaManager()->getFreeMemory();
    uint32_t start = Kernel::Management::getInstance().getPagingAreaManager()->getStartAddress();
    uint32_t end = Kernel::Management::getInstance().getPagingAreaManager()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}