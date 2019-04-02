#include <stdio.h>
#include <stdint.h>
#include <inttypes.h> //ovaj zapravo uklju�uje prethodni
#include <time.h>
#include <stdlib.h>

#define LAZ 0
#define ISTINA 1
#define KRAJ_RADA 2
#define NIJE_KRAJ 3

uint64_t MS[10] = {0}; //popunjavat cu od MS[0] do MS[9] kruzno, globalno polje
int sljedeci = 0; //broj za kojim je dan zahtjev

int provjera_zahtjeva(double *t)
{
    clock_t trenutno = clock();
    double proslo_vrijeme = ((double)(trenutno-*t))/CLOCKS_PER_SEC;// u sekundama
    //printf("Proslo vrijeme: %f\n", proslo_vrijeme);
    *t = trenutno;
    if(!(proslo_vrijeme >= 1))
        return NIJE_KRAJ;
    else{
        double p = rand()/((double)RAND_MAX); //koristim random broj za utvrdivanje vjerojatnosti
        printf("Random number: %f\n", p);
        if(p <= 0.5){ // vjerojatnost za novim zahtjevom je 50%
            printf("Proslo vrijeme: %d , ", (int) proslo_vrijeme);
            printf("I = %d, ", sljedeci);
            printf("MS[I] = ""%" PRIu64 " ", MS[sljedeci]);
            sljedeci += 1;
            if (sljedeci == 10)
                sljedeci = 0;
            int i, z;
            printf(", MS[] = { ");
            for(i=0; i<10; i++){
                z = MS[i]%100;
                printf("%02d ", (int)z); //printamo samo zadnje 2 znamenke
            }
            printf("}\n");
            return NIJE_KRAJ;
        }
        else if(0.5 < p  && p <= 0.6) //vjerojatnost za zahtjevom za kraj rada je 10%
            return KRAJ_RADA;
        else return NIJE_KRAJ;
    }
}

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
    while( djelitelj <= 100000 ){
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

int main()
{
    int trenutni = 0; //trenutno mjesto na koje cemo spremiti 64-bitovni posebni broj u MS
    uint64_t A=8531648002905263869ULL, B=18116652324302926351ULL; //dano u objasnjenju zadatka
    srand(time(NULL));
    int broj_operacija;
    double zadnja_provjera = 0;

    /*uint64_t x = 17881514302879451429;
    printf("%" PRIu64 "\n", x);
    if(test_bitovi(x)==LAZ)
        printf("Ne valja!\n");
    else printf("Valja!\n");

    uint64_t y=x;
    printf("\n");
    while(y>0){
        printf("%" PRIu64 " ", y%2);
        y/=2;
    }*/

    do{
        uint64_t x = ((rand() * A) % B) | 1;
        //bitwise OR, da bude neparni, ako je parni onda sigurno nije prost
        //printf("Ucitani broj: ");
        //printf ("%" PRIu64 "\n", x);
        broj_operacija = 0;
        while(test_bitovi(x) == LAZ || test_pseudo_prost(x) == LAZ){
            if(x <= 0xffffffffffffffffULL-2 && broj_operacija <= 1000){
                                                //NAJVE�I_BROJ = 2^64 - 1 = 0xffffffffffffffffULL
                x = x + 2;                     //za veci broj operacija traje nekoliko desetaka sekundi
                broj_operacija += 1;
            }
            else{
                x = ((rand() * A) % B) | 1;
                //printf("Ucitani broj: ");
                //printf ("%" PRIu64 "\n", x);
                broj_operacija = 0;
            }
        }
        MS[trenutni] = x;
        //printf("Popunjeni spremnik MS[%d] s brojem: ", trenutni);
        //printf ("%" PRIu64 "\n", MS[trenutni]);
        trenutni += 1;
        if (trenutni == 10)
            trenutni = 0;
    } while(provjera_zahtjeva(&zadnja_provjera)!= KRAJ_RADA);
    return 0;
}
