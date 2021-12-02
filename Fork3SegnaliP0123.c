/*
	Scrivere un programma in C in cui un processo padre P0 crea tre processi figli P3, P2, P1.

	P0, dopo aver creato i processi figli, invia a P1 il segnale SIGUSR1 e si metterà in attesa di un segnale SIGUSR1.

	P1 attenderà il segnale SIGUSR1 da P0, quando questo arriva, P1 stampa a video il proprio PID, invia un segnale SIGUSR1 a P2 e termina.

	P2 attenderà il segnale SIGUSR1 da P1, quando questo arriva, P2 stampa a video il proprio PID, invia un segnale SIGUSR1 a P3 e termina.

	P3 attenderà il segnale SIGUSR1 da P2, quando questo arriva, P3 stampa a video il proprio PID, invia un segnale SIGUSR1 a P0 e termina.

	P0 all'arrivo del segnale SIGUSR1 stampa a video "Bye!" e termina.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define N 3

int pidvector[N]; //mi serve per conservarmi i pid dei figli (per fare le kill() )

void sleepfunction(int i){
    printf("Sono il figlio %d pid %d, aspetto un segnale...\n",i,pidvector[i]);
    sleep();
    //quando arriva questo segnale, a sua volta il figlio deve mandarlo al figlio successivo
    if(i<N-1) //if i<2  //sono il primo e il secondo figlio
        kill(pidvector[i+1],SIGUSR1);
    else //è il terzo figlio, deve inviarlo al padre
        kill(getppid(),SIGUSR1);
}

void signal_handler(int sig){
    printf("Child %d, segnale SIGUSR1 Ricevuto.\n",getpid());
}

void Parent_handler(int sig){
    printf("Ricevuto segnale finale");
    exit(EXIT_SUCCESS);
}

int main(){
    int i,status;
    pid_t pid;
    pid_t ppid;
    struct sigaction action;

    action.sa_handler=signal_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;

	sigaction(SIGUSR1, &action, NULL);

    //int pidvector[N]; //definito fuori per la f //mi serve per conservarmi i pid dei figli (per fare le kill() )
    for(i=0;i<N;i++){ //inizializzo il vettore
        pidvector[i]=0;
    }
    
    for(i=0;i<N;i++){
        pid=fork(); //Parent crea P1,P2,P3, che si mettono in attesa di un segnale con sleep().
        switch(pid){
            case 0:
                sleepfunction(i);
                exit(EXIT_SUCCESS); //quando i figli finiscono della funzione, e hanno killato, finiscono.
                break;
            default:
                pidvector[i]=pid;

                break;
        }
    }

    //ora è il padre, dopo aver creato i 3 figli : essi sono in attesa di segnale.
    //esso invia il segnale al primo figlio
    kill(pidvector[0],SIGUSR1);
    //si scatena la catena di invii tra i figli
    sleep(); //il padre si mette in attesa di ricevere il segnale dall'ultimo figlio, cioè l'else di sleepfunction
    signal(SIGUSR1,Parent_handler);
    //la funzione fa l'exit

}