#pragma once
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include "cmd_generator.h" // >> KKM for CG<<

// #include "../xdma/cdev_sgdma.h"
#ifdef __cplusplus
extern "C" {
#endif

uint64_t pimExecution(uint32_t addr, uint32_t *data, int iswrite);
bool CmdGenInit(struct PimCgInitData cg_init_data);
bool CmdGenExecute(struct PimCmdMetadata cmd_metadata);
void memAccess(void *data, uint32_t addr, bool is_write, uint32_t transfer_size);

#ifdef __cplusplus
}
#endif
