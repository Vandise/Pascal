#include <stdio.h>
#include "ch12/symbol_table/symbol_table.h"
#include "ch12/code/code.h"

int label_index = 0;

char asm_buffer[MAX_PRINT_LINE_LENGTH];     /* assembly stmt buffer */
char *asm_bufferp = asm_buffer;             /* ptr into asm buffer */

char *register_strings[] = {
    "ax", "ah", "al", "bx", "bh", "bl", "cx", "ch", "cl",
    "dx", "dh", "dl", "cs", "ds", "es", "ss",
    "sp", "bp", "si", "di",
};

char *instruction_strings[] = {
    "mov", "rep\tmovsb", "lea", "xchg", "cmp", "repe\tcmpsb",
    "pop", "push", "and", "or", "xor",
    "neg", "inc", "dec", "add", "sub", "imul", "idiv",
    "cld", "call", "ret",
    "jmp", "jl", "jle", "je", "jne", "jge", "jg",
};

void label(char *prefix, int index) {
    sprintf(asm_bufferp, "%s_%03d", prefix, index);
    advance_asm_bufferp();
}

void word_label(char *prefix, int index) {
    sprintf(asm_bufferp, "WORD PTR %s_%03d", prefix, index);
    advance_asm_bufferp();
}

void high_dword_label(char *prefix, int index) {
    sprintf(asm_bufferp, "WORD PTR %s_%03d+2", prefix, index);
    advance_asm_bufferp();
}

void reg(REGISTER r) {
    sprintf(asm_bufferp, "%s", register_strings[r]);
    advance_asm_bufferp();
}

void operator(INSTRUCTION opcode) {
    sprintf(asm_bufferp, "\t%s", instruction_strings[opcode]);
    advance_asm_bufferp();
}

void byte(SYMTAB_NODE_PTR idp) {
    sprintf(asm_bufferp, "BYTE PTR %s_%03d",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

void byte_indirect(REGISTER r) {
    sprintf(asm_bufferp, "BYTE PTR [%s]", register_strings[r]);
    advance_asm_bufferp();
}

void word(SYMTAB_NODE_PTR idp) {
    sprintf(asm_bufferp, "WORD PTR %s_%03d",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

void high_dword(SYMTAB_NODE_PTR idp) {
    sprintf(asm_bufferp, "WORD PTR %s_%03d+2",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

void word_indirect(REGISTER r) {
    sprintf(asm_bufferp, "WORD PTR [%s]", register_strings[r]);
    advance_asm_bufferp();
}

void high_dword_indirect(REGISTER r) {
    sprintf(asm_bufferp, "WORD PTR [%s+2]", register_strings[r]);
    advance_asm_bufferp();
}

void tagged_name(SYMTAB_NODE_PTR idp) {
    sprintf(asm_bufferp, "%s_%03d", idp->name, idp->label_index);
    advance_asm_bufferp();
}

void name_lit(char *name) {
    sprintf(asm_bufferp, "%s", name);
    advance_asm_bufferp();
}

void integer_lit(int n) {
    sprintf(asm_bufferp, "%d", n);
    advance_asm_bufferp();
}

void char_lit(char ch) {
    sprintf(asm_bufferp, "'%c'", ch);
    advance_asm_bufferp();
}