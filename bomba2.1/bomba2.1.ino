
//teclat
#include <Keypad.h>            //llibreria            
const byte FILAS=4;
const byte COLUMNAS=3;
char keys[FILAS][COLUMNAS]=
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte pinesFilas[FILAS] = {9,8,7,6};    
byte pinesColumnas[COLUMNAS] = {5,4,3};
Keypad teclat=Keypad(makeKeymap(keys),pinesFilas, pinesColumnas, FILAS, COLUMNAS);

//lcd I2C
byte PANT=0;                        //on guardar valors array per ensenyar per pantalla
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   // (POL) Valor en hexadecimal per I2C + tamany horitzontalment + tamany verticalment  --> Es troba el valor amb llibreria arduino 
//LiquidCrystal_I2C lcd(0x5A,16,2); // (PAU)
  /*
  lcd.init();
  lcd.backlight();             
  lcd.noBacklight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HOLAa");
  lcd.display();
  lcd.noDisplay();
  
  */

//crear PASS_RANDOM i variables per teclat
char PASS_RANDOM[7];
char PASS[7];
byte INDICE=0;
char TECLA;
byte LLETRA=0;

//coses de la bomba
const int POWERPin=12;                                       //definir pin 12 com a entrada POWER
bool statePOWER=false;
const int LedBomba=13;                                       //Led de la bomba 
int stateLedBomba=false;
unsigned long bip=0;                                                        //variable per les pampallugues
unsigned long biptime=2000;                                                     //variable per les pampallugues
const unsigned long boomTIME=100000;                                       //temps entre plantar i detonar bomba

//variables de detonacio
unsigned long detonationTIME=0;

//Etapes
int ETAPA=1;
unsigned long RESET=0;                                              //variable per reiniciar el joc

//variables de desactivacio optima
const int connectatPin=10;                                     //pin 10 es entrada de desactivador connectat
int estatconnectat=false;
unsigned long defuseTIME=0;                                   //per carregar tempo de desactivacio
const int tempsDEFUSING=5000;                                 //milis per desactivar                  
const int LedMando=11;                                        //definir pin 11 per sortida led mando desactivador
int stateLedMando=false; 
unsigned long bip2=0; 
unsigned long biptime2=2000;  

//variables desactivacio FORZOSA
const int ForcaPin[] = {A3,A2,A1,A0};                      //definir pins de FORZOSA
const char ForcaPASS[]={1,1,1,1,0};                           //array de les 4 donat senyal
char ForcaNOW[]={1,1,1,1,0};                                  //array amb estat entrades FORZOSA
char ForcaPOTRA[]={1,1,1,1,0};                               //array que es modificara i sera la clau de FORZOSA
int POTRA=0;                                               //per guardar entrada que FORZOSA

int prova=0;  
int validacio1=0;                                            //guarda les restes entre arrays ForcaNOW i ForcaPASS
int validacio2=0;                                            //guarda les restes entre arrays ForcaNOW i ForcaDEFUSE
int comprova=0;

//variables per guardar estat entrades FORZOSA
int stateFORCA0=1;
int stateFORCA1=1;
int stateFORCA2=1;
int stateFORCA3=1;

//altaveu
/*#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
SoftwareSerial DFPlayerSerial(10,11);   //Rx, Tx al loro que ara es fan servir, no solapar 
DFRobotDFPlayerMini myDFPlayer;

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


//********** OB100 *********************************************************
void setup() {
randomSeed(analogRead(A5)); 
Serial.begin(9600);
pinMode(POWERPin,INPUT);                                                    //definir POWER com a entrada
pinMode(connectatPin,INPUT);                                                //definir entrada
pinMode(LedBomba,OUTPUT);
for(int i = 0; i <4; i++)                                       // definir array de FORZOSA
  {
    pinMode(ForcaPin[i],INPUT);
  }

//reset leds
digitalWrite(LedBomba,stateLedBomba);



//lcd
lcd.noBacklight();

//altaveu
  /*DFPlayerSerial.begin(9600);
  myDFPlayer.begin(DFPlayerSerial);
  myDFPlayer.volume(15);   //De 0 a 30*/

