#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _dev_info() {}
void _dev_warn() {}
void _dev_err() {}
void __dynamic_pr_debug() {}
void hid_dump_input() {}
void hid_input_report() {}

unsigned long random_kmalloc_seed;

#define PAGE_SHIFT 12
#define KMALLOC_SHIFT_HIGH (PAGE_SHIFT + 1)
/* good enough */
#define NR_KMALLOC_TYPES 64

typedef struct kmem_cache* kmem_buckets[KMALLOC_SHIFT_HIGH + 1];
kmem_buckets kmalloc_caches[NR_KMALLOC_TYPES];

struct hid_device;
typedef uint8_t u8;

const u8* call_hid_bpf_rdesc_fixup(struct hid_device* hdev, const u8* rdesc,
                                   unsigned int* size) {
  void* out = malloc(*size);
  memcpy(out, rdesc, *size);
  return out;
}

void kfree(void* a) { free(a); }

#define __GFP_ZERO (1 << 8)
typedef unsigned int gfp_t;

void* __kmalloc(size_t size, gfp_t flags) {
  if (flags & __GFP_ZERO) {
    return calloc(1, size);
  }
  return malloc(size);
}

void* kmalloc_trace(void* a, gfp_t flags, size_t size) {
  return __kmalloc(size, flags);
}

void* kmemdup(void* src, size_t len, gfp_t gfp) {
  void* out = malloc(len);
  memcpy(out, src, len);
  return out;
}

void* vzalloc(size_t size) { return calloc(1, size); }

void vfree(void* a) { free(a); }

void* krealloc(void* objp, size_t new_size, gfp_t flags) {
  return realloc(objp, new_size);
}

void fortify_panic() { abort(); }

struct hid_driver {
  char pad[0x138];
};

struct hid_ll_driver {
  char pad[0x40]; // 0
int                        (*raw_request)(struct hid_device *, unsigned char, u8 *, size_t, unsigned char, int);
char pad2[0x68 - 0x48]; // 0x48
};

struct hid_device {
  uint8_t* dev_rdesc;                     // 0x0
  unsigned int dev_rsize;                 // 0x8
  char padding1[0x1be0 - 0xc];            // 0xc
  struct hid_driver* driver;              // 0x1be0
  char padding2[0x1bf0 - 0x1be8];         // 0x1be8
  const struct hid_ll_driver* ll_driver;  // 0x1bf0
  char padding3[0x1e38 - 0x1bf8];         // 0x1bf8
};

static_assert(offsetof(struct hid_device, driver) == 0x1be0, "driver");
static_assert(offsetof(struct hid_device, ll_driver) == 0x1bf0, "ll_driver");

int hid_open_report(struct hid_device* device);
void hid_close_report(struct hid_device* device);
void zhuowei_hid_init_report(struct hid_device* device);
int zhuowei_hid_send_reports(struct hid_device* hid);

struct hid_driver kEmptyDriver;
struct hid_ll_driver kEmptyLlDriver;

static int ll_raw_request(struct hid_device* hid, unsigned char arg2, u8* buf, size_t size, unsigned char arg3, int arg4) {
	for (int i = 1; i < size; i++) {
		u8 a = buf[i];
		if (a == 0xbe) {
			abort();
		}
	}
	return 0;
}

int LLVMFuzzerTestOneInput(uint8_t* Data, size_t Size) {
  struct hid_device device = {
      .dev_rdesc = Data,
      .dev_rsize = Size,
      .driver = &kEmptyDriver,
      .ll_driver = &kEmptyLlDriver,
  };
  kEmptyLlDriver.raw_request = &ll_raw_request;
  zhuowei_hid_init_report(&device);
  int err = hid_open_report(&device);
  if (err) {
    return 0;
  }
  zhuowei_hid_send_reports(&device);
  hid_close_report(&device);
  return 0;
}
