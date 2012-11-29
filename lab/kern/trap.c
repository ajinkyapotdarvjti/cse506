#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/spinlock.h>
#include <kern/time.h>

extern uintptr_t gdtdesc_64;
static struct Taskstate ts;
extern struct Segdesc gdt[];
extern long gdt_pd;

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { { 0 } };
struct Pseudodesc idt_pd = {0,0};
extern uint32_t traphandler[];

static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < sizeof(excnames)/sizeof(excnames[0]))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}


void
trap_init(void)
{
	extern struct Segdesc gdt[];

	// LAB 3: Your code here.
	idt_pd.pd_lim = sizeof(idt)-1;
	idt_pd.pd_base = (uint64_t)idt;

	extern void trap_divide();
        extern void trap_debug();
        extern void trap_nmi();
        extern void trap_brkpt();
        extern void trap_oflow();
        extern void trap_bound();
        extern void trap_illop();
        extern void trap_device();
        extern void trap_dblflt();
        //extern void trap_coproc();
        extern void trap_tss();
        extern void trap_segnp();
        extern void trap_stack();
        extern void trap_gpflt();
        extern void trap_pgflt();
        //extern void trap_res();
        extern void trap_fperr();
        extern void trap_align();
        extern void trap_mchk();
        extern void trap_simderr();
        extern void trap_syscall();

	//aj-code
	extern void int_nr_0();
	extern void int_nr_1();
	extern void int_nr_2();
 	extern void int_nr_3();
	extern void int_nr_4();
 	extern void int_nr_5();
 	extern void int_nr_6();
 	extern void int_nr_7();
	extern void int_nr_8();
 	extern void int_nr_9();
 	extern void int_nr_10();
 	extern void int_nr_11();
 	extern void int_nr_12();
 	extern void int_nr_13();
 	extern void int_nr_14();
	extern void int_nr_15();

	SETGATE(idt[T_DIVIDE], 1, GD_KT, trap_divide, 0);
        SETGATE(idt[T_DEBUG], 0, GD_KT, trap_debug, 0);
        SETGATE(idt[T_NMI], 0, GD_KT, trap_nmi, 0);
        SETGATE(idt[T_BRKPT], 0, GD_KT, trap_brkpt, 3);
        SETGATE(idt[T_OFLOW], 1, GD_KT, trap_oflow, 3);
        SETGATE(idt[T_BOUND], 1, GD_KT, trap_bound, 3);
        SETGATE(idt[T_ILLOP], 1, GD_KT, trap_illop, 3);
        SETGATE(idt[T_DEVICE], 1, GD_KT, trap_device, 3);
        SETGATE(idt[T_DBLFLT], 1, GD_KT, trap_dblflt, 3);
        //SETGATE(idt[T_COPROC], 0, GD_KT, trap_coproc, 0);
        SETGATE(idt[T_TSS], 1, GD_KT, trap_tss, 3);
        SETGATE(idt[T_SEGNP], 1, GD_KT, trap_segnp, 3);
        SETGATE(idt[T_STACK], 1, GD_KT, trap_stack, 3);
        SETGATE(idt[T_GPFLT], 1, GD_KT, trap_gpflt, 3);
        SETGATE(idt[T_PGFLT], 0, GD_KT, trap_pgflt, 3);
        //SETGATE(idt[T_RES], 0, GD_KT, trap_res, 0);
        SETGATE(idt[T_FPERR], 1, GD_KT, trap_fperr, 3);
        SETGATE(idt[T_ALIGN], 1, GD_KT, trap_align, 3);
        SETGATE(idt[T_MCHK], 1, GD_KT, trap_mchk, 3);
        SETGATE(idt[T_SIMDERR], 1, GD_KT, trap_simderr, 3);

	//aj-code
	SETGATE(idt[IRQ_OFFSET], 0, GD_KT, int_nr_0, 0)
	SETGATE(idt[IRQ_OFFSET+1], 0, GD_KT, int_nr_1, 0)
	SETGATE(idt[IRQ_OFFSET+2], 0, GD_KT, int_nr_2, 0)
	SETGATE(idt[IRQ_OFFSET+3], 0, GD_KT, int_nr_3, 0)
	SETGATE(idt[IRQ_OFFSET+4], 0, GD_KT, int_nr_4, 0)
	SETGATE(idt[IRQ_OFFSET+5], 0, GD_KT, int_nr_5, 0)
	SETGATE(idt[IRQ_OFFSET+6], 0, GD_KT, int_nr_6, 0)
	SETGATE(idt[IRQ_OFFSET+7], 0, GD_KT, int_nr_7, 0)
	SETGATE(idt[IRQ_OFFSET+8], 0, GD_KT, int_nr_8, 0)
	SETGATE(idt[IRQ_OFFSET+9], 0, GD_KT, int_nr_9, 0)
	SETGATE(idt[IRQ_OFFSET+10], 0, GD_KT, int_nr_10,0)
	SETGATE(idt[IRQ_OFFSET+11], 0, GD_KT, int_nr_11,0)
	SETGATE(idt[IRQ_OFFSET+12], 0, GD_KT, int_nr_12,0)
	SETGATE(idt[IRQ_OFFSET+13], 0, GD_KT, int_nr_13,0)
	SETGATE(idt[IRQ_OFFSET+14], 0, GD_KT, int_nr_14,0)
	SETGATE(idt[IRQ_OFFSET+15], 0, GD_KT, int_nr_15,0)
      //Initial system call entry
        SETGATE(idt[T_SYSCALL], 0, GD_KT, trap_syscall, 3);

	trap_init_percpu();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu(void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct Cpu;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + 2*i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:

	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	thiscpu->cpu_ts.ts_esp0 = KSTACKTOP - thiscpu->cpu_id * (KSTKSIZE + KSTKGAP);

	// Initialize the TSS slot of the gdt.
	SETTSS((struct SystemSegdesc64 *)((gdt_pd>>16)+40+ 2*8*thiscpu->cpu_id),STS_T64A, (uint64_t) (&thiscpu->cpu_ts),sizeof(struct Taskstate), 0);
	// Load the TSS selector (like other segment selectors, the
	// bottom three bits are special; we leave them 0)
	ltr(GD_TSS0 + 2*8*(thiscpu->cpu_id));

	// Load the IDT
	lidt(&idt_pd);
}

void
print_trapframe(struct Trapframe *tf)
{
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  rip  0x%08x\n", tf->tf_rip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  rsp  0x%08x\n", tf->tf_rsp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs(struct PushRegs *regs)
{
	cprintf("  r15  0x%08x\n", regs->reg_r15);
	cprintf("  r14  0x%08x\n", regs->reg_r14);
	cprintf("  r13  0x%08x\n", regs->reg_r13);
	cprintf("  r12  0x%08x\n", regs->reg_r12);
	cprintf("  r11  0x%08x\n", regs->reg_r11);
	cprintf("  r10  0x%08x\n", regs->reg_r10);
	cprintf("  r9  0x%08x\n", regs->reg_r9);
	cprintf("  r8  0x%08x\n", regs->reg_r8);
	cprintf("  rdi  0x%08x\n", regs->reg_rdi);
	cprintf("  rsi  0x%08x\n", regs->reg_rsi);
	cprintf("  rbp  0x%08x\n", regs->reg_rbp);
	cprintf("  rbx  0x%08x\n", regs->reg_rbx);
	cprintf("  rdx  0x%08x\n", regs->reg_rdx);
	cprintf("  rcx  0x%08x\n", regs->reg_rcx);
	cprintf("  rax  0x%08x\n", regs->reg_rax);
}

static void
trap_dispatch(struct Trapframe *tf)
{
	// Handle processor exceptions.
	// LAB 3: Your code here.

	// Handle spurious interrupts
	// The hardware sometimes raises these because of noise on the
	// IRQ line or other reasons. We don't care.
	if (tf->tf_trapno == IRQ_OFFSET + IRQ_SPURIOUS) {
		cprintf("Spurious interrupt on irq 7\n");
		print_trapframe(tf);
		return;
	}

	// Handle clock interrupts. Don't forget to acknowledge the
	// interrupt using lapic_eoi() before calling the scheduler!
	// LAB 4: Your code here.

	// Add time tick increment to clock interrupts.
	// Be careful! In multiprocessors, clock interrupts are
	// triggered on every CPU.
	// LAB 6: Your code here.


	// Handle keyboard and serial interrupts.
	// LAB 7: Your code here.

	if (tf->tf_trapno == IRQ_OFFSET+IRQ_KBD) {
		kbd_intr();
		return;
	}

	if (tf->tf_trapno == IRQ_OFFSET+IRQ_SERIAL) {
		serial_intr();
		return;
	}

	// Unexpected trap: The user process or the kernel has a bug.
	switch((int)tf->tf_trapno){
        	case T_DIVIDE:
            		divide_error_handler(tf);
            		return;
        	case T_PGFLT:
			//cprintf("Page fault occuring at %0x\n", rcr2());
            		page_fault_handler(tf);
            		return;
        	case T_BRKPT:
            		breakpoint_handler(tf);
			return;
        	case IRQ_OFFSET + IRQ_TIMER: 
			time_tick();
			lapic_eoi();
			sched_yield();
			return;
		case T_SYSCALL:
			//cprintf("calling system calls with parameters: %0x, %0x \n", tf->tf_regs.reg_rdx, tf->tf_regs.reg_rcx);
                        tf->tf_regs.reg_rax = syscall(tf->tf_regs.reg_rax,   
						tf->tf_regs.reg_rdx,
                    				tf->tf_regs.reg_rcx,
                    				tf->tf_regs.reg_rbx,
                    				tf->tf_regs.reg_rdi,
                    				tf->tf_regs.reg_rsi);
		

			return;
    	};

	//aj-code
        if (tf->tf_trapno >= IRQ_OFFSET && tf->tf_trapno < IRQ_OFFSET + 16) {
		//cprintf("IRQ_OFFSET, tf->tf_trapno = %d\n", tf->tf_trapno);
		lapic_eoi();
		sched_yield();
	}

	// Unexpected trap: The user process or the kernel has a bug.
	print_trapframe(tf);
	if (tf->tf_cs == GD_KT)
		panic("unhandled trap in kernel");
	else {
		env_destroy(curenv);
		return;
	}
}

void
trap(struct Trapframe *tf)
{

        //struct Trapframe *tf = &tf_;
	// The environment may have set DF and some versions
	// of GCC rely on DF being clear
	asm volatile("cld" ::: "cc");

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");

	// Check that interrupts are disabled.  If this assertion
	// fails, DO NOT be tempted to fix it by inserting a "cli" in
	// the interrupt path.
	//cprintf("trap number = %d\n", tf->tf_trapno);
	assert(!(read_eflags() & FL_IF));

	if ((tf->tf_cs & 3) == 3) {
		//cprintf("Trap from user mode\n");
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.
		lock_kernel();
		assert(curenv);

		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;

		//unlock_kernel();
	}

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	int oldsystemcallno = tf->tf_regs.reg_rax;
	//cprintf("before dispath cs = %0x, rip = %0x, trap number = %d, system call number = %d, env = %d\n", tf->tf_cs,tf->tf_rip, tf->tf_trapno, tf->tf_regs.reg_rax, (thiscpu->cpu_env-envs));
	// Dispatch based on what type of trap occurred
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.

	//cprintf("after dispath cs = %0x, rip = %0x, trap number = %d, system call number = %d, env = %d\n", tf->tf_cs,tf->tf_rip, tf->tf_trapno, tf->tf_regs.reg_rax, (thiscpu->cpu_env-envs));
	//cprintf("after dispath rip = %0x\n", curenv->env_tf.tf_rip);

	if (curenv && curenv->env_status == ENV_RUNNING)
		env_run(curenv);
	else
		sched_yield();
}

void
divide_error_handler(struct Trapframe *tf)
{
	// Destroy the environment that caused the fault.
	cprintf("[%08x] user divide error ip %08x\n", curenv->env_id, tf->tf_rip);
	print_trapframe(tf);
	env_destroy(curenv);
}

void
page_fault_handler(struct Trapframe *tf)
{
	uint64_t fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();

	// Handle kernel-mode page faults.

	// LAB 3: Your code here.
	if (tf->tf_cs == 0x8) {
        	
		//page_insert(boot_pml4e, page_alloc(ALLOC_ZERO), (void *)fault_va, PTE_P|PTE_W);
		//return;
		panic("page_fault_handler: page fault in kernel mode!");
	}

	// We've already handled kernel-mode exceptions, so if we get here,
	// the page fault happened in user mode.

	// Call the environment's page fault upcall, if one exists.  Set up a
	// page fault stack frame on the user exception stack (below
	// UXSTACKTOP), then branch to curenv->env_pgfault_upcall.

        //cprintf("utf = %08x, sizeof(struct UTrapframe) = %08x\n", utf, sizeof(struct UTrapframe));
        //print_regs(&(tf->tf_regs));

			
	if(curenv->env_pgfault_upcall != NULL){

		uintptr_t stktop = UXSTACKTOP;

		//page_insert(curenv->env_pml4e, page_alloc(ALLOC_ZERO), (void *)UXSTACKTOP, PTE_U|PTE_W|PTE_P);
          	if (tf->tf_rsp < UXSTACKTOP && tf->tf_rsp >= UXSTACKTOP-PGSIZE)
            		stktop = tf->tf_rsp - sizeof(uintptr_t);
	
          	stktop -= sizeof(struct UTrapframe);
		// what if the user program runs out of space for exception stack
		// i.e. sktop < UXSTACKTOP-PGSIZE
		// i think we need to allocate a new page in that case - Siddhesh

		struct UTrapframe* utf = (struct UTrapframe*)(stktop);
		//cprintf("curenv->env_pml4e = %0x, fault_va = %0x\n", curenv->env_pml4e, fault_va);
		user_mem_assert(curenv, utf, sizeof (struct UTrapframe), PTE_W);
        	utf->utf_rsp = tf->tf_rsp;
        	utf->utf_eflags = tf->tf_eflags;
       	 	utf->utf_rip = tf->tf_rip;
        	utf->utf_regs = tf->tf_regs;
        	utf->utf_err = tf->tf_err;
        	utf->utf_fault_va = fault_va;

        	tf->tf_rip = (uintptr_t)curenv->env_pgfault_upcall;
        	tf->tf_rsp = (uintptr_t)utf;

        	env_run(curenv);
    	}
	
	//
	// The page fault upcall might cause another page fault, in which case
	// we branch to the page fault upcall recursively, pushing another
	// page fault stack frame on top of the user exception stack.
	//
	// The trap handler needs one word of scratch space at the top of the
	// trap-time stack in order to return.  In the non-recursive case, we
	// don't have to worry about this because the top of the regular user
	// stack is free.  In the recursive case, this means we have to leave
	// an extra word between the current top of the exception stack and
	// the new stack frame because the exception stack _is_ the trap-time
	// stack.
	//
	//
	// If there's no page fault upcall, the environment didn't allocate a
	// page for its exception stack or can't write to it, or the exception
	// stack overflows, then destroy the environment that caused the fault.
	// Note that the grade script assumes you will first check for the page
	// fault upcall and print the "user fault va" message below if there is
	// none.  The remaining three checks can be combined into a single test.
	//
	// Hints:
	//   user_mem_assert() and env_run() are useful here.
	//   To change what the user environment runs, modify 'curenv->env_tf'
	//   (the 'tf' variable points at 'curenv->env_tf').

	// LAB 4: Your code here.

	// Destroy the environment that caused the fault.
	cprintf("[%08x] user fault va %08x ip %08x\n",
		curenv->env_id, fault_va, tf->tf_rip);
	print_trapframe(tf);
	env_destroy(curenv);
}


void
breakpoint_handler(struct Trapframe* tf){
    print_trapframe(tf);
    monitor(tf);
}
