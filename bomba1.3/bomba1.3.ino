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
#include <LiquidCrystal_I2C.h> 


/*
 * myDFPlayer.begin(DFPlayerSerial)
 * myDFPlayer.volume(volume); 0-30
 * myDFPlayer.play(); 
 * myDFPlayer.play(5);  Reproduce el archivo 0005
 * myDFPlayer.next();
 * myDFPlayer.previous();
 * myDFPlayer.pause();
 * myDFPlayer.start();
 * myDFPlayer.randomAll();
 * myDFPlayer.enableLoop();
 * myDFPlayer.disableLoop();
 */
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
SoftwareSerial DFPlayerSerial(10,11);   //Rx, Tx
DFRobotDFPlayerMini myDFPlayer;


MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset
LiquidCrystal_I2C lcd(0x27,16,2); // (POL) Valor en hexadecimal per I2C + tamany horitzontalment + tamany verticalment  --> Es troba el valor amb llibreria arduino 
//LiquidCrystal_I2C lcd(0x5A,16,2); // (PAU)

byte LecturaUID[4];         // crea array para almacenar el UID leido
byte Usuario1[4]= {0x2A, 0xD6, 0x9F, 0x80} ;    // UID de tarjeta leido en programa 1   ----Rfid POL 1----
byte Usuario2[4]= {0x09, 0xFF, 0x40, 0xE8} ;    // UID de llavero leido en programa 1   ----Rfid POL 2----
//byte Usuario1[4]= {0xD7, 0x1D, 0xC7, 0xB5} ;   //                                     ----Rfid Pau 1----
//byte Usuario2[4]= {0x5A, 0x8C, 0xB8, 0x81} ;   //                                     ----Rfid Pau 2----
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
int ARMED=0;
unsigned long currentmillis = 0;
unsigned long DETONATION = 10000;                                                              //temps de detonacio
unsigned long RESETmillis = 0;
unsigned long prova = 0;                                                                 
const int POWERPin = 22;                                                                       //definir pin 22 com a entrada POWER
bool statePOWER = false;                                                                       //per guardar estat de power
const int DEFUSEPin = 25;                                                                       //definir pin de boto DEFUSE
const int DEFUSE_TIME = 10;
bool stateDEFUSE = false;                                                                       //per guardar estat de DEFUSE
bool stateINTENT = false;                                                                       //memoria d'intent de DEFUSE
byte PANT = 0;  
byte DEFUSED = 0;   
int BOOMtime = 2000;                                                                           // temps entre boom/defuse i reset
byte TERRORWIN = 0;                                                                             // 1 quan peta bomba                                                                            
const int SUICIDIPin = 23;                                                                              //pin 23 sortida RESET
bool stateSUICIDI = false;

//********** OB100 *********************************************************
void setup() 
{Serial.begin(9600);
 DFPlayerSerial.begin(9600);
 myDFPlayer.begin(DFPlayerSerial);
 myDFPlayer.volume(5);                        //De 0 a 30


  
  randomSeed(analogRead(A0)); 
  
  Serial.println (" ");
  Serial.print ("PASS ");
  lcd.init();
  lcd.backlight();
  ClearLcd();
  
  
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
  SPI.begin();            // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector 
  pinMode(POWERPin,INPUT);                                                  //definir POWER com a entrada
  pinMode(DEFUSEPin, INPUT);                                                //definir DEFUSE com a entrada
  pinMode(SUICIDIPin,OUTPUT);                                               //definir SUICIDI com a sortida
  TERRORWIN=0;
  DEFUSED=0;
  digitalWrite(SUICIDIPin,0);
}

//********** MAIN *********************************************************
void loop() {
  TECLA = teclat.getKey();      // obtiene tecla presionada y asigna a variable
  statePOWER=digitalRead(POWERPin);
  stateDEFUSE=digitalRead(DEFUSEPin);
  digitalWrite(SUICIDIPin,stateSUICIDI);
  // myDFPlayer.play(5);
    // myDFPlayer.randomAll();
 
  
  switch (ARMED)
  {
    case 0:
      PLANTAR();
      break;
  
    case 1:          
      PLANTADA();
      
      if (stateDEFUSE)   
      {
        DESACTIVAR();
        FORZOSA();
      }
      else
      {
        
      }
      RESETmillis=millis();  
     
      break;
  
    case 2:
      RESET();
     
     
      break;    
  }
}

//********** FC's *********************************************************

void PLANTADA()
{ 
  unsigned long actualTime = millis() - currentmillis;
  bool timeIsDone = (actualTime > DETONATION);
  bool isNotDefused = (DEFUSED == 0);
  if (timeIsDone)
  { 
    if(isNotDefused)
    {
      BombExploded();
    }
  }
  else
  {
    Serial.println(actualTime); 
    stateDEFUSE = true;                       //pq?
    ClearLcd();
    PrintInLcd(1, 0, "DETONATION IN"); 
    int actualTimeInSeconds = ((millis() - currentmillis) / 1000);
    int remainingTimeInSeconds = (DEFUSE_TIME - actualTimeInSeconds);   
    PrintInLcd(3, 1 , String(remainingTimeInSeconds));
    PrintInLcd(7, 1, "SECONDS");
    delay(500);
  }    
}

