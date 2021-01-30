// List of opcodes 

OPCODE(NOP);

// dec
OPCODE(DEX);
OPCODE(DEY);
OPCODE(SBC);

// inc
OPCODE(INX);
OPCODE(INY);
OPCODE(ADC);

// load
OPCODE(LDA);
OPCODE(LDX);
OPCODE(LDY);

// store
OPCODE(STA);

// transfer
OPCODE(TAX);
OPCODE(TAY);
OPCODE(TXA);
OPCODE(TYA);
OPCODE(TSX);
OPCODE(TXS);

// shift
OPCODE(LSR);
OPCODE(ASL);
OPCODE(ROL);
OPCODE(ROR);

// status
OPCODE(CLC);
OPCODE(SEC);
OPCODE(SEI);
OPCODE(CLI);
OPCODE(CLV);

// stack
OPCODE(PHA);
OPCODE(PHP);
OPCODE(PLA);
OPCODE(PLP);

// compare
OPCODE(CMP);
OPCODE(CPX);
OPCODE(CPY);
OPCODE(BIT);

// logic
OPCODE(EOR);
OPCODE(AND);
OPCODE(ORA);

// Branch
OPCODE(BCC);
OPCODE(BCS);
OPCODE(BEQ);
OPCODE(BMI);
OPCODE(BNE);
OPCODE(BPL);
OPCODE(BVC);
OPCODE(BVS);
OPCODE(JMP);

// subroutine
OPCODE(JSR);
OPCODE(RTS);
