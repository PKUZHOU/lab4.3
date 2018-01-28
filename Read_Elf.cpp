#include <Read_Elf.h>
#include "iostream"
#include "string.h"
#define _GLIBCXX_USE_CXX11_ABI 0
#include<stdio.h>
#include<string.h>
typedef struct{
    unsigned char b[8];
}int64;

typedef struct{
    unsigned char b[4];
}int32;

typedef struct{
    unsigned char b[2];
}int16;

typedef struct{
    unsigned char b;
}int8;

typedef long  Elf64_Addr;
typedef long  Elf64_Off;
typedef long  Elf64_Xword;
typedef long  Elf64_Sxword;
typedef int   Elf64_Word;
typedef int   Elf64_Sword;
typedef short Elf64_Half;


#define	EI_CLASS        4
#define	EI_DATA         5
#define	EI_VERSION      6
#define	EI_OSABI        7
#define	EI_ABIVERSION   8
#define	EI_PAD          9
#define	EI_NIDENT       16

#define	SHN_UNDEF  0
#define	SHN_LOPROC 0xFF00
#define	SHN_HIPROC 0xFF1F
#define	SHN_LOOS   0xFF20
#define	SHN_HIOS   0xFF3F
#define	SHN_ABS    0xFFF1
#define	SHN_COMMON 0xFFF2


typedef struct
{
    unsigned char   e_ident[16]; /* ELF identification */
    Elf64_Half      e_type; /* Object file type */
    Elf64_Half      e_machine; /* Machine type */
    Elf64_Word      e_version; /* Object file version */
    Elf64_Addr      e_entry; /* Entry point address */
    Elf64_Off       e_phoff; /* Program header offset */
    Elf64_Off       e_shoff; /* Section header offset */
    Elf64_Word      e_flags; /* Processor-specific flags */
    Elf64_Half      e_ehsize; /* ELF header size */
    Elf64_Half      e_phentsize; /* Size of program header entry */
    Elf64_Half      e_phnum; /* Number of program header entries */
    Elf64_Half      e_shentsize; /* Size of section header entry */
    Elf64_Half      e_shnum; /* Number of section header entries */
    Elf64_Half      e_shstrndx; /* Section name string table index */
} Elf64_Ehdr;

typedef struct
{
    Elf64_Word      sh_name; /* Section name */
    Elf64_Word      sh_type; /* Section type */
    Elf64_Xword     sh_flags; /* Section attributes */
    Elf64_Addr      sh_addr; /* Virtual address in memory */
    Elf64_Off       sh_offset; /* Offset in file */
    Elf64_Xword     sh_size; /* Size of section */
    Elf64_Word      sh_link; /* Link to other section */
    Elf64_Word      sh_info; /* Miscellaneous information */
    Elf64_Xword     sh_addralign; /* Address alignment boundary */
    Elf64_Xword     sh_entsize; /* Size of entries, if section has table */
} Elf64_Shdr;

typedef struct
{
    Elf64_Word      st_name; /* Symbol name */
    unsigned char   st_info; /* Type and Binding attributes */
    unsigned char   st_other; /* Reserved */
    Elf64_Half      st_shndx; /* Section table index */
    Elf64_Addr      st_value; /* Symbol value */
    Elf64_Xword     st_size; /* Size of object (e.g., common) */
} Elf64_Sym;


typedef struct
{
    Elf64_Word      p_type; /* Type of segment */
    Elf64_Word      p_flags; /* Segment attributes */
    Elf64_Off       p_offset; /* Offset in file */
    Elf64_Addr      p_vaddr; /* Virtual address in memory */
    Elf64_Addr      p_paddr; /* Reserved */
    Elf64_Xword     p_filesz; /* Size of segment in file */
    Elf64_Xword     p_memsz; /* Size of segment in memory */
    Elf64_Xword     p_align; /* Alignment of segment */
} Elf64_Phdr;


void read_elf();
void read_Elf_header();
void read_elf_sections();
void read_symtable();
void read_Phdr();


//代码段在解释文件中的偏移地址
unsigned int cadr=0;

//代码段的长度
unsigned int csize=0;

//代码段在内存中的虚拟地址
unsigned int vadr=0;

//全局数据段在内存的地址
unsigned long long gp=0;

//main函数在内存中地址
unsigned int madr=0;

//程序结束时的PC
unsigned int endPC=0;

//程序的入口地址
unsigned int entry=0;

FILE *file=NULL;



