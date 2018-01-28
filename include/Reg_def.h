typedef  long long REG;

struct IFID{

	unsigned int inst;
	long long PC;
    int bubble = 0;




}IF_ID,IF_ID_old;


struct IDEX{

	int Rd,Rs1,Rs2 = NULL;
    unsigned int inst;
	long long PC;
	long long Imm;

	REG Reg_Rs1,Reg_Rs2;

	char Ctrl_EX_ALUSrc  = 0;
	char Ctrl_EX_ALUOp   = 0;
	char Ctrl_EX_RegDs1  = 0;
    char Ctrl_M_Memtype  = 0;
	char Ctrl_M_Branch   = 0;
	char Ctrl_M_MemWrite = 0;
	char Ctrl_M_MemRead  = 0;

	char Ctrl_WB_RegWrite =0;
	char Ctrl_WB_MemtoReg =0;

    void bubble()
    {
         Ctrl_EX_ALUSrc  = 0;
         Ctrl_EX_ALUOp   = 0;
         Ctrl_EX_RegDs1  = 0;
         Ctrl_M_Memtype  = 0;
         Ctrl_M_Branch   = 0;
         Ctrl_M_MemWrite = 0;
         Ctrl_M_MemRead  = 0;
         Ctrl_WB_RegWrite =0;
         Ctrl_WB_MemtoReg =0;
    }

}ID_EX,ID_EX_old;

struct EXMEM{


	long long  PC;
    unsigned int inst;
	int Reg_dst = NULL;
	REG ALU_out;
	int Zero = 0;
	REG Reg_Rs2;
    int Rs2;

	char Ctrl_M_Branch   = 0;
	char Ctrl_M_MemWrite = 0;
	char Ctrl_M_MemRead  = 0;
    char Ctrl_M_Memtype  = 0;
	char Ctrl_WB_RegWrite= 0;
	char Ctrl_WB_MemtoReg= 0;

    void bubble()
    {
         Ctrl_M_Branch   = 0;
         Ctrl_M_MemWrite = 0;
         Ctrl_M_MemRead  = 0;
         Ctrl_M_Memtype  = 0;
         Ctrl_WB_RegWrite= 0;
         Ctrl_WB_MemtoReg= 0;
    }

}EX_MEM,EX_MEM_old;

struct MEMWB{
    unsigned int inst;
	long long  Mem_out;
    int PC;
    int Memread = 0;
	REG ALU_out;
	int Reg_dst;
		
	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}MEM_WB,MEM_WB_old;