void BombExploded()
{
  Serial.println("BOOOOOOM!");
  TERRORWIN = 1;
  ARMED = 2;
  ClearLcd();
  PrintInLcd(4,0,"BOOOOOM!");
  
}
//***********************************************************************

void DESACTIVAR()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
  {
    return;           // retorna al loop esperando por una tarjeta
  }
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
  {
    return;           // retorna al loop esperando por otra tarjeta
  }  
    Serial.print("DESACTIVADA AMB:");                 // muestra texto UID:
    for (byte i = 0; i < mfrc522.uid.size; i++)       // bucle recorre de a un byte por vez el UID
    { 
      if (mfrc522.uid.uidByte[i] < 0x10)              // si el byte leido es menor a 0x10
      {             
       
      }
      else             // sino 
      {          
      }       
      LecturaUID[i]=mfrc522.uid.uidByte[i];         // almacena en array el byte del UID leido      
    }
          
    Serial.print("\t");                             // imprime un espacio de tabulacion             
                  
    if(comparaUID(LecturaUID, Usuario1))            // llama a funcion comparaUID con Usuario1
    {
      Serial.println("TARGETA");                     // si retorna verdadero muestra texto bienvenida  
      BombDefused();
    }
    else if(comparaUID(LecturaUID, Usuario2))       // llama a funcion comparaUID con Usuario2
    {
      Serial.println("CLAUER");                    // si retorna verdadero muestra texto bienvenida
      BombDefused();
    }
    else                                          // si retorna falso
    {
      Serial.println ("Targeta Incorrecta");                                                  
      mfrc522.PICC_HaltA();                       // detiene comunicacion con tarjeta 
    }               
}

void BombDefused()
{
  DEFUSED = 1;
  ARMED = 2;
  ClearLcd();
  PrintInLcd(4, 0, "BOMB HAS");
  PrintInLcd(2, 1, "BEEN DEFUSED");
   myDFPlayer.play(5);
 myDFPlayer.disableLoop();

 delay(1000);
}

boolean comparaUID(byte lectura[],byte usuario[])               
{
  for (byte i = 0; i < mfrc522.uid.size; i++)                   
  {    
    bool UIDisDifferentToUser = lectura[i] != usuario[i];
    if(UIDisDifferentToUser)                                   
    {
      return(false);                                          
    }
  }
  return(true);                                                 // si los 4 bytes coinciden retorna verdadero
}


//***********************************************************************

void FORZOSA()
{
  CheckTecla();
  CheckIndexForzosa(); 
}

void CheckTecla()
{  
  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    PASS[INDICE] = TECLA-48;    // almacena en array la tecla presionada
    INDICE++;                 // incrementa indice en uno
    Serial.print(TECLA);    // envia a monitor serial la tecla presionada  
  }
}

void CheckIndexForzosa()
{   
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
  bool securityButton = statePOWER;
  if (securityButton==1)
  {
    PrintSequenceInLcd();
    CheckIfNumPadButtonIsPressed();
    CheckIfAllButtonsArePressed();
  }
  else if (securityButton==0)
  {
    ClearLcd();
  }
}

void PrintSequenceInLcd()
{
  for (int i = 0; i < 6; i++)
  {
    lcd.setCursor(i + 2, 0);
    PANT = PASS_RANDOM[i];  
    lcd.print(PANT);
    lcd.setCursor(8, 0);
    lcd.print(" Code");
  }
}

void CheckIfNumPadButtonIsPressed()
{
  bool buttonIsPressed = TECLA;
  if (buttonIsPressed)                 
  {
    PASS[INDICE] = TECLA-48;    
    INDICE++;                 
    Serial.print(buttonIsPressed);   
 
  }  
}

void CheckIfAllButtonsArePressed()
{
  bool allDigitsStored = INDICE == 6;
  if(allDigitsStored)      
  {
    // INFO COMENTARIS IMPORTANTS " ESTRUCTURA "
    // TODO: xxx
    // FIXME: xxx
    // INFO: xxx
    bool isValidPassword = (strcmp(PASS, PASS_RANDOM) == 0);
    if(isValidPassword)   // compara clave ingresada con clave maestra
    {
      Serial.println(" Correcta");  
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
      Serial.println(" Incorrecta");  
      INDICE = 0; 
      ClearLcd();                                  //afegit 2  
    } 
  } 
}

//***********************************************************************

void RESET()
{
  if(millis()-RESETmillis>BOOMtime)
  {
    stateSUICIDI=1;                                                                //programa
  }
}

void ClearLcd()
{
  lcd.clear();
}

void PrintInLcd(int x, int y, String text)
{
  lcd.setCursor(x,y);
  lcd.print(text);
}
