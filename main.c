#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define MAX 100
#define Com1 0xff000000 
#define Com2 0x00ff0000 
#define Com3 0x0000ff00 
#define Com4 0x000000ff 
#define Icom 0xfc000000 
#define Rcom 0x0000003F 
#define rsCo 0x03e00000 
#define rtCo 0x001f0000 
#define rdCo 0x0000f800 
#define ImC1 0x0000ffff 
#define ImC2 0x00007fff 
#define JC_1 0x03ffffff 
#define JC_2 0x01ffffff 
#define min_im 32768       // two's complement(2^15) 
#define min_j 33554432     // two's complement(2^25) 
#define mem_int 268435456  // 0x10000000


int data[1000] = {0, };

int reg[32] = {0, };
int in_mem[65537];   // 0 - 0x00010000(65536)
int Da_mem[65537];   // 0x10000000(268435456) - 0x10010000 
int PC = 0x00000000;
int instru = 0;
unsigned int Checksum = 0x00000000;
int i = 1;


void i_mem(unsigned int rt, unsigned int rs, int im_1, int opnum){ //mem

  switch(opnum){
    case 0x20:  //addi
      reg[rt] = reg[rs] + im_1;
      break;
    case 0x30:   //andi
      reg[rt] = reg[rs] & im_1;
      break;
    case 0x34:    //ori
      reg[rt] = reg[rs] | im_1;
      break;
    case 0x28:   //slti
      if(reg[rs] < im_1)
        reg[rt]=1;
      else{
        reg[rt]=0;
      }
      break;
  }
}




void iformat(unsigned int a, int opnum, int cycle){     // rs, rt , im   ->   rt  rs  im
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);

  printf("i\n");
  if(cycle == 0)
    return;

  data[i] = rt;
  if(i!=1 && data[i-1] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
    printf("Checksum: 0x%08x\n", Checksum);
  }
  else{
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    printf("Checksum: 0x%08x\n", Checksum);
  }

  printf("reg[rs] 0x%08x\n", reg[rs]);
  

  printf("[%d] EX\n", i);
  switch(opnum){
    case 0x20:  //addi
      if(im_1 != im_2)
        im_1 = im_2 - min_im;
      i_mem(rt, rs, im_1, opnum);
      break;
    case 0x30:   //andi
      i_mem(rt, rs, im_1, opnum);
      break;
    case 0x34:    //ori
      i_mem(rt, rs, im_1, opnum);
      break;
    case 0x28:   //slti
      i_mem(rt, rs, im_1, opnum);
      break;
  }
  cycle--;
  PC=PC+4;
}


void bformat(unsigned int a, int opnum, int cycle){     // rs, rt , im   ->   rs  rt  im
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  if(cycle ==0)
    return;
  
  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  
  if(opnum == 0x10){  // beq
    if(reg[rs] == reg[rt]){
      cycle--;
      PC = PC + 4 + im_1 *4;
      return;
    }
  }
  else if(opnum == 0x14){  // bne
    if(reg[rs] != reg[rt]){
      cycle--;
      PC = PC + 4 + im_1 *4;
      return;
    }
  }

  Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs];
  printf("Checksum: 0x%08x\n", Checksum); 
  PC=PC+4;
}

void luiformat(unsigned int a, int cycle){
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);

  if(cycle == 0)
    return;

  data[i] = rt;

  Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
  printf("Checksum: 0x%08x\n", Checksum);

  

  printf("[%d] EX\n", i);
  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  im_1 = im_1 << 16;
  reg[rt] = im_1; 


  cycle--;
  PC=PC+4;
}


void lsiformat(unsigned int a, int opnum, int cycle){  // rt  im_1(rs)
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs];
  printf("Checksum: 0x%08x\n", Checksum); 
  if(cycle ==0)
    return;
  
  printf("[%d] EX\n", i);
  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  
  if(opnum == 0x8c){        //lw
    im_2 = reg[rs] - mem_int + im_1;
    reg[rt] = Da_mem[im_2];
  }
  else if(opnum == 0xac){   //sw
    im_2 = reg[rs] - mem_int + im_1;
    Da_mem[im_2] = reg[rt];
  }
  PC=PC+4;
  cycle--;  
}


void jformat(unsigned int a, int cycle){
  int rt_1 = (a & JC_1);
  int rt_2 = (a & JC_2);
  printf("[%d] EX\n", i);
  if(cycle ==0)
    return;

  if( rt_1 != rt_2)
    rt_1 = rt_2 - min_j;
  
  cycle--;
  PC = rt_1 *4;
}


