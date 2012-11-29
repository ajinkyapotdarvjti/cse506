/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>
#include <kern/macro.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushq $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushq $0;							\
	pushq $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
   Find the PUSHA,POPA macros in kern/macro.h. Since amd64 doesn't support
   pusha,popa so you will find these macros handy.
 */
TRAPHANDLER_NOEC(trap_divide, T_DIVIDE);
TRAPHANDLER_NOEC(trap_debug, T_DEBUG);
TRAPHANDLER_NOEC(trap_nmi, T_NMI);
TRAPHANDLER_NOEC(trap_brkpt, T_BRKPT);
TRAPHANDLER_NOEC(trap_oflow, T_OFLOW);
TRAPHANDLER_NOEC(trap_bound, T_BOUND);
TRAPHANDLER_NOEC(trap_illop, T_ILLOP);
TRAPHANDLER_NOEC(trap_device, T_DEVICE);
TRAPHANDLER(trap_dblflt, T_DBLFLT);
//TRAPHANDLER_NOEC(trap_coproc, T_DBLFLT); //Reserved in inc/trap.h, so skip it in kern/trap.c
TRAPHANDLER(trap_tss, T_TSS);
TRAPHANDLER(trap_segnp, T_SEGNP);
TRAPHANDLER(trap_stack, T_STACK);
TRAPHANDLER(trap_gpflt, T_GPFLT);
TRAPHANDLER(trap_pgflt, T_PGFLT);
//TRAPHANDLER_NOEC(trap_res, T_PGFLT); //Reserved in inc/trap.h, so skip it in kern/trap.c
TRAPHANDLER_NOEC(trap_fperr, T_FPERR);
TRAPHANDLER_NOEC(trap_align, T_ALIGN);
TRAPHANDLER_NOEC(trap_mchk, T_MCHK);
TRAPHANDLER_NOEC(trap_simderr, T_SIMDERR);
TRAPHANDLER_NOEC(trap_syscall, T_SYSCALL);

TRAPHANDLER_NOEC(int_nr_0, IRQ_OFFSET);
TRAPHANDLER_NOEC(int_nr_1, IRQ_OFFSET+1);
TRAPHANDLER_NOEC(int_nr_2, IRQ_OFFSET+2);
TRAPHANDLER_NOEC(int_nr_3, IRQ_OFFSET+3);
TRAPHANDLER_NOEC(int_nr_4, IRQ_OFFSET+4);
TRAPHANDLER_NOEC(int_nr_5, IRQ_OFFSET+5);
TRAPHANDLER_NOEC(int_nr_6, IRQ_OFFSET+6);
TRAPHANDLER_NOEC(int_nr_7, IRQ_OFFSET+7);
TRAPHANDLER_NOEC(int_nr_8, IRQ_OFFSET+8);
TRAPHANDLER_NOEC(int_nr_9, IRQ_OFFSET+9);
TRAPHANDLER_NOEC(int_nr_10, IRQ_OFFSET+10);
TRAPHANDLER_NOEC(int_nr_11, IRQ_OFFSET+11);
TRAPHANDLER_NOEC(int_nr_12, IRQ_OFFSET+12);
TRAPHANDLER_NOEC(int_nr_13, IRQ_OFFSET+13);
TRAPHANDLER_NOEC(int_nr_14, IRQ_OFFSET+14);
TRAPHANDLER_NOEC(int_nr_15, IRQ_OFFSET+15);


/*
 * Lab 3: Your code here for _alltraps
 */

.global _alltraps;
.type _alltraps, @function;
.align 2;
_alltraps:

#movl  %ds, %eax 
#pushw %ax
#movl %es, %eax
#pushw %ax
#PUSHA

#movl $GD_KD, %eax
#mov %ax,%ds
#mov %ax, %es

#movq  %rsp, %rax
#pushw %rsp
pushq $GD_KD
pushq $GD_KD
PUSHA
movl %esp, %edi

call trap

