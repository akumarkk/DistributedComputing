DistributedComputing
====================
$How to compile :
		
		-	make Makefile 				//To compile server program
		-	make -f Makefile_client  	//To compile client program


Distributed Computing Client-Server implementation 

$Over-all-idea - There are 3 binaries used here:

					> User 		(Presenting the problem)
					> Server 	(Acts as a Controller. It is the one which receives the problem,
								 divide it, delegate it to the sub-servers(Retainers) and collates
								 the results once processing is done)
					> Retainers/Sub-servers 
								(These are the sub-servers associated with the Controller.
								 They first joins the Server-multicast group to associate themselves
								 with the server)


	3 Multi-cast IPs are used here:

		*	Retainer-Join-Address - Retainers should join this address. This is the communication 
									IP between Retainers and Server.
									[Server]--------------->[Retainers]

		*	Server-Join-Address	  - Server has to join this IP. This is the communication IP between
									Rerainers and Server.
									[Retainers]------------>[Server]

		*	User-Join-Address	  - User program has to join this IP.
									[User-App]------------->[Server]

$Config-files - server.conf client.conf
	These are the server and Client Configuration files. Here we can specify the IP 
	address to join, bind and port numbers. 
	The Keywords for configuration file can be found in "dc_parse_utils.h" file.

$Message-COmmunication - 
					ADDME_MESSAGE
	1.	[Server] <-----------------------------------------[Retainers] 

					AVAILABLE_GROUPS
	2.  [Server] ----------------------------------------->[Retainers]

					JOINED_GROUPS
	3.	[Server] <-----------------------------------------[Retainers]

					HEARTBEAT_MESSAGE
	4.	[Server] <-----------------------------------------[Retainers]

					PROBLEM_REQUEST
	5.  [Server] ----------------------------------------->[Retainers]

					PROBLEM_SOLUTION
	6.	[Server] <-----------------------------------------[Retainers]

	Server will collate the result and send it to the User-App
