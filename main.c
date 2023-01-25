#include "lib/include.h"

int main(void)
{
    //Variables
    char c='5';
    int Largo = 100; //Variables para el tamaño del arreglo
    char Nombre[Largo]; //Arreglo donde se guarda la palabra

    //Configuraciones
    Configurar_PLL(_10MHZ);  //Confiuracion de velocidad de reloj
    Configurar_UART4();//yo: 10MHZ Baud-rate 4800
    Configurar_GPIO();

    while(1)
    {
         c = readChar();
         switch(c)
         {
             case 'r':
                 //GPIODATA port F 662
                 printChar('a'); //enviar e imprimir 'a'
                 GPIOF->DATA = (1<<1); //prender un led rojo
                 break;
             case 'b':
                 //GPIODATA port F 662
                 printChar('b');
                 GPIOF->DATA = (1<<2);
                 break;
             case 'g':
                 //GPIODATA port F 662
                 printChar('c');
                 GPIOF->DATA = (1<<3);
                 break;

            case 'i':
                Largo = readString('%', &Nombre[0]);
                Invertir(&Nombre[0],Largo);
                printString(&Nombre[0], Largo);
                break;

            default:
                GPIOF->DATA = (1<<1) | (1<<2) | (1<<3);//PRENDEMOS TODOS LOS LED'S
                break;
         }
    }
}
