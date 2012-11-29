#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <kern/macro.h>

// Page fault upcall entrypoint.

// This is where we ask the kernel to redirect us to whenever we cause
// a page fault in user space (see the call to sys_set_pgfault_handler
// in pgfault.c).
//
// When a page fault actually occurs, the kernel switches our ESP to
// point to the user exception stack if we're not already on the user
// exception stack, and then it pushes a UTrapframe onto our user
// exception stack:
//
//	trap-time esp
//	trap-time eflags
//	trap-time eip
//	utf_regs.reg_rax
//	...
//	utf_regs.reg_rsi
//	utf_regs.reg_rdi
//	utf_err (error code)
//	utf_fault_va            <-- %esp
//
// If this is a recursive fault, the kernel will reserve for us a
// blank word above the trap-time esp for scratch work when we unwind
// the recursive call.
//
// We then have call up to the appropriate page fault handler in C
// code, pointed to by the global variable '_pgfault_handler'.

.text
.globl _pgfault_upcall
_pgfault_upcall:
	// Call the C page fault handler.
	pushq %rsp			// function argument: pointer to UTF
	
	movq  %rsp,%rdi
	addq  $8,%rdi      // passing the function argument in rdi
	movabs _pgfault_handler, %rax
	call *%rax
	
	popq %rsp
	// Now the C page fault handler has returned and you must return
	// to the trap time state.
	// Push trap-time %eip onto the trap-time stack.
	//
	// Explanation:
	//   We must prepare the trap-time stack for our eventual return to
	//   re-execute the instruction that faulted.
	//   Unfortunately, we can't return directly from the exception stack:
	//   We can't call 'jmp', since that requires that we load the address
	//   into a register, and all registers must have their trap-time
	//   values after the return.
	//   We can't call 'ret' from the exception stack either, since if we
	//   did, %esp would have the wrong value.
	//   So instead, we push the trap-time %eip onto the *trap-time* stack!
	//   Below we'll switch to that stack and call 'ret', which will
	//   restore %eip to its pre-fault value.
	//
	//   In the case of a recursive fault on the exception stack,
	//   note that the word we're pushing now will fit in the
	//   blank word that the kernel reserved for us.
	//
	// Throughout the remaining code, think carefully about what
	// registers are available for intermediate calculations.  You
	// may find that you have to rearrange your code in non-obvious
	// ways as registers become unavailable as scratch space.
	//
	// LAB 4: Your code here.
	addq $16, %rsp //skip fault_va and tf_err
    	movq 0x78(%rsp), %rax //rax = trap-time rip
    	movq 0x88(%rsp), %rbx //rbx = trap-time rsp
	movq %rsp,%rdx
	movq %rbx,%rsp
	pushq %rax 
    	movq %rsp, 0x88(%rdx)
	movq %rdx, %rsp

	#addq $16, %rsp 
	#movq 0x90(%rsp), %rax
	#movq 0x78(%rsp), %rdx
	#subq $8, %rax
	#movq %rdx, (%rax)
	#movq %rax, 0x88(%rsp)

	/*add $16, %rsp
	movq %rdi, %rsp*/

	//movq 0x88(%rdi), %rax
	//movq 0x98(%rdi), %rbx
	//movq (%rax),%rcx
	//movq %rcx, -8(%rbx)
	//subq $0x08, (%rbx)
	//movq (%rdi), %rsp
	//addq $0x10, %rsp

	// Restore the trap-time registers.  After you do this, you
	// can no longer modify any general-purpose registers.
	// LAB 4: Your code here.
	//POPA
        movq 0(%rsp),%r15
        movq 8(%rsp),%r14
        movq 16(%rsp),%r13
        movq 24(%rsp),%r12
        movq 32(%rsp),%r11
        movq 40(%rsp),%r10
        movq 48(%rsp),%r9
        movq 56(%rsp),%r8
        movq 64(%rsp),%rsi
        movq 72(%rsp),%rdi
        movq 80(%rsp),%rbp
        movq 88(%rsp),%rdx
        movq 96(%rsp),%rcx
        movq 104(%rsp),%rbx
        movq 112(%rsp),%rax
        addq $120,%rsp

	// Restore eflags from the stack.  After you do this, you can
	// no longer use arithmetic operations or anything else that
	// modifies eflags.
	// LAB 4: Your code here.
	add $0x8, %rsp # skip eip
        popf
	
	// Switch back to the adjusted trap-time stack.
	// LAB 4: Your code here.
	pop %rsp
	
	// Return to re-execute the instruction that faulted.
	// LAB 4: Your code here.
	ret