//generar random
  for(int i = 0; i < 5 ; i++)                
  {                
    PASS_RANDOM[i]=random(1,10);
  }

 //generar entrada FORZADA
 POTRA=random(0,4);
 Serial.println(POTRA); 
 ForcaPOTRA[POTRA]=0;                                          //canvia el cable que FORZOSA

 //inicia lcd
  lcd.init();  
  lcd.noBacklight();
 
  
  
  
}
//********** MAIN *********************************************************
void loop() 
{
  Serial.println(ETAPA);
                                 //inicia lcd
  
switch (ETAPA)
{ 
  //nomes es pot plantar bomba
  case 1:
   TECLA = teclat.getKey();               // obtiene tecla presionada y asigna a variable
   statePOWER=digitalRead(POWERPin);      //mira estat del boto per armano
   PLANTAR();
   detonationTIME=millis();              
   RESET=millis();
   
   break;
   
  //opcions defuse o DETONACIO
  case 2:
   RESET=millis();
   estatconnectat=digitalRead(connectatPin);
   DETONACIO();
   libertina();
   switch (estatconnectat)
   {
   case false:
   defuseTIME=millis();
   stateLedMando=LOW;
   digitalWrite(11,stateLedMando);
   break;
   
   case true:
   defuse();
   break;
   }
   break;

   //terrors win
  case 3:
   if(millis()-RESET>5000)
   {
    ETAPA=5;  
   }
  lcd.setCursor(0,0);
  lcd.print("   TERRORIST    "); 
  lcd.setCursor(0,1); 
  lcd.print("      WIN       "); 
  stateLedBomba=true;
  digitalWrite(13,stateLedBomba);
  
  break;

   //polis win
  case 4:
    if(millis()-RESET>5000)
   {
    ETAPA=5;
   }
  lcd.setCursor(0,0);
  lcd.print("COUNTERTERRORIST"); 
  lcd.setCursor(0,1); 
  lcd.print("      WIN       ");
  stateLedBomba=false;
  digitalWrite(13,stateLedBomba);
  
  break;

  case 5:
  //etapa de reset
  
  prova=0;  
  comprova=0;
  estatconnectat=false; 
  validacio1=0;
  validacio2=0;
  stateLedBomba=false;
  statePOWER=false;
  digitalWrite(13,LOW);                  //per apagar llum al fer RESET
  digitalWrite(11,LOW);                  //per apagar llum al fer RESET
  lcd.noBacklight();
  lcd.clear();
  for(int i=0;i<4;i++)
  {
  ForcaNOW[i]=0;
  }
  ETAPA=1;
  
  break;
 }
}

//***********************************************************************
void PLANTAR()
 {
  if(strcmp(ForcaPASS, ForcaNOW)==0)
  {
  switch (statePOWER)
  {
    case 0:
    lcd.clear();
    lcd.noBacklight();
    break;
    
    case 1:
    PrintSequenceInLcd();   
    
  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    PASS[INDICE] = TECLA-48;    // almacena en array la tecla presionada -48?
    INDICE++;                 // incrementa indice en uno
   // Serial.print(TECLA);    // envia a monitor serial la tecla presionada 

    lcd.setCursor(INDICE+1,1);
    lcd.print(TECLA);
  }

  if(INDICE == 6)       // si ya se almacenaron los 6 digitos
  {    lcd.setCursor(INDICE+1,1);
       lcd.print(TECLA);
       delay(500);
   
    
    if(strcmp(PASS, PASS_RANDOM)==0)   // compara clave ingresada con clave maestra
    {
      Serial.println(" Correcta");  // imprime en monitor serial que es correcta la clave

      lcd.setCursor(0,0);
      lcd.print("  Bomb has been "); 
      lcd.setCursor(0,1); 
      lcd.print("    PLANTED!    ");     
      ETAPA=2;
      delay(1000);
      
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
       
      lcd.setCursor(0,1);
      lcd.print("  WRONG  Code ");
      delay(500);
      lcd.clear();
      
    } 
  }
  break;
  }
 }
}

