// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#include <kern/pmap.h>
#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
        { "backtrace", "Display stack backtrace", mon_backtrace },
	// Challenge Problem Lab 1
	{ "colortext", "Displays data in color", mon_color },

	// Challenge Problem Lab 2 
	{ "page_alloc", "Allocate a page", mon_page_alloc },
	{ "page_availability", "Check Page Availability", mon_check_page_availability}, 
	{ "page_free", "Free page", mon_page_free},
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

void cga_put_color();

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	cprintf("\n");
	cprintf("Stack backtrace:\n");

	uint64_t rbpValue = read_rbp();
	uint64_t *rbp = (uint64_t *)rbpValue;

	struct Ripdebuginfo rdbg;

	while(rbp != 0) {
		cprintf("rbp %016x  ", rbp);
		uint64_t *rip = (uint64_t *)(((uint64_t)rbp)+0x08);
		cprintf("rip %016x  ", *rip);

		cprintf("args  ");
		int i;
		for(i=0;i<4;i++) {
			uint64_t *arg = (uint64_t *)(((uint64_t)rbp)-0x04*(i+1));
			cprintf("%016x ", *arg);
		}

		debuginfo_rip(*rip, &rdbg);
		cprintf("\n\t%s:%d: ", rdbg.rip_file, rdbg.rip_line);
		for(i = 0; i < rdbg.rip_fn_namelen; i++) {
			cputchar(rdbg.rip_fn_name[i]);
		}

		cprintf("+%d\n", rip - rdbg.rip_fn_addr);

		rbp = (uint64_t *)(*rbp);
		cprintf("\n");
	}

	//__asm __volatile("leaq (%%rip), %0;": "=r"(a));
	//cprintf("value at a = %0x\n", *b);

	cprintf("\n");
	return 0;
}

int
mon_color()
{
        /* Challenge Problem Lab 1 */
        cga_put_color(LIGHTGREEN,WHITE);
        cprintf("A colorful Hello World! :-)\n");
        return 0;
}

 /* Challenge Problem Lab 2 */      
int
mon_page_alloc(int argc, char **argv, struct Trapframe *tf)
{
	int r;
	struct Page *pg;

	pg = page_alloc(1);
	if (pg) {
		cprintf("Page allocation failed\n");
		return 0;
	}

	cprintf("phys: 0x%08x virt: 0x%08x\n", page2pa(pg),
		(uintptr_t) page2kva(pg));
	return 0;
}

/* Challenge Problem Lab 2 */
int
mon_check_page_availability(int argc, char **argv, struct Trapframe *tf)
{
	struct Page *pp;
	physaddr_t ph;

	if (argc != 2) {
		cprintf("Usage: page_availability <page_phys_addr>\n");
		return 0;
	}

	ph = (physaddr_t) strtol(argv[1], NULL, 16);
	for (pp = page_free_list; pp; pp = pp->pp_link) {
 		if (page2pa(pp) == ph) {
			cprintf("free\n");
			return 0;
		}
	}

	cprintf("allocated\n");
	return 0;
}

/* Challenge Problem Lab 2 */
int
mon_page_free(int argc, char **argv, struct Trapframe *tf)
{
	struct Page *pp;
	physaddr_t ph;

	if (argc != 2) {
		cprintf("Usage: page_free <page_phys_addr>\n");
		return 0;
	}

	ph = (physaddr_t) strtol(argv[1], NULL, 16);
	pp = pa2page(ph);
	page_free(pp);
	cprintf("Page returned to free list\n");
	return 0;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

	static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

	void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
