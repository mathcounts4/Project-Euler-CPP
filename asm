	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	__Z1fNSt3__16vectorIiNS_9allocatorIiEEEEiii
	.p2align	4, 0x90
__Z1fNSt3__16vectorIiNS_9allocatorIiEEEEiii: ## @_Z1fNSt3__16vectorIiNS_9allocatorIiEEEEiii
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi0:
	.cfi_def_cfa_offset 16
Lcfi1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi2:
	.cfi_def_cfa_register %rbp
	movslq	%esi, %rax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	__Z1gRKNSt3__16vectorIiNS_9allocatorIiEEEERKi
	.p2align	4, 0x90
__Z1gRKNSt3__16vectorIiNS_9allocatorIiEEEERKi: ## @_Z1gRKNSt3__16vectorIiNS_9allocatorIiEEEERKi
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi3:
	.cfi_def_cfa_offset 16
Lcfi4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi5:
	.cfi_def_cfa_register %rbp
	movl	$5, %eax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	__Z1qNSt3__16vectorIlNS_9allocatorIlEEEE
	.p2align	4, 0x90
__Z1qNSt3__16vectorIlNS_9allocatorIlEEEE: ## @_Z1qNSt3__16vectorIlNS_9allocatorIlEEEE
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi6:
	.cfi_def_cfa_offset 16
Lcfi7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi8:
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	__Z1zRKi
	.p2align	4, 0x90
__Z1zRKi:                               ## @_Z1zRKi
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi9:
	.cfi_def_cfa_offset 16
Lcfi10:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi11:
	.cfi_def_cfa_register %rbp
	movslq	(%rdi), %rax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
## BB#0:
	pushq	%rbp
Lcfi12:
	.cfi_def_cfa_offset 16
Lcfi13:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi14:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$40, %rsp
Lcfi15:
	.cfi_offset %rbx, -24
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -48(%rbp)
	movq	$0, -32(%rbp)
Ltmp0:
	movl	$4, %edi
	callq	__Znwm
Ltmp1:
## BB#1:
	movq	%rax, -48(%rbp)
	movq	%rax, %rcx
	addq	$4, %rcx
	movq	%rcx, -32(%rbp)
	movl	$3, (%rax)
	movq	%rcx, -40(%rbp)
	movabsq	$25769803781, %rax      ## imm = 0x600000005
	movq	%rax, -24(%rbp)
	movl	$7, -16(%rbp)
Ltmp3:
	leaq	__Z1fNSt3__16vectorIiNS_9allocatorIiEEEEiii(%rip), %rdi
	leaq	-48(%rbp), %rsi
	callq	__Z14apply_functionIlJNSt3__16vectorIiNS0_9allocatorIiEEEEiiiEET_PFS5_DpT0_ERKNS0_5tupleIJDpNS0_16remove_referenceINS0_9remove_cvIS6_E4typeEE4typeEEEE
Ltmp4:
## BB#2:
	movq	-48(%rbp), %rdi
	testq	%rdi, %rdi
	je	LBB4_6
## BB#3:
	movq	-40(%rbp), %rax
	cmpq	%rdi, %rax
	je	LBB4_5
## BB#4:
	leaq	-4(%rax), %rcx
	subq	%rdi, %rcx
	notq	%rcx
	andq	$-4, %rcx
	addq	%rax, %rcx
	movq	%rcx, -40(%rbp)
LBB4_5:
	callq	__ZdlPv
LBB4_6:
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	retq
LBB4_8:
Ltmp5:
	movq	%rax, %rbx
	movq	-48(%rbp), %rdi
	testq	%rdi, %rdi
	je	LBB4_12
## BB#9:
	movq	-40(%rbp), %rax
	cmpq	%rdi, %rax
	je	LBB4_11
## BB#10:
	leaq	-4(%rax), %rcx
	subq	%rdi, %rcx
	notq	%rcx
	andq	$-4, %rcx
	addq	%rax, %rcx
	movq	%rcx, -40(%rbp)
LBB4_11:
	callq	__ZdlPv
