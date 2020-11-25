#include "ysim.h"

#include "errors.h"

/************************** Utility Routines ****************************/

/** Return nybble from op (pos 0: least-significant; pos 1:
 *  most-significant)
 */
static Byte
get_nybble(Byte op, int pos) {
  return (op >> (pos * 4)) & 0xF;
}

/************************** Condition Codes ****************************/

/** Conditions used in instructions */
typedef enum {
  ALWAYS_COND, LE_COND, LT_COND, EQ_COND, NE_COND, GE_COND, GT_COND
} Condition;

/** accessing condition code flags */
static inline bool get_cc_flag(Byte cc, unsigned flagBitIndex) {
  return !!(cc & (1 << flagBitIndex));
}
static inline bool get_zf(Byte cc) { return get_cc_flag(cc, ZF_CC); }
static inline bool get_sf(Byte cc) { return get_cc_flag(cc, SF_CC); }
static inline bool get_of(Byte cc) { return get_cc_flag(cc, OF_CC); }

/** Return true iff the condition specified in the least-significant
 *  nybble of op holds in y86.  Encoding of Figure 3.15 of Bryant's
 *  CompSys3e.
 */
bool
check_cc(const Y86 *y86, Byte op)
{
  bool ret = false;
  Condition condition = get_nybble(op, 0);
  Byte cc = read_cc_y86(y86);
  switch (condition) {
  case ALWAYS_COND:
    ret = true;
    break;
  case LE_COND:
    ret = (get_sf(cc) ^ get_of(cc)) | get_zf(cc);
    break;
  case LT_COND:
    ret = (get_sf(cc) ^ get_of(cc));
    break;
  case EQ_COND:
    ret = get_zf(cc);
    break;
  case NE_COND:
    ret = !(get_zf(cc));
    break;
  case GT_COND:
    ret = !(get_sf(cc) ^ get_of(cc)) & !(get_zf(cc));
    break;
  case GE_COND:
    ret = !(get_sf(cc) ^ get_of(cc));
    break;
  //@TODO add other cases
  default: {
    Address pc = read_pc_y86(y86);
    fatal("%08lx: bad condition code %d\n", pc, condition);
    break;
    }
  }
  return ret;
}

/** return true iff word has its sign bit set */
static inline bool
isLt0(Word word) {
  return (word & (1UL << (sizeof(Word)*CHAR_BIT - 1))) != 0;
}

/** Set condition codes for addition operation with operands opA, opB
 *  and result with result == opA + opB.
 */
static void
set_add_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
  //@TODO
  if (result == 0)
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~(1<<ZF_CC));
  }
  if((isLt0(opA) == isLt0(opB)) && (isLt0(result) != isLt0(opA)))
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<OF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~(1<<OF_CC));
  }
}

/** Set condition codes for subtraction operation with operands opA, opB
 *  and result with result == opA - opB.
 */
static void
set_sub_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
  //@TODO
  if (result == 0)
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~(1<<ZF_CC));
  }
  if((isLt0(opA) != isLt0(opB)) && (isLt0(result) != isLt0(opA)))
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<OF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~(1<<OF_CC));
  }
}

static void
set_logic_op_cc(Y86 *y86, Word result)
{
  //@TODO
  if (result == 0)
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~((1<<ZF_CC)));
  }
  if(isLt0(result))
  {
    write_cc_y86(y86, read_cc_y86(y86) | (1<<SF_CC));
  }
  else
  {
    write_cc_y86(y86, read_cc_y86(y86) & ~((1<<SF_CC)));
  }
  write_cc_y86(y86, read_cc_y86(y86) & ~((1<<OF_CC)));
}

/**************************** Operations *******************************/

