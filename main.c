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
int tem_reg[32] = {0, };
int reg[32] = {0, };
int in_mem[65537];   // 0 - 0x00010000(65536)
int Da_mem[65537];   // 0x10000000(268435456) - 0x10010000 
int PC = 0x00000000;
int instru = 0;
unsigned int Checksum = 0x00000000;
int j = 1;
int chg = 0;
int bn = 0;
int cycle;

void i_mem(int rt, int rs, int im_1, int opnum){      //mem

  switch(opnum){
    case 0x20:  //addi
      tem_reg[rt] = reg[rt];
      reg[rt] = reg[rs] + im_1;
      break;
    case 0x30:   //andi
      tem_reg[rt] = reg[rt];
      reg[rt] = reg[rs] & im_1;
      break;
    case 0x34:    //ori
      tem_reg[rt] = reg[rt];
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

void iformat(unsigned int a, int opnum){     // rs, rt , im   ->   rt  rs  im
  int rs = ((a & rsCo) >> 21);
  int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);


  data[j] = rt;
  if( j != 1 && data[j-1] == rs ){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
    if(chg == 1){
      Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
      chg = 0;
      cycle--;
    }
  }
  else if(j != 2 && data[j-2] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
  }
  else{
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    if(chg == 1){
      Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs];
      chg = 0;
      cycle--;
    }
  }

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
  PC=PC+4;
}


void bformat(unsigned int a, int opnum){     // rs, rt , im   ->   rs  rt  im
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  chg = 0;
  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  
  if(j!=1 && data[j-1] == rt){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    data[j] = 0 ;
    j++;
    cycle--;
  }
  else if(j!=1 && data[j-1] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    data[j] = 0 ;
    j++;
    data[j] = 0 ;
    j++;
    cycle--;
    cycle--;
  }
  else{
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
  }

  if(opnum == 0x10){  // beq
    if(reg[rs] == reg[rt]){
      PC = PC + 4 + im_1 *4;
      Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
      cycle--;
      return;
    }
  }
  else if(opnum == 0x14){  // bne
    if(reg[rs] != reg[rt]){
      PC = PC + 4 + im_1 *4;
      Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
      cycle--;
      return;
    }
  }
  PC=PC+4;
}

void luiformat(unsigned int a){
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);

  data[j] = rt ;
  Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
  
  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  im_1 = im_1 << 16;

  tem_reg[rt] = reg[rt];
  reg[rt] = im_1; 

  PC=PC+4;
}


void lsiformat(unsigned int a, int opnum){  // rt  im_1(rs)
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);

  data[j] = rt;
  if( j != 1 && data[j-1] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
  }
  else if(j != 2 && data[j-2] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
  }
  else{
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
  }

  if(im_1 != im_2)
    im_1 = im_2 - min_im;
  
  if(opnum == 0x8c){        //lw
    im_2 = reg[rs] - mem_int + im_1;
    tem_reg[rt] = reg[rt];
    reg[rt] = Da_mem[im_2];
    chg = 1;
  }
  else if(opnum == 0xac){   //sw
    im_2 = reg[rs] - mem_int + im_1;
    Da_mem[im_2] = reg[rt];
  }
  PC=PC+4;
}

void jformat(unsigned int a){
  int rt_1 = (a & JC_1);
  int rt_2 = (a & JC_2);

  Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;

  if( rt_1 != rt_2)
    rt_1 = rt_2 - min_j;
  data[j]=0;
  j++;
  PC = rt_1 *4;
}


void rformat(unsigned int a, int opnum){    // rs  rt  rd   -->  rd  rs  rt
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  unsigned int rd = ((a & rdCo) >> 11);

  data[j] = rd;
  if( j != 1 && data[j-1] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
    if(chg == 1){
      Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
      chg = 0;
    }
  }
  else if(j != 2 && data[j-2] == rs){
    Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
    if(chg == 1){
      Checksum = (Checksum << 1 | Checksum >> 31) ^ tem_reg[rs];
      chg = 0;
    }
  }
  else{
    Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs] ;
    if(chg == 1){
      Checksum = (Checksum << 1 | Checksum >> 31) ^ reg[rs];
      chg = 0;
    }
  }


  switch (opnum)
    {
    case 0x20:  // add
      tem_reg[rd] = reg[rd];
      reg[rd] = reg[rs] + reg[rt];
      break;
    case 0x24:  // and
      tem_reg[rd] = reg[rd];
      reg[rd] = reg[rs] & reg[rt];
      break;
    case 0x25:  // or    
      tem_reg[rd] = reg[rd];
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
      tem_reg[rd] = reg[rd];
      reg[rd] = reg[rs] - reg[rt];
      break;    
    default:
      break;
    }
  PC=PC+4;
}

void findop(unsigned int a){    // ID part
  unsigned int opnum;

  if(a == 0){ //nop
    Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
    chg = 0;
    bn = 0;
    PC=PC+4;
    return;
  }
  opnum = ((a & Icom) >> 24);
  if(opnum == 0){
    opnum = (a & Rcom);
    rformat(a, opnum);
  }
  else{        // i format
    if(opnum == 0x08)  //j
      jformat(a);
    else if(opnum == 0x3c)  // lui
      luiformat(a);
    else if(opnum == 0x10 || opnum == 0x14)  // beq, bne
      bformat(a, opnum);
    else if(opnum == 0x8c || opnum == 0xac)  // lw, sw
      lsiformat(a, opnum);
    else if(opnum == 0x20 || opnum == 0x30 || opnum == 0x34 || opnum == 0x28)
      iformat(a, opnum);
    else{
      Checksum = (Checksum << 1 | Checksum >> 31) ^ 0;
      chg = 0;
      bn = 0;
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
  cycle = atoi(argv[2]);
  buff = argv[1];
  file = fopen(buff, "rb");


  memset(in_mem, -1, sizeof(in_mem));
  memset(Da_mem, -1, sizeof(Da_mem));

  if(file==NULL){
    printf( "파일을 읽는데 실패했습니다\n");
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
    if(cycle == 1)
      break;
    big_e = in_mem[PC];
    cycle --;
    findop(big_e);       // ID stage
    j++;
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
  return 0;
}