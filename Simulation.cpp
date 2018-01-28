#include <elf.h>
#include "include/Simulation.h"
#include "stdlib.h"
#include "string.h"
#include "signal_def.h"

using namespace std;
extern void     read_elf();
extern unsigned int cadr;
extern unsigned int csize;
extern unsigned int vadr;
extern unsigned long long gp;
extern unsigned int madr;
extern unsigned int endPC;
extern FILE         *file;
extern Elf64_Ehdr   elf64_hdr;
Elf64_Phdr          phdr;
bool    sigle_step   = 0;
int     debug_adress = 0;
int     trace_size   = 0;
int     branch_harzard = 0;
int     freeze = 0;
//freeze IF/ID


int total_instructions       = 0;
int totol_cycles             = 0;
int total_loadUseHarzard     = 0;
int total_save_Fetch_Harzard = 0;
int total_ForwardA01         = 0;
int total_ForwardA10         = 0;
int total_ForwardB01         = 0;
int total_FOrwardB10         = 0;


char ForwardA = 00;
char ForwardB = 00;
char ForwardD = 00;
/*some masks*/
unsigned long long step;
//记录步数
void LOG()
{   // Log information
    system("clear");
    printf("step:        %d\n", step);
    printf("adress:      0x%x\n",PC_old);
    printf("instruction  0x%08x\n",IF_ID.inst);
    printf("zero  %x\n",reg[0]);
    printf("ra    %x\n",reg[1]);
    printf("sp    %x\n",reg[2]);
    printf("gp    %x\n",reg[3]);
    printf("tp    %x\n",reg[4]);
    printf("t0    %x\n",reg[5]);
    printf("t1    %x\n",reg[6]);
    printf("t2    %x\n",reg[7]);
    printf("s0/fp %x\n",reg[8]);
    printf("s1    %x\n",reg[9]);
    printf("a0    %x\n",reg[10]);
    printf("a1    %x\n",reg[11]);
    printf("a2    %x\n",reg[12]);
    printf("a3    %x\n",reg[13]);
    printf("a4    %x\n",reg[14]);
    printf("a5    %x\n",reg[15]);
    printf("a6    %x\n",reg[16]);
    printf("a7    %x\n",reg[17]);
    printf("s2    %x\n",reg[18]);
    printf("s3    %x\n",reg[19]);
    printf("s4    %x\n",reg[20]);
    printf("s5    %x\n",reg[21]);
    printf("s6    %x\n",reg[22]);
    printf("s7    %x\n",reg[23]);
    printf("s8    %x\n",reg[24]);
    printf("s9    %x\n",reg[25]);
    printf("s10   %x\n",reg[26]);
    printf("s11   %x\n",reg[27]);
    printf("t3    %x\n",reg[28]);
    printf("t4    %x\n",reg[29]);
    printf("t5    %x\n",reg[30]);
    printf("t6    %x\n",reg[31]);
    printf(" ");
//    PC_old = PC;
}

void load_use_hazard()
{
    if(ID_EX.Ctrl_M_MemRead && (ID_EX.Rd == ID_EX_old.Rs1) | ID_EX.Rd == ID_EX_old.Rs2)
    {
        ID_EX_old.bubble();
        freeze = 2;
        total_loadUseHarzard++;
    }
}
void  save_fetch_hazard()
{

    if (ID_EX.Ctrl_M_MemWrite&&ID_EX_old.Ctrl_M_MemRead && ID_EX_old.Rd == ID_EX.Rs2 )
    {
        ID_EX_old.bubble();
        freeze = 2;
        total_save_Fetch_Harzard++;
    }
}
//加载代码段
//初始化PC
void load_memory()
{
	for (long i =0;i < elf64_hdr.e_phnum;i++ )
    {
        fseek(file,elf64_hdr.e_phoff+i*elf64_hdr.e_phentsize,SEEK_SET);
        fread(&phdr,1,56,file);
        vadr  = phdr.p_vaddr;
        csize = phdr.p_filesz;
        cadr  = phdr.p_offset;
        if (phdr.p_type ==1 )//代表可加载段
        {
            fseek(file,cadr,SEEK_SET);
            fread(&memory[vadr],1,csize,file);
        }
    }
	fclose(file);

}

int main(int argc, char* argv[])
{
//
//    	if(argc < 2)
//	{
//		printf("Parameter missing!\n");
//		return 0;
//	}
//    if(! fopen(argv[1],"rb"))
//    {
//        printf("can't open file");
//        return 0;
//    };
//
//    if (argc ==4)
//    {
//        if(strcmp(argv[2],"-d")==0)
//        {
//            sigle_step = 1;
//            sscanf(argv[3],"%x",&debug_adress);
//        }
//
//    } else if (argc ==3)
//    {
//        if(strcmp(argv[2],"-d")==0)
//        {
//            sigle_step = 1;
//        }
//
//        if(strcmp(argv[2],"-r")==0)
//        {
//            char c;
//            freopen( "elf.txt", "r", stdin );
//            while ( scanf( "%c", &c )!=EOF )
//                printf( "%c", c );
//            return 0;
//        }
//    }



    file = fopen("/home/zhou/cnn","rb");
	read_elf();
	//加载内存
	load_memory();
	//设置入口地址
	PC=madr;
	//设置全局数据段地址寄存器
	reg[3]=gp;
	reg[2]=MAX/2;//栈基址 （sp寄存器）

	simulate();
	cout <<"simulate over!"<<endl;

	return 0;
}

