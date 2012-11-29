	.file	"init.c"
	.stabs	"kern/init.c",100,0,2,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.stabs	"int:t(0,1)=r(0,1);-2147483648;2147483647;",128,0,0,0
	.stabs	"char:t(0,2)=r(0,2);0;127;",128,0,0,0
	.stabs	"long int:t(0,3)=r(0,3);-0;4294967295;",128,0,0,0
	.stabs	"unsigned int:t(0,4)=r(0,4);0;4294967295;",128,0,0,0
	.stabs	"long unsigned int:t(0,5)=r(0,5);0;-1;",128,0,0,0
	.stabs	"long long int:t(0,6)=r(0,6);-0;4294967295;",128,0,0,0
	.stabs	"long long unsigned int:t(0,7)=r(0,7);0;-1;",128,0,0,0
	.stabs	"short int:t(0,8)=r(0,8);-32768;32767;",128,0,0,0
	.stabs	"short unsigned int:t(0,9)=r(0,9);0;65535;",128,0,0,0
	.stabs	"signed char:t(0,10)=r(0,10);-128;127;",128,0,0,0
	.stabs	"unsigned char:t(0,11)=r(0,11);0;255;",128,0,0,0
	.stabs	"float:t(0,12)=r(0,1);4;0;",128,0,0,0
	.stabs	"double:t(0,13)=r(0,1);8;0;",128,0,0,0
	.stabs	"long double:t(0,14)=r(0,1);16;0;",128,0,0,0
	.stabs	"_Decimal32:t(0,15)=r(0,1);4;0;",128,0,0,0
	.stabs	"_Decimal64:t(0,16)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Decimal128:t(0,17)=r(0,1);16;0;",128,0,0,0
	.stabs	"void:t(0,18)=(0,18)",128,0,0,0
	.stabs	"./inc/stdio.h",130,0,0,0
	.stabs	"./inc/stdarg.h",130,0,0,0
	.stabs	"va_list:t(2,1)=(2,2)=ar(2,3)=r(2,3);0;-1;;0;0;(2,4)=xs__va_list_tag:",128,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"./inc/string.h",130,0,0,0
	.stabs	"./inc/types.h",130,0,0,0
	.stabs	"bool:t(4,1)=(4,2)=eFalse:0,True:1,;",128,0,0,0
	.stabs	" :T(4,3)=efalse:0,true:1,;",128,0,0,0
	.stabs	"int8_t:t(4,4)=(0,10)",128,0,0,0
	.stabs	"uint8_t:t(4,5)=(0,11)",128,0,0,0
	.stabs	"int16_t:t(4,6)=(0,8)",128,0,0,0
	.stabs	"uint16_t:t(4,7)=(0,9)",128,0,0,0
	.stabs	"int32_t:t(4,8)=(0,1)",128,0,0,0
	.stabs	"uint32_t:t(4,9)=(0,4)",128,0,0,0
	.stabs	"int64_t:t(4,10)=(0,6)",128,0,0,0
	.stabs	"uint64_t:t(4,11)=(0,7)",128,0,0,0
	.stabs	"intptr_t:t(4,12)=(4,8)",128,0,0,0
	.stabs	"uintptr_t:t(4,13)=(4,11)",128,0,0,0
	.stabs	"physaddr_t:t(4,14)=(4,11)",128,0,0,0
	.stabs	"ppn_t:t(4,15)=(4,11)",128,0,0,0
	.stabs	"size_t:t(4,16)=(4,11)",128,0,0,0
	.stabs	"ssize_t:t(4,17)=(4,8)",128,0,0,0
	.stabs	"off_t:t(4,18)=(4,8)",128,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"kernel warning at %s:%d: "
.LC1:
	.string	"\n"
	.text
	.p2align 4,,15
	.stabs	"_warn:F(0,18)",36,0,0,_warn
	.stabs	"file:P(0,19)=*(0,2)",64,0,0,5
	.stabs	"line:P(0,1)",64,0,0,4
	.stabs	"fmt:P(0,19)",64,0,0,3
.globl _warn
	.type	_warn, @function
_warn:
	.stabn	68,0,84,.LM0-.LFBB1
.LM0:
.LFBB1:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movzbl	%al, %eax
	pushq	%rbx
	.cfi_def_cfa_offset 24
	movq	%rdx, %rbx
	.cfi_offset 3, -24
	.cfi_offset 6, -16
	.stabn	68,0,88,.LM1-.LFBB1
.LM1:
	movl	%esi, %edx
	movq	%rdi, %rsi
	movl	$.LC0, %edi
	.stabn	68,0,84,.LM2-.LFBB1
