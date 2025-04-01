	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"main_orig.c"
	.text
	.align	1
	.p2align 2,,3
	.global	reverse_int
	.syntax unified
	.code	16
	.thumb_func
	.type	reverse_int, %function
reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0
	push	{lr}
	movs	r2, #31
	movs	r0, #0
.L2:
	movs	r1, #1
	ands	r1, r3
	subs	r2, r2, #1
	orrs	r0, r1
	lsls	r0, r0, #1
	lsrs	r3, r3, #1
	cmp	r2, #0
	bne	.L2
	movs	r1, #1
	ands	r1, r3
	orrs	r0, r1
	@ sp needed
	pop	{pc}
	.size	reverse_int, .-reverse_int
	.ident	"GCC: (15:12.2.rel1-1) 12.2.1 20221205"
