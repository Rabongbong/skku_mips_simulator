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
#define imCo 0x0000ffff
#define rdCo 0x0000f800
#define saCo 0x000007c0
#define ImC1 0x0000ffff
#define ImC2 0x00007fff
#define JC_1 0x03ffffff
#define JC_2 0x01ffffff
#define min_im 32768     // two's complement(2^15) 
#define min_j 33554432   // two's complement(2^25) 

void iformat(unsigned int a){     // rs, rt , im   ->   rt   rs  im
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  
  if( im_1 != im_2)
    im_1 = im_2 - min_im;
  
  printf("$%d, $%d, %d\n", rt, rs, im_1); 
}

void bformat(unsigned int a){     // rs, rt , im   ->   rs  rt  im
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  
  if( im_1 != im_2)
    im_1 = im_2 - min_im;
  
  printf("$%d, $%d, %d\n", rs, rt, im_1); 
}
void luiformat(unsigned int a){
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  
  if( im_1 != im_2)
    im_1 = im_2 - min_im;
  
  printf("$%d, %d\n", rt, im_1); 
}

void lsiformat(unsigned int a){

  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  int im_1 = (a & ImC1);
  int im_2 = (a & ImC2);
  
  if( im_1 != im_2)
    im_1 = im_2 - min_im;
  
  printf("$%d, %d($%d)\n", rt, im_1, rs);  
}

void jformat(unsigned int a){
  int rt_1 = (a & JC_1);
  int rt_2 = (a & JC_2);
  
  if( rt_1 != rt_2)
    rt_1 = rt_2 - min_j;
  
  printf("%d\n", rt_1);
}

void rformat(unsigned int a){    // rs  rt  rd   -->  rd  rs  rt
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  unsigned int rd = ((a & rdCo) >> 11);


  printf("$%d, $%d, $%d\n", rd, rs, rt); 
}

void rdmformat(unsigned int a){
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  printf("$%d, $%d\n", rs, rt);  
}

void rsformat(unsigned int a){
  unsigned int rt = ((a & rtCo) >> 16);
  unsigned int rd = ((a & rdCo) >> 11);
  unsigned int sa = ((a & saCo) >> 6);
  printf("$%d, $%d, %d\n", rd, rt, sa); 
}

void rsvformat(unsigned int a){    //  rs  rt  rd   -->  rd  rt  rs
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rt = ((a & rtCo) >> 16);
  unsigned int rd = ((a & rdCo) >> 11);
  printf("$%d, $%d, $%d\n", rd, rt, rs);
}

void jalrformat(unsigned int a){ // jalr
  unsigned int rs = ((a & rsCo) >> 21);
  unsigned int rd = ((a & rdCo) >> 11);
  printf("$%d, $%d\n", rd, rs);
}

void jrmtrformat(unsigned int a){ // jr, mthi, mtlo
  unsigned int rs = ((a & rsCo) >> 21);
  printf("$%d\n", rs);
}

void mfrformat(unsigned int a){  // mfhi, mflo
  unsigned int rd = ((a & rdCo) >> 11);
  printf("$%d\n", rd);
} 