FILE *elf;
Elf64_Ehdr elf64_hdr;
using namespace std;
//Program headers
unsigned int padr=0;
unsigned int psize=0;
unsigned int pnum=0;

//Section Headers
unsigned int sadr=0;
unsigned int ssize=0;
unsigned int snum=0;
unsigned int symb_off=0;

//Symbol table
unsigned int symnum=0;
unsigned int symadr=0;
unsigned int symsize=0;
int shstrtab_adr;
int shstrtab_size;

//用于指示 包含节名称的字符串是第几个节（从零开始计数）
unsigned int index2=0;

//字符串表在文件中地址，其内容包括.symtab和.debug节中的符号表
unsigned int stradr=0;
int stradr_size = 0;


bool open_file()
{   if (file == NULL)
        return false;
    else
	    return true;
}

void read_elf()
{
    elf = fopen("elf.txt","w");
	if(!open_file()) {
        printf("file isn't opened\n");
        return;
    }

    read_Elf_header();
    read_elf_sections();
    read_Phdr();
    read_symtable();
    fclose(elf);
}

void read_Elf_header()
{   fprintf(elf,"ELF Header\n");
	//file should be relocated
	fread(&elf64_hdr,1,sizeof(elf64_hdr),file);

	fprintf(elf," magic number:  ");
    for (int i=0;i<16;i++)
    {
        fprintf(elf,"%x ",elf64_hdr.e_ident[i]);
    }
    entry = elf64_hdr.e_entry;
    fprintf(elf,"\n");

	fprintf(elf," Class:                                ELFCLASS32\n");
	
	fprintf(elf," Data:                                 little-endian\n");
		
	fprintf(elf," Version:                              %x\n",elf64_hdr.e_version);

	fprintf(elf," OS/ABI:	                            System V ABI\n");
	
	fprintf(elf," ABI Version:                          EXEC  \n");
	
	fprintf(elf," Type:                                 EXEC\n");
	
	fprintf(elf," Machine:                              RSCV\n");

	fprintf(elf," Entry point address:                  0x%x\n",elf64_hdr.e_entry);

	fprintf(elf," Start of program headers:             %d bytes into  file\n",elf64_hdr.e_phoff);

	fprintf(elf," Start of section headers:             %d bytes into  file\n",elf64_hdr.e_shoff);

	fprintf(elf," Flags:  0x%x\n",elf64_hdr.e_flags);

	fprintf(elf," Size of this header:                  %d Bytes\n",elf64_hdr.e_ehsize);

	fprintf(elf," Size of program headers:              %d Bytes\n",elf64_hdr.e_phentsize);

	fprintf(elf," Number of program headers:            %d \n",elf64_hdr.e_phnum);

	fprintf(elf," Size of section headers:              %d  Bytes\n",elf64_hdr.e_shentsize);

	fprintf(elf," Number of section headers:            %d \n",elf64_hdr.e_shnum);

	fprintf(elf," Section header string table index:    %d \n",elf64_hdr.e_shstrndx);
    fprintf(elf,"\n");
}

