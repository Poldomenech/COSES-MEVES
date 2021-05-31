
/**********************************************************************************
**                                                                               **
**                         Airsoft Boom                                          **
**                                                                               **
**                                                                               **
**********************************************************************************/

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
int ARMED=0;
unsigned long currentmillis=0;
unsigned long DETONATION=10000;                                                              //temps de detonacio
unsigned long RESETmillis=0;                                                                
const int POWERPin=22;                                                                       //definir pin 22 com a entrada POWER
bool statePOWER=false;                                                                       //per guardar estat de power
const int DEFUSEPin=25;                                                                       //definir pin de boto DEFUSE
bool stateDEFUSE=false;                                                                       //per guardar estat de DEFUSE
bool stateINTENT=false;                                                                       //memoria d'intent de DEFUSE
byte PANT=0;  
byte DEFUSED=0;   
int BOOMtime=5000;                                                                           // temps entre boom/defuse i reset
byte TERRORWIN=0;                                                                             // 1 quan peta bomba                                                                            
const int SUICIDIPin=23;                                                                              //pin 23 sortida RESET

bool stateSUICIDI=false;
const int ledPin = 13;
const int buzzerPin = 29;
bool INTENT=0;





//********** OB100 *********************************************************
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
  PANT=PASS_RANDOM[i];
  Serial.print(PANT);
  }
  INDICE=0;
  Serial.println(""); 
    SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector 
  pinMode(POWERPin,INPUT);                                                  //definir POWER com a entrada
  pinMode(DEFUSEPin, INPUT);                                                //definir DEFUSE com a entrada
  pinMode(SUICIDIPin,OUTPUT);                                               //definir SUICIDI com a sortida
  TERRORWIN=0;
  DEFUSED=0;
  digitalWrite(SUICIDIPin,0);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

//********** MAIN *********************************************************
void loop() {
TECLA = teclat.getKey();      // obtiene tecla presionada y asigna a variable
statePOWER=digitalRead(POWERPin);
stateDEFUSE=digitalRead(DEFUSEPin);
digitalWrite(SUICIDIPin,stateSUICIDI);
INTENTDONE();
 

switch (ARMED)
{
  case 0:
PLANTAR();
RESETmillis=millis();
  break;

case 1:
      
  PLANTADA();
  if (stateDEFUSE)
  {
  DESACTIVAR();
  FORZOSA();
  INTENT=1;
  
  }
  else
  {
    
  }
  RESETmillis=millis();  
  break;

case 2:
 RESET();
 if (TERRORWIN==1)
 {
    tone(buzzerPin, 100);
    digitalWrite(ledPin, HIGH);
    delay(100);

    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
    delay(100);  
  }
  else if(DEFUSED==1)
  {
                                 //so per polis
  }
  else 
  {
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}
  
}

//********** FC's *********************************************************

void PLANTADA()
{ 
  if (millis()-currentmillis>DETONATION&&DEFUSED==0)
  {
    Serial.println("BOOOOOOM!");
    TERRORWIN=1;
    ARMED=2;
  }
  else if (millis()-currentmillis<=DETONATION)
  {
                                              //temps en pantalla   
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
       
        }
        else{           // sino 
          }       
          LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
          
          Serial.print("\t");         // imprime un espacio de tabulacion             
                    
          if(comparaUID(LecturaUID, Usuario1))    // llama a funcion comparaUID con Usuario1
          {
            Serial.println("CLAUER"); // si retorna verdadero muestra texto bienvenida  
             DEFUSED=1;
             ARMED=2;
          }
          else if(comparaUID(LecturaUID, Usuario2)) // llama a funcion comparaUID con Usuario2
          {
            Serial.println("TARGETA"); // si retorna verdadero muestra texto bienvenida
            DEFUSED=1;
            ARMED=2;
          }
           else           // si retorna falso
           {
            Serial.println("No te conozco");    // muestra texto equivalente a acceso denegado       
                  
            mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta 
           }               
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
  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    PASS[INDICE] = TECLA-48;    // almacena en array la tecla presionada
    INDICE++;                 // incrementa indice en uno
    Serial.print(TECLA);    // envia a monitor serial la tecla presionada  
  }

  if(INDICE == 6)       // si ya se almacenaron los 6 digitos
  {
    if(strcmp(PASS, PASS_RANDOM)==0)   // compara clave ingresada con clave maestra
    {
      Serial.println(" Correcta");  // imprime en monitor serial que es correcta la clave
      ARMED=2;
      DEFUSED=1;
      currentmillis=millis();
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
}
//***********************************************************************
void PLANTAR()
{
  switch (statePOWER)
  {
    case 1:
  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    PASS[INDICE] = TECLA-48;    // almacena en array la tecla presionada
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
}
}

//***********************************************************************
void RESET()
{
  if(millis()-RESETmillis>BOOMtime)
  {
    stateSUICIDI=1;                                                                //programa
  }
  else
  {
  }
}

//***********************************************************************

void INTENTDONE()
{
   if(INTENT==1&&stateDEFUSE==0&&DEFUSED==0)
  {
    TERRORWIN=1;
    ARMED=2;
  }
}
