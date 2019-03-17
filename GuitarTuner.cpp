#include "mbed.h"

#define C1 329.63   //Fréquences des cordes C1 corde aigue
#define C2 246.94
#define C3 196.00
#define C4 146.83
#define C5 110.00
#define C6 82.41

void flip();
void ctrl_LEDs(unsigned int);
void select_corde();

PwmOut mypwm(PB_8);
DigitalOut led(LED1);   //LED sur la carte
DigitalIn button(USER_BUTTON);    //Bouton bleu selection de la corde

DigitalOut led0(PA_7);  //LEDs externes affichage du réglage
DigitalOut led1(PA_6);
DigitalOut led2(PA_10);
DigitalOut led3(PB_3);
DigitalOut led4(PB_5);
DigitalOut led5(PB_4);
DigitalOut led6(PB_10);

DigitalOut Aff0(PA_9);
DigitalOut Aff1(PC_7);
DigitalOut Aff2(PB_6);

Ticker tick;
Serial pc(USBTX, USBRX);
InterruptIn in(PA_8);               //PIN entrée
Timer t1;

int t_period = 0;                   // Temps entre 2 front montants
unsigned int t_freq = 0;
int i = 2;

const int nEchantillons = 20;
int echantillon[nEchantillons];    // un tableau pour stocker les échantillons lus
int indice = 0;                    // l'indice de l'échantillon courant
unsigned int total = 0;                   // la somme des échantillons mémorisés
unsigned int f_moy = 0;                 // la f_moy des échantillons mémorisés
float fc = C1;
char c = 1; //corde selectionnee
//float T_PWM = 10000/fc;               //Periode PWM en µs
 
int main()
{
    
    
    in.mode(PullDown);              // Pull Down mode
    in.fall(&flip);                 // Interruption sur front descendant
    t1.start();                     // Démarrge timer
    
    for (int i = 0; i < nEchantillons; i++) {   //Init tableau echantillons
        echantillon[i] = 0;
    }

    while(1) {
        wait_ms(100);
        total = 0;
        mypwm.period_us(21320/fc);            //Periode
        mypwm.pulsewidth_us(21320/(2*fc));      //Temps etat haut, periode/2 rapport cyclique 50%
        for(int j=0;j<nEchantillons;j++){
            total += echantillon[j];
        }
        f_moy = total / nEchantillons;
        pc.printf("corde : %d\tfc = %f\tf = %uHz\tf_moy = %uHZ\r", c,fc,t_freq,f_moy);
        
        select_corde();
        ctrl_LEDs(f_moy);
          
    }
}

void flip(){
    t_period = t1.read_us();                // Mesure periode en µs
    t_freq = (1000000/t_period);   // Calcul frequence en Hz
    
    if((t_freq > (float)(0.80*fc))&&(t_freq < (float)(1.20*fc))){ //On ignore les valeurs abérantes 
        echantillon[indice] = t_freq;
    }  
    indice = (indice + 1)%nEchantillons;   // Incrémentation de l'indice
  
    t1.reset();                             // Reset timer
}

void ctrl_LEDs(unsigned int f){
    char p; //pourcentage d'ecart par rapport à la note
    p = (f / fc)*100;
    if(p<=100-5*i/2){
        led6 = 0;
        led5 = 0;
        led4 = 0;
        led3 = 0;
        led2 = 0;
        led1 = 0;
        led0 = 1;
    }else if((p>100-5*i/2)&&(p<=100-3*i/2)){
        led6 = 0;
        led5 = 0;
        led4 = 0;
        led3 = 0;
        led2 = 0;
        led1 = 1;
        led0 = 0;
    }else if((p>100-3*i/2)&&(p<=100-i/2)){
        led6 = 0;
        led5 = 0;
        led4 = 0;
        led3 = 0;
        led2 = 1;
        led1 = 0;
        led0 = 0;
    }else if((p>100-i/2)&&(p<=100+i/2)){
        led6 = 0;
        led5 = 0;
        led4 = 0;
        led3 = 1;
        led2 = 0;
        led1 = 0;
        led0 = 0;
    }else if((p>100+i/2)&&(p<=100+3*i/2)){
        led6 = 0;
        led5 = 0;
        led4 = 1;
        led3 = 0;
        led2 = 0;
        led1 = 0;
        led0 = 0;
    }else if((p>100+3*i/2)&&(p<=100+5*i/2)){
        led6 = 0;
        led5 = 1;
        led4 = 0;
        led3 = 0;
        led2 = 0;
        led1 = 0;
        led0 = 0;
    }else if(p>100+5*i/2){
        led6 = 1;
        led5 = 0;
        led4 = 0;
        led3 = 0;
        led2 = 0;
        led1 = 0;
        led0 = 0;
    }
    
}

void select_corde(){
    if(button == 0){    //bouton pressé
        c++;
        if(c>6)
            c = 1;
    }
    
    if((c == 1)||(c == 2)||(c==3)){
        i = 2;
    }else if((c == 4)||(c == 5)||(c==6)){
        i=4;
    }
    switch (c) {
        case 1 :
            Aff0 = 1;
            Aff1 = 0;
            Aff2 = 0;
            fc = C1;
            break;
        case 2 :
            Aff0 = 0;
            Aff1 = 1;
            Aff2 = 0;
            fc = C2;
            break;
        case 3 :
            Aff0 = 1;
            Aff1 = 1;
            Aff2 = 0;
            fc = C3;
            break;
        case 4 :
            Aff0 = 0;
            Aff1 = 0;
            Aff2 = 1;
            fc = C4;
            break;
        case 5 :
            Aff0 = 1;
            Aff1 = 0;
            Aff2 = 1;
            fc = C5;
            break;
        case 6 :
            Aff0 = 0;
            Aff1 = 1;
            Aff2 = 1;
            fc = C6;
            break;
        default :
            Aff0 = 0;
            Aff1 = 0;
            Aff2 = 0;
            fc = C6;
            break;
    }
}
