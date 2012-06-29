;#ifdef WINCE
	CODE32

	AREA	|.drectve|, DRECTVE

	EXPORT _colorCopy_arm
	EXPORT _colorBlend_arm
	EXPORT _colorBlendWithAlpha_arm

;----------------------------------------------------------------------------------
	AREA	|.text|, CODE, ARM


;void _colorCopy_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width);
_colorCopy_arm PROC
	stmdb       sp!, {r4 - r12, lr}
	mov         r10, r3
	ldr         r11, [sp, #40]
	ldr         lr, [sp, #44]
	cmp         r10, #0
	sub         r9, r11, r2, lsl #2
	sub         r12, lr, r2, lsl #2
	ble         _loop_y_end_colorCopy

_loop_y_begin_colorCopy
		bic         r11, r2, #7
		add         r11, r0, r11, lsl #2
		cmp         r0, r11
		add         lr, r0, r2, lsl #2
		beq         _loop_x_fast_end_colorCopy
		
		stmdb       sp!, {r9 - r10}
_loop_x_fast_begin_colorCopy
			ldmia       r1!, {r3 - r10}
			stmia       r0!, {r3 - r10}
			cmp         r0, r11
			bne         _loop_x_fast_begin_colorCopy
		ldmia       sp!, {r9 - r10}
_loop_x_fast_end_colorCopy
		
		cmp         r0, lr
		beq         _loop_x_end_colorCopy
_loop_x_begin_colorCopy
			ldr         r3, [r1], #4
			str         r3, [r0], #4
			cmp         r0, lr
			bne         _loop_x_begin_colorCopy
_loop_x_end_colorCopy
		add         r0, r0, r9
		add         r1, r1, r12
		subs        r10, r10, #1
		bne         _loop_y_begin_colorCopy
_loop_y_end_colorCopy
	ldmia       sp!, {r4 - r12, pc}
	ENDP  ; _colorCopy_arm
		
;----------------------------------------------------------------------------------

;void _colorBlend_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width);
_colorBlend_arm PROC
	stmdb       sp!, {r4 - r12, lr}
	mov         r10, r3
	ldr         r11, [sp, #40]
	ldr         lr, [sp, #44]
	cmp         r10, #0
	sub         r9, r11, r2, lsl #2
	sub         r12, lr, r2, lsl #2
	ble         _loop_y_end_colorBlend
		mov        r8, #0xFF
		orr        r8, r8, r8, lsl #16
_loop_y_begin_colorBlend
		bic         r11, r2, #3
		add         r11, r0, r11, lsl #2
		cmp         r0, r11
		add         lr, r0, r2, lsl #2
		beq         _loop_x_fast_end_colorBlend
_loop_x_fast_begin_colorBlend

			ldr         r3, [r1]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlend_0
			cmp         r4, #0xFF
			beq         _blendAsCopy_fast_colorBlend_0
				ldr         r6, [r0]
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
_blendAsCopy_fast_colorBlend_0
			str         r3, [r0]
_blendAsSkip_fast_colorBlend_0

			ldr         r3, [r1, #4]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlend_1
			cmp         r4, #0xFF
			beq         _blendAsCopy_fast_colorBlend_1
				ldr         r6, [r0, #4]
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
_blendAsCopy_fast_colorBlend_1
			str         r3, [r0, #4]
_blendAsSkip_fast_colorBlend_1


			ldr         r3, [r1, #8]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlend_2
			cmp         r4, #0xFF
			beq         _blendAsCopy_fast_colorBlend_2
				ldr         r6, [r0, #8]
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
_blendAsCopy_fast_colorBlend_2
			str         r3, [r0, #8]
_blendAsSkip_fast_colorBlend_2


			ldr         r3, [r1, #12]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlend_3
			cmp         r4, #0xFF
			beq         _blendAsCopy_fast_colorBlend_3
				ldr         r6, [r0, #12]
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
_blendAsCopy_fast_colorBlend_3
			str         r3, [r0, #12]
_blendAsSkip_fast_colorBlend_3

			add         r0, r0, #16
			add         r1, r1, #16
			cmp         r0, r11
			bne         _loop_x_fast_begin_colorBlend
_loop_x_fast_end_colorBlend
		cmp         r0, lr
		beq         _loop_x_end_colorBlend
_loop_x_begin_colorBlend
			ldr         r3, [r1]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_colorBlend
			cmp         r4, #0xFF
			beq         _blendAsCopy_colorBlend
				ldr         r6, [r0]
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
_blendAsCopy_colorBlend
			str         r3, [r0]
_blendAsSkip_colorBlend

			add         r0, r0, #4
			add         r1, r1, #4
			cmp         r0, lr
			bne         _loop_x_begin_colorBlend
_loop_x_end_colorBlend
		add         r0, r0, r9
		add         r1, r1, r12
		subs        r10, r10, #1
		bne         _loop_y_begin_colorBlend
_loop_y_end_colorBlend
	ldmia       sp!, {r4 - r12, pc}
	ENDP  ; _colorBlend_arm
	
;----------------------------------------------------------------------------------
	
;void _colorBlendWithAlpha_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width,long alpha);
_colorBlendWithAlpha_arm PROC
	stmdb       sp!, {r4 - r12, lr}
	mov         r10, r3
	ldr         r11, [sp, #40]
	ldr         lr, [sp, #44]
	cmp         r10, #0
	sub         r9, r11, r2, lsl #2
	sub         r12, lr, r2, lsl #2
	ble         _loop_y_end_colorBlendWithAlpha
		mov        r8, #0xFF
		orr        r8, r8, r8, lsl #16
_loop_y_begin_colorBlendWithAlpha
		stmdb       sp!, {r9}
		ldr         r9, [sp, #52] ;alpha
		
		bic         r11, r2, #3
		add         r11, r0, r11, lsl #2
		cmp         r0, r11
		add         lr, r0, r2, lsl #2
		beq         _loop_x_fast_end_colorBlendWithAlpha
_loop_x_fast_begin_colorBlendWithAlpha

			ldr         r3, [r1]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlendWithAlpha_0
				ldr         r6, [r0]
				mul         r4, r4, r9
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				mov         r4, r4, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
			str         r3, [r0]
_blendAsSkip_fast_colorBlendWithAlpha_0

			ldr         r3, [r1, #4]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlendWithAlpha_1
				ldr         r6, [r0, #4]
				mul         r4, r4, r9
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				mov         r4, r4, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
			str         r3, [r0, #4]
_blendAsSkip_fast_colorBlendWithAlpha_1


			ldr         r3, [r1, #8]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlendWithAlpha_2
				ldr         r6, [r0, #8]
				mul         r4, r4, r9
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				mov         r4, r4, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
			str         r3, [r0, #8]
_blendAsSkip_fast_colorBlendWithAlpha_2


			ldr         r3, [r1, #12]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_fast_colorBlendWithAlpha_3
				ldr         r6, [r0, #12]
				mul         r4, r4, r9
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				mov         r4, r4, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
			str         r3, [r0, #12]
_blendAsSkip_fast_colorBlendWithAlpha_3

			add         r0, r0, #16
			add         r1, r1, #16
			cmp         r0, r11
			bne         _loop_x_fast_begin_colorBlendWithAlpha
_loop_x_fast_end_colorBlendWithAlpha
		cmp         r0, lr
		beq         _loop_x_end_colorBlendWithAlpha
_loop_x_begin_colorBlendWithAlpha
			ldr         r3, [r1]
			movs        r4, r3, lsr #24
			beq         _blendAsSkip_colorBlendWithAlpha
				ldr         r6, [r0]
				mul         r4, r4, r9
				and         r5, r3, r8
				and         r3, r8, r3, lsr #8
				mov         r4, r4, lsr #8
				and         r7, r6, r8
				mul         r5, r5, r4
				mul         r3, r3, r4
				rsb         r4, r4, #0xFF
				and         r6, r8, r6, lsr #8
				mla         r5, r7, r4, r5
				mla         r3, r6, r4, r3
				and         r5, r8, r5, lsr #8
				and         r3, r3, r8, lsl #8
				orr         r3, r3, r5
			str         r3, [r0]
_blendAsSkip_colorBlendWithAlpha

			add         r0, r0, #4
			add         r1, r1, #4
			cmp         r0, lr
			bne         _loop_x_begin_colorBlendWithAlpha
_loop_x_end_colorBlendWithAlpha
		ldmia       sp!, {r9}
		
		add         r0, r0, r9
		add         r1, r1, r12
		subs        r10, r10, #1
		bne         _loop_y_begin_colorBlendWithAlpha
_loop_y_end_colorBlendWithAlpha
	ldmia       sp!, {r4 - r12, pc}
	ENDP  ; _colorBlendWithAlpha_arm
	
	
;----------------------------------------------------------------------------------
	


	END
	
;#endif