MSC_base	:	.word	0x4800000C
MSC_value	:	.word	0x00005AA8
LED_base	:	.word	0x10600000
	.text
	.global debug_led_test
debug_led_test:
	stmfd sp!, {r0-r12, lr}
	ldr r0, MSC_base
	ldr r1, MSC_value
	str r1, [r0, #0]
	ldr	r0, LED_base
	mov r1, #0x11
	str r1, [r0, #0]
	ldmfd sp!,{r0-r12, pc}^
	.end
