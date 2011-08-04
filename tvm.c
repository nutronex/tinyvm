#include "tvm.h"
#include "tvm_reg_idx.h"

tvm_t* create_vm(char* filename)
{
	tvm_t* vm = (tvm_t*)malloc(sizeof(tvm_t));

	vm->pMemory = create_memory(MIN_MEMORY_SIZE);
	if(!vm->pMemory) return NULL;

	create_stack(vm->pMemory, MIN_STACK_SIZE);

	vm->pProgram = create_program();
	if(!vm->pProgram) return NULL;

	if(interpret_program(vm->pProgram, filename, vm->pMemory) != 0)
		return NULL;

	return vm;
}

void destroy_vm(tvm_t* vm)
{
	if(vm)
	{
		if(vm->pMemory) destroy_memory(vm->pMemory);
		if(vm->pProgram) destroy_program(vm->pProgram);

		free(vm);
	}
}

void run_vm(tvm_t* vm)
{
	int* instr_idx = &vm->pMemory->registers[IP].i32;
	*instr_idx = vm->pProgram->start;

	for(;vm->pProgram->instr[*instr_idx] != END; ++(*instr_idx))
	{
		int *arg0 = vm->pProgram->args[*instr_idx][0], *arg1 = vm->pProgram->args[*instr_idx][1];

		switch(vm->pProgram->instr[*instr_idx])
		{
			case MOV:  *arg0 = *arg1; break;
			case PUSH: stack_push(vm->pMemory, arg0); break;
			case POP:  stack_pop(vm->pMemory, arg0); break;
			case PUSHF: stack_push(vm->pMemory, &vm->pMemory->FLAGS); break;
			case POPF:  stack_pop(vm->pMemory, arg0); break;
			case INC:  ++(*arg0); break;
			case DEC:  --(*arg0); break;
			case ADD:  *arg0 += *arg1; break;
			case SUB:  *arg0 -= *arg1; break;
			case MUL:  *arg0 *= *arg1; break;
			case DIV:  *arg0 /= *arg1; break;
			case MOD:  vm->pMemory->remainder = *arg0 % *arg1; break;
			case REM:  *arg0 = vm->pMemory->remainder; break;
			case NOT:  *arg0 = ~(*arg0); break;
			case XOR:  *arg0 ^= *arg1;   break;
			case OR:   *arg0 |= *arg1;   break;
			case AND:  *arg0 &= *arg1;   break;
			case SHL:  *arg0 <<= *arg1;  break;
			case SHR:  *arg0 >>= *arg1;  break;
			case CMP:  vm->pMemory->FLAGS = ((*arg0 == *arg1) | (*arg0 > *arg1) << 1); break;
			case JMP:  *instr_idx = *arg0 - 1; break;
			case JE:   if(vm->pMemory->FLAGS   & 0x1)  *instr_idx = *arg0 - 1; break;
			case JNE:  if(!(vm->pMemory->FLAGS & 0x1)) *instr_idx = *arg0 - 1; break;
			case JG:   if(vm->pMemory->FLAGS   & 0x2)  *instr_idx = *arg0 - 1; break;
			case JGE:  if(vm->pMemory->FLAGS   & 0x3)  *instr_idx = *arg0 - 1; break;
			case JL:   if(!(vm->pMemory->FLAGS & 0x3)) *instr_idx = *arg0 - 1; break;
			case JLE:  if(!(vm->pMemory->FLAGS & 0x2)) *instr_idx = *arg0 - 1; break;
		};
	}
}
