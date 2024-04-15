.text
.align 8
.global Multimixer128field
.type	Multimixer128field, %function;

Multimixer128field:

	vmov.u64	q0, #0
	vmov.u64	q1, #0
	vmov.u64	q2, #0
	vmov.u64	q3, #0
	subs		r3, r3, #32
	bcc		.Lloop3_done
.Lloop3Once:

	vld1.32	{q4,q5}, [r1]!				
	vld1.32	{q6,q7}, [r0]!				
	
	vadd.u32	q4, q4, q6   				
	vadd.u32	q5, q5, q7   				
	
	vrev64.32	q6, q4					
	vrev64.32	q7, q5					
	
	vadd.u32	q8, q4,q5				
	vadd.u32	q6, q8,q6				
	vadd.u32	q7, q8,q7				
	
	vmlal.u32	q0, d8, d9					
	vmlal.u32	q1, d10 ,d11				
	vmlal.u32	q2, d12, d13				
	vmlal.u32	q3, d14, d15				

	
	subs		r3, r3, #32
	bcc		.Lloop3_done

.Lloop3:

	
	vld1.32		{q4,q5}, [r1]!				
	vld1.32		{q6,q7}, [r0]!				
	
	vadd.u32	q4, q4, q6   				
	vadd.u32	q5, q5, q7   				
	
	vrev64.32	q6, q4					
	vrev64.32	q7, q5					
	
	vadd.u32	q8, q4,q5				
	vadd.u32	q6, q8,q6				
	vadd.u32	q7, q8,q7				
	
	vmlal.u32	q0, d8, d9				
	vmlal.u32	q1, d10 ,d11				
	vmlal.u32	q2, d12, d13				
	vmlal.u32	q3, d14, d15				
	
	subs		r3, r3, #32
	bcs     	.Lloop3
		
.Lloop3_done:
	vmov.64		d8,d7
	vmov.64		d7,d5
	vmov.64		d5,d8
		
	push		{r4,lr}	
	ldr 		r4,	[sp,#0]   
	vld1.32		{q4,q5}, [r4]!				
	vld1.32		{q6,q7}, [r4]!
	
	vadd.i64	q0, q4, q0           
	vadd.i64	q1, q5, q1
	vadd.i64	q2, q6, q2
	vadd.i64	q3, q7, q3
	
	pop			{r4,pc}
	
	vst1.8		{q0,q1}, [r2]!
	vst1.8		{q2,q3}, [r2]!	
	bx		lr

