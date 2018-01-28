#include<iostream>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include"Reg_def.h"

#define OP_JAL 111
#define OP_R 51

#define F3_ADD 0
#define F3_MUL 0

#define F7_MSE 1
#define F7_ADD 0

#define OP_I 19
#define F3_ADDI 0

#define OP_SW 35
#define F3_SB 0

#define OP_LW 3
#define F3_LB 0

#define OP_BEQ 99
#define F3_BEQ 0

#define OP_IW 27
#define F3_ADDIW 0

#define OP_RW 59
#define F3_ADDW 0
#define F7_ADDW 0


#define OP_SCALL 115
#define F3_SCALL 0
#define F7_SCALL 0

#define MAX 100000000

//主存
char memory[MAX]={0};
//寄存器堆
REG reg[32]={0};
//PC
long long PC=0;


//各个指令解析段
unsigned int OP=0;
unsigned int fuc3=0,fuc7=0;
int shamt=0;
int rs1=0,rs2=0,rd=0;
long long  imm12=0;
long long  imm20=0;
long long  imm7=0;
long long  imm5=0;

unsigned uint_mask = 0xffffffff;
long long ll_mask = -1;
unsigned long long slr_mask = -1;


//加载内存
void load_memory();

void simulate();

void IF();

void ID();

void EX();

void MEM();

void WB();


//符号扩展
int ext_signed(unsigned int src,int bit);

//获取指定位
unsigned int getbit(int s,int e);

unsigned int getbit(unsigned inst,int s,int e)
{
    int bits=(inst>>s);
    bits=bits&((2<<e-s)-1);
	return bits;
}

int ext_signed(unsigned int src,int bit)
{
    return 0;
}

struct ALU_{
    long long input1;
    long long input2;
    int  zero;
    long long out_put;
} ALU;

template <typename T>
long long  sign_ext(T a ,long long mask,unsigned int bit)
{
    if (a>>bit-1){
        a = a|(mask<<bit);
    }
    return a;
}
void LOG();
