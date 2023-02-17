#include "pim_blas.h"

uint8_t *null_ptr = (uint8_t *)malloc(sizeof(uint8_t) * WORD_SIZE);

void blas_init(uint64_t num) {
	struct PimCgInitData pim_hbm_init;

	pim_hbm_init.pim_reg[IDX_SBMR] = MAP_SBMR << ro_pos;
	pim_hbm_init.pim_reg[IDX_ABMR] = MAP_ABMR << ro_pos;
	pim_hbm_init.pim_reg[IDX_PIM_OP_MODE] = MAP_PIM_OP_MODE << ro_pos;
	pim_hbm_init.pim_reg[IDX_CRF] = MAP_CRF << ro_pos;
	pim_hbm_init.pim_reg[IDX_GRF] = MAP_GRF << ro_pos;
	pim_hbm_init.pim_reg[IDX_SRF] = MAP_SRF << ro_pos;

	pim_hbm_init.pim_op[0] = 0; // RD
	pim_hbm_init.pim_op[1] = 1; // WR

	CmdGenInit(pim_hbm_init);
}

bool C_pimblasAddPreprocess(int len, uint8_t **in0, uint8_t **in1) {
	return true;
}

bool C_pim_add(int len, uint8_t *in0, uint8_t *in1, uint8_t *out) {
	return true;	
}

bool C_pimblasMulPreprocess(int len, uint8_t **in0, uint8_t **in1) {
	return true;
}

bool C_pim_mul(int len, uint8_t *in0, uint8_t *in1, uint8_t *out) {
	return true;	
}

bool C_pimblasGemvPreprocess(int len_in, int len_out, uint8_t **w) {
	return true;
}

bool C_pim_gemv(int len_in, int len_out, uint8_t *in, uint8_t *w, uint8_t *out) {
	return true;
}

bool pimblasAddPreprocess(struct PimCmdMetadata *add_cmd_metadata, int len, uint8_t **in0, uint8_t **in1) {
	int in0_idx = 0;  // mem
	int in1_idx = 1;  // mem
	int out_idx = 2;  // mem
	int in0_addr = 0;
	int in1_addr = in0_addr + len / UNITS_PER_WORD;
	int out_addr = in1_addr + len / UNITS_PER_WORD;

	// Write Input to Memory
	memAccess(in0, in0_addr, true, (uint32_t)(len * UNIT_SIZE));
	memAccess(in1, in1_addr, true, (uint32_t)(len * UNIT_SIZE));

	add_cmd_metadata->n_iter = len * UNIT_SIZE / GRF_SIZE;
	add_cmd_metadata->n_cmdgroup = 4;
	add_cmd_metadata->operand[in0_idx] = in0_addr;
	add_cmd_metadata->operand[in1_idx] = in1_addr;

	// Read SB → AB (Automatic in FPGA, ok)

	// Write ukernel code in Metadata (Only Send Data to FPGA)
	add_cmd_metadata->code[0] = 0b01000010000000001000000000000000; // MOV(A)  GRF_A[A0]  BANK
	add_cmd_metadata->code[1] = 0b00010000000001000000100000000111; // JUMP    -1         7
	add_cmd_metadata->code[2] = 0b10000010000010001000000000000000; // ADD(A)  GRF_A[A0]  BANK      GRF_A[A0]
	add_cmd_metadata->code[3] = 0b00010000000001000000100000000111; // JUMP    -1         7
	add_cmd_metadata->code[4] = 0b01000000010000001000000000000000; // MOV(A)  BANK       GRF_A[A0]
	add_cmd_metadata->code[5] = 0b00010000000001000000100000000111; // JUMP    -1         7
	add_cmd_metadata->code[6] = 0b00100000000000000000000000000000; // EXIT	

	// Change to AB-PIM Mode
	add_cmd_metadata->cmd_info[0].op_code_tg = 0;  // RD
	add_cmd_metadata->cmd_info[0].addr_tg = IDX_PIM_OP_MODE;
	add_cmd_metadata->cmd_info[0].data_tg = 0;
	add_cmd_metadata->cmd_info[0].addr_tg_step = 0;
	add_cmd_metadata->cmd_info[0].data_tg_step = 0;
	add_cmd_metadata->cmd_info[0].n_cmd = 1;

	// Read Input0 8 Col
	add_cmd_metadata->cmd_info[1].op_code_tg = 0;  // RD
	add_cmd_metadata->cmd_info[1].addr_tg = in0_idx;
	add_cmd_metadata->cmd_info[1].data_tg = 0;
	add_cmd_metadata->cmd_info[1].addr_tg_step = 1 << co_pos;
	add_cmd_metadata->cmd_info[1].data_tg_step = 0;
	add_cmd_metadata->cmd_info[1].n_cmd = 8;

	// Read Input1 8 Col
	add_cmd_metadata->cmd_info[2].op_code_tg = 0;  // RD
	add_cmd_metadata->cmd_info[2].addr_tg = in1_idx;
	add_cmd_metadata->cmd_info[2].data_tg = 0;
	add_cmd_metadata->cmd_info[2].addr_tg_step = 1 << co_pos;
	add_cmd_metadata->cmd_info[2].data_tg_step = 0;
	add_cmd_metadata->cmd_info[2].n_cmd = 8;

	// WRITE Output 8 Col
	add_cmd_metadata->cmd_info[3].op_code_tg = 0;  // WR
	add_cmd_metadata->cmd_info[3].addr_tg = out_idx;
	add_cmd_metadata->cmd_info[3].data_tg = 0;
	add_cmd_metadata->cmd_info[3].addr_tg_step = 1 << co_pos;
	add_cmd_metadata->cmd_info[3].data_tg_step = 0;
	add_cmd_metadata->cmd_info[3].n_cmd = 8;
	
	return true;
}

