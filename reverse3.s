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
	.file	"reverse3.s"
	.text
	.align	1
	.p2align 2,,3
	.global	reverse_int
	.syntax unified
	.code	16
	.thumb_func
	.type	reverse_int, %function
reverse_int:
    movs r1, #0x55
    lsls r1, #8
    movs r1, #0x55
    lsls r1, #8
    movs r1, #0x55
    lsls r1, #8
    movs r1, #0x55
    push {lr}
    movs r2, r0
    ands r2, r1
    lsrs r0, r0, #1
    ands r0, r1
    lsls r2, r2, #1
    orrs r0, r2
    movs r1, #0x33
    lsls r1, #8
    movs r1, #0x33
    lsls r1, #8
    movs r1, #0x33
    lsls r1, #8
    movs r1, #0x33
    movs r2, r0
    ands r2, r1
    lsrs r0, r0, #2
    ands r0, r1
    lsls r2, r2, #2
    orrs r0, r2
    movs r1, #0x0F
    lsls r1, #8
    movs r1, #0x0F
    lsls r1, #8
    movs r1, #0x0F
    lsls r1, #8
    movs r1, #0x0F
    movs r2, r0
    ands r2, r1
    lsrs r0, r0, #4
    ands r0, r1
    lsls r2, r2, #4
    orrs r0, r2
    rev r0, r0 
    pop {pc}