#include "fpga_runtime.h"

uint64_t pimExecution(uint32_t addr, uint32_t *data, int iswrite) {
	printf("   fpga: %x\n", addr);
	return 1;
}

bool CmdGenInit(struct PimCgInitData cg_init_data) {
	printf("   fpga: CmdGenInit\n");
	return true;
}

bool CmdGenExecute(struct PimCmdMetadata cmd_metadata) {
	printf("   fpga: CmdGenExecute\n");
	return true;
}

void memAccess(void *data, uint32_t addr, bool is_write, uint32_t transfer_size) {
	printf("   fpga: memAccess\n");
	return;
}