LBB4_12:
	movq	%rbx, %rdi
	callq	__Unwind_Resume
LBB4_7:
Ltmp2:
	movq	%rax, %rbx
	movq	%rbx, %rdi
	callq	__Unwind_Resume
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table4:
Lexception0:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.byte	41                      ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	39                      ## Call site table length
Lset0 = Ltmp0-Lfunc_begin0              ## >> Call Site 1 <<
	.long	Lset0
Lset1 = Ltmp1-Ltmp0                     ##   Call between Ltmp0 and Ltmp1
	.long	Lset1
Lset2 = Ltmp2-Lfunc_begin0              ##     jumps to Ltmp2
	.long	Lset2
	.byte	0                       ##   On action: cleanup
Lset3 = Ltmp3-Lfunc_begin0              ## >> Call Site 2 <<
	.long	Lset3
Lset4 = Ltmp4-Ltmp3                     ##   Call between Ltmp3 and Ltmp4
	.long	Lset4
Lset5 = Ltmp5-Lfunc_begin0              ##     jumps to Ltmp5
	.long	Lset5
	.byte	0                       ##   On action: cleanup
Lset6 = Ltmp4-Lfunc_begin0              ## >> Call Site 3 <<
	.long	Lset6
Lset7 = Lfunc_end0-Ltmp4                ##   Call between Ltmp4 and Lfunc_end0
	.long	Lset7
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z14apply_functionIlJNSt3__16vectorIiNS0_9allocatorIiEEEEiiiEET_PFS5_DpT0_ERKNS0_5tupleIJDpNS0_16remove_referenceINS0_9remove_cvIS6_E4typeEE4typeEEEE
	.weak_def_can_be_hidden	__Z14apply_functionIlJNSt3__16vectorIiNS0_9allocatorIiEEEEiiiEET_PFS5_DpT0_ERKNS0_5tupleIJDpNS0_16remove_referenceINS0_9remove_cvIS6_E4typeEE4typeEEEE
	.p2align	4, 0x90
__Z14apply_functionIlJNSt3__16vectorIiNS0_9allocatorIiEEEEiiiEET_PFS5_DpT0_ERKNS0_5tupleIJDpNS0_16remove_referenceINS0_9remove_cvIS6_E4typeEE4typeEEEE: ## @_Z14apply_functionIlJNSt3__16vectorIiNS0_9allocatorIiEEEEiiiEET_PFS5_DpT0_ERKNS0_5tupleIJDpNS0_16remove_referenceINS0_9remove_cvIS6_E4typeEE4typeEEEE
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
## BB#0:
	pushq	%rbp
Lcfi16:
	.cfi_def_cfa_offset 16
Lcfi17:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi18:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$40, %rsp
Lcfi19:
	.cfi_offset %rbx, -56
Lcfi20:
	.cfi_offset %r12, -48
Lcfi21:
	.cfi_offset %r13, -40
Lcfi22:
	.cfi_offset %r14, -32
Lcfi23:
	.cfi_offset %r15, -24
	movq	%rdi, -80(%rbp)         ## 8-byte Spill
	movl	32(%rsi), %eax
	movl	%eax, -72(%rbp)         ## 4-byte Spill
	movl	24(%rsi), %eax
	movl	%eax, -68(%rbp)         ## 4-byte Spill
	movl	28(%rsi), %r13d
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -64(%rbp)
	movq	$0, -48(%rbp)
	movq	(%rsi), %r14
	movq	8(%rsi), %rbx
	subq	%r14, %rbx
	movq	%rbx, %r12
	sarq	$2, %r12
	je	LBB5_7
## BB#1:
	movq	%r12, %rax
	shrq	$62, %rax
	jne	LBB5_2
## BB#4:
Ltmp6:
	movq	%rbx, %rdi
	callq	__Znwm
	movq	%rax, %r15
Ltmp7:
## BB#5:
	movq	%r15, -56(%rbp)
	movq	%r15, -64(%rbp)
	leaq	(%r15,%r12,4), %rax
	movq	%rax, -48(%rbp)
	testq	%rbx, %rbx
	jle	LBB5_7