bool pimblasMulPreprocess(PIMKernel *micro_kernel, int len, uint8_t **in0, uint8_t **in1) {
	return true;
}

bool pimblasReluPreprocess(PIMKernel *micro_kernel, int len, uint8_t **in) {
	return true;
}

bool pimblasBn1dPreprocess(PIMKernel *micro_kernel, int len_batch, int len_feature, uint8_t **w_mul,
						  uint8_t **w_add) {
	return true;
}

bool pimblasGemvPreprocess(struct PimCmdMetadata *cmd_metadata0, struct PimCmdMetadata *cmd_metadata1,
						   int len_in, int len_out, uint8_t **w) {
	int len_in_ = Ceiling(len_in, 8);
	int len_out_ = Ceiling(len_out, 4096);
	
	int w_idx = 0;   // mem
	int out_idx = 1; // mem
	int in_idx = 0;  // data

	int w_addr = 0;
	int out_addr = w_addr + len_in_ * len_out_ / UNITS_PER_WORD;
	int in_addr = 0;

	*w = GemvReshape(*w, len_in, len_out);
	*w = Transpose(*w, len_in, len_out);

	memAccess(w, w_addr, true, (uint32_t)(len_in_ * len_out_ * UNIT_SIZE));
	
	cmd_metadata0->n_iter = (len_in_ + 8 - 1) / 8;
	cmd_metadata0->n_cmdgroup = 3;
	cmd_metadata0->operand[w_idx] = w_addr;
	cmd_metadata0->operand[out_idx] = out_addr;
	cmd_metadata0->data[in_idx] = in_addr;

	// Read SB → AB (Automatic in FPGA, ok)

	// Write ukernel code in Metadata (Only Send Data to FPGA)
	cmd_metadata0->code[0] = 0b10100100001000001000100000000000; // MAC(A)  GRF_B[A0]  BANK      SRF_M[A0]
	cmd_metadata0->code[1] = 0b00010000000001000000100000000111; // JUMP    -1         7
	cmd_metadata0->code[2] = 0b00100000000000000000000000000000; // EXIT

	// Write Input to SRF
	cmd_metadata0->cmd_info[0].op_code_tg = 0;  // RD
	cmd_metadata0->cmd_info[0].addr_tg = IDX_SRF;
	cmd_metadata0->cmd_info[0].data_tg = in_idx;
	cmd_metadata0->cmd_info[0].addr_tg_step = 0;
	cmd_metadata0->cmd_info[0].data_tg_step = 16;  // 16 Byte
	cmd_metadata0->cmd_info[0].n_cmd = 8;
	
	// Change to AB-PIM Mode
	cmd_metadata0->cmd_info[1].op_code_tg = 0;  // RD
	cmd_metadata0->cmd_info[1].addr_tg = IDX_PIM_OP_MODE;
	cmd_metadata0->cmd_info[1].data_tg = 0;
	cmd_metadata0->cmd_info[1].addr_tg_step = 0;
	cmd_metadata0->cmd_info[1].data_tg_step = 0;
	cmd_metadata0->cmd_info[1].n_cmd = 1;

	// Read Weight 8 Col
	cmd_metadata0->cmd_info[2].op_code_tg = 0;  // RD
	cmd_metadata0->cmd_info[2].addr_tg = w_idx + 16;
	cmd_metadata0->cmd_info[2].data_tg = 0;
	cmd_metadata0->cmd_info[2].addr_tg_step = 1 << co_pos;
	cmd_metadata0->cmd_info[2].data_tg_step = 0;
	cmd_metadata0->cmd_info[2].n_cmd = 8;
	
	cmd_metadata1->n_iter = 1;
	cmd_metadata1->n_cmdgroup = 2;
	cmd_metadata1->operand[w_idx] = w_addr;
	cmd_metadata1->operand[out_idx] = out_addr;
	cmd_metadata1->data[in_idx] = in_addr;
	cmd_metadata1->input_size = 0;
		
	cmd_metadata1->code[0] = 0b01000000100000000000000000000000; // MOV     BANK       GRF_B[0]
	cmd_metadata1->code[1] = 0b00100000000000000000000000000001; // EXIT
	// Read SB → AB (Automatic in FPGA, ok)
	
	// Change to AB-PIM Mode
	cmd_metadata1->cmd_info[0].op_code_tg = 0;  // RD
	cmd_metadata1->cmd_info[0].addr_tg = IDX_PIM_OP_MODE;
	cmd_metadata1->cmd_info[0].data_tg = 0;
	cmd_metadata1->cmd_info[0].addr_tg_step = 0;
	cmd_metadata1->cmd_info[0].data_tg_step = 0;
	cmd_metadata1->cmd_info[0].n_cmd = 1;
	
	// Write Output 1 Col
	cmd_metadata1->cmd_info[1].op_code_tg = 1;  // WR
	cmd_metadata1->cmd_info[1].addr_tg = out_idx;
	cmd_metadata1->cmd_info[1].data_tg = 0;
	cmd_metadata1->cmd_info[1].addr_tg_step = 1 << co_pos;
	cmd_metadata1->cmd_info[1].data_tg_step = 0;
	cmd_metadata1->cmd_info[1].n_cmd = 1;

	return true;
}

