.global f0
f0:
	pushl %ebp
	movl %esp, %ebp



/*   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
*/
	mov 8(%ebp), %ecx
	mov 12(%ebp), %ecx
	mov $5, %edx
	mov $15, %ecx
	mov $5, %ecx
	add $4, 8(%ebp)
	add $64, 8(%ebp)
	add $5, %edx
	add $127, %edx
	add %edx, 8(%ebp)
	mov %edx,  8(%ebp)

	add %ecx,%edx
	sub %ecx,%edx
	imul %ecx,%edx

	movl %ebp, %esp
	popl %ebp
	ret
	
/* 0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	b8 01 00 00 00       	mov    $0x1,%eax
   8:	89 ec                	mov    %ebp,%esp
   a:	5d                   	pop    %ebp
   b:	c3                   	ret 
*/