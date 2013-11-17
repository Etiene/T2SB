.global f0
f0:
	pushl %ebp
	movl %esp, %ebp



/*   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
*/
	mov $1, %eax

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