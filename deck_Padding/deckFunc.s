.text
.align 8
.global deckFunction
.type	deckFunction, %function;

//********************************************************************
// r0 is the address for input messages
// r1 is the address for input keys
// r2 is the address for output
// r3 is the address for savestate
// r4 is the amount of output blocks (output of epsilon)
// r5 is the amount of input blocks (input to Multimixer-128)
//********************************************************************

deckFunction:
	
	push 		{r3,r4,r5,r6,r7,lr}	   
	ldr 		r5,	[sp,#28]   		
	ldr 		r4,	[sp,#24] 
	
	vmov.u64	q0, #0
	vmov.u64	q1, #0
	vmov.u64	q2, #0
	vmov.u64	q3, #0
	subs		r5, r5, #32
	bcc			.Lloop3_done_Multimixer

.Lloop3Once_Multimixer:

	vld1.32	    {q4,q5}, [r0]!			// MESSAGE ADDRESS is held by r0
	vld1.32	    {q6,q7}, [r1]!			// KEY ADDRESS is held by r1
	
	vadd.u32	q4, q4, q6   			// q4 contains X0,X1,Y1,Y2
	vadd.u32	q5, q5, q7   			// q5 contains X2,X3,Y3,Y0
	
	vrev64.32	q6, q4					// q6 contains X1,X0,Y2,Y1
	vrev64.32	q7, q5					// q7 contains X3,X2,Y0,Y3
	
	vadd.u32	q8, q4,q5				// q8 contains X0+X2,X1+X3,Y1+Y3, Y0+Y2
	vadd.u32	q6, q8,q6				// q6 contains P0:X0+X2+X1,P3:X1+X3+X0, Q0:Y1+Y3+Y2, Q3:Y0+Y2+Y1,
	vadd.u32	q7, q8,q7				// q7 contains P2:X0+X2+X3,P1:X1+X3+X2, Q2:Y1+Y3+Y0, Q1:Y0+Y2+Y3 	
	
	vmlal.u32	q0, d8, d9				// q0 contains X0Y0,X1Y1 
	vmlal.u32	q1, d10 ,d11			// q1 contains X2Y2,X3Y3
	vmlal.u32	q2, d12, d13			// q2 contains P0Q0,P3Q3
	vmlal.u32	q3, d14, d15			// q3 contains P2Q2,P1Q1

	subs		r5, r5, #32
	bcc			.Lloop3_done_Multimixer
	
.Lloop3_Multimixer:

	vld1.32		{q4,q5}, [r0]!			
	vld1.32		{q6,q7}, [r1]!			
	
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
	
	subs		r5, r5, #32
	bcs     	.Lloop3_Multimixer

.Lloop3_done_Multimixer:

	vmov.64		d8,d7
	vmov.64		d7,d5
	vmov.64		d5,d8		
	
	vmov.i32	q8, #251
	vmov.i32	q10, #251
	vmov.i32	q15, #33
	vmov.i32	q9, #1
	
	vmul.u32	q15, q8, q15   		    // q15 has (33*a)
	
.PrepLoopr4: 
	vmov		r6, r7, d18      
	cmp 		r6, #1
	beq			.PrepLoop
	add 		r4, r4, #64
	
.PrepLoop:

	vmov.i32	q8, #251
	vmov.i32	q9, #1
	vadd.u32	q0, q0, q15   

	vmov.u32	q11, q0         		// this is for xor loop
	vmov.u32	q12, q1
	vmov.u32	q13, q2
	vmov.u32	q14, q3
	
	//************** round1&2 **************  
	vmlal.u32	q1, d0, d4				// (l)q0*q2+q1
	vmlal.u32	q3, d1, d5				// (h)q0*q2+q3
	
	vadd.u32	q4, q1, q3				// Apply matrix
	vrev64.32	q5, q1					// Apply matrix
	vrev64.32	q6, q3					// Apply matrix
	vadd.u32	q3, q4, q5				// Apply matrix
	vadd.u32	q1, q4, q6				// Apply matrix
	
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
	
	veor.u32	q0, q11, q0
	veor.u32	q1, q12, q1
	veor.u32	q2, q13, q2
	veor.u32	q3, q14, q3	
	
	vst1.8		{q0,q1}, [r3]!
	vst1.8		{q2,q3}, [r3]
	
	subs 		r4, r4, #64
	bcc	 		.Lloop3_done
		
.Lloop3Once:

	vadd.u32	q0, q0, q8      		// a is added we always seperate the first loop getting z_0 out.
	vadd.u32	q2, q2, q9      		// 1 is added to q2
	
	vmov.u32	q11, q0         		// this is for xor loop
	vmov.u32	q12, q1
	vmov.u32	q13, q2
	vmov.u32	q14, q3

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
	
	veor.u32	q4, q11, q0
	veor.u32	q5, q12, q1
	veor.u32	q6, q13, q2
	veor.u32	q7, q14, q3	
	
	vst1.8		{q4,q5}, [r2]!	
	vst1.8		{q6,q7}, [r2]!
	
	vadd.u32	q8, q8, q10     		// here we have generated 2*a
	vshl.u32	q9, q9, #1	    		// here we have generated 2,  after this we always have 2 in q9.
	
	subs		r4, r4, #64
	bcc			.Lloop3_done

.Lloop3:

	sub 		r3, r3, #32
	vld1.8		{q0,q1},[r3]!
	vld1.8		{q2,q3},[r3]

	vmov		r6, r7, d18       		// we just want to see if it became zero!
	cmp 		r6, #0
	beq			.PrepLoopr4
	
	vadd.u32	q0, q0, q8      		// first time that we come in this loop (2*a,2) added 
	vadd.u32	q2, q2, q9      		// second time we come in this loop (3*a, 4) is added and so on. 
	
	vmov.u32	q11, q0         		// this is for xor loop
	vmov.u32	q12, q1
	vmov.u32	q13, q2
	vmov.u32	q14, q3

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
	
	veor.u32	q4, q11, q0
	veor.u32	q5, q12, q1
	veor.u32	q6, q13, q2
	veor.u32	q7, q14, q3	
	
	vst1.8		{q4,q5}, [r2]!	
	vst1.8		{q6,q7}, [r2]!	
	
	vadd.u32	q8, q8, q10     // here we have 3*a, 4*a, etc.
	vshl.u32	q9, q9, #1	    // here we have 4, 8, etc.
	
	subs		r4, r4, #64
	bcs     	.Lloop3
	
		
.Lloop3_done:
	pop		{r3,r4,r5,r6,r7,pc}
	bx		lr