//***********************************************************************
void PrintSequenceInLcd()
{
  for (int i = 0; i < 6; i++)
  {
  
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("  ");
     lcd.setCursor(14,0);
    lcd.print("   ");
    
    lcd.setCursor(i + 2, 0);
    PANT = PASS_RANDOM[i];  
    lcd.print(PANT);
    lcd.setCursor(8, 0);
    lcd.print(" Code");   
  }
}

//***********************************************************************
void DETONACIO()
{ 
  //variables count down
  int minuts=(boomTIME-(millis()-detonationTIME))/60000;
  unsigned long segons=((boomTIME-(millis()-detonationTIME))%60000)/1000;
  char text[10];
  
     //led de la bomba
     biptime=minuts*600+segons*10+50;               
  
     if(millis()-bip>biptime)                     //funcio per velocitat blip
      {
        stateLedBomba=!stateLedBomba;
        bip=millis();     
      }
      else if(biptime<60)
      {
        stateLedBomba=true;
      }
      else
      {
       digitalWrite(13,stateLedBomba);
      } 
    //segons temps plantada                              
  if(millis()-detonationTIME>boomTIME)
  {
    //bomba peta
    lcd.setCursor(0,0);
    lcd.print("    BOOOOOM!    ");
    lcd.setCursor(0,1);
    lcd.print("                ");
    delay(1000);
    lcd.setCursor(0,0);
    lcd.print("   TERRORIST    "); 
    lcd.setCursor(0,1); 
    lcd.print("      WIN       ");
    ETAPA=3;   
  }
  else
  { 
   lcd.setCursor(0,0);
   lcd.print(" DETONATION IN: ");
   lcd.setCursor(0,1);
   sprintf(text, "     %02d:%02d     ",minuts,segons);
   lcd.print(text);

   

  }

}
//***********************************************************************
void defuse()
{ 
   //contador led de desactivacio
  biptime2=(tempsDEFUSING-(millis()-defuseTIME))/10;
  
  if(millis()-bip2>biptime2)
  {
    stateLedMando=!stateLedMando;
    bip2=millis();
  }
  else
  {   
  }

  //desactivacio
 if(millis()-defuseTIME>tempsDEFUSING)
 {
  //desactivada
  lcd.setCursor(0,0);
  lcd.print("  BOMB HAS BEEN "); 
  lcd.setCursor(0,1); 
  lcd.print("    DEFUSED!    ");     
  ETAPA=2;
  delay(2000);
  ETAPA=4;
 }
 //desactivant
 else
 {
  digitalWrite(11,stateLedMando);
 } 
}

//***********************************************************************
void libertina()
{
ForcaNOW[0]=digitalRead(ForcaPin[0]);
ForcaNOW[1]=digitalRead(ForcaPin[1]);
ForcaNOW[2]=digitalRead(ForcaPin[2]);
ForcaNOW[3]=digitalRead(ForcaPin[3]);

if(strcmp(ForcaPASS, ForcaNOW)==0)
{
  //palante
}
else
{
  validacio1=1;
 ForcaNOW[POTRA]=1;
 for(int i=0;i<4;i++)
 {
   
  comprova=ForcaNOW[i];
  if(comprova==1)
  {
  }
  else
  {
    //segueix connectat
      validacio2=1;
  }
 }
    
    switch (validacio1+validacio2)
    {
      case 1:      
       //desactivada
     lcd.setCursor(0,0);
     lcd.print("  BOMB HAS BEEN "); 
     lcd.setCursor(0,1); 
     lcd.print("    DEFUSED!    ");     
     ETAPA=2;
     delay(2000);
     ETAPA=4;
     
     break;

     case 2:
      //BOOOOM
    lcd.setCursor(0,0);
    lcd.print("    BOOOOOM!    ");
    lcd.setCursor(0,1); 
    lcd.print("LA LIASTE BOLUDO");
    delay(2000); 
    ETAPA=3;
    
    break;
    }  
  } 
}
