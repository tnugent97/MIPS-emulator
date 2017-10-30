#include "mips.h"
#include <cmath>
#include <bitset>
#include <iostream>

struct mips_cpu_impl
{
  uint32_t pc;
  uint32_t pcNext;
  uint32_t regs[32];
  uint32_t HI;
  uint32_t LO;
  mips_mem_h mem;
};

mips_error rInstruction(mips_cpu_h state, uint8_t *data);
mips_error jInstruction(mips_cpu_h state, uint8_t *data, uint8_t opcode);
mips_error iInstruction(mips_cpu_h state, uint8_t *data, uint8_t opcode);
uint32_t twosComp(uint32_t a);

mips_cpu_h mips_cpu_create(mips_mem_h mem){
  mips_cpu_impl* state = new mips_cpu_impl;
  state->pc = 0;
  state->pcNext = 4;
  state->HI = 0;
  state->LO = 0;
  for(unsigned i = 0; i < 32; i++){
    state->regs[i] = 0;
  }
  state->mem = mem;

  return state;
}

mips_error mips_cpu_reset(mips_cpu_h state){
  if(state == NULL){
    return mips_ErrorInvalidHandle;
  }
  else{
    for(unsigned i = 0; i < 32; i++){
      state->regs[i] = 0;
    }
    state->pc = 0;
    state->pcNext = 4;
    state->HI = 0;
    state->LO = 0;
    state = NULL;
    return mips_Success;
  }
}

mips_error mips_cpu_get_register(mips_cpu_h state, unsigned index, uint32_t* value){
  if(state == NULL){
    return mips_ErrorInvalidHandle;
  }
  if(index > 31){
    return mips_ErrorInvalidArgument;
  }
  *value = state->regs[index];
  return mips_Success;
}

mips_error mips_cpu_set_register(mips_cpu_h state, unsigned index, uint32_t value){
  if(state == NULL){
    return mips_ErrorInvalidHandle;
  }
  if(index > 31){
    return mips_ErrorInvalidArgument;
  }
  if(index != 0){
    state->regs[index] = value;
  }
  return mips_Success;
}

mips_error mips_cpu_set_pc(mips_cpu_h state, uint32_t pc){
  if(state==NULL){
    return mips_ErrorInvalidHandle;
  }
  if(pc%4 != 0){
    return mips_ExceptionInvalidLength;
  }
  state->pc = pc;
  state->pcNext = pc+4;
  return mips_Success;
}

mips_error mips_cpu_get_pc(mips_cpu_h state,	uint32_t *pc){
  if(state == NULL){
    return mips_ErrorInvalidHandle;
  }
  *pc = state->pc;
  return mips_Success;
}

mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level, FILE *dest){
  if(state==NULL){
    return mips_ErrorInvalidHandle;
  }
  return mips_Success;
}


void mips_cpu_free(mips_cpu_h state){
  delete state;
}


mips_error mips_cpu_step(mips_cpu_h state){
  if(state == NULL){
    return mips_ErrorInvalidHandle;
  }

  uint8_t data[4];

  //GET MEMORY
  mips_error memRead = mips_mem_read(state->mem,state->pc,4,data);
  if(memRead!=mips_Success){
    return memRead;
  }

  //reads instruction from memory address defined by PC.
  uint8_t opcode = ((data[0]&0b11111100) >> 2);

  if(opcode == 0x00){
    return rInstruction(state, data);
  }
  else if((opcode&0b111110) == 0x02){
    return jInstruction(state, data, opcode);
  }
  else{
    return iInstruction(state, data, opcode);
  }
}

