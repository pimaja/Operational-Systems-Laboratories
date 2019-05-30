#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <semaphore.h>

#define LAZ 0
#define ISTINA 1
#define KRAJ_RADA 2
uint64_t A=8531648002905263869ULL, B=18116652324302926351ULL;
uint64_t MS[10] = {0};
int kraj;
int N=3;

int trenutni = 0; // za popunjavanje spremnika
int sljedeci = 0; //broj za kojim je dan zahtjev
int BROJ[3];
int ULAZ[3];

char spremnik[50];
int lista_blokova[5]={4, 6, 8, 13, 19};

sem_t KO, NEMA_MEM;

int test_bitovi( uint64_t x )
{
    int i, brojac0 = 0, brojac1 = 0;
    for(i = 0; i < 64; i++){
        if((x & (1ULL<<i)) != 0){  // 1<<i je shift 1 lijevo za i pozicija, & je logicko I
            brojac1++;
            if(brojac1==3)
                return LAZ;
            brojac0=0;
        }
        else{
            brojac0++;
            if(brojac0==3)
                return LAZ;
            brojac1=0;
        }
    }
    return ISTINA;
}

int test_pseudo_prost( uint64_t x )
{
    int djelitelj = 3; //2 smo eliminirali s bitwise OR, gledamo sljedeci prost djelitelj
    while( djelitelj <= 80000 ){
        if( x % djelitelj == 0){
            //printf("Nije prost broj!\n");
            return LAZ;
        }
        else
            //gledamo sljedeci neparan broj s kojim bi mogao biti djeljiv
            //sigurno nije djeljiv s parnim brojem jer nije djeljiv s 2
            djelitelj += 2;
    }
    return ISTINA;
}

void *radna_dretva(void *id){
	do{
        uint64_t x = ((rand() * A) % B) | 1;
        int broj_operacija = 0;
        while(test_bitovi(x) == LAZ || test_pseudo_prost(x) == LAZ){
            if(x <= 0xffffffffffffffffULL-2 && broj_operacija <= 400){
                x = x + 2;
                broj_operacija += 1;
            }
            else{
                x = ((rand() * A) % B) | 1;
                broj_operacija = 0;
            }
        }

		sem_wait(&KO);

		MS[trenutni] = x;
		trenutni += 1;
		if (trenutni == 10)
		    trenutni = 0;

		sem_post(&KO);

	}while (kraj!=KRAJ_RADA);
}

int zauzmi(uint64_t velicina, int id_dretve){
	int i, j, index, test;
	for(i=0; i<5; i++)
		if(lista_blokova[i]>=velicina){
			index=0;
			for(j=0; j<i; j++)
				index += lista_blokova[j];
			test=1;
			for(j=index; j<index+velicina; j++)
				if(spremnik[j]!='-'){
					test=0;
					break;
				}
			if(test==1){
				for(j=index; j<index+velicina; j++)
					spremnik[j]=id_dretve+'0';
				return 1;
			}
		}
	return -1;
}

void oslobodi(int x, int id_dretve){
	int i=0;
	while(spremnik[i] != id_dretve+'0')
		i++;
	while(spremnik[i]==id_dretve+'0'){
		spremnik[i]='-';
		i++;
	}
	x=1;
	return;
}

void *dretva_provjera(void *id){
	do{
		sem_wait(&KO);

		uint64_t y = MS[sljedeci];
		sljedeci += 1;
		if (sljedeci == 10)
			sljedeci = 0;
		int *d = id;
		printf("id = %d, ", *d);
		printf("y = ""%" PRIu64 ",", y);
		printf(" uzeo broj\n");
		printf("y mod 20 = ""%" PRIu64 ",", y%20);
		
		int x;
		do{
			x = zauzmi(y%20, *d);
			printf("x=%d\n", x);
			if(x==-1){
				sem_post(&KO);
				sem_wait(&NEMA_MEM);
				sem_wait(&KO);
			}
		}while (x==-1);
		
		int i;
		printf("spremnik: ");
		for(i=0; i<50; i++)
			printf("%c ", spremnik[i]);
		printf("\n");
		
		sem_post(&KO);
		
		sleep(y%5);
		
		sem_wait(&KO);
		
		printf("id = %d, ", *d);
		printf("y = ""%" PRIu64 ",", y);
		printf(" potrosio broj\n");
		
		oslobodi(x, *d);
		printf("spremnik: ");
		for(i=0; i<50; i++)
			printf("%c ", spremnik[i]);
		printf("\n");
		
		sem_post(&KO);
		sem_post(&NEMA_MEM);
		
		sleep(y%5);

	}while (kraj!=KRAJ_RADA);
}

int main()
{
	srand(time(NULL));

	int i, j, BRR[3], BRP[3];
	pthread_t radna[3];
	pthread_t provjera[3];

	sem_init(&KO, 0, 1);
	sem_init(&NEMA_MEM, 0, 1);
	
	for(i=0; i<50; i++)
		spremnik[i]= '-';

	for(i=0; i<3; i++){
		BRR[i] = i;
		if(pthread_create(&radna[i], NULL, radna_dretva, &BRR[i]) != 0){
			printf("Ne mogu stvoriti dretvu!");
			exit(1);
		}
	}
	sleep(2);
	for(i=0; i<3; i++){
		BRP[i] = i;
		if(pthread_create(&provjera[i], NULL, dretva_provjera, &BRP[i]) != 0){
			printf("Ne mogu stvoriti dretvu!");
			exit(1);
		}
	}
	sleep(30);
	kraj = KRAJ_RADA;
	for(j=0; j<3; j++)
		pthread_join(radna[j], NULL);
	for(j=0; j<3; j++)
		pthread_join(provjera[j], NULL);
	return 0;
}