bool pimblasLstmPreprocess(PIMKernel *micro_kernel, int len_in, int len_out, uint8_t **w, uint8_t **b) {
	return true;
}

bool pim_add(struct PimCmdMetadata cmd_metadata, int len, uint8_t *in0, uint8_t *in1, uint8_t *out) {
	CmdGenExecute(cmd_metadata);
	return true;
}

bool pim_mul(PIMKernel micro_kernel, int len, uint8_t *in0, uint8_t *in1, uint8_t *out) {
	return true;
}

bool pim_relu(PIMKernel micro_kernel, int len, uint8_t *in0, uint8_t *out) {
	return true;
}

bool pim_bn1d(PIMKernel micro_kernel, int len_batch, int len_feature, uint8_t *in, uint8_t *w_mul,
			  uint8_t *w_add, uint8_t *out) {
	return true;
}

bool pim_gemv(struct PimCmdMetadata cmd_metadata0, struct PimCmdMetadata cmd_metadata1, int len_in, int len_out, uint8_t *in, uint8_t *weight, uint8_t *out) {
	cmd_metadata0.input = (uint8_t *)calloc(len_in + 16, UNIT_SIZE);
	for (int i=0; i< len_in * UNIT_SIZE; i++) {
		cmd_metadata0.input[i + 16] = in[i];  // +16 for SRF_A (We use SRF_M!)	
	}
	cmd_metadata0.input_size = len_in * UNIT_SIZE;
	cmd_metadata1.input_size = 0;

	int code_iter = 2 * ((len_out + 4096 - 1) / 4096);
	for (int i=0; i< code_iter; i++) {
		CmdGenExecute(cmd_metadata0);
		CmdGenExecute(cmd_metadata1);
	}

	return true;
}

bool pim_lstm(PIMKernel micro_kernel, int len_in, int len_out, uint8_t *in, uint8_t *weight, uint8_t *bias, 
			  uint8_t *out) {
	return true;
}

uint8_t *Bn1dReshape(uint8_t *w, int l, int f) {
	uint8_t *w_ = (uint8_t*)calloc(l * f, UNIT_SIZE);

	for (int fi=0; fi<f; fi++)
		for (int li=0; li<l; li++)
			((uint16_t*)w_)[fi + li*f] = ((uint16_t*)w)[fi];

	return w_;	
}

uint8_t *GemvReshape(uint8_t *w, int m, int n) {
	int m_ = Ceiling(m, 8);
	int n_ = Ceiling(n, 4096);
	uint8_t *w_ = (uint8_t *)malloc(sizeof(uint16_t) * m_ * n_);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			((uint16_t *)w_)[i * m_ + j] = ((uint16_t *)w)[i * m + j];
		}
	}
	return w_;
}

uint8_t *LstmReshape(uint8_t *w, int m, int n) {
	int m_ = Ceiling(m, 8);
	int n_ = Ceiling(n, 4096);
	uint8_t *w_ = (uint8_t *)malloc(sizeof(uint16_t) * m_ * n_);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			((uint16_t *)w_)[i * m_ + j] = ((uint16_t *)w)[i * m + j];
		}
	}
	return w_;
}

uint8_t *Transpose(uint8_t *w, int m, int n) {
	m = Ceiling(m, 8);
	n = Ceiling(n, 4096);
	uint8_t *w_ = (uint8_t *)malloc(sizeof(uint16_t) * m * n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			((uint16_t *)w_)[j * n + i] = ((uint16_t *)w)[i * m + j];
		}
	}
	return w_;
}
