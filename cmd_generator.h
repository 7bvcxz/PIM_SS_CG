#ifndef __CMD_GENERATOR_H_
#define __CMD_GENERATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAP_SBMR 0x3fff
#define MAP_ABMR 0x3ffe
#define MAP_PIM_OP_MODE 0x3ffd
#define MAP_CRF 0x3ffc
#define MAP_GRF 0x3ffb
#define MAP_SRF 0x3ffa

#define IDX_SBMR 0
#define IDX_ABMR 1
#define IDX_PIM_OP_MODE 2
#define IDX_CRF 3
#define IDX_GRF 4
#define IDX_SRF 5

struct CmdInfo {
	uint8_t op_code_tg;
	uint8_t addr_tg;
	uint8_t data_tg;
	uint16_t addr_tg_step;
	uint16_t data_tg_step;
	uint16_t n_cmd;
};

struct PimCgInitData {
	uint32_t pim_reg[16];
	uint8_t pim_op[16];
};

struct PimCmdMetadata {
	uint16_t n_iter;
	uint16_t n_cmdgroup;
	uint32_t operand[16];
	uint32_t data[16];
	uint32_t code[32];
	uint8_t *input;
	uint32_t input_size;
	struct CmdInfo cmd_info[16];
};

#ifdef __cplusplus
}
#endif

#endif // __CMD_GENERATOR_H_