void simulate()
{
	//结束PC的设置

	int end=(int)endPC;
	while(PC!=end)
	{
		//运行
		IF();
		ID();
		EX();
		MEM();
		WB();
		//更新中间寄存器
		IF_ID=IF_ID_old;
		ID_EX=ID_EX_old;
		EX_MEM=EX_MEM_old;
		MEM_WB=MEM_WB_old;
#ifdef Debug
        printf("IF_ID         \n");
        printf("IF_ID.PC        : 0x%x\n",IF_ID.PC);
        printf("IF_ID.inst      : 0x%x\n", IF_ID.inst );
        printf("______________________\n");
        printf("ID_EX           : \n");
        printf("ID_EX.PC        : 0x%x\n",ID_EX.PC);
        printf("ID_EX.inst      : 0x%x\n",ID_EX.inst);
        printf("ID_EX.rs1       : %d\n", ID_EX.Rs1);
        printf("ID_EX.REG_Rs1   : %d\n", ID_EX.Reg_Rs1);
        printf("ID_EX.rs2       : %d\n", ID_EX.Rs2);
        printf("ID_EX.REG_Rs2   : %d\n", ID_EX.Reg_Rs2);
        printf("ID_EX.Rd        : %d\n", ID_EX.Rd);
        printf("ID_EX.Imm       :  %d\n", ID_EX.Imm);
        printf("ID_EX.Ctrl_EX_ALUSrc   %d\n",ID_EX.Ctrl_EX_ALUSrc);
        printf("ID_EX.Ctrl_EX_ALUOp    %d\n",ID_EX.Ctrl_EX_ALUOp);
        printf("ID_EX.Ctrl_EX_RegDs1   %d\n",ID_EX.Ctrl_EX_RegDs1);
        printf("ID_EX.Ctrl_M_Memtype   %d\n",ID_EX.Ctrl_M_Memtype);
        printf("ID_EX.Ctrl_M_Branch    %d\n",ID_EX.Ctrl_M_Branch);
        printf("ID_EX.Ctrl_M_MemWrite  %d\n",ID_EX.Ctrl_M_MemWrite);
        printf("ID_EX.Ctrl_M_MemRead   %d\n",ID_EX.Ctrl_M_MemRead);
        printf("ID_EX.Ctrl_WB_RegWrite %d\n",ID_EX.Ctrl_WB_RegWrite);
        printf("ID_EX.Ctrl_WB_MemtoReg %d\n",ID_EX.Ctrl_WB_MemtoReg);
        printf("______________________\n");


        printf("EX_MEM\n");
        printf("EX_MEM.PC        : 0x%x\n",  EX_MEM.PC);
        printf("EX_MEM.inst      : 0x%x\n",EX_MEM.inst);

        printf("EX_MEM.rd        : %d\n", EX_MEM.Reg_dst);
        printf("EX_MEM.ALU_OUT   : %d\n", EX_MEM.ALU_out);
        printf("EX_MEM.REG_RS2   : %d\n", EX_MEM.Reg_Rs2);

        printf("EX_MEM.Ctrl_M_Branch    %d\n",EX_MEM.Ctrl_M_Branch);
        printf("EX_MEM.Ctrl_M_MemWrite  %d\n",EX_MEM.Ctrl_M_MemWrite);
        printf("EX_MEM.Ctrl_M_MemRead   %d\n",EX_MEM.Ctrl_M_MemRead);
        printf("EX_MEM.Ctrl_WB_RegWrite %d\n",EX_MEM.Ctrl_WB_RegWrite);
        printf("EX_MEM.Ctrl_WB_MemtoReg %d\n",EX_MEM.Ctrl_WB_MemtoReg);
        printf("______________________\n");

        printf("MEM_WB:\n");
        printf("MEM_WB.inst      :0x%x\n",MEM_WB.inst);

        printf("MEM_WB.rd        : %d\n", MEM_WB.Reg_dst);
        printf("MEM_WB.ALUout    : %d\n", MEM_WB.ALU_out);
        printf("MEM_WB.Ctrl_WB_RegWrite %d\n",MEM_WB.Ctrl_WB_RegWrite);
        printf("MEM_WB.Ctrl_WB_MemtoReg %d\n",MEM_WB.Ctrl_WB_MemtoReg);
        printf("MEM_WB.Memout           0x%x\n",MEM_WB.Mem_out);


        printf("\n");
//        LOG();
        printf("\n");
#endif
        step++;

        //debug steps
        if (sigle_step == 1 && debug_adress!=0){

            if (PC_old == debug_adress)
            {
                char dmadr_buff[20];
                int  dmadr;
//                LOG();
                printf("Print the memory address to check the memory data\n");
                try {
                    scanf("%s",&dmadr_buff);
                    sscanf(dmadr_buff,"%x",&dmadr);
                    printf("Memory data at %x is 0x%x\n",dmadr,*(unsigned int*)(&memory[dmadr]));
                }
                catch (char *string1){
                    printf("wrong adress");
                }

            }

        } else if (sigle_step == 1 && debug_adress ==0 )
        {
            int dmadr;
            if (trace == 0){
//                LOG();
                printf("Print the memory address to check the memory data print T and size to trace\n");
                scanf("%s",&dmadr_buff);
                if (strcmp(dmadr_buff,"T")!=0)
                {
                    sscanf(dmadr_buff,"%x",&dmadr);
                    printf("Memory data at 0x%x is 0x%x",dmadr,*(unsigned int*)(&memory[dmadr]));

                } else{
                    trace = 1;
                    scanf("%d",&trace_size);
                }
            } else{
                    printf("step:        %d\n", step);
                    for (int i=0;i<trace_size;i++)
                    {
                        printf("Memory data at 0x%x is 0x%x\n",dmadr+i*4,*(unsigned int*)(&memory[dmadr+i*4]));
                    }
            }
        }

//        if(step>1500)
//            exit(0);

        if(exit_flag==1)
            break;
        reg[0]=0;//一直为零

        totol_cycles++;


	}
}