mips_error rInstruction(mips_cpu_h state, uint8_t *data){
  uint8_t source1Address = (((data[0]&0b11) << 3)|((data[1]&0b11100000) >> 5));
  //std::cout << "src1Address: " << std::bitset<8>(source1Address) << std::endl;
  uint8_t source2Address = (data[1]&0b11111); //remove extra bits
  //std::cout << std::bitset<8>(source2Address) << std::endl;
  uint8_t dest = (data[2] >> 3);
  //std::cout << std::bitset<8>(dest) << std::endl;
  uint8_t amt = (((data[2]&0b111) << 2)|(data[3] >> 6));
  //std::cout << std::bitset<8>(amt) << std::endl;
  uint8_t fnCode = (data[3] & 0b111111); //remove extra bits
  //std::cout << std::bitset<8>(fnCode) << std::endl;
  uint64_t source1 = state->regs[source1Address];
  //std::cout << "src1: " << source1 << std::endl;
  uint64_t source2 = state->regs[source2Address];
  //std::cout << "src2: " << source2 << std::endl;
  uint64_t result;
  bool negate = false;
  bool negateSource1 = false;
  mips_error error;

  switch(fnCode){
    case(0x00): //SLL
      //std::cout << "SLL" << std::endl;
      state->regs[dest] = source2 << amt;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x02): //SRL
      //std::cout << "SRL" << std::endl;
      state->regs[dest] = (uint32_t)(source2 >> amt);
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x03): //SRA
      //std::cout << "SRA" << std::endl;
      if(source2 >> 31){
          negate = true;
      }
      result = source2 >> amt;
      if(negate){
        for(int i = 0;i<amt;i++){
          result = result | (0x80000000 >> i);
        }
      }
      negate = false;
      state->regs[dest] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x04): //SLLV
      result = source2 << (source1&0b11111);
      state->regs[dest] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x06): //SRLV
      result = source2 >> (source1&0b11111);
      state->regs[dest] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x07): //SRAV
      if(source2 >> 31){
          negate = true;
        }
        result = source2 >> (source1&0b11111);
        if(negate){
          for(unsigned i = 0;i<source1;i++){
            result = result | (0x80000000 >> i);
          }
        }
        negate = false;
        state->regs[dest] = (uint32_t)result;
        state->pc = state->pcNext;
        state->pcNext += 4;
        break;
    case(0x09): //JALR
      state->regs[dest] = (state->pc + 8);
      //no break so performs standard J instruction too
    case(0x08): //JR
      state->pc = state->pcNext;
      state->pcNext = source1;
      break;
    case(0x10): //MFHI
      state->regs[dest] = state->HI;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x11): //MTHI
      state->HI = state->regs[source1Address];
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x12): //MFLO
      state->regs[dest] = state->LO;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x13): //MTLO
      state->LO = state->regs[source1Address];
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x18): //MULT
      if((source1 >> 31) == 1){
        source1 = (uint32_t)twosComp(source1);
        negate = true;
      }
      if((source2 >> 31) == 1){
        source2 = (uint32_t)twosComp(source2);
        if(negate){
          negate = false;
        }
        else{
          negate = true;
        }
      }
      //no break, so normal multu is carried out
    case(0x19): //MULTU
      result = source1 * source2;
      if(negate){
        result = ~result + 1;
      }
      state->HI = (uint32_t)(result >> 32);
      state->LO = (uint32_t)(result);
      state->pc = state->pcNext;
      state->pcNext += 4;
      negate = false;
      break;
    case(0x1A): //DIV
      if((source1 >> 31) == 1){
        source1 = (uint32_t)twosComp(source1);
        negate = true;
        negateSource1 = true;
      }
      if((source2 >> 31) == 1){
        source2 = (uint32_t)twosComp(source2);
        if(negate){
          negate = false;
        }
        else{
          negate = true;
        }
      }
      //no break, so normal divu is carried out
    case(0x1B): //DIVU
      if(source2 != 0){
        result = source1 / source2;
        if(negate){
          result = twosComp((uint32_t)result);
        }
        state->LO = (uint32_t)(result);
        result = source1%source2;
        if(negateSource1){
          result = ~result + 1;
        }
        state->HI = (uint32_t)result;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      negate = false;
      break;
    case(0x20): //ADD
      result = source1 + source2;
      if( ((source1 >> 31) == 1) && ((source2 >> 31) == 1) ){ //overflow could occur
        if(((result>>31)&0b1) == 0){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      else if( ((source1 >> 31) == 0) && ((source2 >> 31) == 0) ){
        if(((result>>31)&0b1) == 1){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      state->regs[dest] = (uint32_t)result;
      break;
    case(0x21): //ADDU
      //std::cout << "ADDU" << std::endl;
      result = source1 + source2;
      state->regs[dest] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x22): //SUB
      result = source1 - source2;
      if( ((source1 >> 31) == 1) && ((source2 >> 31) == 0) ){ //overflow could occur
        if(((result>>31)&0b1) == 0){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      else if( ((source1 >> 31) == 0) && ((source2 >> 31) == 1) ){
        if(((result>>31)&0b1) == 1){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      state->regs[dest] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x23): //SUBU
      state->regs[dest] = (uint32_t)(source1 - source2);
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x24): //AND
      state->regs[dest] = (uint32_t)(source1 & source2);
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x25): //OR
      state->regs[dest] = (uint32_t)(source1 | source2);
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x26): //XOR
      state->regs[dest] = (uint32_t)(source1 ^ source2);
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x2A): //SLT
      if(((source1>>31)==1) && ((source2>>31)==0)){
        state->regs[dest] = 1;
        break;
      }
      else if(((source1>>31)==0) && ((source2>>31)==1)){
        state->regs[dest] = 0;
        break;
      }
      else if(((source1>>31)==1) && ((source2>>31)==1)){
        source1 = source1|0xFFFFFFFF00000000;
        source2 = source2|0xFFFFFFFF00000000;
      }
      //no break statement so it carries on to execute next bit
    case(0x2B): //SLTU
      if(source1 < source2){
        state->regs[dest] = 1;
      }
      else{
        state->regs[dest] = 0;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
  }
  return mips_Success;
}

mips_error jInstruction(mips_cpu_h state, uint8_t *data, uint8_t opcode){
  //extract from instruction
  uint32_t address = ((data[0]&0b11) << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
  //address to jump to
  address = (state->pcNext & 0xF0000000)|(address << 2);
  switch(opcode){
    case(0x03): //JAL
      state->regs[31] = (state->pc + 8);
      //no break so performs standard J instruction too
    case(0x02): //J
      state->pc = state->pcNext;
      state->pcNext = address;
      break;
  }
  return mips_Success;
}

mips_error iInstruction(mips_cpu_h state, uint8_t *data, uint8_t opcode){
  //std::cout << "opcode: " << std::bitset<8>(opcode) << std::endl;
  uint8_t source1Address = (((data[0]&0b11) << 3)|((data[1]&0b11100000) >> 5));
  //std::cout << "source1: " << std::bitset<8>(source1Address) << std::endl;
  uint8_t source2Address = (data[1]&0b11111); //remove extra bits
  //std::cout << "source2: " << std::bitset<8>(source2Address) << std::endl;
  uint64_t immediate = (((uint16_t)data[2]) << 8)|data[3];
  //std::cout << std::bitset<16>(immediate) << std::endl;
  uint64_t source1 = state->regs[source1Address];
  //std::cout << "source1: " << std::bitset<32>(source1) << std::endl;
  uint64_t source2 = state->regs[source2Address];
  //std::cout << "source2: " << std::bitset<8>(source2Address) << std::endl;
  uint64_t result;
  mips_error error;
  uint32_t address;
  unsigned count;
  uint8_t dataToMem[4];
  for(unsigned i = 0; i<4; i++){
    dataToMem[i] = 0;
  }
  //std::cout << "I: " << std::bitset<32>(immediate) << std::endl;
  //std::cout << std::endl;

  switch(opcode){
    case(0x01): //ONE OPERAND BRANCHES
      switch(source2Address){
        case(0x10): //BLTZAL
          state->regs[31] = (state->pc + 8);
        case(0x00): //BLTZ
          state->pc = state->pcNext;
          if((source1 >> 31) == 1){
            if((immediate >> 15) == 1){
              immediate = immediate | 0xFFFF0000;
              immediate = ~immediate + 1;
              result = state->pcNext - (immediate << 2);
            }
            else{
              result = state->pcNext + (immediate << 2);
            }
            state->pcNext = result;
          }
          else{
            state->pcNext += 4;
          }
          break;
        case(0x11): //BGEZAL
          state->regs[31] = (state->pc + 8);
        case(0x01): //BGEZ
          state->pc = state->pcNext;
          if((source1 >> 31) == 0){
            if((immediate >> 15) == 1){
              immediate = immediate | 0xFFFF0000;
              immediate = ~immediate + 1;
              result = state->pcNext - (immediate << 2);
            }
            else{
              result = state->pcNext + (immediate << 2);
            }
            state->pcNext = result;
          }
          else{
            state->pcNext += 4;
          }
          break;
      }
      break;
    case(0x04): //BEQ
      state->pc = state->pcNext;
      if(source1 == source2){
        if((immediate >> 15) == 1){
          immediate = immediate | 0xFFFF0000;
          immediate = twosComp((uint32_t)immediate);
          result = state->pcNext - (immediate << 2);
        }
        else{
          result = state->pcNext + (immediate << 2);
        }
        state->pcNext = result;
      }
      else{
        state->pcNext += 4;
      }
      break;
    case(0x05): //BNE
      state->pc = state->pcNext;
      if(source1 != source2){
        if((immediate >> 15) == 1){
          immediate = immediate | 0xFFFF0000;
          immediate = twosComp((uint32_t)immediate);
          result = state->pcNext - (immediate << 2);
        }
        else{
          result = state->pcNext + (immediate << 2);
        }
        state->pcNext = result;
      }
      else{
        state->pcNext += 4;
      }
      break;
    case(0x06): //BLEZ
      state->pc = state->pcNext;
      if( ((source1 >> 31) == 1) || (source1 == 0) ){
        if((immediate >> 15) == 1){
          immediate = immediate | 0xFFFF0000;
          immediate = twosComp((uint32_t)immediate);
          result = state->pcNext - (immediate << 2);
        }
        else{
          result = state->pcNext + (immediate << 2);
        }
        state->pcNext = result;
      }
      else{
        state->pcNext += 4;
      }
      break;
    case(0x07): //BGTZ
      state->pc = state->pcNext;
      if( ((source1 >> 31) == 0) && (source1 != 0) ){
        if((immediate >> 15) == 1){
          immediate = immediate | 0xFFFF0000;
          immediate = twosComp((uint32_t)immediate);
          result = state->pcNext - (immediate << 2);
        }
        else{
          result = state->pcNext + (immediate << 2);
        }
        state->pcNext = result;
      }
      else{
        state->pcNext += 4;
      }
      break;
    case(0x08): //ADDI
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = (uint32_t)twosComp(immediate);
        result = source1 - immediate;
        if( ((result >> 31) == 0) && ((source1 >> 31) == 1) ){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      else{
        result = source1 + immediate;
        if( ((result >> 31) == 1) && ((source1 >> 31) == 0) ){
          return mips_ExceptionArithmeticOverflow;
        }
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      state->regs[source2Address] = (uint32_t)result;
      break;
    case(0x09): //ADDIU
      if((immediate >> 15) == 1){
        immediate = (uint16_t)twosComp(immediate);
        result = source1 - immediate;
      }
      else{
        result = source1 + immediate;
      }
      state->regs[source2Address] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x0C): //ANDI
      result = source1 & (uint16_t)immediate;
      state->regs[source2Address] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x0D): //ORI
      result = source1 | (uint16_t)immediate;
      state->regs[source2Address] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x0E): //XORI
      result = source1 ^ (uint16_t)immediate;
      state->regs[source2Address] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x20): //LB
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      error = mips_mem_read(state->mem, address, 1, dataToMem);
      if(error != mips_Success){
        return error;
      }
      if((dataToMem[0] >> 7) == 0){
        state->regs[source2Address] = dataToMem[0];
      }
      else{
        state->regs[source2Address] = 0xFFFFFF00 | dataToMem[0];
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x21): //LH
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = ~immediate + 1;
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      error = mips_mem_read(state->mem, address, 2, dataToMem);
      if(error != mips_Success){
        return error;
      }
      if( (dataToMem[0] >> 7) == 0){
        result = (uint16_t)(dataToMem[0] << 8) | dataToMem[1];
        state->regs[source2Address] = (uint32_t)result;
      }
      else{
        result = 0xFFFF0000 | (uint16_t)(dataToMem[0] << 8) | dataToMem[1];
        state->regs[source2Address] = (uint32_t)result;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x22): //LWL
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        address = (uint32_t)source1 + (uint32_t)immediate;
      }
      else{
        address = (uint32_t)source1 + (uint32_t)immediate;
      }
      result = 0;
      count = 24;
      do{
        error = mips_mem_read(state->mem, address, 1, dataToMem);
        result = result | (dataToMem[0] << count);
        count = count - 8;
        address++;
      }while((address % 4) != 0);
      if(count == 16){
        state->regs[source2Address] = (source2 & 0x00FFFFFF) | result;
      }
      else if(count == 8){
        state->regs[source2Address] = (source2 & 0x0000FFFF) | result;
      }
      else if(count == 0){
        state->regs[source2Address] = (source2 & 0x000000FF) | result;
      }
      else{
        state->regs[source2Address] = result;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x23): //LW
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      error = mips_mem_read(state->mem, address, 4, dataToMem);
      if(error != mips_Success){
        return error;
      }
      state->regs[source2Address] = (dataToMem[0] << 24) | (dataToMem[1] << 16) | (dataToMem[2] << 8) | dataToMem[3];
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x24): //LBU
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      error = mips_mem_read(state->mem, address, 1, dataToMem);
      if(error != mips_Success){
        return error;
      }
      state->regs[source2Address] = dataToMem[0];
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x25): //LHU
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      error = mips_mem_read(state->mem, address, 2, dataToMem);
      if(error != mips_Success){
        return error;
      }
      result = (uint16_t)(dataToMem[0] << 8) | dataToMem[1];
      state->regs[source2Address] = (uint32_t)result;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x26): //LWR
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        address = (uint32_t)source1 + (uint32_t)immediate;
      }
      else{
        address = source1 + immediate;
      }
      result = 0;
      count = 0;
      do{
        error = mips_mem_read(state->mem, address, 1, dataToMem);
        result = result | (dataToMem[0] << count);
        count = count + 8;
        address--;
      }while(((address+1) % 4) != 0);

      if(count == 8){
        state->regs[source2Address] = (source2 & 0xFFFFFF00) | result;
      }
      else if(count == 16){
        state->regs[source2Address] = (source2 & 0xFFFF0000) | result;
      }
      else if(count == 24){
        state->regs[source2Address] = (source2 & 0xFF000000) | result;
      }
      else{
        state->regs[source2Address] = result;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x28): //SB
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      dataToMem[0] = (uint8_t)(source2);
      error = mips_mem_write(state->mem, address,1,dataToMem);
      if(error != mips_Success){
        return error;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x29): //SH
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      dataToMem[0] = (uint8_t)(source2 >> 8);
      dataToMem[1] = (uint8_t)(source2);
      error = mips_mem_write(state->mem, address,2,dataToMem);
      if(error != mips_Success){
        return error;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x2B): //SW
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
        immediate = twosComp((uint32_t)immediate);
        address = source1 - immediate;
      }
      else{
        address = source1 + immediate;
      }
      dataToMem[0] = (uint8_t)(source2 >> 24);
      dataToMem[1] = (uint8_t)(source2 >> 16);
      dataToMem[2] = (uint8_t)(source2 >> 8);
      dataToMem[3] = (uint8_t)(source2);
      error = mips_mem_write(state->mem, address,4,dataToMem);
      if(error != mips_Success){
        return error;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x0A): //SLTI
      if(((source1>>31)==1) && ((immediate>>15)==0)){
        state->regs[source2Address] = 1;
        break;
      }
      else if(((source1>>31)==0) && ((immediate>>15)==1)){
        state->regs[source2Address] = 0;
        break;
      }
      else if(((source1>>31)==1) && ((immediate>>15)==1)){
        source1 = source1|0xFFFFFFFF00000000;
        immediate = immediate|0xFFFFFFFFFFFF0000;
      }
      //no break statement so it carries on to execute next bit
    case(0x0B): //SLTIU
      if((immediate >> 15) == 1){
        immediate = immediate | 0xFFFF0000;
      }
      if(source1 < immediate){
        state->regs[source2Address] = 1;
      }
      else{
        state->regs[source2Address] = 0;
      }
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    case(0x0F): //LUI
      immediate = immediate << 16;
      source2 = (uint32_t)immediate;
      state->regs[source2Address] = source2;
      state->pc = state->pcNext;
      state->pcNext += 4;
      break;
    return mips_ExceptionInvalidInstruction;
  }
  return mips_Success;
}

uint32_t twosComp(uint32_t a){
  return ~a + 1;
}
