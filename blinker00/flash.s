



.equ GPIOABASE, 0x48000000
.equ RCCBASE  , 0x40021000
.equ DELAYCOUNTS, 0x40000

.thumb

.thumb_func
.global _start
_start:
stacktop: .word 0x20001000
.word reset
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang

.thumb_func
reset:

    ldr r0,=RCCBASE
    ldr r1,[r0,#0x14]
    ldr r2,=0x00020000
    orr r1,r2
    str r1,[r0,#0x14]

    ldr r0,=GPIOABASE

    ldr r1,[r0,#0x00]
    ldr r2,=0x00000300
    bic r1,r2
    ldr r2,=0x00000100
    orr r1,r2
    str r1,[r0,#0x00]

    ldr r1,[r0,#0x04]
    ldr r2,=0x00000010
    bic r1,r2
    str r1,[r0,#0x04]

loop_top:
    ldr r1,=0x00000010
    str r1,[r0,#0x18]
    
    ldr r2,=DELAYCOUNTS
loop0:
    sub r2,#1 ;@ subs
    bne loop0
    
    ldr r1,=0x00100000
    str r1,[r0,#0x18]
    
    ldr r2,=DELAYCOUNTS
loop1:
    sub r2,#1 ;@ subs
    bne loop1

    b loop_top

.thumb_func
hang:   b .

.end

