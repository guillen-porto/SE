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
	.file	"reverse2.s"
	.text
	.align	1
	.p2align 2,,3
	.global	reverse_int
	.syntax unified
	.code	16
	.thumb_func
	.type	reverse_int, %function
reverse_int:
    movs r3, r0
    push {lr}
    movs r2, #32
    movs r0, #0
    movs r1, #0
.L2:
    lsls r0, r0, #1
    lsrs r3, r3, #1
    adcs r0, r1
    subs r2, r2, #1
    bne .L2
    pop {pc}