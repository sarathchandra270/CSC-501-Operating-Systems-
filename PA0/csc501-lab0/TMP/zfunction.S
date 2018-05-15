.globl zfunction

zfunction: pushl %ebp
	   movl %esp,%ebp
	   movl 8(%esp), %eax
	   AND $0xffc01fff, %eax
	   shl $4, %eax

	   leave
	   ret