static void
op1(Y86 *y86, Byte op, Register regA, Register regB)
{
 enum {ADDL_FN, SUBL_FN, ANDL_FN, XORL_FN };
 //@TODO
  Word regAval = read_register_y86(y86, regA);
  Word regBval = read_register_y86(y86, regB);
  Word result;
  switch(get_nybble(op, 0)){
  case ADDL_FN:
  regAval = read_register_y86(y86, regA);
  regBval = read_register_y86(y86, regB);
  result = regAval + regBval;
  set_add_arith_cc(y86, regAval, regBval, result);
  break;
  case SUBL_FN:
  regAval = read_register_y86(y86, regA);
  regBval = read_register_y86(y86, regB);
  result = regAval - regBval;
  set_sub_arith_cc(y86, regAval, regBval, result);
  break;
  case ANDL_FN:
  regAval = read_register_y86(y86, regA);
  regBval = read_register_y86(y86, regB);
  result = regAval & regBval;
  set_logic_op_cc(y86, result);
  break;
  case XORL_FN:
  regAval = read_register_y86(y86, regA);
  regBval = read_register_y86(y86, regB);
  result = regAval ^ regBval;
  set_logic_op_cc(y86, result);
  write_register_y86(y86, regB, result);
  break;
  }
/*********************** Single Instruction Step ***********************/

typedef enum {
  HALT_CODE, NOP_CODE, CMOVxx_CODE, IRMOVQ_CODE, RMMOVQ_CODE, MRMOVQ_CODE,
  OP1_CODE, Jxx_CODE, CALL_CODE, RET_CODE,
  PUSHQ_CODE, POPQ_CODE } BaseOpCode;

/** Execute the next instruction of y86. Must change status of
 *  y86 to STATUS_HLT on halt, STATUS_ADR or STATUS_INS on
 *  bad address or instruction.
 */
void
step_ysim(Y86 *y86)
{
  //@TODO
  if(read_status_y86(y86) != STATUS_AOK) return;
  Address pc = read_pc_y86(y86);
  Byte instrCd = read_memory_byte_y86(y86, pc);
  
switch(get_nybble(instrCd, 1)){
	
	case HALT_CODE:
	write_status_y86(y86, STATUS_HLT);
	break;
	case NOP_CODE:
	write_pc_y86(y86, pc+=sizeof(Byte));
	break;
  case CMOVxx_CODE:
  {
	Byte reg = read_memory_byte_y86(y86, pc += sizeof(Byte));
	Register a = get_nybble(reg, 1);
	Register b = get_nybble(reg, 0);
	Word data_word = read_register_y86(y86, a);
	if(check_cc(y86, instrCd)) write_register_y86(y86, b, data_word);
	write_pc_y86(y86, pc += sizeof(Byte));
	break;
  }
	case IRMOVQ_CODE:
	{
	Word tmpWord = read_memory_word_y86(y86, pc + 1 + sizeof(Byte));
	write_register_y86(y86, get_nybble(read_memory_byte_y86(y86, pc + 1), 0), tmpWord);
	write_pc_y86(y86, pc + 1 + sizeof(Byte) + sizeof(Word));
	break;
	}
  case RMMOVQ_CODE:
  {
 	Byte reg = read_memory_byte_y86(y86, pc += sizeof(Byte));
	Register a = get_nybble(reg, 1);
	Register b = get_nybble(reg, 0);
	Address address = read_register_y86(y86, b) + read_memory_word_y86(y86, pc += sizeof(Byte));
	Word data_word = read_register_y86(y86, a);
	write_memory_word_y86(y86, address, data_word);
	write_pc_y86(y86, pc += sizeof(Word));
	break;
	}
  case MRMOVQ_CODE:
  {
	Byte reg = read_memory_byte_y86(y86, pc += sizeof(Byte));
	Register a = get_nybble(reg, 1);
	Register b = get_nybble(reg, 0);
	Address address = read_register_y86(y86, b) + read_memory_word_y86(y86, pc += sizeof(Byte));
	Word data_word = read_memory_word_y86(y86, address);
	write_register_y86(y86, a, data_word);
	write_pc_y86(y86, pc += sizeof(Word));
	break;
	}
  case OP1_CODE:
  {
  Byte reg = read_memory_byte_y86(y86, pc += sizeof(Byte));
	Register a = get_nybble(reg, 1);
	Register b = get_nybble(reg, 0);
	op1(y86, instrCd, a, b);
	write_pc_y86(y86, pc += sizeof(Byte));
	break;
  }
  case Jxx_CODE:
  {
    Word targ = read_memory_word_y86(y86, pc+sizeof(Byte));
    if (check_cc(y86, instrCd)) {
      write_pc_y86(y86, targ);
      break;
    }
    write_pc_y86(y86, pc+sizeof(Byte)+sizeof(Word));
    break;
  }
  case CALL_CODE:
	{
	Word fAddr = read_memory_word_y86(y86, pc += sizeof(Byte));
	Word stackAddr = read_register_y86(y86, REG_RSP);
	write_register_y86(y86, REG_RSP, stackAddr -= sizeof(Word));
	write_memory_word_y86(y86, stackAddr, pc += sizeof(Word));
	write_pc_y86(y86, fAddr);
	break;
	}
  case RET_CODE:
  {
	Address stackAddr = read_register_y86(y86, REG_RSP);
	Address ret = read_memory_word_y86(y86, stackAddr);
	write_register_y86(y86, REG_RSP, stackAddr += sizeof(Word));
	write_pc_y86(y86, ret);
	break;
	}
  case PUSHQ_CODE:
  {
	Byte regByte = read_memory_byte_y86(y86, pc+sizeof(Byte));
	Register reg = get_nybble(regByte, 1);
	Address stack_addr = read_register_y86(y86, REG_RSP);
	stack_addr -= sizeof(Address);
	Word reg_value = read_register_y86(y86, reg);
	write_register_y86(y86, REG_RSP, stack_addr);
	write_memory_word_y86(y86, stack_addr, reg_value);
	write_pc_y86(y86, pc + 2*sizeof(Byte));
	break;
  }
  case POPQ_CODE:
  {
	Byte regByte = read_memory_byte_y86(y86, pc+sizeof(Byte));
	Register reg = get_nybble(regByte, 1);
	Address stack_addr = read_register_y86(y86, REG_RSP);
	Word stack_value = read_memory_word_y86(y86, stack_addr);
	stack_addr += sizeof(Address);
	write_register_y86(y86, REG_RSP, stack_addr);
	write_register_y86(y86, reg, stack_value);
	write_pc_y86(y86, pc + 2*sizeof(Byte));
	break;
  }
 }
}
}