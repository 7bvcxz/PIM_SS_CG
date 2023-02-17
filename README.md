# PIM Software Stack
This is about `PIM Software Stack + Command Generator`

Contact me on [iESLAB GyungMo Kim](mailto:7bvcxz@gmail.com) for more information.

## 사용 설명서 (PIM_SS_CG + FPGA)
### 파일 위치
fpga_runtime.c

PIM_SS_CG 폴더

└─ fpga_runtime.h

└─ etc.
   
### fpga_runtime.c 수정 필요사항
1. #include "PIM_SS_CG/fpga_runtime.h" 추가
2. 아래 함수들 추가 (일단 pimExecution은 그대로 나두었음)
![image](https://user-images.githubusercontent.com/80901560/219535741-eecfb462-0c92-4aaa-a4be-f54c59699253.png)

### 빌드 및 실행방법
1. ./build_fpga.sh
2. ./test add 혹은 ./test gemv (bn, relu, lstm은 추후)
