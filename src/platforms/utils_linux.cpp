#include "utils.h"

#include <sys/types.h>
#include <sys/sysinfo.h>

unsigned long long GetMemoryTotal() {
    struct sysinfo mem_info;
    sysinfo(&mem_info);

    unsigned long long total_virtual_mem = mem_info.totalram;
    total_virtual_mem += mem_info.totalswap;
    total_virtual_mem *= mem_info.mem_unit;

    return total_virtual_mem;
}

unsigned long long GetMemoryUsage() {
    struct sysinfo mem_info;
    sysinfo(&mem_info);

    unsigned long long total_virtual_mem = mem_info.totalram - mem_info.freeram;
    total_virtual_mem += mem_info.totalswap - mem_info.freeswap;
    total_virtual_mem *= mem_info.mem_unit;

    return total_virtual_mem;
}