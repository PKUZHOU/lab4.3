//
// Created by zhou on 17-11-20.
//

#ifndef RISCV_SIMULATOR_SIGNAL_DEF_H_H
#define RISCV_SIMULATOR_SIGNAL_DEF_H_H

#endif //RISCV_SIMULATOR_SIGNAL_DEF_H_H
//ALUops :
char add    = 0;
char sub    = 1;
char mul    = 2;
char sll    = 3;
char srl    = 4;
char mulh   = 5;
char slt    = 6;
char Xor    = 7;
char Div    = 8;
char Or     = 9;
char mod    = 10;
char And    = 11;
char sra    = 12;
char addiw  = 13;
char jalr   = 14;
char eq     = 15;
char ne     = 16;
char lt     = 17;
char ge     = 18;
char jal    = 19;
char addw   = 20;
char subw   = 21;
char mulw   = 22;
char slliw  = 23;
char srliw  = 24;
char sraiw  = 25;
char divw   = 26;
char ltu    = 27;
char geu    = 28;
long long inst_num=0;

//muti cache used for mul and mulh
long long AlBlh;
long long AlBll;
long long AhBll;
long long AhBlh;
long long AlBhl;
long long AhBh;
//long long jump_addr;/**/
long long reg_out1;
long long reg_out2;
unsigned long PC_old;
//immdeiate num
long long  imm;

//control signals
int Reg_Dst;
int trace = 0;
char RegDst,ALUSrc,PCsrc = 0;
char Branch,MemRead,MemWrite,MemW_type;
char RegWrite,MemtoReg;

long long Memory_out;

//系统调用退出指示
int exit_flag=0;


//memory read type
char byte = 0;
char half = 1;
char word = 2;
char double_word = 3;
char ubyte = 4;
char uhalf = 5;
char dmadr_buff[20];