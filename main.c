#include <stdint.h>
#include <string.h>
#include <drm/radeon_drm.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xf86drm.h>
#include <amdgpu.h>
#include <amdgpu_drm.h>
#include <unistd.h>
#include "httpd.h"

static amdgpu_device_handle amdgpu_dev;

static void start_server();

static int getsclk_amdgpu(uint32_t *out) {
  return amdgpu_query_sensor_info(amdgpu_dev, AMDGPU_INFO_SENSOR_GFX_SCLK, sizeof(uint32_t), out);
}

static int getvram_amdgpu(uint64_t *out) {
  return amdgpu_query_info(amdgpu_dev, AMDGPU_INFO_VRAM_USAGE, sizeof(uint64_t), out);
}

static int getmclk_amdgpu(uint32_t *out) {
  return amdgpu_query_sensor_info(amdgpu_dev, AMDGPU_INFO_SENSOR_GFX_MCLK, sizeof(uint32_t), out);
}

static int getgtt_amdgpu(uint64_t *out) {
  return amdgpu_query_info(amdgpu_dev, AMDGPU_INFO_GTT_USAGE, sizeof(uint64_t), out);
}

enum {
	GRBM_STATUS = 0x8010,
	SRBM_STATUS = 0xe50,
	SRBM_STATUS2 = 0xe4c,
	MMAP_SIZE = 0x14,
	SRBM_MMAP_SIZE = 0xe54,
	VENDOR_AMD = 0x1002
};

// returns a bitfield of what hw blocks are active?
static int getgrbm_amdgpu(uint32_t *out) {
  return amdgpu_read_mm_registers(amdgpu_dev, GRBM_STATUS/4, 1, 0xffffffff, 0, out);
}

int main(int argc, char **argv) {
  uint32_t drm_major, drm_minor;
  int drm_fd = drmOpenRender(128);
  if (amdgpu_device_initialize(drm_fd, &drm_major, &drm_minor, &amdgpu_dev)) {
    return -1;
  }
  /*
  while (true) {
    printf("sclk/mclk: %d/%d\tvram: %ld, grbm: 0x%x\n", sclk, mclk, vram >> 20, grbm);
    sleep(1);
  }*/
  start_server();
  return 0;
}

void start_server() {
  serve_forever("12913");
}

void route() {
  ROUTE_START();
  ROUTE_GET("/metrics") {
    uint32_t sclk; // mhz
    uint32_t mclk; // mhz
    getsclk_amdgpu(&sclk);
    getmclk_amdgpu(&mclk);
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("amdgpu_shader_clock %d\n", sclk);
    printf("amdgpu_memory_clock %d\n", mclk);
    uint64_t vram; // bytes
    getvram_amdgpu(&vram);
    uint32_t grbm;
    getgrbm_amdgpu(&grbm);
    printf("amdgpu_vram %ld\n", vram);
    uint64_t gtt;
    getgtt_amdgpu(&gtt);
    printf("amdgpu_gtt_used %ld\n", gtt);
  }
  ROUTE_END();
}
