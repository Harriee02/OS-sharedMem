#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

enum AccountOrTurn {BankAccount = 0, Turn =  1};
void  ClientProcess(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

		 time_t tt;
  		srandom((unsigned) time( & tt));

     if (argc != 5) {
          printf("Use: %s #1 #2 #3 #4\n", argv[0]);
          exit(1);
     }

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
    
     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }

     ShmPTR[BankAccount] = 0;
     ShmPTR[Turn] = 0;
     printf("Server has filled %d %d %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

     printf("Server is about to fork a child process...\n");
     pid = fork(); // forks and creates a new process
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {// child process
          ClientProcess(ShmPTR);
          exit(0);
     }
	// parent process

		int account;
		int amount;
		int i;

		// loop goes 25 times
		for (i=0; i < 25; i++){

			// random number from 0-5
			sleep(random() % 6);
			
			account = ShmPTR[BankAccount];
			while (ShmPTR[Turn] != 0){}
			if (account <= 100){
				amount = random() % 101;
				 if (amount % 2 == 0){
					 account += amount;
					 printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, account);
				 }
				 else{
					 printf("Dear old Dad: Doesn't have any money to give\n");
				 }
			}
			else{
				printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
			}
			ShmPTR[BankAccount] = account;
			ShmPTR[Turn] = 1;
		}
     wait(&status);
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void  ClientProcess(int  SharedMem[])
{
     printf("   Client process started\n");
     printf("   Client found %d %d %d %d in shared memory\n",
                SharedMem[0], SharedMem[1], SharedMem[2], SharedMem[3]);

    int account;
		int amount;
		int i;
		// loop goes 25 times
		for (i=0; i < 25; i++){
			sleep(random() % 6);
			account = SharedMem[BankAccount];
			while (SharedMem[Turn] != 1){}

			// random number from 0-5
			amount = sleep(random() % 6);
			printf("Poor Student needs $%d\n", amount);

			if (amount <= account){
				account -= amount;
				printf("Poor Student: Withdraws $%d / Balance = $%d\n", amount, account);
			}
			else{
				printf("Poor Student: Not Enough Cash ($%d)\n", account );
			}
			SharedMem[BankAccount] = account;
			SharedMem[Turn] = 0;
		}
		 printf("Client is about to exit\n");
}