// Abstractions over our kernel to get uACPI work
#include <uacpi/kernel_api.h>
#include <liballoc.h>
#include <log.h>

void *uacpi_kernel_alloc(uacpi_size size) {
	return kmalloc(size);
}

void *uacpi_kernel_calloc(uacpi_size count, uacpi_size size) {
	return kcalloc(count, size);
}

void uacpi_kernel_free(void *ptr) {
	return kfree(ptr);
}

void *uacpi_kernel_map(uacpi_phys_addr physical, uacpi_size length) {
    (void)length;
    return (void*)physical;
}

void uacpi_kernel_unmap(void *ptr, uacpi_size length) {
    (void)ptr;
    (void)length;
}

uacpi_status uacpi_kernel_raw_memory_read(
	uacpi_phys_addr address, uacpi_u8 byte_width, uacpi_u64 *out
) {
	void *ptr = uacpi_kernel_map(address, byte_width);

	switch (byte_width) {
	case 1:
		*out = *(volatile uint8_t *)ptr;
		break;
	case 2:
		*out = *(volatile uint16_t *)ptr;
		break;
	case 4:
		*out = *(volatile uint32_t *)ptr;
		break;
	case 8:
		*out = *(volatile uint64_t *)ptr;
		break;
	default:
		uacpi_kernel_unmap(ptr, byte_width);
		return UACPI_STATUS_INVALID_ARGUMENT;
	}

	uacpi_kernel_unmap(ptr, byte_width);
	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_raw_memory_write(
	uacpi_phys_addr address, uacpi_u8 byte_width, uacpi_u64 in
) {
	void *ptr = uacpi_kernel_map(address, byte_width);

	switch (byte_width) {
	case 1:
		*(volatile uint8_t *)ptr = in;
		break;
	case 2:
		*(volatile uint16_t *)ptr = in;
		break;
	case 4:
		*(volatile uint32_t *)ptr = in;
		break;
	case 8:
		*(volatile uint64_t *)ptr = in;
		break;
	default:
		uacpi_kernel_unmap(ptr, byte_width);
		return UACPI_STATUS_INVALID_ARGUMENT;
	}

	uacpi_kernel_unmap(ptr, byte_width);
	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_raw_io_read(
	uacpi_io_addr, uacpi_u8, uacpi_u64*
) {
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_raw_io_write(
	uacpi_io_addr, uacpi_u8, uacpi_u64
) {
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_io_map(
	uacpi_io_addr base, uacpi_size, uacpi_handle *out_handle
) {
	*out_handle = (uint64_t*)(base);
	return UACPI_STATUS_OK;
}
void uacpi_kernel_io_unmap(uacpi_handle) { }


uacpi_status uacpi_kernel_io_read(
	uacpi_handle handle, uacpi_size offset,
	uacpi_u8 byte_width, uacpi_u64 *value
) {
	uint64_t addr = (uint64_t)(uintptr_t)(*(uacpi_handle*)(handle));

	return uacpi_kernel_raw_io_read(addr + offset, byte_width, value);
}

uacpi_status uacpi_kernel_io_write(
	uacpi_handle handle, uacpi_size offset,
	uacpi_u8 byte_width, uacpi_u64 value
) {
	uint64_t addr = (uint64_t)(uintptr_t)(*(uacpi_handle*)(handle));

	return uacpi_kernel_raw_io_write(addr + offset, byte_width, value);
}

uacpi_status uacpi_kernel_pci_read(
	uacpi_pci_address *address, uacpi_size offset,
	uacpi_u8 byte_width, uacpi_u64 *value
) {
    (void)address;
    (void)offset;
    (void)byte_width;
    (void)value;
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_write(
	uacpi_pci_address *address, uacpi_size offset,
	uacpi_u8 byte_width, uacpi_u64 value
) {
    (void)address;
    (void)offset;
    (void)byte_width;
    (void)value;
	return UACPI_STATUS_UNIMPLEMENTED;
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char* text, ...) {
    if(level == UACPI_LOG_INFO) {
        log_info(text);
    } else {
        log_info(text);
    }
}

uacpi_u64 uacpi_kernel_get_ticks(void) {
	return 0;
}

void uacpi_kernel_stall(uacpi_u8 usec) {
    (void)usec;
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
    (void)msec;
}

uacpi_handle uacpi_kernel_create_mutex(void)
{
    // any non-null value should work
    return (uacpi_handle)1;
}

uacpi_handle uacpi_kernel_create_event(void)
{
    // any non-null value should work
    return (uacpi_handle)1;
}

void uacpi_kernel_free_mutex(uacpi_handle) {}
void uacpi_kernel_free_event(uacpi_handle) {}
uacpi_thread_id uacpi_kernel_get_thread_id(void) { return (uacpi_thread_id)1; }

uacpi_bool uacpi_kernel_acquire_mutex(uacpi_handle, uacpi_u16)
{
    return true;
}

void uacpi_kernel_release_mutex(uacpi_handle)
{
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle, uacpi_u16) {return true;}

void uacpi_kernel_signal_event(uacpi_handle){}

void uacpi_kernel_reset_event(uacpi_handle){}

uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request*){return UACPI_STATUS_OK;}

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq, uacpi_interrupt_handler, uacpi_handle ctx,
    uacpi_handle *out_irq_handle
) {
    (void)irq;
    (void)ctx;
    (void)out_irq_handle;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler, uacpi_handle irq_handle
){(void)irq_handle;return UACPI_STATUS_OK;}

uacpi_handle uacpi_kernel_create_spinlock(void) {return (uacpi_handle)1;}
void uacpi_kernel_free_spinlock(uacpi_handle) {}

uacpi_cpu_flags uacpi_kernel_spinlock_lock(uacpi_handle) {return 0;}
void uacpi_kernel_spinlock_unlock(uacpi_handle, uacpi_cpu_flags) {}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type, uacpi_work_handler, uacpi_handle ctx
){(void)ctx;return UACPI_STATUS_OK;}

uacpi_status uacpi_kernel_wait_for_work_completion(void){return UACPI_STATUS_OK;}

