
/**********************************************************************************
**                                                                               **
**                         Airsoft Boom                                          **
**                                                                               **
**                                                                               **
**********************************************************************************/
//********** Includes *************************************************************
//********** Variables ************************************************************
#include <Keypad.h> 
#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522
#define RST_PIN  5      // constante para referenciar pin de reset
#define SS_PIN  53      // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

byte LecturaUID[4];         // crea array para almacenar el UID leido
byte Usuario1[4]= {0x2A, 0xD6, 0x9F, 0x80} ;    // UID de tarjeta leido en programa 1
byte Usuario2[4]= {0x09, 0xFF, 0x40, 0xE8} ;    // UID de llavero leido en programa 1
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
byte pinesColumnas[COLUMNAS] = {28,4,3,2}; 
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
    SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector 
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
      DESACTIVAR();
      break;
          
                         

}
  
}

//********** FC's *********************************************************



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

//***********************************************************************

void DESACTIVAR()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta
    
    Serial.print("DESACTIVADA AMB:");       // muestra texto UID:
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0");       // imprime espacio en blanco y numero cero
        }
        else{           // sino
          Serial.print(" ");        // imprime un espacio en blanco
          }
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal
          LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
          
          Serial.print("\t");         // imprime un espacio de tabulacion             
                    
          if(comparaUID(LecturaUID, Usuario1))    // llama a funcion comparaUID con Usuario1
            Serial.println("Bienvenido Usuario 1"); // si retorna verdadero muestra texto bienvenida
          else if(comparaUID(LecturaUID, Usuario2)) // llama a funcion comparaUID con Usuario2
            Serial.println("Bienvenido Usuario 2"); // si retorna verdadero muestra texto bienvenida
           else           // si retorna falso
            Serial.println("No te conozco");    // muestra texto equivalente a acceso denegado          
                  
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta                
}

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}


//***********************************************************************
void FORZOSA()
{
  
}
