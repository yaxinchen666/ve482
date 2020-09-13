1. layer directory

	This directory contains the implementation of three layers and two main functions corresponding to two user interfaces. 

	Three layers: 
	list: list_t.c/list_t.h
	logic: logic.c/logic.h
	interface: interface.c/interface.h

	Two interfaces:
	main.c: command line interface
	mainmenu.c: Menu interface

	To run this program, type:
		gcc –o commandlinemain main.c interface.c list_t.c logic.c
		./commandlinemain
	or:
		gcc –o menumain mainmenu.c interface.c list_t.c logic.c
		./menumain


2. API_list directory
	
	This directory contains revised list_t.c/list_t.h and logic.c/logic.h, as well as the API implemented in lab5_dlist.c/lab5_dlist.h.

	The main function is used to test the functions in lab5_dlist.c/h.