//取指令
void IF()
{

	//write IF_ID_old
    if (freeze>0)

    {
        freeze --;

    }
    if (freeze >0)

        {

            IF_ID_old.inst = IF_ID.inst;
            IF_ID_old.PC   = IF_ID.PC;
            PC             = IF_ID_old.PC;
        }
    else
    {
        IF_ID_old.inst = *(unsigned int * )(&memory[PC]);
        IF_ID_old.PC   = PC;
        total_instructions++;
    }

}

//译码
void ID()
{
	//Read IF_ID
    reg[0]=0;
    if(MEM_WB.Ctrl_WB_RegWrite)
    {
        WB();
        // half cycle write,half cycle read;
    }


    unsigned int inst;
    if(freeze>0)
    {
        inst = ID_EX.inst;
        ID_EX_old.PC   = ID_EX.PC;
        ID_EX_old.inst = inst;
    } else
    {
        inst= IF_ID.inst;
        ID_EX_old.PC   = IF_ID.PC;
        ID_EX_old.inst = inst;
    }
    printf("%x\n",inst);


    OP   = getbit(inst,0,6);
	fuc3 = getbit(inst,12,14);
	if(OP==0x33)
	{

        fuc3= getbit(inst,12,14);
        fuc7= getbit(inst,25,31);
        rs1 = getbit(inst,15,19);
        rs2 = getbit(inst,20,24);

        ID_EX_old.Rd      = getbit(inst,7, 11);
        ID_EX_old.Reg_Rs1 = reg[rs1];
        ID_EX_old.Reg_Rs2 = reg[rs2];

        ID_EX_old.Rs1     = rs1;
        ID_EX_old.Rs2     = rs2;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 0; //0：第二个ALU的操作数来自寄存器堆的第二个输出
        ID_EX_old.Ctrl_M_Branch    = 0; //没有分支跳转
        ID_EX_old.Ctrl_M_MemRead   = 0;//不用读内存
        ID_EX_old.Ctrl_M_MemWrite  = 0; //    mem不可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1： 寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0： 写入寄存器来自ALU  1：写入寄存器来自memory



        PCsrc=0;    //PC=PC+4

		if(     fuc3 == 0x0 && fuc7==0x00)
		{
            ID_EX_old.Ctrl_EX_ALUOp=add;  //
		}
		else if(fuc3 == 0x0 && fuc7==0x01)
        {
            ID_EX_old.Ctrl_EX_ALUOp=mul;
		}
        else if(fuc3 == 0x0 && fuc7==0x20)
        {
            ID_EX_old.Ctrl_EX_ALUOp= sub;
        }
        else if(fuc3 == 0x1 && fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp = sll;
        }
        else if(fuc3 == 0x1 && fuc7==0x01)
        {
            ID_EX_old.Ctrl_EX_ALUOp = mulh;
        }
        else if(fuc3 == 0x2 && fuc7==0x00)
        {   ID_EX_old.Ctrl_EX_ALUOp = slt;
        }
        else if(fuc3 == 0x4 && fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp = Xor;
        }
        else if(fuc3 == 0x4 && fuc7==0x01)
        {
            ID_EX_old.Ctrl_EX_ALUOp = Div;
        }
        else if(fuc3 == 0x5 && fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp = srl;
        }
        else if(fuc3 == 0x5 && fuc7==0x20)
        {
            ID_EX_old.Ctrl_EX_ALUOp = sra;
        }
        else if(fuc3 == 0x6 && fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp = Or;
        }
        else if(fuc3 == 0x6 && fuc7==0x01)
        {
            ID_EX_old.Ctrl_EX_ALUOp = mod;
        }
        else if(fuc3 == 0x7 && fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp = And;
        }
        else{
            printf("instruction unsupported! %x",inst);
            exit(0);
        }


        //ok
	}
	else if(OP==0x03)
    {   //lb

        fuc3  = getbit(inst,12,14);
        imm12 = getbit(inst,20,31);
        //ALUop = add;


        ID_EX_old.Rd      = getbit(inst,7,11);
        ID_EX_old.Rs1     = getbit(inst,15,19);
        ID_EX_old.Rs2     = NULL;
        ID_EX_old.Reg_Rs1 = reg[ID_EX_old.Rs1];
        ID_EX_old.Imm     = sign_ext(imm12,ll_mask,12);

        ID_EX_old.Ctrl_EX_RegDs1   = 1;
        ID_EX_old.Ctrl_EX_ALUOp    = add;  //都是加的操作
        ID_EX_old.Ctrl_EX_ALUSrc   = 1;   //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 1; //memory can be read
        ID_EX_old.Ctrl_M_MemWrite  = 0;//1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1;//1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 1;//0：    写入寄存器来自ALU  1：写入寄存器来自memory


        switch (fuc3) {
            case 0:
            {   //lb
                ID_EX_old.Ctrl_M_Memtype = byte;
                break;
            }
            case 1:
            {   //lh
                ID_EX_old.Ctrl_M_Memtype = half;
                break;
            }
            case 2:
            {   //lw
                ID_EX_old.Ctrl_M_Memtype = word;
                break; //ok
            }
            case 3:
            {
                //ld
                ID_EX_old.Ctrl_M_Memtype = double_word;
                break;
            }
            case 4:
            {
                ID_EX_old.Ctrl_M_Memtype = ubyte;
                break;
            }
            case 5:
            {
                ID_EX_old.Ctrl_M_Memtype = uhalf;
                break;
            }
            default:
                printf("unsupported 0x3 instruction %x",inst);
                exit(0);
                //ok
        }
    }
    else if(OP==0x13)
    {
        fuc3         = getbit(inst,12,14);
        fuc7         = getbit(inst,25,31);
        rs1          = getbit(inst,15,19);
        imm12        = getbit(inst,20,31);
        imm5         = getbit(inst,20,24);

        ID_EX_old.Rd   = getbit(inst,7,11);
        ID_EX_old.Rs1  = rs1;
        ID_EX_old.Rs2  = NULL;

        ID_EX_old.Reg_Rs1          = reg[rs1];
        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory

        if(fuc3==0x0)
        {
            ID_EX_old.Ctrl_EX_ALUOp=add;
            ID_EX_old.Imm = sign_ext(imm12,ll_mask,12);//ok
        }
        else if(fuc3==0x1&&fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp=sll;
            ID_EX_old.Imm = sign_ext(imm5,ll_mask,5);

        }
        else if(fuc3==0x2)
        {
            ID_EX_old.Ctrl_EX_ALUOp=slt;
            ID_EX_old.Imm = sign_ext(imm12,ll_mask,12);
        }
        else if(fuc3==0x4)
        {
            ID_EX_old.Ctrl_EX_ALUOp=Xor;
            ID_EX_old.Imm = sign_ext(imm12,ll_mask,12);
        }
        else if(fuc3==0x5&&fuc7==0x00)
        {
            ID_EX_old.Ctrl_EX_ALUOp=srl;
            ID_EX_old.Imm = sign_ext(imm5,ll_mask,5);
        }
        else if(fuc3==0x5&&fuc7==0x20)
        {
            ID_EX_old.Ctrl_EX_ALUOp = sra;
            ID_EX_old.Imm = sign_ext(imm5,ll_mask,5);
        }
        else if(fuc3==0x6)
        {
            ID_EX_old.Ctrl_EX_ALUOp = Or;
            ID_EX_old.Imm = sign_ext(imm12,ll_mask,12);
        }
        else if(fuc3==0x7)
        {
            ID_EX_old.Ctrl_EX_ALUOp = And;
            ID_EX_old.Imm = sign_ext(imm12,ll_mask,12);
        }

            //ok

    }
    else if(OP==0x1B)
    {
        rs1          = getbit(inst,15,19);
        fuc3         = getbit(inst,12,14);
        fuc7         = getbit(inst,25,31);
        imm12        = getbit(inst,20,31);


        ID_EX_old.Rs1              = rs1;
        ID_EX_old.Rs2              = NULL;
        ID_EX_old.Rd               = getbit(inst,7, 11);
        ID_EX_old.Reg_Rs1          = reg[rs1];
        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory

        if(fuc3==0x0)
        {
            ID_EX_old.Ctrl_EX_ALUOp = addiw;
            ID_EX_old.Imm           = sign_ext(imm12,ll_mask,12);
            if(ID_EX_old.Imm == -5)
            {
                printf("test");
            }
        }
        else if(fuc3==0x1)
        {
            ID_EX_old.Ctrl_EX_ALUOp = slliw;
            ID_EX_old.Imm           = sign_ext(getbit(inst,20,24),ll_mask,5);
        }
        else if(fuc3==0x5&&fuc7==0)
        {
            ID_EX_old.Ctrl_EX_ALUOp = srliw;
            ID_EX_old.Imm           = sign_ext(getbit(inst,20,24),ll_mask,5);
        }
        else if(fuc3==0x5&&fuc7==0x20)
        {
            ID_EX_old.Ctrl_EX_ALUOp = sraiw;
            ID_EX_old.Imm           = sign_ext(getbit(inst,20,24),ll_mask,5);
        }
        else
        {
            printf("unsupported 0x1B instruction");
            exit(0);
        }

    }
    else if(OP==0x67)
    {
        rs1          = getbit(inst,15,19);
        fuc3         = getbit(inst,12,14);
        imm12        = getbit(inst,20,31);

        ID_EX_old.Rd               = getbit(inst,7,11);
        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 1;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory
        ID_EX_old.Rs1              = rs1;




        if(fuc3==0x0)
        {
            ID_EX_old.Ctrl_EX_ALUOp = jalr;
            ID_EX_old.Reg_Rs1       = reg[rs1];
            ID_EX_old.Imm           = sign_ext(imm12,ll_mask,12);
            ID_EX_old.Ctrl_EX_RegDs1= 1;
            ID_EX_old.Rs2           = NULL;

        }
        else
        {
           printf("unsupported 0x67 instruction");
            exit(0);
        }
    }
    else if(OP==0x23)
    {
        rs1 = getbit(inst,15,19);
        rs2 = getbit(inst,20,24);

        fuc3= getbit(inst,12,14);
        imm5= getbit(inst, 7,11);
        imm7= getbit(inst,25,31);


        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 1; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 0; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory

        ID_EX_old.Ctrl_EX_ALUOp = add;
        ID_EX_old.Reg_Rs1 = reg[rs1];
        ID_EX_old.Reg_Rs2 = reg[rs2];
        ID_EX_old.Rs1     = rs1;
        ID_EX_old.Rs2     = rs2;
        ID_EX_old.Rd      = NULL;
        ID_EX_old.Imm     = sign_ext((imm7<<5)+imm5,ll_mask,12);


        if      (fuc3 == 0x0)
        {
            ID_EX_old.Ctrl_M_Memtype = byte;
        }
        else if (fuc3 == 0x1)
        {
            ID_EX_old.Ctrl_M_Memtype = half;
        }
        else if (fuc3 == 0x2)
        {
            ID_EX_old.Ctrl_M_Memtype = word;
        }
        else if (fuc3 == 0x3 )
        {
            ID_EX_old.Ctrl_M_Memtype = double_word;
        }
        else{
            printf("unsupported 0x23 instuction");
            exit(0);
        }

    }
    else if(OP ==0x63)
    {
        rs1 = getbit(inst,15,19);
        rs2 = getbit(inst,20,24);

        fuc3= getbit(inst,12,14);
        imm5= getbit(inst, 7,11);
        imm7= getbit(inst,25,31);

        ID_EX_old.Reg_Rs1 = reg[rs1];
        ID_EX_old.Reg_Rs2 = reg[rs2];
        ID_EX_old.Imm     = sign_ext((imm5>>1<<1)|((imm5&1)<<11)|((imm7&0b111111)<<5)|((imm7>>6)<<12),ll_mask,13);
        ID_EX_old.Rs1     = rs1;
        ID_EX_old.Rs2     = rs2;
        ID_EX_old.Rd      = NULL;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 0; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 1;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 0; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory



        if      (fuc3 == 0x0)
        {
            ID_EX_old.Ctrl_EX_ALUOp = eq;

        }
        else if (fuc3 == 0x1)
        {
            ID_EX_old.Ctrl_EX_ALUOp = ne;
        }
        else if (fuc3 == 0x4)
        {
            ID_EX_old.Ctrl_EX_ALUOp = lt;
        }
        else if (fuc3 == 0x5)
        {
            ID_EX_old.Ctrl_EX_ALUOp = ge;
        }
        else if(fuc3 == 0x6)
        {
            ID_EX_old.Ctrl_EX_ALUOp = ltu;
        }
        else if (fuc3 ==0x7)
        {
            ID_EX_old.Ctrl_EX_ALUOp = geu;
        }
        else{
            printf("unsupported 0x63 instruction %x",inst);
            exit(0);
        }
    }
    else if(OP == 0x17)
    {
        ID_EX_old.Rd               = getbit(inst,7,11);
        ID_EX_old.Imm              = sign_ext((getbit(inst,12,31)<<12),ll_mask,32);
        ID_EX_old.Rs1              = NULL;
        ID_EX_old.Rs2              = NULL;
        ID_EX_old.Reg_Rs1          = IF_ID.PC;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory
        ID_EX_old.Ctrl_EX_ALUOp    = add;


    }
    else if(OP == 0x37)
    {

        ID_EX_old.Rd               = getbit(inst,7,11);
        ID_EX_old.Imm              = (getbit(inst,12,31)<<12);
        ID_EX_old.Reg_Rs1          = 0;
        ID_EX_old.Rs1              = NULL;
        ID_EX_old.Rs2              = NULL;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 0;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory
        ID_EX_old.Ctrl_EX_ALUOp    = add;


    }
    else if(OP==0X6f)
    {   //jal
        imm20         = getbit(inst,12,31);
        ID_EX_old.Rd  = getbit(inst,7,11);
        ID_EX_old.Imm = sign_ext((((imm20&0b11111111)<<12)|(((imm20>>8)&1)<<11)|(((imm20>>9)&1023)<<1)|(((imm20>>19)&1)<<20)),ll_mask,20);
        ID_EX_old.Rs1 = NULL;
        ID_EX_old.Rs2 = NULL;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //0:写寄存器的目标来自rs2, 1:写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 1; //0：第二个ALU的操作数来自寄存器堆的第二个输出 1：第二个操作数为指令立即数部分
        ID_EX_old.Ctrl_M_Branch    = 1;   //
        ID_EX_old.Ctrl_M_MemRead   = 0;
        ID_EX_old.Ctrl_M_MemWrite  = 0; //1：    mem可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1：    寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0：    写入寄存器来自ALU  1：写入寄存器来自memory
        ID_EX_old.Ctrl_EX_ALUOp    = jal;

    }
    else if(OP==0x3b)
    {

        rs1  = getbit(inst,15,19);
        rs2  = getbit(inst,20,24);
        fuc3 = getbit(inst,12,14);
        fuc7 = getbit(inst,25,31);

        ID_EX_old.Rd      = getbit(inst,7,11);
        ID_EX_old.Reg_Rs1 = reg[rs1];
        ID_EX_old.Reg_Rs2 = reg[rs2];
        ID_EX_old.Rs1     = rs1;
        ID_EX_old.Rs2     = rs2;

        ID_EX_old.Ctrl_EX_RegDs1   = 1; //写寄存器的目标来自rd
        ID_EX_old.Ctrl_EX_ALUSrc   = 0; //0：第二个ALU的操作数来自寄存器堆的第二个输出
        ID_EX_old.Ctrl_M_Branch    = 0; //没有分支跳转
        ID_EX_old.Ctrl_M_MemRead   = 0;//不用读内存
        ID_EX_old.Ctrl_M_MemWrite  = 0; //    mem不可写
        ID_EX_old.Ctrl_WB_RegWrite = 1; //1： 寄存器堆可写
        ID_EX_old.Ctrl_WB_MemtoReg = 0; //0： 写入寄存器来自ALU  1：写入寄存器来自memory

        if      (fuc3 == 0&&fuc7 == 0)
        {
            ID_EX_old.Ctrl_EX_ALUOp = addw;
        }
        else if (fuc3 == 0&&fuc7 == 0x20)
        {
            ID_EX_old.Ctrl_EX_ALUOp = subw;
        }
        else if (fuc3 == 0&&fuc7 == 1)
        {
            ID_EX_old.Ctrl_EX_ALUOp = mulw;
        }
        else if(fuc3 == 4&&fuc7 == 1)
        {
            ID_EX_old.Ctrl_EX_ALUOp = divw;
        }
        else{
            printf("unsupported 0x3b instruction %x",inst);
            exit(0);
        }
    } else if (OP == 0 )
    {
        if(step>0 )
        {
            printf("main function ends ,exit\n");
            printf("total instructions %d\n",total_instructions);
            printf("total cycles       %d\n",totol_cycles);
            printf("load_use hazards   %d\n",total_loadUseHarzard);
            printf("save_fetch hazards %d\n",total_save_Fetch_Harzard);
            printf("branch_harzards    %d\n",branch_harzard);
            printf("ForwardA01 %d\n",total_ForwardA01);
            printf("ForwardA10 %d\n",total_ForwardA10);
            printf("ForwardB01 %d\n",total_ForwardB01);
            printf("ForwardB10 %d\n",total_FOrwardB10);
            exit(0);
        }
    }
    else{

        printf("unsupported instruction %x",inst);
        exit(0);
    }


    if (IF_ID.bubble)
    {
//        ID_EX_old.bubble();
        IF_ID.bubble = 0;
        ID_EX_old.bubble();
        IF_ID_old.bubble = 0;
    }
    // detect load_use_harzard;
    load_use_hazard();
    save_fetch_hazard();



}
//执行
void EX()
{




    //read ID_EX
    //reset Forward
    ForwardA = 0x00;
    ForwardB = 0x00;
    ForwardD = 0x00;

    long long  temp_PC =ID_EX.PC ;
    char RegDst        =ID_EX.Ctrl_EX_RegDs1;
    char ALUOp         =ID_EX.Ctrl_EX_ALUOp;

    if(EX_MEM.Ctrl_WB_RegWrite && EX_MEM.Reg_dst!=0 && EX_MEM.Reg_dst == ID_EX.Rs1 &&  EX_MEM.Reg_dst !=NULL)
        ForwardA = 0x10;
    if(EX_MEM.Ctrl_WB_RegWrite && EX_MEM.Reg_dst!=0 && EX_MEM.Reg_dst == ID_EX.Rs2 &&  EX_MEM.Reg_dst !=NULL)
    {
        if(ID_EX.Ctrl_M_MemWrite)
        {
            ForwardD = 1;
            // sw hazard


        } else
        {
            ForwardB = 0x10;
        }
    }

    if(MEM_WB.Ctrl_WB_RegWrite && MEM_WB.Reg_dst!=0 && !(EX_MEM.Ctrl_WB_RegWrite&& EX_MEM.Reg_dst!=0&& (EX_MEM.Reg_dst == ID_EX.Rs1) &&ID_EX.Rs1!=NULL) &&MEM_WB.Reg_dst == ID_EX.Rs1 &&  MEM_WB.Reg_dst !=NULL)
        ForwardA = 0x01;
    if(MEM_WB.Ctrl_WB_RegWrite && MEM_WB.Reg_dst!=0 && !(EX_MEM.Ctrl_WB_RegWrite&& EX_MEM.Reg_dst!=0&& (EX_MEM.Reg_dst == ID_EX.Rs2) &&ID_EX.Rs2!=NULL) &&MEM_WB.Reg_dst == ID_EX.Rs2 &&  MEM_WB.Reg_dst !=NULL)
        ForwardB = 0x01;




	//choose ALU input number

    switch (ForwardA)
    {
        case  0x00:
        {
            ALU.input1 = ID_EX.Reg_Rs1;
            break;
        }
        case 0x10:
        {
            if(freeze==0)
            {
                total_ForwardA10++;
            }
            ALU.input1 = EX_MEM.ALU_out;
           // printf("forward A10 : 0x%d -> %d\n",EX_MEM.ALU_out,ID_EX.Rs1);
            break;
        }
        case 0x01:
        {
            if(freeze==0)
            {
                total_ForwardA01++;
            }
            if (MEM_WB.Memread)
            {
                ALU.input1 = MEM_WB.Mem_out;
            } else
            {
                ALU.input1 = MEM_WB.ALU_out;
            }
           // printf("forward A01 : 0x%d -> %d\n",ALU.input1,ID_EX.Rs1);
            break;
        }
        default: ;

    }

    switch (ForwardB)
    {
        case 0x00:
        {
            if (ID_EX.Ctrl_EX_ALUSrc)
            {
                ALU.input2 = ID_EX.Imm;//ALU的第二个输入来自立即数
            }
            else
            {
                ALU.input2 = ID_EX.Reg_Rs2;//ALU的第二个输入来自寄存器
            }
            break;
        }
        case 0x10:
        {
            if(freeze==0)
            {
                total_FOrwardB10++;
            }
            ALU.input2 = EX_MEM.ALU_out;
            //printf("forward B10 : 0x%d -> %d\n",EX_MEM.ALU_out,ID_EX.Rs2);
            break;
        }
        case 0x01:
        {

            total_ForwardB01++;
            if(!ID_EX.Ctrl_EX_ALUSrc)
            {
                if (MEM_WB.Memread)
                {
                    ALU.input2 = MEM_WB.Mem_out;
                } else
                {
                    ALU.input2 = MEM_WB.ALU_out;
                }
                //printf("forward B01 : 0x%d -> %d\n",ALU.input2,ID_EX.Rs2);
            } else
            {
                ALU.input2  =  ID_EX.Imm;

                if (MEM_WB.Memread)
                {
                    ID_EX.Reg_Rs2 = MEM_WB.Mem_out;
                } else
                {
                    ID_EX.Reg_Rs2 = MEM_WB.ALU_out;
                }

                //printf("forward B01 : 0x%d -> %d\n",MEM_WB.Mem_out,ID_EX.Rs2);
            }
            break;
        }
        default:;

    }



    if(ALUOp == 2|ALUOp==5)
    {
        printf("test mul");
        totol_cycles++;
    }
    if(ALUOp == 26)
    {
        printf("test div");
        totol_cycles+=39;
    }
	//...
	//alu calculate

	switch(ALUOp){
        case 0:
            //add
            ALU.out_put = ALU.input1+ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 1:
            //sub
            ALU.out_put = ALU.input1-ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 2:
            //mul
            AlBll       = (ALU.input1&uint_mask)*(ALU.input2&uint_mask)&uint_mask;
            AlBlh       = (ALU.input1&uint_mask)*(ALU.input2&uint_mask)>>32;
            AhBll       = (ALU.input1>>32)*(ALU.input2&uint_mask)&uint_mask;
            AhBlh       = (ALU.input1>>32)*(ALU.input2&uint_mask)>>32;
            ALU.out_put = AlBll+(((AhBll+AlBhl+AlBlh)&uint_mask)<<32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 3:
            //sll
            ALU.out_put = ALU.input1<<ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 4:
            //srl
            ALU.out_put = (ALU.input1>>ALU.input2)&(slr_mask<<ALU.input2);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 5:
            //mulh
            AlBlh       = ((ALU.input1&(uint_mask))*(ALU.input2&uint_mask))>>32;
            AhBll       = ((ALU.input2&(uint_mask))*(ALU.input1>>32)&(uint_mask));
            AhBlh       = (ALU.input1>>32)*(ALU.input2&uint_mask)>>32;
            AlBhl       = (ALU.input2>>32)*(ALU.input1&uint_mask)&uint_mask;
            AhBh        = (ALU.input2>>32)*(ALU.input1>>32);
            ALU.out_put = ((AlBlh+AhBll+AlBhl)>>32)+AhBlh+AhBh;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 6:
            //slt
            ALU.out_put = ALU.input1 < ALU.input2?1:0;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 7:
            //xor
            ALU.out_put = ALU.input1 ^ ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 8:
            //div
            ALU.out_put = ALU.input1 / ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 9:
            //or
            ALU.out_put = ALU.input1 | ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 10:
            //mod
            ALU.out_put = ALU.input1 % ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 11:
            //and
            ALU.out_put = ALU.input1 & ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 12:
            //sra
            ALU.out_put = ALU.input1 >>ALU.input2;
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 13:
            //addiw
            ALU.out_put = sign_ext((ALU.input1+ALU.input2)&0xffffffff,ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 14:
            //jalr
//            jump_addr   = ;
            ALU.out_put = ID_EX.PC+4;
            temp_PC     = ((ALU.input1+ALU.input2)&(ll_mask-1));
            ALU.zero    = 1;

            break;
        case 15:
            //eq
            ALU.out_put = ALU.input1 == ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;

            break;
        case 16:
            //ne
            ALU.out_put = ALU.input1 != ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;

            break;
        case 17:
            //lt
            ALU.out_put = ALU.input1 < ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;

            break;
        case 27:
            //ltu
            ALU.out_put = (unsigned long long) ALU.input1 < (unsigned long long )ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;
            break;

        case 28:
            //geu
            ALU.out_put = (unsigned long long) ALU.input1 > (unsigned long long )ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;
            break;


        case 18:
            //ge
            ALU.out_put = ALU.input1 >= ALU.input2;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero = ALU.out_put ==0?0:1;

            break;
        case 19:
            //jal
            ALU.out_put = ID_EX.PC+4;
            temp_PC     = temp_PC+(ID_EX.Imm&ll_mask-1);
            ALU.zero    = 1;
            break;
        case 20:
            //addw
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)+(ALU.input2&0xffffffff)),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 21:
            //subw
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)-(ALU.input2&0xffffffff)),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 22:
            //mulw
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)*(ALU.input2&0xffffffff)),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 23:
            //slliw
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)<<ALU.input2),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 24:
            //srliw
            ALU.out_put = sign_ext((((ALU.input1&0xffffffff)>>ALU.input2)&(slr_mask<<ALU.input2)),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;

            break;
        case 25:
            //sraiw
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)>>ALU.input2),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;
            break;
        case 26:
            ALU.out_put = sign_ext(((ALU.input1&0xffffffff)/ALU.input2&0xffffffff),ll_mask,32);
            ALU.zero = ALU.out_put ==0?1:0;
            break;
        default:
            printf("unsupported calculation!");
            exit(0);
	}

    //choose reg dst address

	if(RegDst)
	{
        EX_MEM_old.Reg_dst = ID_EX.Rd;
	}
	else
	{
        EX_MEM_old.Reg_dst = ID_EX.Rs2;
	}


	//write EX_MEM_old
	EX_MEM_old.ALU_out          = ALU.out_put;
	EX_MEM_old.PC               = temp_PC;
    EX_MEM_old.Zero             = ALU.zero;
    EX_MEM_old.Reg_dst          = ID_EX.Rd;
    EX_MEM_old.Reg_Rs2          = ID_EX.Reg_Rs2;
    EX_MEM_old.Ctrl_M_Branch    = ID_EX.Ctrl_M_Branch;
    EX_MEM_old.Ctrl_M_MemRead   = ID_EX.Ctrl_M_MemRead;
    EX_MEM_old.Ctrl_M_MemWrite  = ID_EX.Ctrl_M_MemWrite;
    EX_MEM_old.Ctrl_WB_MemtoReg = ID_EX.Ctrl_WB_MemtoReg;
    EX_MEM_old.Ctrl_WB_RegWrite = ID_EX.Ctrl_WB_RegWrite;
    EX_MEM_old.Ctrl_M_Memtype   = ID_EX.Ctrl_M_Memtype;
    EX_MEM_old.inst             = ID_EX.inst;

    if(ForwardD)
    {
        EX_MEM_old.Reg_Rs2  = EX_MEM.ALU_out;
       // printf("forwardD : 0x%d -> %d\n",EX_MEM_old.Rs2,EX_MEM_old.Reg_Rs2);
    }

}
//访问存储器
void MEM()
{

    MEM_WB_old.Memread = 0;
    //INITIAL SIGINAL


	//complete Branch instruction PC change
    PCsrc = EX_MEM.Ctrl_M_Branch&&EX_MEM.Zero;


    if(PCsrc){
        PC = EX_MEM.PC;
        IF_ID_old.bubble = 1;
        ID_EX_old.bubble();
        EX_MEM_old.bubble();
        freeze = 0;
        branch_harzard++;

    } else{
        PC+=4;
    }

    if (EX_MEM.Ctrl_M_MemRead)
    {
        //Memory can be read
        switch (EX_MEM.Ctrl_M_Memtype)
        {
            case 0:
            {   Memory_out  = *(unsigned char *)&memory[EX_MEM.ALU_out];
                sign_ext(Memory_out,ll_mask,8);
                break;
            }
            case 1:
            {   Memory_out  = *(unsigned short *)&memory[EX_MEM.ALU_out];
                sign_ext(Memory_out,ll_mask,16);
                break;
            }
            case 2:
            {   Memory_out  = *(unsigned int *)&memory[EX_MEM.ALU_out];
                sign_ext(Memory_out,ll_mask,32);
                break;

            }
            case 3:
            {    Memory_out = *(unsigned long long *)&memory[EX_MEM.ALU_out];
                break;
            }
            case 4:
            {
                Memory_out = *(unsigned char*)&memory[EX_MEM.ALU_out];
//                (Memory_out,ll_mask,8);
                break;
            }
            case 5:
            {
                Memory_out = *(unsigned short*)&memory[EX_MEM.ALU_out];
                break;
            }
            default:;
        }

        MEM_WB_old.Mem_out = Memory_out;
        MEM_WB_old.Memread = 1;
    }
    if (EX_MEM.Ctrl_M_MemWrite)
    {   //Memory can be written



        switch (EX_MEM.Ctrl_M_Memtype)
        {

            case 0:
            {
                *(unsigned char *)&memory[EX_MEM.ALU_out]      = EX_MEM.Reg_Rs2&0xff;
                break;
            }
            case 1:
            {
                *(unsigned short *)&memory[EX_MEM.ALU_out]     = EX_MEM.Reg_Rs2&0xffff;
                break;
            }
            case 2:
            {
                *(unsigned int *)&memory[EX_MEM.ALU_out]       = EX_MEM.Reg_Rs2&0xffffffff;
                break;
            }
            case 3:
            {
                *(unsigned long long *)&memory[EX_MEM.ALU_out] = EX_MEM.Reg_Rs2;
                break;
            }

            default:;
        }
    }

	//write MEM_WB_old
    MEM_WB_old.Ctrl_WB_RegWrite = EX_MEM.Ctrl_WB_RegWrite;
    MEM_WB_old.Ctrl_WB_MemtoReg = EX_MEM.Ctrl_WB_MemtoReg;
    MEM_WB_old.ALU_out          = EX_MEM.ALU_out;
    MEM_WB_old.Reg_dst          = EX_MEM.Reg_dst;
    MEM_WB_old.inst             = EX_MEM.inst;
    MEM_WB_old.PC               = EX_MEM.PC;
}