.LM2:
	subq	$216, %rsp
	.cfi_def_cfa_offset 240
	.stabn	68,0,84,.LM3-.LFBB1
.LM3:
	movq	%rcx, 56(%rsp)
	leaq	0(,%rax,4), %rcx
	movl	$.L2, %eax
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	subq	%rcx, %rax
	leaq	207(%rsp), %rcx
	jmp	*%rax
	movaps	%xmm7, -15(%rcx)
	movaps	%xmm6, -31(%rcx)
	movaps	%xmm5, -47(%rcx)
	movaps	%xmm4, -63(%rcx)
	movaps	%xmm3, -79(%rcx)
	movaps	%xmm2, -95(%rcx)
	movaps	%xmm1, -111(%rcx)
	movaps	%xmm0, -127(%rcx)
.L2:
	.stabn	68,0,87,.LM4-.LFBB1
.LM4:
	leaq	240(%rsp), %rax
	movl	$24, (%rsp)
	movl	$48, 4(%rsp)
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	.stabn	68,0,88,.LM5-.LFBB1
.LM5:
	xorl	%eax, %eax
	call	cprintf
	.stabn	68,0,89,.LM6-.LFBB1
.LM6:
	movq	%rsp, %rsi
	movq	%rbx, %rdi
	call	vcprintf
	.stabn	68,0,90,.LM7-.LFBB1
.LM7:
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	cprintf
	.stabn	68,0,92,.LM8-.LFBB1
.LM8:
	addq	$216, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.cfi_endproc
.LFE3:
	.size	_warn, .-_warn
	.stabs	"ap:(2,1)",128,0,0,0
	.stabn	192,0,0,.LFBB1-.LFBB1
	.stabn	224,0,0,.Lscope1-.LFBB1
.Lscope1:
	.section	.rodata.str1.1
.LC2:
	.string	"kernel panic at %s:%d: "
	.text
	.p2align 4,,15
	.stabs	"_panic:F(0,18)",36,0,0,_panic
	.stabs	"file:P(0,19)",64,0,0,5
	.stabs	"line:P(0,1)",64,0,0,4
	.stabs	"fmt:P(0,19)",64,0,0,3
.globl _panic
	.type	_panic, @function
_panic:
	.stabn	68,0,59,.LM9-.LFBB2
.LM9:
.LFBB2:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movzbl	%al, %eax
	pushq	%rbx
	.cfi_def_cfa_offset 24
	movq	%rdx, %rbx
	.cfi_offset 3, -24
	.cfi_offset 6, -16
	subq	$216, %rsp
	.cfi_def_cfa_offset 240
	.stabn	68,0,59,.LM10-.LFBB2
.LM10:
	movq	%rcx, 56(%rsp)
	leaq	0(,%rax,4), %rcx
	movl	$.L5, %eax
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	subq	%rcx, %rax
	leaq	207(%rsp), %rcx
	jmp	*%rax
	movaps	%xmm7, -15(%rcx)
	movaps	%xmm6, -31(%rcx)
	movaps	%xmm5, -47(%rcx)
	movaps	%xmm4, -63(%rcx)
	movaps	%xmm3, -79(%rcx)
	movaps	%xmm2, -95(%rcx)
	movaps	%xmm1, -111(%rcx)
	movaps	%xmm0, -127(%rcx)
.L5:
	.stabn	68,0,62,.LM11-.LFBB2
.LM11:
	cmpq	$0, panicstr(%rip)
	je	.L10
	.p2align 4,,10
	.p2align 3
.L8:
	.stabn	68,0,78,.LM12-.LFBB2
.LM12:
	xorl	%edi, %edi
	call	monitor
	jmp	.L8
.L10:
	.stabn	68,0,64,.LM13-.LFBB2
.LM13:
	movq	%rdx, panicstr(%rip)
	.stabn	68,0,67,.LM14-.LFBB2
.LM14:
#APP
# 67 "kern/init.c" 1
	cli; cld
# 0 "" 2
	.stabn	68,0,69,.LM15-.LFBB2
.LM15:
#NO_APP
	leaq	240(%rsp), %rax
	.stabn	68,0,70,.LM16-.LFBB2
.LM16:
	movl	%esi, %edx
	movq	%rdi, %rsi
	movl	$.LC2, %edi
	.stabn	68,0,69,.LM17-.LFBB2
.LM17:
	movl	$24, (%rsp)
	movl	$48, 4(%rsp)
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	.stabn	68,0,70,.LM18-.LFBB2
.LM18:
	xorl	%eax, %eax
	call	cprintf
	.stabn	68,0,71,.LM19-.LFBB2
