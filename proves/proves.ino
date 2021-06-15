//variables desactivacio FORZOSA
const int ForcaPin[] = {A3,A2,A1,A0};                      //definir pins de FORZOSA
const char ForcaPASS[]={1,1,1,1,0};                           //array de les 4 donat senyal
char ForcaNOW[]={1,1,1,1,0};                                  //array amb estat entrades FORZOSA
char ForcaPOTRA[]={1,1,1,1,0};                               //array que es modificara i sera la clau de FORZOSA
int POTRA=0;  
int prova=0;  
int validacio1=0;                                            //guarda les restes entre arrays ForcaNOW i ForcaPASS
int validacio2=0;                                            //guarda les restes entre arrays ForcaNOW i ForcaDEFUSE
int comprova=0;

//variables per guardar estat entrades FORZOSA
int stateFORCA0=1;
int stateFORCA1=1;
int stateFORCA2=1;
int stateFORCA3=1;

void setup() {
randomSeed(analogRead(A5)); 
Serial.begin(9600);

 for(int i = 0; i <4; i++)                                       // definir array de FORZOSA
  {
    pinMode(ForcaPin[i],INPUT);
  }

   //generar entrada FORZADA
 POTRA=random(0,4);
 Serial.print("POTRA ");
 Serial.println(POTRA);
 ForcaPOTRA[POTRA]=0;                                          //canvia el cable que FORZOSA
 


}

void loop() {
ForcaNOW[0]=digitalRead(ForcaPin[0]);
//Serial.print(stateFORCA0);
ForcaNOW[1]=digitalRead(ForcaPin[1]);
//Serial.print(stateFORCA1);
ForcaNOW[2]=digitalRead(ForcaPin[2]);
//Serial.print(stateFORCA2);
ForcaNOW[3]=digitalRead(ForcaPin[3]);
//Serial.println(stateFORCA3);




/*prova=ForcaNOW[0];
Serial.print(prova);
delay(500);
prova=ForcaNOW[1];
Serial.print(prova);
delay(500);
prova=ForcaNOW[2];
Serial.print(prova);
delay(500);
prova=ForcaNOW[3];
Serial.println(prova);
delay(500);*/

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
      case 0:
      break;

      case 1:
      Serial.println("Defused");
     
      delay(1000);
      break;

      case 2:
      Serial.println("BOOOOM");
      delay(1000);
      break;
    }
  

    
   
  }

}











  
  
