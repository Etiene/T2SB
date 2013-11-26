.global f0
f0:
	pushl %ebp
	movl %esp, %ebp



/*   0:	55                   	push   %ebp
  	 1:	89 e5                	mov    %esp,%ebp
*/
	cmp $0,8(%ebp)
	jne L1
	mov 8(%ebp), %eax
	mov 12(%ebp), %eax
	mov -4(%ebp), %eax
	mov -4(%ebp), %edx
	mov $5, %eax
	mov $15, %eax
	mov $5, %ecx
	mov $5, %edx
L1:
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
	
/*00000000 <f0>:
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	8b 45 08             	mov    0x8(%ebp),%eax
   6:	8b 45 0c             	mov    0xc(%ebp),%eax
   9:	b8 05 00 00 00       	mov    $0x5,%eax
   e:	b8 0f 00 00 00       	mov    $0xf,%eax
  13:	b9 05 00 00 00       	mov    $0x5,%ecx
  18:	83 45 08 04          	addl   $0x4,0x8(%ebp)
  1c:	83 45 08 40          	addl   $0x40,0x8(%ebp)
  20:	83 c2 05             	add    $0x5,%edx
  23:	83 c2 7f             	add    $0x7f,%edx
  26:	01 55 08             	add    %edx,0x8(%ebp)
  29:	89 45 08             	mov    %eax,0x8(%ebp)
  2c:	89 45 0c             	mov    %eax,0xc(%ebp)
  2f:	01 d0                	add    %edx,%eax
  31:	29 d0                	sub    %edx,%eax
  33:	0f af c2             	imul   %edx,%eax
  36:	ff 75 08             	pushl  0x8(%ebp)
  39:	e8 fc ff ff ff       	call   3a <f0+0x3a>
  3e:	83 c4 04             	add    $0x4,%esp
  41:	89 ec                	mov    %ebp,%esp
  43:	5d                   	pop    %ebp
  44:	c3                   	ret    

00000045 <f1>:
  45:	55                   	push   %ebp
  46:	89 e5                	mov    %esp,%ebp
  48:	b8 03 00 00 00       	mov    $0x3,%eax
  4d:	89 ec                	mov    %ebp,%esp
  4f:	5d                   	pop    %ebp
  50:	c3                   	ret 
*/


.global f1
f1:
	pushl %ebp
	movl %esp, %ebp

	mov $3, %eax
	movl %ebp, %esp
	popl %ebp
	ret