void findop(unsigned int a){
  unsigned int opnum;
  opnum = ((a & Icom) >> 24);
  
  if(opnum == 0){
    opnum = (a & Rcom);
    switch (opnum)
    {
    case 0x20:  // add
      printf("add ");
      rformat(a);
      break;
    case 0x21:  // addu
      printf("addu ");
      rformat(a);
      break;
    case 0x24:  // and
      printf("and ");
      rformat(a);
      break;
    case 0x27:  // nor
      printf("nor ");
      rformat(a);
      break;
    case 0x25:  // or
      printf("or ");      
      rformat(a);
      break;
    case 0x2a:  // slt
      printf("slt ");
      rformat(a);
      break;
    case 0x29:  // sltu
      printf("sltu ");
      rformat(a);
      break;
    case 0x22:  // sub
      printf("sub ");
      rformat(a);
      break;
    case 0x23:  // subu
      printf("subu ");
      rformat(a);
      break;
    case 0x26:  // xor
      printf("xor ");
      rformat(a);
      break;
    case 0xc:  //syscall
      printf("syscall\n");
      break;
    case 0x1a:  //div
      printf("div ");
      rdmformat(a);      
      break;
    case 0x1b:  //divu
      printf("divu ");
      rdmformat(a);
      break;
    case 0x18:  //mult
      printf("mult ");
      rdmformat(a);
      break;
    case 0x19:  //multu
      printf("multu ");
      rdmformat(a);
      break;
    case 0:  //sll
      printf("sll ");
      rsformat(a);
      break;
    case 3:  //sra
      printf("sra ");
      rsformat(a);
      break;
    case 2:  //srl
      printf("srl ");
      rsformat(a);
      break;
    case 4:  //sllv
      printf("sllv ");
      rsvformat(a);
      break;
    case 6:  //srlv
      printf("srlv ");
      rsvformat(a);
      break;  
    case 7:  //srav
      printf("srav ");
      rsvformat(a);
      break;
    case 0x10:  //mfhi
      printf("mfhi ");
      mfrformat(a);
      break;
    case 0x12:  //mflo
      printf("mflo ");
      mfrformat(a);
      break;
    case 8:  //jr
      printf("jr ");      
      jrmtrformat(a);
      break;
    case 9:  //jr
      printf("jalr ");      
      jalrformat(a);
      break;
    case 0x11:  //mthi
      printf("mthi ");
      jrmtrformat(a);
      break;
    case 0x13:  //mtlo
      printf("mtlo ");
      jrmtrformat(a);
      break;            
    default:
      printf("unknown instruction\n");
      break;
    }
  }
  else{        // i format
    switch(opnum){
      case 0x20:   //addi
        printf("addi ");
        iformat(a);
        break;
      case 0x24:   // addiu
        printf("addiu ") ;
        iformat(a);
        break;
      case 0x30:   //andi
        printf("andi ");
        iformat(a);
        break;
      case 0x10:  // beq
        printf("beq ");
        bformat(a);
        break;
      case 0x14:  //bne
        printf("bne ");
        bformat(a);
        break;
      case 0x80:   //lb
        printf("lb ");
        lsiformat(a);
        break;
      case 0x90:   //lbu
        printf("lbu ");
        lsiformat(a);
        break;
      case 0x84:   //lh
        printf("lh ");
        lsiformat(a);
        break;
      case 0x94:   //lhu
        printf("lhu ");
        lsiformat(a);
        break;
      case 0x3c:   //lui
        printf("lui ");
        luiformat(a);
        break;
      case 0x8c:   //lw
        printf("lw ");
        lsiformat(a);
        break;
      case 0x34:    //ori
        printf("ori ");
        iformat(a);
        break;
      case 0xa0:   //sb
        printf("sb ");
        lsiformat(a);
        break;
      case 0x28:   //slti
        printf("slti ");
        iformat(a);
        break;
      case 0x2c:   //sltiu
        printf("slti ");
        iformat(a);
        break;
      case 0xa4:   //sh
        printf("sh ");
        lsiformat(a);
        break;
      case 0xac:   //sw
        printf("sw ");
        lsiformat(a);
        break;
      case 0x38:   //xori
        printf("xori ");
        iformat(a);
        break;
      case 0x08:   //j
        printf("j ");
        jformat(a);
        break;
      case 0x0c:   //jal
        printf("jal ");
        jformat(a);
        break;
      default:
        printf("unknown instruction\n");
        break;
    }
  }
}

unsigned int little_to_big(unsigned int a){
  unsigned int big;
  big = ((a & Com1) >> 24) + ((a & Com2) >> 8) + ((a & Com3) << 8) + ((a & Com4) << 24);
  return big;
}

int main(int argc, char * argv[]){

  char *buff;
  char * a;
  FILE * file;
  unsigned int little_e;
  unsigned int big_e;
  int count=0;
  buff = argv[1];
  a = strtok(buff," ");
  file = fopen(buff, "rb");

  if(file==NULL){
    printf( "파일을 읽는데 실패했습니다 \n");\
    return 0;
  }
  while(!feof(file)){
    fread(&little_e, 4, 1, file);
    if(feof(file))
			break;
		printf("inst %d: ", count++);
    big_e=little_to_big(little_e);
    printf("%08x ", big_e);
    findop(big_e);
  }
  return 0;
}