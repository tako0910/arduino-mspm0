.syntax unified
.thumb

.section .bss._variant_heap_end, "aw", %nobits
.align 2
.global _variant_heap_end
_variant_heap_end:
    .word 0

.text
.align 2
.global _sbrk
.type _sbrk, %function
.thumb_func
_sbrk:
    push    {r4, lr}
    ldr     r1, =_variant_heap_end
    ldr     r2, [r1]
    cmp     r2, #0
    bne     1f
    ldr     r2, =_end
    str     r2, [r1]
1:
    mov     r4, r2
    adds    r3, r2, r0
    ldr     r2, =__StackLimit
    cmp     r3, r2
    bhi     2f
    str     r3, [r1]
    mov     r0, r4
    pop     {r4, pc}
2:
    movs    r0, #0
    mvns    r0, r0
    pop     {r4, pc}

.size _sbrk, . - _sbrk