## BB#6:
	movq	%r15, %rdi
	movq	%r14, %rsi
	movq	%rbx, %rdx
	callq	_memcpy
	addq	%rbx, %r15
	movq	%r15, -56(%rbp)
LBB5_7:
Ltmp11:
	leaq	-64(%rbp), %rdi
	movl	-68(%rbp), %esi         ## 4-byte Reload
	movl	%r13d, %edx
	movl	-72(%rbp), %ecx         ## 4-byte Reload
	callq	*-80(%rbp)              ## 8-byte Folded Reload
	movq	%rax, %rbx
Ltmp12:
## BB#8:
	movq	-64(%rbp), %rdi
	testq	%rdi, %rdi
	je	LBB5_12
## BB#9:
	movq	-56(%rbp), %rax
	cmpq	%rdi, %rax
	je	LBB5_11
## BB#10:
	leaq	-4(%rax), %rcx
	subq	%rdi, %rcx
	notq	%rcx
	andq	$-4, %rcx
	addq	%rax, %rcx
	movq	%rcx, -56(%rbp)
LBB5_11:
	callq	__ZdlPv
LBB5_12:
	movq	%rbx, %rax
	addq	$40, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB5_2:
Ltmp8:
	leaq	-64(%rbp), %rdi
	callq	__ZNKSt3__120__vector_base_commonILb1EE20__throw_length_errorEv
Ltmp9:
## BB#3:
LBB5_13:
Ltmp10:
	jmp	LBB5_15
LBB5_14:
Ltmp13:
LBB5_15:
	movq	%rax, %rbx
	movq	-64(%rbp), %rdi
	testq	%rdi, %rdi
	je	LBB5_19
## BB#16:
	movq	-56(%rbp), %rax
	cmpq	%rdi, %rax
	je	LBB5_18
## BB#17:
	leaq	-4(%rax), %rcx
	subq	%rdi, %rcx
	notq	%rcx
	andq	$-4, %rcx
	addq	%rax, %rcx
	movq	%rcx, -56(%rbp)
LBB5_18:
	callq	__ZdlPv
LBB5_19:
	movq	%rbx, %rdi
	callq	__Unwind_Resume
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table5:
Lexception1:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.asciz	"\303\200"              ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	65                      ## Call site table length
Lset8 = Ltmp6-Lfunc_begin1              ## >> Call Site 1 <<
	.long	Lset8
Lset9 = Ltmp7-Ltmp6                     ##   Call between Ltmp6 and Ltmp7
	.long	Lset9
Lset10 = Ltmp10-Lfunc_begin1            ##     jumps to Ltmp10
	.long	Lset10
	.byte	0                       ##   On action: cleanup
Lset11 = Ltmp7-Lfunc_begin1             ## >> Call Site 2 <<
	.long	Lset11
Lset12 = Ltmp11-Ltmp7                   ##   Call between Ltmp7 and Ltmp11
	.long	Lset12
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset13 = Ltmp11-Lfunc_begin1            ## >> Call Site 3 <<
	.long	Lset13
Lset14 = Ltmp12-Ltmp11                  ##   Call between Ltmp11 and Ltmp12
	.long	Lset14
Lset15 = Ltmp13-Lfunc_begin1            ##     jumps to Ltmp13
	.long	Lset15
	.byte	0                       ##   On action: cleanup
Lset16 = Ltmp8-Lfunc_begin1             ## >> Call Site 4 <<
	.long	Lset16
Lset17 = Ltmp9-Ltmp8                    ##   Call between Ltmp8 and Ltmp9
	.long	Lset17
Lset18 = Ltmp10-Lfunc_begin1            ##     jumps to Ltmp10
	.long	Lset18
	.byte	0                       ##   On action: cleanup
Lset19 = Ltmp9-Lfunc_begin1             ## >> Call Site 5 <<
	.long	Lset19
Lset20 = Lfunc_end1-Ltmp9               ##   Call between Ltmp9 and Lfunc_end1
	.long	Lset20
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2


.subsections_via_symbols
