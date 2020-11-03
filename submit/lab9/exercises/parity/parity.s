	.text
	.globl get_parity
#edi contains n	
get_parity:

	#@TODO: add code here to set eax to 1 iff edi has even parity
	test	%eax, %eax	#set flag
	jpe	if_even_par
	xorl	%eax, %eax
	ret
	
if_even_par:
	movl	$1, %eax
	ret
