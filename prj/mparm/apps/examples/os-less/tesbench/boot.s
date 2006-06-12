	.section .text.init, #alloc
	.type	stext, #function
	.align  2

__start:	/* set stack pointer and bss limits */
		adr	r3, __switch_data
		ldmia	r3, {r4, r5, sp}@       @ sp = stack pointer

		mov	fp, #0			@ Clear BSS (and zero fp)
1:		cmp	r4, r5
		strcc	fp, [r4],#4
		bcc	1b

		bl	main
		b .

__switch_data:	.long	__bss_start
		.long	__bss_end
		.long	__stack_start

	.section .resetvector1, #alloc
	.type	stext, #function
		b	__start
