#include "lib/include.h"

//Ashley Roxana configuración de UART4 que corresponde al PC4 y PC5 pag 895
extern void Configurar_UART4(void)
{
    //CONFIGURACIÓN DE LOS PINES
    //Paso 1 (RCGCUART) pag.344 habilita UART4  0->Disable 1->Enable
    SYSCTL->RCGCUART  = (1<<4);   
    
    //Paso 2 (RCGCGPIO) pag.340 habilita puerto C 
    SYSCTL->RCGCGPIO |= (1<<2);     
    
    //(GPIOAFSEL) pag.688 Habilita función alternativa
    GPIOC->AFSEL = (1<<5) | (1<<4); //GPIO Port Control (GPIOPCTL) Habilita función alternativa en el pin C4 y C5 pag.688

    //Indicar entradas y salidas pag. 895 
            //      Rx(in) | Tx(out)
    //GPIOC -> DIR |= (0<<4) | (1<<5); //salida -> 1, entrada -> 0

    //Desabilitan resistencias pull up 
    //GPIOC -> PUR |= (0<<4) | (0<<5);

    //Desabilitan resistencias pull down 
    //GPIOC -> PDR |= (0<<4) | (0<<5);

    //Para indicar que el pin 5 y 4 funcionan como UART, de acuerdo a los bits de la tabla de AFSEL pag. 672
    GPIOC->PCTL = (1<<20) | (1<<16);  //pag. 1351 para conocer que valor poner
    // ejemplo con A1 y A0: (1<<0) | (1<<4); otra forma de ponerlo: (GPIOA->PCTL&0xFFFFFF00) | 0x00000011;
    
    // Habilitar el pin como digital GPIO (GPIODEN) pag.682
    GPIOC->DEN = (1<<4) | (1<<5);//PC4 PC5

    //CONFIGURACIÓN DEL PERIFERICO UART
    // Desabilita/Limpia los registros pag. 918
    UART4->CTL = (0<<9) | (0<<8) | (0<<0); //bit 9 es de Tx, bit 8 de Rx, y 0 al uart en genral. 

    // UART Integer Baud-Rate Divisor (UARTIBRD) pag.914
    /*     velocidad  (estandar*baud Rate)
    BRD = 20,000,000 / (16 * 9600) = 130.2       yo: 10MHZ Baud-rate 4800
    UARTFBRD[DIVFRAC] = integer(0.2 * 64 + 0.5) = 14
                                0.2 -> part. decimal*/
    UART4->IBRD = 130;

    // UART Fractional Baud-Rate Divisor (UARTFBRD) pag.915
    UART4->FBRD = 14;  /* (parte fraccionaria del calculo) */

    //  UART Line Control (UARTLCRH) pag.916 bits
    UART4->LCRH = (0x3<<5)|(1<<4);  //Para que se tengan 8 bits, y |(1<<4); activar el bit de paridad pero solo con el uart0
   
    //  UART Clock Configuration(UARTCC) pag.939
    UART4->CC =(0<<0); /*PARA USAR EL RELOJ INTERNO*/
    
    //Disable UART0 UART Control (UARTCTL) pag.918
    UART4->CTL = (1<<0) | (1<<8) | (1<<9);   //se habilitan los pines de recepción, el pin 8 y 9 se deben prender juntos

}

extern char readChar(void)
{
    //UART FR flag pag 911
    //UART DR data 906
    int v;  //BANDERA
    char c;  
    while((UART4->FR & (1<<4)) != 0 ); //se mantiene en el ciclo cuando no llega un dato 
    v = UART4->DR & 0xFF; //DR - registro del dato y 0xff es la mascara 
    c = v; //convertir el entero a caracter
    return c; //solo regresa un caracter 
}

extern void printChar(char c) //mandar un caracter
{
    while((UART4->FR & (1<<5)) != 0 ); //pregunta si el dato 5 está desocupado, si lo está sigue con la linea de datos y transmite
    UART4->DR = c; 
}

//Para enviar cadena de caracteres 
extern void printString(char* string, int Largo)   
{
    int i = 0; 

    //string es la direccion de memoria para saber si está vacía o np, si está vacia se sale del ciclo
    //Se le da unvalor mayor a 47 porque es a partir de donde son los valores de las letras en el ASCII
    while(string[i]>47) 
    {
        printChar(string[i]);
        i++; 
    }
}

//Para calcular el tamaño del arreglo 
extern int readString(char delimitador, char *string)  
{
   int i=0;
   char c = readChar();

   while(c != delimitador) //lee cuando el caracter es distinto al caracter delimitador 
   {
       string[i] = c;
       i++;
       c = readChar();
   }

   return i; //regresa i para saber el tamaño del arreglo
}

//Para invertir
extern void Invertir(char *Nombre, int Largo) 
{

    //Variables 
    int i = 1; //para indicar la posición del arreglo con el nombre
    int a = 1; //para indicar que número se colocan entre las letras
    int b = 0;//para indicar la posición del arreglo donde se guarda la inversión
    int flag = 0;
    int Size = Largo; //para guardar tamaño del nombre

    if (Largo < 10) //Tamaño de memoria para nombres cortos

        Largo = Largo + (Largo-1); //Para considerar el esapacio que el nombre ocupa y el que se ocuparía con los numeroes que van entre espacios
     
    else //Tamaño de memoria para nombres largos
    {
        Largo = 18 + ((Largo-9)*3); 
    }

    //Declaración del arreglo a ocupar para guardar la inverción
    char invert[Largo]; 

    //Para colocar los numeros entre letras
    while (i<Largo) 
    {
        //Para leer e invertir las letras del nombre
        if (flag == 0)
        {
            invert[b] = Nombre[Size-i]; //Size-i para indicar en que posición está la letra a guardar
            i = i + 1;
            b = b + 1;
            flag  = 1;
        }

        //Para colocar los numero entre las letras
        else
        {
            flag = 0; 
            invert[b] = a; //Numero a colocar en el arreglo
            a = a + 1;
            b = b + 1;
        }
    }

    //Se reasignación del valor de invert en nombre
    for (int j = 0; j<=(Largo-1); j++)
    {
        Nombre[j] = invert[j];
    }
    for(int n = Largo; n <= 100; n++) //Hasta 100 porque fue lo max en lo que se declaró
        Nombre[n] = 0;
    return;
}

