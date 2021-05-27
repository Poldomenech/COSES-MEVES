
/**********************************************************************************
**                                                                               **
**                         Airsoft Boom                                          **
**                                                                               **
**                                                                               **
**********************************************************************************/
//********** Includes *************************************************************
//********** Variables ************************************************************
#include <Keypad.h> 
byte teta;
char TECLA;
const byte FILAS=4;
const byte COLUMNAS=4;
char keys[FILAS][COLUMNAS]=
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {9,8,7,6};   
byte pinesColumnas[COLUMNAS] = {5,4,3,2}; 
char PASS_RANDOM[7];
char PASS[7];
byte INDICE=0;
Keypad teclat=Keypad(makeKeymap(keys),pinesFilas, pinesColumnas, FILAS, COLUMNAS);
bool ARMED=0;
unsigned long currentmillis=0;
unsigned long DETONATION=5000;                                                           //temps de detonacio


void setup() {
randomSeed(analogRead(A0)); 
Serial.begin(9600);
Serial.println (" ");
Serial.print ("PASS ");
for(int i = 0; i < 6 ; i++)
{                
  PASS_RANDOM[i]=random(1,10);
}
  for (int i=0; i<6;i++)
  {
  INDICE=PASS_RANDOM[i];
  Serial.print(INDICE);
  }
  INDICE=0;
  Serial.println("");
  
}






void loop() {
TECLA = teclat.getKey();      // obtiene tecla presionada y asigna a variable

switch (ARMED)
{
  case 0:

  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    PASS[INDICE] = TECLA-48;    // almacena en array la tecla presionada
    //Serial.println(INDICE);
    INDICE++;                 // incrementa indice en uno
    Serial.print(TECLA);    // envia a monitor serial la tecla presionada
    
  }

  if(INDICE == 6)       // si ya se almacenaron los 6 digitos
  {
    if(strcmp(PASS, PASS_RANDOM)==0)   // compara clave ingresada con clave maestra
    {
      Serial.println(" Correcta");  // imprime en monitor serial que es correcta la clave
      ARMED=1;
      currentmillis=millis();
       Serial.println(ARMED);
       for (int i=0; i<6;i++)
       {
       PASS[i]=0;
       }
      INDICE = 0;
    }
    else
    {
      Serial.println(" Incorrecta");  // imprime en monitor serial que es incorrecta la clave

    INDICE = 0;
    
    }
   
  }
  break;
 
case 1:
      
      PLANTADA();
      break;
          
                         

}
  
}

//********** FC's *************************************************************



void PLANTADA()
{
  
  if (millis()-currentmillis>DETONATION)
  {
    Serial.println("BOOOOOOM!");
    ARMED=0;
  }
  else
  {
    Serial.println(millis()-currentmillis);
    delay(500); 
  }
    
}