//写回
void WB()
{
	//read MEM_WB

    if(MEM_WB.Ctrl_WB_RegWrite){
        if (!MEM_WB.Ctrl_WB_MemtoReg)
            //写入数据来自ALU
        {
            reg[MEM_WB.Reg_dst] = MEM_WB.ALU_out;
        }
        else{
            //写入数据来自Memory
            reg[MEM_WB.Reg_dst] = MEM_WB.Mem_out;

        }
        //write reg
    }


#ifdef Debug
    system("clear");
    printf("step:        %d\n", step);
    printf("adress:      0x%x\n",MEM_WB.PC);
    printf("instruction  0x%08x\n",MEM_WB.inst);
    printf("zero  %x\n",reg[0]);
    printf("ra    %x\n",reg[1]);0x
    printf("sp    %x\n",reg[2]);
    printf("gp    %x\n",reg[3]);
    printf("tp    %x\n",reg[4]);
    printf("t0    %x\n",reg[5]);
    printf("t1    %x\n",reg[6]);
    printf("t2    %x\n",reg[7]);
    printf("s0/fp %x\n",reg[8]);
    printf("s1    %x\n",reg[9]);
    printf("a0    %x\n",reg[10]);
    printf("a1    %x\n",reg[11]);
    printf("a2    %x\n",reg[12]);
    printf("a3    %x\n",reg[13]);
    printf("a4    %x\n",reg[14]);
    printf("a5    %x\n",reg[15]);
    printf("a6    %x\n",reg[16]);
    printf("a7    %x\n",reg[17]);
    printf("s2    %x\n",reg[18]);
    printf("s3    %x\n",reg[19]);
    printf("s4    %x\n",reg[20]);
    printf("s5    %x\n",reg[21]);
    printf("s6    %x\n",reg[22]);
    printf("s7    %x\n",reg[23]);
    printf("s8    %x\n",reg[24]);
    printf("s9    %x\n",reg[25]);
    printf("s10   %x\n",reg[26]);
    printf("s11   %x\n",reg[27]);
    printf("t3    %x\n",reg[28]);
    printf("t4    %x\n",reg[29]);
    printf("t5    %x\n",reg[30]);
    printf("t6    %x\n",reg[31]);
    printf(" ");
    PC_old = MEM_WB.PC;
#endif
}

