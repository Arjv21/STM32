ATCMD_A7672_Callback.c -- This File contains all the callbacks that processes the responses of the AT Commands issued to the Modem
ATCMD_A7672_Commands.c -- This File contains all the sets of commands issued for initializing the Modem, Checking the network Status, 
						  Connecting the Modem to HTTPS and MQTTS sessions and Sending and Recieving Messages.
ATCMD_Core.c		   -- This contains the core Thread which runs the State Machine of the System and Manages when and how to issue 
						  AT Commands.
ATCMD_Queue_Mgmt.c	   -- This File contains all the functions that are used to Manage the Queue of AT Commands to be issued to the 
						  Modem
							