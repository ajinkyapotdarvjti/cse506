// User-level page fault handler support.
// Rather than register the C page fault handler directly with the
// kernel as the page fault handler, we register the assembly language
// wrapper in pfentry.S, which in turns calls the registered C
// function.

#include <inc/lib.h>


// Assembly language pgfault entrypoint defined in lib/pfentry.S.
extern void _pgfault_upcall(void);

// Pointer to currently installed C-language pgfault handler.
void (*_pgfault_handler)(struct UTrapframe *utf);

//
// Set the page fault handler function.
// If there isn't one yet, _pgfault_handler will be 0.
// The first time we register a handler, we need to
// allocate an exception stack (one page of memory with its top
// at UXSTACKTOP), and tell the kernel to call the assembly-language
// _pgfault_upcall routine when a page fault occurs.
//
void
set_pgfault_handler(void (*handler)(struct UTrapframe *utf))
{
	int r;

	//cprintf("inside page fault handler, _pgfault_handler = %0x\n", _pgfault_handler);
	if (_pgfault_handler == 0) {
		// First time through!
		// LAB 4: Your code here.
		envid_t envid = sys_getenvid();
		r = sys_page_alloc(envid, (void*) (UXSTACKTOP - PGSIZE), PTE_P|PTE_U|PTE_W);
		r = sys_page_alloc(envid, (void*) (UXSTACKTOP - 2*PGSIZE), PTE_P|PTE_U|PTE_W);
		if(r < 0)
			panic("page_alloc: %e", r);
		//cprintf("setting the upcall\n");
		r = sys_env_set_pgfault_upcall(envid, _pgfault_upcall);
		//cprintf("set the upcall with result = %d\n", r);
		if(r < 0)
			panic("pgfault_upcall: %e", r);
		//panic("set_pgfault_handler not implemented");
	}

	// Save handler pointer for assembly to call.
	_pgfault_handler = handler;
}
