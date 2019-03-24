#include <stdio.h>
#include <stdint.h>
#include <inttypes.h> //ovaj zapravo ukljuèuje prethodni
#include <time.h>
#include <stdlib.h>

#define LAZ 0
#define ISTINA 1
#define KRAJ_RADA 2
#define NIJE_KRAJ 3

//ispis 64-bitovnih brojeva, koristi pozitivne brojeve, sufiks ULL
/*int main() {
	int64_t  sx = 0xfedcba9876543210LL;  //negativan broj -81985529216486896
	uint64_t ux = 0xfedcba9876543210ULL; //pozitivan broj 18364758544493064720

	printf ("%" PRId64 "\n", sx); //pravi odabir za sx
	printf ("%" PRIu64 "\n", ux); //pravi odabir za ux
	//po potrebi koristiti PRIx64 za heksadekadski ispis

	return 0;
}*/

int provjera_zahtjeva(double *t)
{
    clock_t trenutno = clock();
    double proslo_vrijeme = ((double)(trenutno-*t))/CLOCKS_PER_SEC;// u sekundama
    *t = trenutno;
    if(!(proslo_vrijeme >= 1))
        return NIJE_KRAJ;
    else{

    }
}

int test_bitovi( uint64_t x )
{
    int brojac = 0;
    int i;
    for(i = 0; i < 64; i++){
        if(((x & (1<<i)) != 0))  // 1<<i je shift 1 lijevo za i pozicija, & je logicko I
            brojac++; //nasli smo (jos) jednu jednicu
        else
            brojac = 0;
        if(brojac == 3){
            printf("3 uzastopne jedinice!\n");
            return LAZ;
        }
    }
    for(i = 0; i < 64; i++){
        if(((x & (1<<i)) == 0))  // 1<<i je shift 1 lijevo za i pozicija, & je logicko I
            brojac++; //nasli smo (jos) jednu nulu
        else
            brojac = 0;
        if(brojac == 3){
            printf("3 uzastopne nule!\n");
            return LAZ;
        }
    }
    return ISTINA;
}


int test_pseudo_prost( uint64_t x )
{
    int djelitelj = 3; //2 smo eliminirali s bitwise OR, gledamo sljedeci prost djelitelj
    while( djelitelj <= 100000 ){
        if( x % djelitelj == 0){
            printf("Nije prost broj!\n");
            return LAZ;
        }
        else
            //gledamo sljedeci neparan broj s kojim bi mogao biti djeljiv
            //sigurno nije djeljiv s parnim brojem jer nije djeljiv s 2
            djelitelj += 2;
    }
    return ISTINA;
}

int main()
{
    uint64_t MS[10]; //popunjavat cu od MS[0] do MS[9] kruzno
    int trenutni = 0; //trenutno mjesto na koje cemo spremiti 64-bitovni posebni broj u MS
    uint64_t A=8531648002905263869ULL, B=18116652324302926351ULL; //dano u objasnjenju zadatka
    srand(time(NULL));
    int broj_operacija;
    double zadnja_provjera = 0;

    do{
        uint64_t x = ((rand() * A) % B) | 1; //bitwise OR, da bude neparni, ako je parni onda sigurno nije prost
        printf("Ucitani broj: ");
        printf ("%" PRIu64 "\n", x);
        broj_operacija = 0;
        while(test_bitovi(x) == LAZ || test_pseudo_prost(x) == LAZ){
            if(x <= 0xffffffffffffffffULL-2 && broj_operacija <= 1000){ //NAJVEÆI_BROJ = 2^64 - 1 = 0xffffffffffffffffULL
                x = x + 2;                     //za veci broj operacija traje nekoliko desetaka sekundi
                broj_operacija += 1;
            }
            else{
                x = ((rand() * A) % B) | 1;
                broj_operacija = 0;
            }
        }
        MS[trenutni] = x;
        printf("Popunjeni MS[%d] spremnik s brojem: ", trenutni);
        printf ("%" PRIu64 "\n", MS[trenutni]);
        trenutni += 1;
        if(trenutni == 10)
            trenutni = 0;
    } while(provjera_zahtjeva(&zadnja_provjera)!= KRAJ_RADA);
}
