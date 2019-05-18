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

pthread_mutex_t monitor;
pthread_cond_t red[3];
int cekaj[3]={1, 1, 1};

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
    while( djelitelj <= 1000000 ){
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
        //bitwise OR, da bude neparni, ako je parni onda sigurno nije prost
        //printf("Ucitani broj: ");
        //printf ("%" PRIu64 "\n", x);
        int broj_operacija = 0;
        while(test_bitovi(x) == LAZ || test_pseudo_prost(x) == LAZ){
            if(x <= 0xffffffffffffffffULL-2 && broj_operacija <= 1000){
                x = x + 2;                    
                broj_operacija += 1;
            }
            else{
                x = ((rand() * A) % B) | 1;
                //printf("Ucitani broj: ");
                //printf ("%" PRIu64 "\n", x);
                broj_operacija = 0;
            }
        }
		
		pthread_mutex_lock(&monitor);
		
		MS[trenutni] = x;
		//printf("Popunjeni spremnik MS[%d] s brojem: ", trenutni);
        //printf ("%" PRIu64 "\n", MS[trenutni]);
        trenutni += 1;
        if (trenutni == 10)
            trenutni = 0;
		
		//kad dobije broj ne treba vise cekati
		int *I=id;
		cekaj[*I]=0;
		pthread_cond_signal(&red[*I]);
		
		pthread_mutex_unlock(&monitor);
		
	}while (kraj!=KRAJ_RADA);
}

void *dretva_provjera(void *id){
	do{
		pthread_mutex_lock(&monitor);
		
		int *I=id;
		while(cekaj[*I]==1)
			pthread_cond_wait(&red[*I],&monitor);
		
		uint64_t y = MS[sljedeci];
		sljedeci += 1;
		if (sljedeci == 10)
			sljedeci = 0;
		int *d = id;
		printf("id = %d, ", *d);
		printf("y = ""%" PRIu64 ",", y);
		printf(" uzeo broj\n");
		//vrati opet uvjet na 0
		cekaj[*I]=0;
		
		pthread_mutex_unlock(&monitor);
		
		sleep(y%5);
		printf("id = %d, ", *d);
		printf("y = ""%" PRIu64 ",", y);
		printf(" potrosio broj\n");
		
	}while (kraj!=KRAJ_RADA);
}

int main()
{
	srand(time(NULL));
	
	int i, j, BRR[3], BRP[3];
	pthread_t radna[3];
	pthread_t provjera[3];
	
	pthread_mutex_init(&monitor, NULL);
	for(i=0; i<3; i++)
		pthread_cond_init(&red[i], NULL);
	
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