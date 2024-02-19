#include <acpi.h>
#include <terminal.h>
#include <log.h>
#include <panic.h>

struct rspd_t* rsdp_header;

struct rspd_t* init_rsdp(void* rsdp_addr) {
    // TODO: Checksum check
    struct rspd_t *rsdp_hdr;
    rsdp_hdr = rsdp_addr;
    //tprintf((char*)(*rsdp_hdr).signature);
    if(rsdp_hdr->revision >= 2) {
        log_info("ACPI Found 2.0");
    } else {
        log_info("ACPI Found 1.0");
        panic("ACPI found 1.0 while we need 2.0 as minimum!");
    }
    tprintf("%x\n",rsdp_hdr->checksum); // внимание казахстан умирает
    return rsdp_hdr;
}