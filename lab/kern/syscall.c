/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>
#include <kern/sched.h>
#include <kern/time.h>
#include <kern/e1000.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs(const char *s, size_t len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.

	// LAB 3: Your code here.
	user_mem_assert(thiscpu->cpu_env, s, len, PTE_U);
	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc(void)
{
	return cons_getc();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid(void)
{
	return thiscpu->cpu_env->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_destroy(envid_t envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	if (e == thiscpu->cpu_env);
		//cprintf("[%08x] exiting gracefully\n", thiscpu->cpu_env->env_id);
	else
		cprintf("[%08x] destroying %08x\n", thiscpu->cpu_env->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
}

// Allocate a new environment.
// Returns envid of new environment, or < 0 on error.  Errors are:
//	-E_NO_FREE_ENV if no free environment is available.
//	-E_NO_MEM on memory exhaustion.
static envid_t
sys_exofork(void)
{
	// Create the new environment with env_alloc(), from kern/env.c.
	// It should be left as env_alloc created it, except that
	// status is set to ENV_NOT_RUNNABLE, and the register set is copied
	// from the current environment -- but tweaked so sys_exofork
	// will appear to return 0.

	// LAB 4: Your code here.
	struct Env *e=NULL;
	int result;

	result = env_alloc(&e, thiscpu->cpu_env->env_id);
	if (result) {
		return -E_NO_FREE_ENV;
	}

	e->env_status = ENV_NOT_RUNNABLE;

	/*
	e->env_tf.tf_ds = thiscpu->cpu_env->env_tf.tf_ds;
        e->env_tf.tf_es = thiscpu->cpu_env->env_tf.tf_es;
        e->env_tf.tf_ss = thiscpu->cpu_env->env_tf.tf_ss;
        e->env_tf.tf_rsp = thiscpu->cpu_env->env_tf.tf_rsp;
        e->env_tf.tf_cs = thiscpu->cpu_env->env_tf.tf_cs;
	*/
	//cprintf("thiscpu->cpu_env->env_tf = %0x\n", thiscpu->cpu_env->env_tf);
	e->env_tf = thiscpu->cpu_env->env_tf;
	e->env_tf.tf_regs.reg_rax = 0;
	return e->env_id;
	panic("sys_exofork not implemented");
}

// Set envid's env_status to status, which must be ENV_RUNNABLE
// or ENV_NOT_RUNNABLE.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if status is not a valid status for an environment.
static int
sys_env_set_status(envid_t envid, int status)
{
	// Hint: Use the 'envid2env' function from kern/env.c to translate an
	// envid to a struct Env.
	// You should set envid2env's third argument to 1, which will
	// check whether the current environment has permission to set
	// envid's status.

	// LAB 4: Your code here.
	struct Env *e = NULL;
	int result = envid2env(envid, &e, 1);


	if(result){
        	return result;
    	}

    	if((status != ENV_RUNNABLE) && (status != ENV_NOT_RUNNABLE)){
        	return -E_INVAL;
    	}

	e->env_status = status;

	return result;

	panic("sys_env_set_status not implemented");
}

// Set envid's trap frame to 'tf'.
// tf is modified to make sure that user environments always run at code
// protection level 3 (CPL 3) with interrupts enabled.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_trapframe(envid_t envid, struct Trapframe *tf)
{
	// LAB 5: Your code here.
	// Remember to check whether the user has supplied us with a good
	// address!

	struct Env *e = NULL;
	int result;

        result = envid2env(envid, &e, 1);

        if (result) {
                return -E_BAD_ENV;
        }

	e->env_tf = *tf;

	return result;

	panic("sys_env_set_trapframe not implemented");

}

// Set the page fault upcall for 'envid' by modifying the corresponding struct
// Env's 'env_pgfault_upcall' field.  When 'envid' causes a page fault, the
// kernel will push a fault record onto the exception stack, then branch to
// 'func'.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_pgfault_upcall(envid_t envid, void *func)
{
	// LAB 4: Your code here.

	struct Env *e = NULL;
	int result;

	result = envid2env(envid, &e, 1);

	if (result) {
		return -E_BAD_ENV;
	}

	e->env_pgfault_upcall = func;

	return result;

	panic("sys_env_set_pgfault_upcall not implemented");
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.  See PTE_SYSCALL in inc/mmu.h.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
//	-E_INVAL if perm is inappropriate (see above).
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int
sys_page_alloc(envid_t envid, void *va, int perm)
{
	// Hint: This function is a wrapper around page_alloc() and
	//   page_insert() from kern/pmap.c.
	//   Most of the new code you write should be to check the
	//   parameters for correctness.
	//   If page_insert() fails, remember to free the page you
	//   allocated!

	// LAB 4: Your code here.

	struct Env *e = NULL;
	int result;

	result = envid2env(envid, &e, 1);

	if (result) {
		return -E_BAD_ENV;
	}

	if ((uintptr_t)va >= UTOP || ROUNDUP(va, PGSIZE) != va) {
		return -E_INVAL;
	}

	if (((perm & (PTE_U|PTE_P)) != (PTE_U|PTE_P)) || ((perm & (~PTE_SYSCALL)) != 0)) {
		return -E_INVAL;
	}

        struct Page *p = page_alloc(ALLOC_ZERO);

        if (!p) {
                return -E_NO_MEM;
        }
	result = page_insert(e->env_pml4e, p, ROUNDDOWN(va, PGSIZE), perm);
	
	if (result) {
		page_free(p);
	}

	return result;
	panic("sys_page_alloc not implemented");
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate any necessary page tables.
static int
sys_page_map(envid_t srcenvid, void *srcva,
	     envid_t dstenvid, void *dstva, int perm)
{
	// Hint: This function is a wrapper around page_lookup() and
	//   page_insert() from kern/pmap.c.
	//   Again, most of the new code you write should be to check the
	//   parameters for correctness.
	//   Use the third argument to page_lookup() to
	//   check the current permissions on the page.

	// LAB 4: Your code here.

	int result_src, result_dst;
	struct Env *src_env = NULL, *dst_env = NULL;

	result_src = envid2env(srcenvid, &src_env, 1);
	result_dst = envid2env(dstenvid, &dst_env, 1);
	if (result_src || result_dst) {
		return -E_BAD_ENV;
	}

	if ((uintptr_t)srcva>=UTOP || ROUNDUP(srcva, PGSIZE) != srcva || (uintptr_t)dstva>=UTOP || ROUNDUP(dstva, PGSIZE) != dstva) {
		return -E_INVAL;
	}

	struct Page *src_page;
	struct Page *dst_page;
	pte_t *src_pte;
	pte_t *dst_pte;

	src_page = page_lookup(src_env->env_pml4e, srcva, &src_pte);

	if (!src_page) {
		return -E_INVAL;
	}

	/*
	uint64_t src_permission = *src_pte & 0xfff;
	int source_page_readonly = (src_permission & PTE_W)==0;
        if (((perm & (PTE_U|PTE_P)) != (PTE_U|PTE_P)) || ((perm & (~PTE_SYSCALL)) != 0) || ((perm & PTE_W) && source_page_readonly)) {
                return -E_INVAL;
        }
	*/

	result_dst = page_insert(dst_env->env_pml4e, src_page, dstva, perm);

	if (result_dst) {
		return -E_NO_MEM;
	}

	return result_dst;

	panic("sys_page_map not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int
sys_page_unmap(envid_t envid, void *va)
{
	// Hint: This function is a wrapper around page_remove().

	// LAB 4: Your code here.
        struct Env *e = NULL;
        int result;
        result = envid2env(envid, &e, 1);
        if (result) {
                return -E_BAD_ENV;
        }

        if ((uintptr_t)va >= UTOP || ROUNDUP(va, PGSIZE) != va) {
                return -E_INVAL;
        }

	page_remove(e->env_pml4e, va);

	return 0;
	panic("sys_page_unmap not implemented");
}

// Try to send 'value' to the target env 'envid'.
// If srcva < UTOP, then also send page currently mapped at 'srcva',
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target is not blocked, waiting for an IPC.
//
// The send also can fail for the other reasons listed below.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends;
//    env_ipc_from is set to the sending envid;
//    env_ipc_value is set to the 'value' parameter;
//    env_ipc_perm is set to 'perm' if a page was transferred, 0 otherwise.
// The target environment is marked runnable again, returning 0
// from the paused sys_ipc_recv system call.  (Hint: does the
// sys_ipc_recv function ever actually return?)
//
// If the sender wants to send a page but the receiver isn't asking for one,
// then no page mapping is transferred, but no error occurs.
// The ipc only happens when no errors occur.
//
// Returns 0 on success, < 0 on error.
// Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist.
//		(No need to check permissions.)
//	-E_IPC_NOT_RECV if envid is not currently blocked in sys_ipc_recv,
//		or another environment managed to send first.
//	-E_INVAL if srcva < UTOP but srcva is not page-aligned.
//	-E_INVAL if srcva < UTOP and perm is inappropriate
//		(see sys_page_alloc).
//	-E_INVAL if srcva < UTOP but srcva is not mapped in the caller's
//		address space.
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in the
//		current environment's address space.
//	-E_NO_MEM if there's not enough memory to map srcva in envid's
//		address space.
static int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, unsigned perm)
{
	// LAB 4: Your code here.
	// aj-code
	struct Env *dstenv, *srcenv;
        int r;
        pte_t *pte_ptr;

        if ((r=envid2env(envid, &dstenv, 0)) < 0) {
		cprintf("\nReached dest env bad environment");
                return -E_BAD_ENV;
	}
        if ((r=envid2env(0, &srcenv, 0)) < 0) {
                return -E_BAD_ENV;
        }

        if (dstenv->env_ipc_recving == 0)
                return -E_IPC_NOT_RECV;

        dstenv->env_ipc_recving = 0;
        dstenv->env_ipc_from = curenv->env_id;
        dstenv->env_ipc_value = value;

	// page transfer
	if ((uintptr_t)srcva<UTOP) {

		struct Page *src_page;
		struct Page *dst_page;
		pte_t *src_pte;
		pte_t *dst_pte;

		src_page = page_lookup(srcenv->env_pml4e, srcva, &src_pte);

		if (!src_page) {
			return -E_INVAL;
		}

		int result_dst = page_insert(dstenv->env_pml4e, src_page, dstenv->env_ipc_dstva, perm);

		if (result_dst) {
			return -E_NO_MEM;
		}

		dstenv->env_ipc_perm = perm;
	} else {

		//not a page transfer
		dstenv->env_ipc_perm = 0;
	}

	dstenv->env_status = ENV_RUNNABLE;
	return 0;

	panic("sys_ipc_try_send not implemented");
}

// Block until a value is ready.  Record that you want to receive
// using the env_ipc_recving and env_ipc_dstva fields of struct Env,
// mark yourself not runnable, and then give up the CPU.
//
// If 'dstva' is < UTOP, then you are willing to receive a page of data.
// 'dstva' is the virtual address at which the sent page should be mapped.
//
// This function only returns on error, but the system call will eventually
// return 0 on success.
// Return < 0 on error.  Errors are:
//	-E_INVAL if dstva < UTOP but dstva is not page-aligned.
	static int
sys_ipc_recv(void *dstva)
{
	// LAB 4: Your code here.
	// aj-code
	struct Env *env;
	int r;

	if ((r=envid2env(0, &env, 0)) < 0)
		panic("sys_ipc_recv: get envid error %e", r);

	if ((uint64_t)dstva<UTOP && (uint64_t)dstva%PGSIZE)
		return -E_INVAL;

	env->env_ipc_recving = true;
	env->env_ipc_dstva = dstva;
	env->env_ipc_value = 0;
	env->env_ipc_perm = 0;
	env->env_ipc_from = 0;
	env->env_status = ENV_NOT_RUNNABLE;
	//sys_yield();
	return 0;
	panic("sys_ipc_recv not implemented");
}

// Return the current time.
	static int
sys_time_msec(void)
{
	// LAB 6: Your code here.
	return time_msec();
	panic("sys_time_msec not implemented");
}


// Try to send packet over network
static int
sys_net_try_send(char *data, int len)
{
        if ((uintptr_t)data >= UTOP) {
                return -E_INVAL;
        }

        return e1000_transmit(data, len);
}

// Try to receive packet over network
static int
sys_net_try_receive(char *data, int *len)
{
	//cprintf("in the system call to receive from network driver\n");
        if ((uintptr_t)data >= UTOP) {
                return -E_INVAL;
        }

	//cprintf("calling e1000_receive()\n");
        *len = e1000_receive(data);
        if (*len > 0) {
		//curenv->env_status = ENV_NOT_RUNNABLE;
                return 0;
        }

        return *len;
}

// Dispatches to the correct kernel function, passing the arguments.
	int64_t
syscall(uint64_t syscallno, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.
	uint64_t retval = 0;

	switch (syscallno) {
		case SYS_cputs:
			sys_cputs((char *) a1, (size_t) a2);
			return retval;
		case SYS_cgetc:
			return (int64_t) sys_cgetc();
		case SYS_getenvid:
			return (int64_t) sys_getenvid();
		case SYS_env_destroy:
			return (int64_t) sys_env_destroy((envid_t) a1);
		case SYS_yield:
			sys_yield();
			return retval;
		case SYS_exofork:
			return (int64_t)sys_exofork();
		case SYS_page_alloc:
			return (int64_t)sys_page_alloc((envid_t)a1, (void *)a2, (int)a3);
		case SYS_page_map:
			return (int64_t)sys_page_map((envid_t)a1, (void *)a2, (envid_t)a3, (void *)a4, (int)a5);
		case SYS_page_unmap:
			return (int64_t)sys_page_unmap((envid_t)a1, (void *)a2);
		case SYS_env_set_status:
			return (int64_t)sys_env_set_status((envid_t)a1, (int)a2);
		case SYS_env_set_pgfault_upcall:
			return (int64_t)sys_env_set_pgfault_upcall((envid_t)a1, (void *)a2);
		case SYS_ipc_try_send:
			return (int64_t) sys_ipc_try_send((envid_t) a1, (uint32_t) a2, (void *) a3, (unsigned) a4);
		case SYS_ipc_recv:
			return (int64_t)sys_ipc_recv((void*)a1);
		case SYS_env_set_trapframe:
			return sys_env_set_trapframe((envid_t)a1, (struct Trapframe*)a2);
		case SYS_time_msec:
			return sys_time_msec();
		case SYS_net_try_send:
			return sys_net_try_send((char *) a1, (int) a2);
		case SYS_net_try_receive:
			return sys_net_try_receive((char *) a1, (int *) a2);
		default:
			return -E_INVAL;
	}

	panic("syscall not implemented");
}

