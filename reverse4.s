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
	.file	"reverse4.c"
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
	@ link register save eliminated.
	movs	r3, r0
	@ sp needed
	ldr	r2, .L3
	lsls	r0, r0, #1
	ands	r0, r2
	ldr	r2, .L3+4
	lsrs	r3, r3, #1
	ands	r3, r2
	orrs	r0, r3
	ldr	r2, .L3+8
	lsls	r3, r0, #2
	ands	r3, r2
	ldr	r2, .L3+12
	lsrs	r0, r0, #2
	ands	r0, r2
	orrs	r3, r0
	ldr	r2, .L3+16
	lsls	r0, r3, #4
	ands	r0, r2
	ldr	r2, .L3+20
	lsrs	r3, r3, #4
	ands	r3, r2
	orrs	r0, r3
	rev	r0, r0
	bx	lr
.L4:
	.align	2
.L3:
	.word	-1431655766
	.word	1431655765
	.word	-858993460
	.word	858993459
	.word	-252645136
	.word	252645135
	.size	reverse_int, .-reverse_int
	.ident	"GCC: (15:12.2.rel1-1) 12.2.1 20221205"
