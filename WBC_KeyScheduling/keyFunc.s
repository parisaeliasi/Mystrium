.text
.align 8
.global keyFunction
.type	keyFunction, %function;
keyFunction:
	
	vld1.32		{q0,q1}, [r0]!	// message is loaded from address of the message saved in r0
	vld1.32		{q2,q3}, [r0]!	// message is loaded from address of the message saved in r0, input is loaded completely
	
	subs 		r2, r2, #64
	bcc	 		.Lloop3_done
		
.Lloop3Once:

	//************** round1&2 **************  expansionFunction(message, output ,(size_t)messageLen) = (const uint8_t *input1, const uint8_t *output, size_t outputLen)
	vmlal.u32	q1, d0, d4		
	vmlal.u32	q3, d1, d5		
	
	vadd.u32	q4, q1, q3		
	vrev64.32	q5, q1			
	vrev64.32	q6, q3			
	vadd.u32	q3, q4, q5		
	vadd.u32	q1, q4, q6		
	
	vmlal.u32	q0, d2, d6		
	vmlal.u32	q2, d3, d7		
	
	vadd.u32	q4, q0, q2	
	vrev64.32	q5, q0			
	vrev64.32	q6, q2			
	vadd.u32	q2, q4, q5		
	vadd.u32	q0, q4, q6		

	//************** round3&4 **************
	vmlal.u32	q1, d0, d4		
	vmlal.u32	q3, d1, d5		
	
	vadd.u32	q4, q1, q3		
	vrev64.32	q5, q1			
	vrev64.32	q6, q3			
	vadd.u32	q3, q4, q5		
	vadd.u32	q1, q4, q6		
	
	vmlal.u32	q0, d2, d6		
	vmlal.u32	q2, d3, d7		
	
	vadd.u32	q4, q0, q2	
	vrev64.32	q5, q0			
	vrev64.32	q6, q2			
	vadd.u32	q2, q4, q5		
	vadd.u32	q0, q4, q6	
	
	//************** round5&6 **************
	vmlal.u32	q1, d0, d4		
	vmlal.u32	q3, d1, d5		
	
	vadd.u32	q4, q1, q3		
	vrev64.32	q5, q1			
	vrev64.32	q6, q3			
	vadd.u32	q3, q4, q5		
	vadd.u32	q1, q4, q6		
	
	vmlal.u32	q0, d2, d6		
	vmlal.u32	q2, d3, d7		
	
	vadd.u32	q4, q0, q2	
	vrev64.32	q5, q0			
	vrev64.32	q6, q2			
	vadd.u32	q2, q4, q5		
	vadd.u32	q0, q4, q6	
	
	vst1.8		{q0,q1}, [r1]!	
	vst1.8		{q2,q3}, [r1]!	
	
	subs		r2, r2, #64
	bcc			.Lloop3_done

.Lloop3:

	//************** round1&2 **************  
	vmlal.u32	q1, d0, d4		
	vmlal.u32	q3, d1, d5		
	
	vadd.u32	q4, q1, q3		
	vrev64.32	q5, q1			
	vrev64.32	q6, q3			
	vadd.u32	q3, q4, q5		
	vadd.u32	q1, q4, q6		
	
	vmlal.u32	q0, d2, d6		
	vmlal.u32	q2, d3, d7		
	
	vadd.u32	q4, q0, q2	
	vrev64.32	q5, q0			
	vrev64.32	q6, q2			
	vadd.u32	q2, q4, q5		
	vadd.u32	q0, q4, q6		

	//************** round3&4 **************
	vmlal.u32	q1, d0, d4		
	vmlal.u32	q3, d1, d5		
	
	vadd.u32	q4, q1, q3		
	vrev64.32	q5, q1			
	vrev64.32	q6, q3			
	vadd.u32	q3, q4, q5		
	vadd.u32	q1, q4, q6		
	
	vmlal.u32	q0, d2, d6		
	vmlal.u32	q2, d3, d7		
	
	vadd.u32	q4, q0, q2	
	vrev64.32	q5, q0			
	vrev64.32	q6, q2			
	vadd.u32	q2, q4, q5		
	vadd.u32	q0, q4, q6
	
	vst1.8		{q0,q1}, [r1]!	
	vst1.8		{q2,q3}, [r1]!	
	
	subs		r2, r2, #64
	bcs     	.Lloop3
		
.Lloop3_done:
	bx		lr

