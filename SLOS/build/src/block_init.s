	.text
	.global block_init
block_init:
	stmfd	sp!, {r0-r2}
	ldr		r0, =0xA0600000	 @ BLOCK_INIT_PTR
	ldr		r1, =0xA4000000  @ BLOCK_END_PTR
	mov		r2, #0x0
1:	str		r2, [r0], #4
	cmp		r0, r1
	bne		1b
	ldmfd	sp!, {r0-r2}
	movs	pc, lr
	.end
