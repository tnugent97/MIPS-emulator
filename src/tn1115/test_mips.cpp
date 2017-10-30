#include "mips.h"
#include <iostream>
#include <string>
#include <bitset>

using namespace std;

void rInstructionGen(uint8_t shiftAmt, string function, uint8_t instr[]);
int testInstructionR(uint32_t value1, uint32_t value2, uint32_t finValue, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
int testInstructionI(uint32_t value1, uint32_t value2, uint32_t finValue, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
int testRegisters(mips_cpu_h cpu, mips_error &error);
uint32_t getPC(mips_cpu_h state);
void iInstructionGen(string function, uint16_t immediate, uint8_t instr[]);
int testStoreInstruction(uint32_t value, uint16_t offset, string func, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
void jInstructionGen(string function, uint32_t address, uint8_t instr[]);
int testInstructionJ(string function, uint32_t address, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
int testBranchInstruction(string function, uint32_t address, uint32_t value, bool branch, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
int testMultDiv(uint32_t value1, uint32_t value2, uint32_t expHI, uint32_t expLO, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);
int testMTHILO(string function, uint32_t value, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError);

int main(){

  mips_mem_h mem = mips_mem_create_ram(8192);
  mips_cpu_h cpu = mips_cpu_create(mem);

  uint32_t pc = 0;
  uint8_t instr[4];
  uint32_t pcJump = 12 >> 2;
  int testPassed, testId;
  mips_error expError = mips_Success;
  mips_error error;

  // INITIALISE INSTRUCTIONS TO 0 TO AVOID ERRORS
  for(int i = 0; i<4;i++){
    instr[i] = 0;
  }

  //BEGIN TESTS
  mips_test_begin_suite();

  //GET INITIAL VALUE OF PC
  testId = mips_test_begin_test("<INTERNAL>");
  error = mips_cpu_get_pc(cpu, &pc);
  if((error != mips_Success) || (pc != 0)){
    testPassed = 0;
  }
  else{
    testPassed = 1;
  }
  mips_test_end_test(testId,testPassed,0);

  //BEGIN REGISTER TESTS
  testId = mips_test_begin_test("<INTERNAL>");
  testPassed = testRegisters(cpu,error);
  //char* msg = new_char[error.length() + 1];
  //strcpy(msg, error.c_str());
  mips_test_end_test(testId,testPassed,0);

  //BEGIN OTHER TESTS
  //SLL
  testId = mips_test_begin_test("SLL");
  rInstructionGen(1,"SLL",instr);
  testPassed = testInstructionR(0,2,4,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //CHECK PC HAS BEEN INCREASED
  testId = mips_test_begin_test("<INTERNAL>");
  error = mips_cpu_get_pc(cpu, &pc);
  if((error != mips_Success) || (pc != 4)){
    testPassed = 0;
  }
  else{
    testPassed = 1;
  }
  mips_test_end_test(testId,testPassed,0);

  //SRL
  testId = mips_test_begin_test("SRL");
  rInstructionGen(1,"SRL",instr);
  testPassed = testInstructionR(0,4,2,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SRA
  testId = mips_test_begin_test("SRA");
  rInstructionGen(1,"SRA",instr);
  testPassed = testInstructionR(0,0xFFFFFFFF,0xFFFFFFFF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SRA");
  rInstructionGen(1,"SRA",instr);
  testPassed = testInstructionR(0,0x0FFFFFFF,0x07FFFFFF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ADDU
  testId = mips_test_begin_test("ADDU");
  rInstructionGen(0,"ADDU",instr);
  testPassed = testInstructionR(27,654,681,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ADD
  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(27,654,681,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(1791095845,4282876139,6073971984,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  expError = mips_ExceptionArithmeticOverflow;
  testPassed = testInstructionR(799981516,1666063943,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  error = mips_cpu_get_pc(cpu, &pc);
  error = mips_cpu_set_pc(cpu,pc+4);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(1904615676,878115723,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  expError = mips_Success;
  error = mips_cpu_get_pc(cpu, &pc);
  error = mips_cpu_set_pc(cpu,pc+4);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(-369530300,-369530300,-739060600,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(-847150073,4158651785,-983465584,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADD");
  rInstructionGen(0,"ADD",instr);
  testPassed = testInstructionR(3715233664,-530904575,3184329089,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ADDIU
  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",1,instr);
  testPassed = testInstructionI(2,0,3,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",60977,instr);
  testPassed = testInstructionI(4114104213,0,-180867642,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",57058,instr);
  testPassed = testInstructionI(1204497775,0,1204489297,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",65177,instr);
  testPassed = testInstructionI(443352346,0,443351987,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",48835,instr);
  testPassed = testInstructionI(-392579319,0,-392596020,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDIU");
  iInstructionGen("ADDIU",60797,instr);
  testPassed = testInstructionI(1235985687,0,1235980948,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ADDI
  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",1,instr);
  testPassed = testInstructionI(2,0,3,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",60977,instr);
  testPassed = testInstructionI(4114104213,0,-180867642,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",57058,instr);
  testPassed = testInstructionI(1204497775,0,1204489297,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",65177,instr);
  testPassed = testInstructionI(443352346,0,443351987,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",48835,instr);
  testPassed = testInstructionI(-392579319,0,-392596020,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ADDI");
  iInstructionGen("ADDI",60797,instr);
  testPassed = testInstructionI(1235985687,0,1235980948,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //AND
  testId = mips_test_begin_test("AND");
  rInstructionGen(0,"AND",instr);
  testPassed = testInstructionR(0b110101,0b011101,0b010101,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //OR
  testId = mips_test_begin_test("OR");
  rInstructionGen(0,"OR",instr);
  testPassed = testInstructionR(0b110101,0b011101,0b111101,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //XOR
  testId = mips_test_begin_test("XOR");
  rInstructionGen(0,"XOR",instr);
  testPassed = testInstructionR(0b110101,0b011101,0b101000,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ANDI
  testId = mips_test_begin_test("ANDI");
  iInstructionGen("ANDI",0xF00F,instr);
  testPassed = testInstructionI(0xFFFFFFFF,0,0xF00F,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SLTU
  testId = mips_test_begin_test("SLTU");
  rInstructionGen(0,"SLTU",instr);
  testPassed = testInstructionR(4,6,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTU");
  rInstructionGen(0,"SLTU",instr);
  testPassed = testInstructionR(6,4,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SLT
  testId = mips_test_begin_test("SLT");
  rInstructionGen(0,"SLT",instr);
  testPassed = testInstructionR(4,6,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLT");
  rInstructionGen(0,"SLT",instr);
  testPassed = testInstructionR(6,4,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLT");
  rInstructionGen(0,"SLT",instr);
  testPassed = testInstructionR(-4,6,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLT");
  rInstructionGen(0,"SLT",instr);
  testPassed = testInstructionR(6,-4,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLT");
  rInstructionGen(0,"SLT",instr);
  testPassed = testInstructionR(-6,-4,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SUBU
  testId = mips_test_begin_test("SUBU");
  rInstructionGen(0,"SUBU",instr);
  testPassed = testInstructionR(60,4,56,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SUBU");
  rInstructionGen(0,"SUBU",instr);
  testPassed = testInstructionR(6,10,4294967292,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SUB
  testId = mips_test_begin_test("SUB");
  rInstructionGen(0,"SUB",instr);
  testPassed = testInstructionR(60,4,56,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SUB");
  rInstructionGen(0,"SUB",instr);
  expError = mips_ExceptionArithmeticOverflow;
  testPassed = testInstructionR(-2147483648,1,2147483647,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  expError = mips_Success;

  //SLLV
  testId = mips_test_begin_test("SLLV");
  rInstructionGen(0,"SLLV",instr);
  testPassed = testInstructionR(2,4,16,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLLV");
  rInstructionGen(0,"SLLV",instr);
  testPassed = testInstructionR(4282876139,1791095845,262219776,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SRLV
  testId = mips_test_begin_test("SRLV");
  rInstructionGen(0,"SRLV",instr);
  testPassed = testInstructionR(2,4,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SRAV
  testId = mips_test_begin_test("SRAV");
  rInstructionGen(0,"SRAV",instr);
  testPassed = testInstructionR(1,0xFFFFFFFF,0xFFFFFFFF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SRAV");
  rInstructionGen(0,"SRAV",instr);
  testPassed = testInstructionR(1,0x0FFFFFFF,0x07FFFFFF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //ORI
  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",0xF00F,instr);
  testPassed = testInstructionI(0x0000F0F0,0,0xF0FF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",9287,instr);
  testPassed = testInstructionI(422396446,0,422405727,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",60977,instr);
  testPassed = testInstructionI(-180863083,0,-180813899,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",11032,instr);
  testPassed = testInstructionI(-1323377724,0,-1323369508,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",19482,instr);
  testPassed = testInstructionI(-1346468276,0,-1346449826,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",29555,instr);
  testPassed = testInstructionI(78547565,0,78576511,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",27712,instr);
  testPassed = testInstructionI(-47841265,0,-47813553,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",57058,instr);
  testPassed = testInstructionI(1204497775,0,1204551663,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",65177,instr);
  testPassed = testInstructionI(443352346,0,443416475,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",48835,instr);
  testPassed = testInstructionI(3902387977,0,-392577077,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("ORI");
  iInstructionGen("ORI",60797,instr);
  testPassed = testInstructionI(1235985687,0,1236004223,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //XORI
  testId = mips_test_begin_test("XORI");
  iInstructionGen("XORI",0xF00F,instr);
  testPassed = testInstructionI(0x0000F0F0,0,0x00FF,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SB
  testId = mips_test_begin_test("SB");
  iInstructionGen("SB",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"SB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SB");
  iInstructionGen("SB",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"SB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SH
  testId = mips_test_begin_test("SH");
  iInstructionGen("SH",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"SH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SH");
  iInstructionGen("SH",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"SH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SW
  testId = mips_test_begin_test("SW");
  iInstructionGen("SW",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"SW",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SW");
  iInstructionGen("SW",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"SW",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SLTIU
  testId = mips_test_begin_test("SLTIU");
  iInstructionGen("SLTIU",5,instr);
  testPassed = testInstructionI(1,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTIU");
  iInstructionGen("SLTIU",1,instr);
  testPassed = testInstructionI(5,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //SLTI
  testId = mips_test_begin_test("SLTI");
  iInstructionGen("SLTI",5,instr);
  testPassed = testInstructionI(1,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTI");
  iInstructionGen("SLTI",1,instr);
  testPassed = testInstructionI(5,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTI");
  iInstructionGen("SLTI",-5,instr);
  testPassed = testInstructionI(1,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTI");
  iInstructionGen("SLTI",1,instr);
  testPassed = testInstructionI(-5,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("SLTI");
  iInstructionGen("SLTI",-1,instr);
  testPassed = testInstructionI(-5,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //J
  testId = mips_test_begin_test("J");
  error = mips_cpu_get_pc(cpu, &pc);
  if(error != mips_Success){
    testPassed = 0;
    mips_test_end_test(testId,testPassed,0);
  }
  else{
    pcJump = ((pc + 12) >> 2) & 0xFFFF; //Jump to 2 instructions after delay slot
    jInstructionGen("J",pcJump,instr);
    testPassed = testInstructionJ("J",pcJump,instr,cpu,mem,error,expError);
    mips_test_end_test(testId,testPassed,0);
  }

  //JAL
  testId = mips_test_begin_test("JAL");
  error = mips_cpu_get_pc(cpu, &pc);
  if(error != mips_Success){
    testPassed = 0;
    mips_test_end_test(testId,testPassed,0);
  }
  else{
    pcJump = ((pc + 12) >> 2) & 0xFFFF; //Jump to 2 instructions after delay slot
    jInstructionGen("JAL",pcJump,instr);
    testPassed = testInstructionJ("JAL",pcJump,instr,cpu,mem,error,expError);
    mips_test_end_test(testId,testPassed,0);
  }

  //JR
  testId = mips_test_begin_test("JR");
  error = mips_cpu_get_pc(cpu, &pc);
  if(error != mips_Success){
    testPassed = 0;
    mips_test_end_test(testId,testPassed,0);
  }
  else{
    pcJump = ((pc + 12) >> 2) & 0xFFFF; //Jump to 2 instructions after delay slot
    rInstructionGen(0,"JR",instr);
    testPassed = testInstructionJ("JR",pcJump,instr,cpu,mem,error,expError);
    mips_test_end_test(testId,testPassed,0);
  }

  //JALR
  testId = mips_test_begin_test("JALR");
  error = mips_cpu_get_pc(cpu, &pc);
  if(error != mips_Success){
    testPassed = 0;
    mips_test_end_test(testId,testPassed,0);
  }
  else{
    pcJump = ((pc + 12) >> 2) & 0xFFFF; //Jump to 2 instructions after delay slot
    rInstructionGen(0,"JALR",instr);
    testPassed = testInstructionJ("JALR",pcJump,instr,cpu,mem,error,expError);
    mips_test_end_test(testId,testPassed,0);
  }

  //BEQ
  pcJump = 12 >> 2;
  testId = mips_test_begin_test("BEQ");
  iInstructionGen("BEQ", pcJump, instr);
  testPassed = testBranchInstruction("BEQ",pcJump,3,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BEQ");
  error = mips_cpu_get_pc(cpu, &pc);
  if(error != mips_Success){
    testPassed = 0;
    mips_test_end_test(testId,testPassed,0);
  }
  else{
    iInstructionGen("BEQ", pcJump, instr);
    testPassed = testBranchInstruction("BEQ",pcJump,0,0,instr,cpu,mem,error,expError);
    mips_test_end_test(testId,testPassed,0);
  }

  //BNE
  testId = mips_test_begin_test("BNE");
  iInstructionGen("BNE", pcJump, instr);
  testPassed = testBranchInstruction("BNE",pcJump,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BNE");
  iInstructionGen("BNE", pcJump, instr);
  testPassed = testBranchInstruction("BNE",pcJump,3,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BGEZ
  testId = mips_test_begin_test("BGEZ");
  iInstructionGen("BGEZ", pcJump, instr);
  testPassed = testBranchInstruction("BGEZ",pcJump,3,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGEZ");
  iInstructionGen("BGEZ", pcJump, instr);
  testPassed = testBranchInstruction("BGEZ",pcJump,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGEZ");
  iInstructionGen("BGEZ", pcJump, instr);
  testPassed = testBranchInstruction("BGEZ",pcJump,-1,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BGEZAL
  testId = mips_test_begin_test("BGEZAL");
  iInstructionGen("BGEZAL", pcJump, instr);
  testPassed = testBranchInstruction("BGEZAL",pcJump,3,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGEZAL");
  iInstructionGen("BGEZAL", pcJump, instr);
  testPassed = testBranchInstruction("BGEZAL",pcJump,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGEZAL");
  iInstructionGen("BGEZAL", pcJump, instr);
  testPassed = testBranchInstruction("BGEZAL",pcJump,-1,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BGTZ
  testId = mips_test_begin_test("BGTZ");
  iInstructionGen("BGTZ", pcJump, instr);
  testPassed = testBranchInstruction("BGTZ",pcJump,-1,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGTZ");
  iInstructionGen("BGTZ", pcJump, instr);
  testPassed = testBranchInstruction("BGTZ",pcJump,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BGTZ");
  iInstructionGen("BGTZ", pcJump, instr);
  testPassed = testBranchInstruction("BGTZ",pcJump,3,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BLEZ
  testId = mips_test_begin_test("BLEZ");
  iInstructionGen("BLEZ", pcJump, instr);
  testPassed = testBranchInstruction("BLEZ",pcJump,-1,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLEZ");
  iInstructionGen("BLEZ", pcJump, instr);
  testPassed = testBranchInstruction("BLEZ",pcJump,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLEZ");
  iInstructionGen("BLEZ", pcJump, instr);
  testPassed = testBranchInstruction("BLEZ",pcJump,3,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BLTZ
  testId = mips_test_begin_test("BLTZ");
  iInstructionGen("BLTZ", pcJump, instr);
  testPassed = testBranchInstruction("BLTZ",pcJump,-1,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLTZ");
  iInstructionGen("BLTZ", pcJump, instr);
  testPassed = testBranchInstruction("BLTZ",pcJump,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLTZ");
  iInstructionGen("BLTZ", pcJump, instr);
  testPassed = testBranchInstruction("BLTZ",pcJump,3,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //BLTZAL
  testId = mips_test_begin_test("BLTZAL");
  iInstructionGen("BLTZAL", pcJump, instr);
  testPassed = testBranchInstruction("BLTZAL",pcJump,-1,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLTZAL");
  iInstructionGen("BLTZAL", pcJump, instr);
  testPassed = testBranchInstruction("BLTZAL",pcJump,0,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("BLTZAL");
  iInstructionGen("BLTZAL", pcJump, instr);
  testPassed = testBranchInstruction("BLTZAL",pcJump,3,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //MULTU
  testId = mips_test_begin_test("MULTU");
  rInstructionGen(0,"MULTU",instr);
  testPassed = testMultDiv(0xFFA810,0x5FB503,0x5F94,0x22C24830,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //MULT
  int passedMult = 0;
  testId = mips_test_begin_test("MULT");
  rInstructionGen(0,"MULT",instr);
  testPassed = testMultDiv(0xFFA810,0x5FB503,0x5F94,0x22C24830,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  testId = mips_test_begin_test("MULT");
  rInstructionGen(0,"MULT",instr);
  testPassed = testMultDiv(0xFFFFFFFF,27,0xFFFFFFFF,0xFFFFFFE5,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  testId = mips_test_begin_test("MULT");
  rInstructionGen(0,"MULT",instr);
  testPassed = testMultDiv(0xFFFFFFFF,0xFFFFFFFF,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  //DIVU
  testId = mips_test_begin_test("DIVU");
  rInstructionGen(0,"DIVU",instr);
  testPassed = testMultDiv(0xFFA810,0x5FB503,0x403E0A,2,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  //DIV
  testId = mips_test_begin_test("DIV");
  rInstructionGen(0,"DIV",instr);
  testPassed = testMultDiv(0xFFA810,0x5FB503,0x403E0A,2,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  testId = mips_test_begin_test("DIV");
  rInstructionGen(0,"DIV",instr);
  testPassed = testMultDiv(0xFFFFFFFF,27,0xFFFFFFFF,0,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  testId = mips_test_begin_test("DIV");
  rInstructionGen(0,"DIV",instr);
  testPassed = testMultDiv(0xFFFFFFFF,0xFFFFFFFF,0,1,instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  if(testPassed){
    passedMult += 1;
  }

  //MFHI AND MFLO
  testId = mips_test_begin_test("MFHI");
  rInstructionGen(0,"MTHI",instr);
  testPassed = testMTHILO("MTHI", 0xFFFFFFFF, instr, cpu, mem, error, expError);
  if((testPassed == 0) && (passedMult >= 1)){
    mips_test_end_test(testId,1,0);
  }
  else if(testPassed == 1){
    mips_test_end_test(testId,1,0);
  }
  else{
    mips_test_end_test(testId,0,0);
  }

  testId = mips_test_begin_test("MTHI");
  if((testPassed == 0) && (passedMult >= 1)){
    mips_test_end_test(testId,0,0);
  }
  else{
      mips_test_end_test(testId,1,0);
  }

  testId = mips_test_begin_test("MFLO");
  rInstructionGen(0,"MTLO",instr);
  testPassed = testMTHILO("MTLO", 0xFFFFFFFF, instr, cpu, mem, error, expError);
  if((testPassed == 0) && (passedMult >= 1)){
    mips_test_end_test(testId,1,0);
  }
  else if(testPassed == 1){
    mips_test_end_test(testId,1,0);
  }
  else{
    mips_test_end_test(testId,0,0);
  }

  testId = mips_test_begin_test("MTLO");
  if((testPassed == 0) && (passedMult >= 1)){
    mips_test_end_test(testId,0,0);
  }
  else{
      mips_test_end_test(testId,1,0);
  }

  //LW
  testId = mips_test_begin_test("LW");
  iInstructionGen("LW",0x4,instr);
  testPassed = testStoreInstruction(0xFF0FFF0F,0x4,"LW",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LW");
  iInstructionGen("LW",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFF0FFF0F,0xFFFC,"LW",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LH
  testId = mips_test_begin_test("LH");
  iInstructionGen("LH",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"LH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LH");
  iInstructionGen("LH",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"LH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LH");
  iInstructionGen("LH",0x4,instr);
  testPassed = testStoreInstruction(0xFFFF0FFF,0x4,"LH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LH");
  iInstructionGen("LH",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFF0FFF,0xFFFC,"LH",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LB
  testId = mips_test_begin_test("LB");
  iInstructionGen("LB",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"LB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LB");
  iInstructionGen("LB",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"LB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LB");
  iInstructionGen("LB",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFF0F,0x4,"LB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LB");
  iInstructionGen("LB",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFF0F,0xFFFC,"LB",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LHU
  testId = mips_test_begin_test("LHU");
  iInstructionGen("LHU",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"LHU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LHU");
  iInstructionGen("LHU",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"LHU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LHU");
  iInstructionGen("LHU",0x4,instr);
  testPassed = testStoreInstruction(0xFFFF0FFF,0x4,"LHU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LHU");
  iInstructionGen("LHU",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFF0FFF,0xFFFC,"LHU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LBU
  testId = mips_test_begin_test("LBU");
  iInstructionGen("LBU",0x4,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x4,"LBU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LBU");
  iInstructionGen("LBU",0xFFFC,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0xFFFC,"LBU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LBU");
  iInstructionGen("LBU",0x4,instr);
  testPassed = testStoreInstruction(0xF0FFFF0F,0x4,"LBU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LBU");
  iInstructionGen("LBU",0xFFFC,instr);
  testPassed = testStoreInstruction(0xF0FFFF0F,0xFFFC,"LBU",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LUI
  testId = mips_test_begin_test("LUI");
  iInstructionGen("LUI",0xFFFF,instr);
  testPassed = testStoreInstruction(0,0xFFFF,"LUI",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LWL
  testId = mips_test_begin_test("LWL");
  iInstructionGen("LWL",0x0,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x0,"LWL",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LWL");
  iInstructionGen("LWL",0x1,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x1,"LWL",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);
  testId = mips_test_begin_test("LWL");
  iInstructionGen("LWL",0x0,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x0,"LWL",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LWL");
  iInstructionGen("LWL",0x1,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x1,"LWL",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LWL");
  iInstructionGen("LWL",0x2,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x2,"LWL",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //LWR
  testId = mips_test_begin_test("LWR");
  iInstructionGen("LWR",0x0,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x0,"LWR",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LWR");
  iInstructionGen("LWR",0x1,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x1,"LWR",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  testId = mips_test_begin_test("LWR");
  iInstructionGen("LWR",0x2,instr);
  testPassed = testStoreInstruction(0xFFFFFFFF,0x2,"LWR",instr,cpu,mem,error,expError);
  mips_test_end_test(testId,testPassed,0);

  //END
  mips_test_end_suite();

  return 0;
}

//FUNCTION ALWAYS ACTS ON REG1 & 2 AND STORES IN 3 (EXCEPT JUMP)
void rInstructionGen(uint8_t shiftAmt, string function, uint8_t instr[]){
  instr[0] = 0;
  instr[1] = (1 << 5)|(2&0b11111);
  instr[2] = (3 << 3)|(shiftAmt>>2);
  instr[3] = (shiftAmt << 6);
  //C++ doesn't like switch case statements for strings unfourtunately :(
  if(function == "SLL"){
    instr[1] = 2;
    return;
  }
  if(function == "SRL"){
    instr[3] = instr[3]|0x02;
    instr[1] = 2;
    return;
  }
  if(function == "SRA"){
    instr[3] = instr[3]|0x03;
    instr[1] = 2;
    return;
  }
  if(function == "JR"){
    instr[3] = 0x08;
    instr[2] = 0;
    //acts on reg 4
    instr[1] = (4 << 5);
    return;
  }
  if(function =="MFHI"){
    instr[1] = 0;
    instr[3] = 0x10;
    return;
  }
  if(function == "MFLO"){
    instr[1] = 0;
    instr[2] = (4 << 3);
    instr[3] = instr[3]|0x12;
    return;
  }
  if(function == "MULT"){
    instr[2] = 0;
    instr[3] = 0x18;
    return;
  }
  if(function == "MULTU"){
    instr[2] = 0;
    instr[3] = 0x19;
    return;
  }
  if(function == "DIV"){
    instr[2] = 0;
    instr[3] = 0x1A;
    return;
  }
  if(function == "DIVU"){
    instr[2] = 0;
    instr[3] = 0x1B;
    return;
  }
  if(function == "ADD"){
    instr[3] = instr[3]|0x20;
    return;
  }
  if(function == "ADDU"){
    instr[3] = instr[3]|0x21;
    return;
  }
  if(function == "SUB"){
    instr[3] = instr[3]|0x22;
    return;
  }
  if(function == "SUBU"){
    instr[3] = instr[3]|0x23;
    return;
  }
  if(function == "AND"){
    instr[3] = instr[3]|0x24;
    return;
  }
  if(function == "OR"){
    instr[3] = instr[3]|0x25;
    return;
  }
  if(function == "XOR"){
    instr[3] = instr[3]|0x26;
    return;
  }
  if(function == "SLT"){
    instr[3] = instr[3]|0x2A;
    return;
  }
  if(function == "SLTU"){
    instr[3] = instr[3]|0x2B;
    return;
  }
  if(function == "SLLV"){
    instr[3] = instr[3]|0x04;
    return;
  }
  if(function == "SRLV"){
    instr[3] = instr[3]|0x06;
    return;
  }
  if(function == "SRAV"){
    instr[3] = instr[3]|0x07;
    return;
  }
  if(function == "JALR"){
    instr[3] = 0x09;
    //acts on reg 4
    instr[1] = (4 << 5);
    instr[2] = 5 << 3;
    return;
  }
  if(function == "MTHI"){
    instr[1] = (1 << 5);
    instr[2] = 0;
    instr[3] = 0x11;
    return;
  }
  if(function == "MTLO"){
    instr[1] = (2 << 5);
    instr[2] = 0;
    instr[3] = 0x13;
    return;
  }
}

//FUNCTION ALWAYS ACTS ON REG1 & 2 AND STORES IN 3
int testInstructionR(uint32_t value1, uint32_t value2, uint32_t finValue, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 1
  error = mips_cpu_set_register(cpu,1,value1);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 2
  error = mips_cpu_set_register(cpu,2,value2);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //write instruction to memory
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //step cpu
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //cout << "ERROR: " << error << endl;
  //check result
  if((error == expError)&&(expError != mips_Success)){
    //cout << "GOT EXPECTED ERROR " << endl;
    return 1;
  }
  else if((expError == mips_Success)&&(error == mips_Success)){
    uint32_t getReg;
    error = mips_cpu_get_register(cpu,3,&getReg);
    //cout << "Reg value " << bitset<32>(getReg) << endl;
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    else if(getReg != finValue){
      //cout << "NOT CORRECT FIN VALUE " << endl;
      return 0;
    }
    else{
      return 1;
    }
  }
  //cout << "REACH END " << endl;
  return 0;
}

//FUNCTION TO TEST IF REGISTERS CAN BE WRITTEN TO AND READ FROM
int testRegisters(mips_cpu_h cpu, mips_error &error){
  uint32_t value;

  //Read reg 0 (REG 0 IS A SPECIAL CASE)
  error = mips_cpu_get_register(cpu,0,&value);
  if((error != mips_Success) || (value != 0)){
    return 0;
  }

  //try to write to reg 0
  error = mips_cpu_set_register(cpu,0,0x12345678);
  if(error != mips_Success){
    return 0;
  }

  //try to re-read reg0
  error = mips_cpu_get_register(cpu,0,&value);
  if((error != mips_Success) || (value != 0)){
    return 0;
  }

  //GENERAL CASE FOR OTHER REGISTERS
  for(int i = 1;i<32;i++){
    //Check reg is originally 0
    error = mips_cpu_get_register(cpu,i,&value);
    if((error!=mips_Success) || (value != 0)){
      return 0;
    }

    //Check reg can be written to
    error = mips_cpu_set_register(cpu,i,0x12345678);
    if(error != mips_Success){
      return 0;
    }

    //Check reg has been written to
    error = mips_cpu_get_register(cpu,i,&value);
    if(error != mips_Success){
      return 0;
    }
    else if(value != 0x12345678){
      return 0;
    }
  }
  //try to write to register outside the range
  error = mips_cpu_set_register(cpu,32,0x12345678);
  if(error == mips_Success){
    return 0;
  }
  return 1;
}

//FUNCTION ALWAYS ACTS ON REG1 & 2 (EXCEPT BRANCH WHICH ACTS ON 5)
void iInstructionGen(string function, uint16_t immediate, uint8_t instr[]){
  instr[0] = 0;
  instr[1] = (1 << 5)|(2&0b11111);
  instr[2] = (uint8_t)(immediate >> 8);
  instr[3] = (uint8_t)immediate;
  if(function == "BGEZ"){
    instr[0] = (0x01 << 2);
    instr[1] = 0x81;
    return;
  }
  if(function == "BLTZ"){
    instr[0] = (0x01 << 2);
    instr[1] = 0x80;
    return;
  }
  if(function == "BLTZAL"){
    instr[0] = (0x01 << 2);
    instr[1] = 0x80|0x10;
    return;
  }
  if(function == "BEQ"){
    instr[0] = (0x04 << 2);
    instr[1] = 0b10000101; //R4 and R5
    return;
  }
  if(function == "BNE"){
    instr[0] = (0x05 << 2);
    instr[1] = 0b10000101;
    return;
  }
  if(function == "BGEZAL"){
    instr[0] = (0x01 << 2);
    instr[1] = 0x80|0x11;
    return;
  }
  if(function == "BGTZ"){
    instr[0] = (0x07 << 2);
    instr[1] = 0x80|0x00;
    return;
  }
  if(function == "BLEZ"){
    instr[0] = (0x06 << 2);
    instr[1] = 0x80|0x00;
    return;
  }
  if(function == "ADDI"){
    instr[0] = (0x08 << 2);
    return;
  }
  if(function == "ADDIU"){
    instr[0] = (0x09 << 2);
    return;
  }
  if(function == "ANDI"){
    instr[0] = (0x0C << 2);
    return;
  }
  if(function == "ORI"){
    instr[0] = (0x0D << 2);
    return;
  }
  if(function == "XORI"){
    instr[0] = (0x0E << 2);
    return;
  }
  if(function == "SB"){
    instr[0] = (0x28 << 2);
    return;
  }
  if(function == "SH"){
    instr[0] = (0x29 << 2);
    return;
  }
  if(function == "SW"){
    instr[0] = (0x2B << 2);
    return;
  }
  if(function == "SLTI"){
    instr[0] = (0x0A << 2);
    return;
  }
  if(function == "SLTIU"){
    instr[0] = (0x0B << 2);
    return;
  }
  if(function == "LB"){
    instr[0] = (0x20 << 2);
    return;
  }
  if(function == "LBU"){
    instr[0] = (0x24 << 2);
    return;
  }
  if(function == "LH"){
    instr[0] = (0x21 << 2);
    return;
  }
  if(function == "LHU"){
    instr[0] = (0x25 << 2);
    return;
  }
  if(function == "LUI"){
    instr[0] = (0x0F << 2);
    instr[1] = 2;
    return;
  }
  if(function == "LW"){
    instr[0] = (0x23 << 2);
    return;
  }
  if(function == "LWL"){
    instr[0] = (0x22 << 2);
    return;
  }
  if(function == "LWR"){
    instr[0] = (0x26 << 2);
    return;
  }
}

//FUNCTION ALWAYS ACTS ON REG1 & 2
int testInstructionI(uint32_t value1, uint32_t value2, uint32_t finValue, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 1
  error = mips_cpu_set_register(cpu,1,value1);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 2
  error = mips_cpu_set_register(cpu,2,value2);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //write instruction to memory
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //step cpu
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //check result
  if((error == expError)&&(expError != mips_Success)){
    //cout << "got expected error" << endl;
    return 1;
  }
  else if((expError == mips_Success)&&(error == mips_Success)){
    uint32_t getReg;
    error = mips_cpu_get_register(cpu,2,&getReg);
    //cout << "Reg3: " << getReg << endl;
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    else if(getReg != finValue){
      return 0;
    }
    else{
      //cout << "finValue is correct" << endl;
      return 1;
    }
  }
  //cout << "Reached end " << endl;
  return 0;
}

//FUNCTION ALWAYS ACTS ON REG 1 (BASE = 0) & 2
int testStoreInstruction(uint32_t value, uint16_t offset, string func, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //value set endianess
  uint32_t newValue = (value << 24) | ((value << 8)&0x00FF0000) | ((value >> 8)&0x0000FF00) | (value >> 24);
  //set register 1
  error = mips_cpu_set_register(cpu,1,4);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  uint32_t address = 0;
  if(offset >> 15){
    address = offset | 0xFFFF0000;
    address = ~address + 1;
    address = 4 - address;
  }
  else{
    address = 4 + offset;
  }
  if( (func == "SB") || (func == "SH") || (func == "SW") || (func == "LWL") || (func == "LWR")){
    //set register 2
    if((func == "LWL") || (func == "LWR")){
      error = mips_cpu_set_register(cpu,2,0);
      if((error != mips_Success)&&(error != expError)){
        return 0;
      }
      uint8_t toWrite[4];
      toWrite[0] = (uint8_t)value;
      toWrite[1] = (uint8_t)(value >> 8);
      toWrite[2] = (uint8_t)(value >> 16);
      toWrite[3] = (uint8_t)(value >> 24);
      error = mips_mem_write(mem,address,4,toWrite);
    }
    else{
      error = mips_cpu_set_register(cpu,2,value);
      if((error != mips_Success)&&(error != expError)){
        return 0;
      }
    }
  }
  else if(func != "LUI"){
    //write number to memory
    uint8_t toWrite[4];
    toWrite[0] = (uint8_t)value;
    toWrite[1] = (uint8_t)(value >> 8);
    toWrite[2] = (uint8_t)(value >> 16);
    toWrite[3] = (uint8_t)(value >> 24);
    error = mips_mem_write(mem,address,4,toWrite);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
  }
  //write instruction to memory
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //step cpu
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //check
  if(func == "SB"){
    error = mips_mem_read(mem,address,1,instr);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if(instr[0] == (uint8_t)value){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "SH"){
    error = mips_mem_read(mem,address,2,instr);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if((instr[0] == (uint8_t)(value >> 8)) && (instr[1] == (uint8_t)value)){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "SW"){
    error = mips_mem_read(mem,address,4,instr);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if( (instr[0] == (uint8_t)(value >> 24)) && (instr[1] == (uint8_t)(value >> 16)) && (instr[2] == (uint8_t)(value >> 8)) && (instr[3] == (uint8_t)value) ){
      return 1;
    }
    else{
      return 0;
    }
  }
  uint32_t regValue;
  //value = newValue;
  if(func == "LW"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    value = newValue;
    if(regValue != value){
      return 0;
    }
    else{
      return 1;
    }
  }
  if(func == "LH"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    value = ((value << 8)&0xFF00) | ((value >> 8)&0xFF);
     if((uint16_t)regValue != (uint16_t)value){
      return 0;
    }
    if( ( ((regValue&0xFFFF0000) == 0xFFFF0000) && (((value >> 15)&0b1) == 1) ) || ( ((regValue&0xFFFF0000) == 0) && (((value >> 15)&0b1) == 0) ) ){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "LB"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if((uint8_t)regValue != (uint8_t)value){
      return 0;
    }
    if( ( ((regValue&0xFFFFFF00) == 0xFFFFFF00) && (((value >> 7)&0b1) == 1) ) || ( ((regValue&0xFFFFFF00) == 0) && (((value >> 7)&0b1) == 0) ) ){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "LHU"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    value = ((value << 8)&0xFF00) | ((value >> 8)&0xFF);
    if((uint16_t)regValue != (uint16_t)value){
      return 0;
    }
    if((regValue&0xFFFF0000) == 0){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "LBU"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if((uint8_t)regValue != (uint8_t)value){
      return 0;
    }
    if((regValue&0xFFFFFF00) == 0){
      return 1;
    }
    else{
      return 0;
    }
  }
  if(func == "LUI"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    //cout << bitset<32>(regValue) << endl;
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if(regValue == ((uint32_t)offset << 16)){
      return 1;
    }
    else{
      return 0;
    }
  }
  value = newValue;
  if(func == "LWL"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    bool check = true;
    unsigned count = 0;
    while(check){
      if(((address)%4) == 0){
        check = false;
      }
      else{
        address++;
        count++;
      }
    }
    if(count == 0){
      if(regValue == value){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if(count == 3){
      if(regValue == (value&0xFFFFFF00)){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if(count == 2){
      if(regValue == (value&0xFFFF0000)){
        return 1;
      }
      else{
        return 0;
      }
    }
    else{
      if(regValue == (value&0x000000FF)){
        return 1;
      }
      else{
        return 0;
      }
    }
  }
  if(func == "LWR"){
    error = mips_cpu_get_register(cpu,2,&regValue);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    bool check = true;
    unsigned count = 0;
    while(check){
      if(((address)%4) == 0){
        check = false;
      }
      else{
        address--;
        count++;
      }
    }
    if(count == 3){
      if(regValue == value){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if(count == 2){
      if(regValue == (value&0x00FFFFFF)){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if(count == 1){
      if(regValue == (value&0x0000FFFF)){
        return 1;
      }
      else{
        return 0;
      }
    }
    else{
      if(regValue == (value&0x000000FF)){
        return 1;
      }
      else{
        return 0;
      }
    }
  }
  return 0;
}

//RETURNS VALUE OF PC (USED FOR TESTING)
uint32_t getPC(mips_cpu_h state){
  uint32_t tmp;
  mips_error error = mips_cpu_get_pc(state,&tmp);
  return tmp;
}

void jInstructionGen(string function, uint32_t address, uint8_t instr[]){
  instr[3] = (uint8_t)address;
  instr[2] = (uint8_t)(address >> 8);
  instr[1] = (uint8_t)(address >> 16);
  instr[0] = (uint8_t)(address >> 24);
  if(function == "J"){
    instr[0] = instr[0] | 0x08;
    return;
  }
  if(function == "JAL"){
    instr[0] = instr[0] | 0x0C;
    return;
  }
}

int testInstructionJ(string function, uint32_t address, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 1 (fixed value)
  error = mips_cpu_set_register(cpu,1,2);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 2 (fixed value)
  error = mips_cpu_set_register(cpu,2,3);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set jump address for R instructions
  if((function == "JR") || (function == "JALR")){
    error = mips_cpu_set_register(cpu,4,(address << 2));
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
  }
  //write instruction to memory (jump instruction)
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set instruction for delay slot (fixed add instruction with answer = 5 in reg3)
  rInstructionGen(0, "ADDU", instr);
  pc += 4;
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //step cpu
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //check result
  if((error == expError)&&(expError != mips_Success)){
    return 1;
  }
  uint32_t answer;
  error = mips_cpu_get_register(cpu,3,&answer);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  if(answer != 5){
    //cout << answer << endl;
    //cout << "Delay slot not correct" << endl;
    return 0;
  }
  //Special case for JAL
  if((function == "JAL") || (function == "JALR")){
    uint32_t returnAddress = pc + 4;
    if(function == "JAL"){
      error = mips_cpu_get_register(cpu,31,&answer);
      if((error != mips_Success)&&(error != expError)){
        return 0;
      }
    }
    else{
      error = mips_cpu_get_register(cpu,5,&answer);
      if((error != mips_Success)&&(error != expError)){
        return 0;
      }
    }
    if(answer != returnAddress){
      return 0;
    }
  }
  address = (pc & 0xF0000000)|(address << 2);
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  if(pc != address){
    cout << bitset<32>(pc) << endl;
    //cout << "jump not correct" << endl;
    return 0;
  }
  else{
    return 1;
  }
}

int testBranchInstruction(string function, uint32_t address, uint32_t value, bool branch, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 1 (fixed value)
  error = mips_cpu_set_register(cpu,1,2);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 2 (fixed value)
  error = mips_cpu_set_register(cpu,2,3);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set register 4
  error = mips_cpu_set_register(cpu,4,value);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  if((function == "BEQ") || (function == "BNE")){
    error = mips_cpu_set_register(cpu,5,3); //choose 3 to compare with
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
  }
  //write instruction to memory (jump instruction)
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    //cout << "HI" << endl;
    return 0;
  }
  //set instruction for delay slot (fixed add instruction with answer = 5 in reg3)
  rInstructionGen(0, "ADDU", instr);
  pc += 4;
  //cout << "ADDU PC: " << pc << endl;
  error = mips_mem_write(mem,pc,4,instr);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //step cpu
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  error = mips_cpu_step(cpu);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //check result
  if((error == expError)&&(expError != mips_Success)){
    return 1;
  }
  uint32_t answer;
  error = mips_cpu_get_register(cpu,3,&answer);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  if(answer != 5){
    return 0;
  }
  if((function == "BLTZAL") || (function == "BGEZAL")){
    uint32_t returnAddress = pc + 4;
    error = mips_cpu_get_register(cpu,31,&answer);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if(answer != returnAddress){
      return 0;
    }
  }
  if(branch){
    if((address >> 15) == 1){
      address = address | 0xFFFF0000;
      address = ~address + 1;
      address = pc - (address << 2);
    }
    else{
      address = pc + (address << 2);
    }
    error = mips_cpu_get_pc(cpu,&pc);
    if((error != mips_Success)&&(error != expError)){
      return 0;
    }
    if(pc != address){
      return 0;
    }
  }
  return 1;
}

int testMultDiv(uint32_t value1, uint32_t value2, uint32_t expHI, uint32_t expLO, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set reg1
  error = mips_cpu_set_register(cpu,1,value1);
  if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //set reg2
   error = mips_cpu_set_register(cpu,2,value2);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //write instr to memory
   error = mips_mem_write(mem,pc,4,instr);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //generate MFHI and send to memory
   rInstructionGen(0, "MFHI", instr);
   error = mips_mem_write(mem,pc+4,4,instr);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //generate MFLO and send to memory
   rInstructionGen(0, "MFLO", instr);
   error = mips_mem_write(mem,pc+8,4,instr);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //step cpu
   error = mips_cpu_step(cpu);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //step cpu 2
   error = mips_cpu_step(cpu);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //step cpu 3
   error = mips_cpu_step(cpu);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //check result
   uint32_t HI,LO;
   error = mips_cpu_get_register(cpu,3,&HI);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   error = mips_cpu_get_register(cpu,4,&LO);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   if((HI != expHI) || (LO != expLO) ){
     return 0;
   }
   else{
     return 1;
   }
 }

int testMTHILO(string function, uint32_t value, uint8_t instr[], mips_cpu_h cpu, mips_mem_h mem, mips_error &error, mips_error expError){
  //get PC
  uint32_t pc;
  error = mips_cpu_get_pc(cpu,&pc);
  if((error != mips_Success)&&(error != expError)){
    return 0;
  }
  //set reg1
  error = mips_cpu_set_register(cpu,1,value);
  if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //write instr to memory
   error = mips_mem_write(mem,pc,4,instr);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   if(function == "MTHI"){
     //generate MFHI and send to memory
     rInstructionGen(0, "MFHI", instr);
     error = mips_mem_write(mem,pc+4,4,instr);
     if((error != mips_Success)&&(error != expError)){
       return 0;
     }
   }
   if(function == "MTLO"){
     //generate MFLO and send to memory
     rInstructionGen(0, "MFLO", instr);
     error = mips_mem_write(mem,pc+4,4,instr);
     if((error != mips_Success)&&(error != expError)){
       return 0;
     }
   }
   //step cpu
   error = mips_cpu_step(cpu);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //step cpu 2
   error = mips_cpu_step(cpu);
   if((error != mips_Success)&&(error != expError)){
     return 0;
   }
   //check result
   uint32_t HI,LO;
   if(function == "MTHI"){
     error = mips_cpu_get_register(cpu,3,&HI);
     if((error != mips_Success)&&(error != expError)){
       return 0;
     }
     if(HI != value){
       return 0;
     }
     else{
       return 1;
     }
   }
   if(function == "MTLO"){
     error = mips_cpu_get_register(cpu,4,&LO);
     if((error != mips_Success)&&(error != expError)){
       return 0;
     }
     if(LO != value){
       return 0;
     }
     else{
       return 1;
     }
   }
   return 0;
}
