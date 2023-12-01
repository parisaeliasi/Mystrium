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

	vld1.32	{q4,q5}, [r1]!				// MESSAGE ADDRESS is held by r1
	vld1.32	{q6,q7}, [r0]!				// KEY ADDRESS is held by r0
	
	vadd.u32	q4, q4, q6   				// q4 contains X0,X1,Y1,Y2
	vadd.u32	q5, q5, q7   				// q5 contains X2,X3,Y3,Y0
	
	vrev64.32	q6, q4					// q6 contains X1,X0,Y2,Y1
	vrev64.32	q7, q5					// q7 contains X3,X2,Y0,Y3
	
	vadd.u32	q8, q4,q5				// q8 contains X0+X2,X1+X3,Y1+Y3, Y0+Y2
	vadd.u32	q6, q8,q6				// q6 contains P0:X0+X2+X1,P3:X1+X3+X0, Q0:Y1+Y3+Y2, Q3:Y0+Y2+Y1,
	vadd.u32	q7, q8,q7				// q7 contains P2:X0+X2+X3,P1:X1+X3+X2, Q2:Y1+Y3+Y0, Q1:Y0+Y2+Y3 	
	
	vmlal.u32	q0, d8, d9					// q0 contains X0Y0,X1Y1 
	vmlal.u32	q1, d10 ,d11				// q1 contains X2Y2,X3Y3
	vmlal.u32	q2, d12, d13				// q2 contains P0Q0,P3Q3
	vmlal.u32	q3, d14, d15				// q3 contains P2Q2,P1Q1

	
	subs		r3, r3, #32
	bcc		.Lloop3_done

.Lloop3:

	
	vld1.32		{q4,q5}, [r1]!				// MESSAGE ADDRESS is held by r1
	vld1.32		{q6,q7}, [r0]!				// KEY ADDRESS is held by r0
	
	vadd.u32	q4, q4, q6   				// q4 contains X0,X1,Y1,Y2
	vadd.u32	q5, q5, q7   				// q5 contains X2,X3,Y3,Y0
	
	vrev64.32	q6, q4					// q6 contains X1,X0,Y2,Y1
	vrev64.32	q7, q5					// q7 contains X3,X2,Y0,Y3
	
	vadd.u32	q8, q4,q5				// q8 contains X0+X2,X1+X3,Y1+Y3, Y0+Y2
	vadd.u32	q6, q8,q6				// q6 contains P0:X0+X2+X1,P3:X1+X3+X0, Q0:Y1+Y3+Y2, Q3:Y0+Y2+Y1,
	vadd.u32	q7, q8,q7				// q7 contains P2:X0+X2+X3,P1:X1+X3+X2, Q2:Y1+Y3+Y0, Q1:Y0+Y2+Y3 	
	
	vmlal.u32	q0, d8, d9				// q0 contains X0Y0,X1Y1 
	vmlal.u32	q1, d10 ,d11				// q1 contains X2Y2,X3Y3
	vmlal.u32	q2, d12, d13				// q2 contains P0Q0,P3Q3
	vmlal.u32	q3, d14, d15				// q3 contains P2Q2,P1Q1
	
	subs		r3, r3, #32
	bcs     	.Lloop3
		
.Lloop3_done:
	vmov.64		d8,d7
	vmov.64		d7,d5
	vmov.64		d5,d8
		
	push		{r4,lr}	
	ldr 		r4,	[sp,#0]   
	vld1.32		{q4,q5}, [r4]!				// this is where we load T
	vld1.32		{q6,q7}, [r4]!
	
	veor.u32	q0, q4, q0
	veor.u32	q1, q5, q1
	veor.u32	q2, q6, q2
	veor.u32	q3, q7, q3
	
	pop			{r4,pc}
	
	vst1.8		{q0,q1}, [r2]!
	vst1.8		{q2,q3}, [r2]!	
	bx		lr