void rformat(unsigned int a, int opnum, int cycle){    // rs  rt  rd   -->  rd  rs  rt
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  unsigned int rd = ((a & rdCo) >> 11);

  Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs];

  printf("Checksum: 0x%08x\n", Checksum);
  
  printf("r");
  if(cycle ==0)
    return;
  printf("[%d] EX\n", i);
  switch (opnum)
    {
    case 0x20:  // add
      reg[rd] = reg[rs] + reg[rt];
      break;
    case 0x24:  // and
      reg[rd] = reg[rs] & reg[rt];
      break;
    case 0x25:  // or    
      reg[rd] = reg[rs] | reg[rt];
      break;
    case 0x2a:  // slt
      if( reg[rs] < reg[rt])
        reg[rd]=1;
      else{
        reg[rd]=0;
      }
      break;
    case 0x22:  // sub
      reg[rd] = reg[rs] - reg[rt];
      break;    
    default:
      break;
    }
  cycle--;
  PC=PC+4;
}

void findop(unsigned int a, int cycle){    // ID part
  unsigned int opnum;

  if(a == 0){ //nop
    Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
    PC=PC+4;
    cycle--;
    return;
  }
  if(cycle ==0)
    return;

  opnum = ((a & Icom) >> 24);
  if(opnum == 0){
    opnum = (a & Rcom);
    rformat(a, opnum, cycle);
  }
  else{        // i format
    if(opnum == 0x08)  //j
      jformat(a, cycle);
    else if(opnum == 0x3c)  // lui
      luiformat(a, cycle);
    else if(opnum == 0x10 || opnum == 0x14)  // beq, bne
      bformat(a, opnum, cycle);
    else if(opnum == 0x8c || opnum == 0xac)  // lw, sw
      lsiformat(a, opnum, cycle);
    else if(opnum == 0x20 || opnum == 0x30 || opnum == 0x34 || opnum == 0x28)
      iformat(a, opnum, cycle);
    else{
      Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
      cycle--;
      PC = PC+4;
    }
  }
}


unsigned int little_to_big(unsigned int a){
  unsigned int big;
  big = ((a & Com1) >> 24) + ((a & Com2) >> 8) + ((a & Com3) << 8) + ((a & Com4) << 24);
  return big;
}


int main(int argc, char * argv[]){  // 1. bin file 2. cycle Number 3. reg or mem  4. 시작 메모리  5. 갯수

  char *buff;
  FILE * file;
  char * n;
  int a=0;
  int start=0; 
  int num;
  unsigned int little_e;
  unsigned int big_e;
  int cycle = atoi(argv[2]);
  buff = argv[1];
  file = fopen(buff, "rb");


  memset(in_mem, -1, sizeof(in_mem));
  memset(Da_mem, -1, sizeof(Da_mem));

  if(file==NULL){
    printf( "파일을 읽는데 실패했습니다 \n");\
    return 0;
  }

  if(argc == 3)  // 입력값이 두 개 일 때
    return 0; 


  while(!feof(file)){  // 먼저 instruction을 순서대로 복사한다.
    fread(&little_e, 4, 1, file);
    if(feof(file))
			break;
    big_e=little_to_big(little_e);
    in_mem[instru] = big_e;
    instru = instru + 4;
  }


  while(1){               // IF stage
    if(cycle == 0)
      break;
    big_e = in_mem[PC];
    cycle--;
    printf("0x%08x\n", big_e);
    printf("[%d] IF\n", i);     // 
    findop(big_e, cycle);       // ID stage
    i++;
  }


  printf("Checksum: 0x%08x\n", Checksum); 

  if(!strcmp(argv[3], "reg")){   //reg 출력
    for(int i = 0; i < 32; i++){
    printf("$%d: 0x%08x\n", i, reg[i]);
    }
    printf("PC: 0x%08x\n", PC);
  }
  else if(!strcmp(argv[3], "mem")){    //mem 출력
    n = (argv[4]);
    for(int k = 0; k<5; k++){   // argv[4]로 받아오는 16진수를 int형으로 바꿔야 한다.
      if(n[k] !='0'){ 
        start += (n[9-k]-'0') * pow(16, (k));
      }
    }      
    num = atoi(argv[5]);
    for(int i=0; i <num; i++){
      printf("0x%08x\n", Da_mem[start +4 * i]);
    }
  }
  else {
    printf("unknown instruction\n");
  }

  return 0;
}