void read_elf_sections()
{

	Elf64_Shdr elf64_shdr;
    snum = elf64_hdr.e_shnum;
//    fseek(file,elf64_hdr.e_shoff+snum*sizeof(elf64_shdr),SEEK_SET);
//    fread(&elf64_shdr,1,sizeof(elf64_shdr),file);
    fseek(file,elf64_hdr.e_shoff+(snum-1)*sizeof(elf64_shdr),SEEK_SET);
    fread(&elf64_shdr,1,sizeof(elf64_shdr),file);
    shstrtab_adr = elf64_shdr.sh_offset;
    shstrtab_size = elf64_shdr.sh_size;

    char S[shstrtab_size] = {0};
    fseek(file,shstrtab_adr,SEEK_SET);
    fread(S,1,shstrtab_size,file);
    char name[40] = {0};

    fprintf(elf,"Section Headers\n ");
	for(int c=0;c<snum;c++)
	{
		fprintf(elf," [%3d]",c);
		
		//file should be relocated
        fseek(file,elf64_hdr.e_shoff+c*sizeof(elf64_shdr),SEEK_SET);
		fread(&elf64_shdr,1,sizeof(elf64_shdr),file);
        for (int i=0;;i++)
        {
            name[i] = S[(int)elf64_shdr.sh_name+i];

            if (name[i] ==0)
            {
                break;
            }
            //read the name
        }


		fprintf(elf," Type:   0x %.8x",elf64_shdr.sh_type);

		fprintf(elf," Address:0x %.8x",elf64_shdr.sh_addr);

		fprintf(elf," Offest: 0x %.8x",elf64_shdr.sh_offset);

		fprintf(elf," Size:   0x %.8x\n",elf64_shdr.sh_size);

		fprintf(elf," Entsize:0x %.8x",elf64_shdr.sh_entsize);

		fprintf(elf," Flags:  0x %.8x",elf64_shdr.sh_flags);
		
		fprintf(elf," Link:   0x %d  ",elf64_shdr.sh_link);

		fprintf(elf," Info:        %d",elf64_shdr.sh_info);

		fprintf(elf," Align:       %x",elf64_shdr.sh_addralign);
        fprintf(elf," Name:    %s\n  ",name);
        if (elf64_shdr.sh_name ==1)
        {   Elf64_Sym elf64_sym;
            symb_off = elf64_shdr.sh_offset+elf64_shdr.sh_entsize;
            int size = sizeof(elf64_sym);
            symnum = (elf64_shdr.sh_size-elf64_shdr.sh_entsize)/sizeof(elf64_sym);
            size = sizeof(elf64_sym);
        }
        if (elf64_shdr.sh_name ==9)
        {
            stradr = elf64_shdr.sh_offset;
            stradr_size = elf64_shdr.sh_size;
        }
        if(elf64_shdr.sh_name == 17)
        {
            shstrtab_adr = elf64_shdr.sh_offset;
            shstrtab_size = elf64_shdr.sh_size;
        }

 	}


}

void read_Phdr()
{
	Elf64_Phdr elf64_phdr;
    pnum = elf64_hdr.e_phnum;
    //number of program header
    fprintf(elf,"\nPhdr                \n");
	for(int c=0;c<pnum;c++)
	{
		fprintf(elf," [%3d]\n",c);

        fseek(file,elf64_hdr.e_phoff+c*sizeof(elf64_phdr),SEEK_SET);
		fread(&elf64_phdr,1,sizeof(elf64_phdr),file);

		fprintf(elf," Type:   ");
		
		fprintf(elf," Flags:   ");
		
		fprintf(elf," Offset:   0x %x ",elf64_phdr.p_offset);
		
		fprintf(elf," VirtAddr: 0x %x",elf64_phdr.p_vaddr);
		
		fprintf(elf," PhysAddr: 0x %x",elf64_phdr.p_paddr);

		fprintf(elf," FileSiz:  0x %x",elf64_phdr.p_filesz);

		fprintf(elf," MemSiz:   0x %x",elf64_phdr.p_memsz);
		
		fprintf(elf," Align:    0x %x\n",elf64_phdr.p_align);
	}
}


void read_symtable()
{
	Elf64_Sym elf64_sym;
    char S[stradr_size] = {0};
    fseek(file,stradr,SEEK_SET);
    fread(S,1,stradr_size,file);
    int EndPC_flag = 0;
    fprintf(elf,"\nSymbol Tabel\n");
    char name[400] = {0};

    for(int c=0;c<symnum;c++)
	{

//        if (c == 366)
//            c = 366;
		fprintf(elf," [%3d]   ",c);

		fseek(file,(c)*sizeof(elf64_sym)+symb_off,SEEK_SET);
		fread(&elf64_sym,1,sizeof(elf64_sym),file);
        for (int i=0;;i++)
        {
            name[i] = S[(int)elf64_sym.st_name+i];

            if (name[i] ==0)
            {
                break;
            }
            //read the name
        }
        if (EndPC_flag)
        {
            endPC = elf64_sym.st_value;
            EndPC_flag = 0;
        }
        if (strcmp(name,"main")==0)
        {
            madr = elf64_sym.st_value;
            EndPC_flag = 1;

        }
        if (strcmp(name,"__global_pointer$")==0)
        {
            gp = elf64_sym.st_value;
        }



		fprintf(elf," Bind:  0x%.3x",elf64_sym.st_info);

		fprintf(elf," Type:  0x%.3x",elf64_sym.st_other);

		fprintf(elf," NDX:   0x%.8x",elf64_sym.st_shndx);

		fprintf(elf," Size:  %.3d",elf64_sym.st_size);

		fprintf(elf," Value: 0x%.8x",elf64_sym.st_value);
        fprintf(elf," Name:  %s\n",name);

	}

}


