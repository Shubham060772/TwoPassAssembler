;NAME- 		Shubham Agrawal
;Roll Number- 	2301AI44
;Decaration of Authorship 
;swap from location 1 and location 2
ldc l1 ;
ldnl 0   
ldc temp 
stnl 0   
ldc l2 
ldnl 0;
ldc l1 
stnl 0;
ldc temp 
ldnl 0
ldc l2
stnl 0
; data segment
l1:	data	0x0008 
temp:   data    0x0000 
l2:     data    0x0005 
