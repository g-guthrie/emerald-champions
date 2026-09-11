.syntax unified
.cpu arm7tdmi
.section .header,"ax"
.incbin "game/assets/header.bin"
.section .text.boot,"ax"
.arm
.global _start
_start:
    mov r0, #0x12
    msr cpsr_c, r0
    ldr sp, =0x03007fa0
    mov r0, #0x1f
    msr cpsr_c, r0
    ldr sp, =0x03007f00
    ldr r0, =__data_load
    ldr r1, =__data_start
    ldr r2, =__data_end
1:  cmp r1, r2
    ldrlo r3, [r0], #4
    strlo r3, [r1], #4
    blo 1b
    ldr r1, =__bss_start
    ldr r2, =__bss_end
    mov r3, #0
2:  cmp r1, r2
    strlo r3, [r1], #4
    blo 2b
    ldr r0, =main
    bx r0
.pool
