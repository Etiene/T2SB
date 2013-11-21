.global f0
f0:
	pushl %ebp
	movl %esp, %ebp



/*   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
*/
	mov 8(%ebp), %eax
	mov 12(%ebp), %eax
	mov $5, %eax
	mov $15, %eax
	mov $5, %ecx
	add $4, 8(%ebp)
	add $64, 8(%ebp)
	add $5, %edx
	add $127, %edx
	add %edx, 8(%ebp)
	mov %eax,  8(%ebp)
	mov %eax,  12(%ebp)

	add %edx,%eax
	sub %edx,%eax
	imul %edx,%eax

	push 8(%ebp)
	call f1
	add $4, %esp

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


.global f1
f1:
	pushl %ebp
	movl %esp, %ebp

	mov $3, %eax
	movl %ebp, %esp
	popl %ebp
	ret