.LM19:
	movq	%rbx, %rdi
	movq	%rsp, %rsi
	call	vcprintf
	.stabn	68,0,72,.LM20-.LFBB2
.LM20:
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	cprintf
	jmp	.L8
	.cfi_endproc
.LFE2:
	.size	_panic, .-_panic
	.stabs	"ap:(2,1)",128,0,0,0
	.stabn	192,0,0,.LFBB2-.LFBB2
	.stabn	224,0,0,.Lscope2-.LFBB2
.Lscope2:
	.section	.rodata.str1.1
.LC3:
	.string	"entering test_backtrace %d\n"
.LC4:
	.string	"leaving test_backtrace %d\n"
	.text
	.p2align 4,,15
	.stabs	"test_backtrace:F(0,18)",36,0,0,test_backtrace
	.stabs	"x:P(0,1)",64,0,0,3
.globl test_backtrace
	.type	test_backtrace, @function
test_backtrace:
	.stabn	68,0,13,.LM21-.LFBB3
.LM21:
.LFBB3:
.LFB0:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.stabn	68,0,14,.LM22-.LFBB3
.LM22:
	movl	%edi, %esi
	.stabn	68,0,13,.LM23-.LFBB3
.LM23:
	movl	%edi, %ebx
	.cfi_offset 3, -16
	.stabn	68,0,14,.LM24-.LFBB3
.LM24:
	xorl	%eax, %eax
	movl	$.LC3, %edi
	call	cprintf
	.stabn	68,0,15,.LM25-.LFBB3
.LM25:
	testl	%ebx, %ebx
	jle	.L12
	.stabn	68,0,16,.LM26-.LFBB3
.LM26:
	leal	-1(%rbx), %edi
	call	test_backtrace
	.stabn	68,0,19,.LM27-.LFBB3
.LM27:
	movl	%ebx, %esi
	movl	$.LC4, %edi
	xorl	%eax, %eax
	.stabn	68,0,20,.LM28-.LFBB3
.LM28:
	popq	%rbx
	.stabn	68,0,19,.LM29-.LFBB3
.LM29:
	jmp	cprintf
	.p2align 4,,10
	.p2align 3
.L12:
	.stabn	68,0,18,.LM30-.LFBB3
.LM30:
	xorl	%esi, %esi
	xorl	%edi, %edi
	xorl	%edx, %edx
	call	mon_backtrace
	.stabn	68,0,19,.LM31-.LFBB3
.LM31:
	movl	%ebx, %esi
	movl	$.LC4, %edi
	xorl	%eax, %eax
	.stabn	68,0,20,.LM32-.LFBB3
.LM32:
	popq	%rbx
	.stabn	68,0,19,.LM33-.LFBB3
.LM33:
	jmp	cprintf
	.cfi_endproc
.LFE0:
	.size	test_backtrace, .-test_backtrace
.Lscope3:
	.section	.rodata.str1.1
.LC5:
	.string	"6828 decimal is %o octal!\n"
	.text
	.p2align 4,,15
	.stabs	"i386_init:F(0,18)",36,0,0,i386_init
.globl i386_init
	.type	i386_init, @function
i386_init:
	.stabn	68,0,24,.LM34-.LFBB4
.LM34:
.LFBB4:
.LFB1:
	.cfi_startproc
	.stabn	68,0,30,.LM35-.LFBB4
.LM35:
	movl	$end, %edx
	.stabn	68,0,24,.LM36-.LFBB4
.LM36:
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	.stabn	68,0,30,.LM37-.LFBB4
.LM37:
	xorl	%esi, %esi
	subq	$edata, %rdx
	movl	$edata, %edi
	call	memset
	.stabn	68,0,34,.LM38-.LFBB4
.LM38:
	call	cons_init
	.stabn	68,0,36,.LM39-.LFBB4
.LM39:
	movl	$.LC5, %edi
	movl	$6828, %esi
	xorl	%eax, %eax
	call	cprintf
	.stabn	68,0,39,.LM40-.LFBB4
.LM40:
	movl	$5, %edi
	call	test_backtrace
	.p2align 4,,10
	.p2align 3
.L16:
	.stabn	68,0,43,.LM41-.LFBB4
.LM41:
	xorl	%edi, %edi
	call	monitor
	jmp	.L16
	.cfi_endproc
.LFE1:
	.size	i386_init, .-i386_init
.Lscope4:
	.comm	panicstr,8,8
	.stabs	"panicstr:G(0,19)",32,0,0,0
	.stabs	"",100,0,0,.Letext0
.Letext0:
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5.1) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
