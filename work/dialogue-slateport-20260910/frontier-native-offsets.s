	.cpu arm7tdmi
	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"frontier-native-offsets.c"
	.text
	.align	1
	.p2align 2,,3
	.global	FrontierRegisteredItemOffset
	.syntax unified
	.code	16
	.thumb_func
	.type	FrontierRegisteredItemOffset, %function
FrontierRegisteredItemOffset:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, .L3
	@ sp needed
	bx	lr
.L4:
	.align	2
.L3:
	.word	1174
	.size	FrontierRegisteredItemOffset, .-FrontierRegisteredItemOffset
	.ident	"GCC: (Arm GNU Toolchain 15.3.Rel1 (Build arm-15.149)) 15.3.1 20260627"
