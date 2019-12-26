#include <SoftwareSerial.h> //bibliotheek voor bluetoothcommunicatie
SoftwareSerial BT(3, 5); //pinnen toekenennen voor bluetoothcommunicatie

//Motor A
int ENA = 11; //Snelheidscontrole pwm signaal motor A
int IN1 = 10; //Richting IN1 = 1; IN2 =0 motor A ene richting
int IN2 = 9;  //Richting IN1 = 0; IN2 =1 motor A andere richting

//Motor B
int IN3 = 7; //Richting IN3 = 1; IN4 =0 motor B ene richting
int IN4 = 8; //Richting IN3 = 0; IN4 =1 motor B andere richting
int ENB = 6; //Snelheidscontrole pwm signaal motor B

//sensoren
int ACL = A5;    //ingang van de accelerometer
int GYR = A0;    //ingang van de gyroscoop
int GYRREF = A2; //referentiespanning Gyrros

//regeling
int Kp = 20;      //proportioneel
int Kd = 1;       //d-actie
int setacl = 412; //sepoint accelerometer
int v;            //snelheid motoren (0-255)

//bluetooth
char a;       // stores incoming character from other device
int va = 0;   //afwijking voor setpoint voor voorwaarts en achterwaarts rijden
int lr = 0;   //afwijking voor setpoint voor links en rechts rijden
int af = 15;  //afwijking voor setpoint

void setup(){
  BT.begin(9600);       //nodig voor de bluetoothcommunicatie
  pinMode(ENA, OUTPUT); //stel pin in als uitgang
  pinMode(IN1, OUTPUT); //stel pin in als uitgang
  pinMode(IN2, OUTPUT); //stel pin in als uitgang
  pinMode(IN3, OUTPUT); //stel pin in als uitgang
  pinMode(IN4, OUTPUT); //stel pin in als uitgang
  pinMode(ENB, OUTPUT); //stel pin in als uitgang
}

void loop(){
   if (BT.available()){ // if text arrived in from BT serial...
    a=(BT.read());
    if (a=='0'){  //select
      va = 0;
      lr = 0;
      setacl=412;
      BT.println("select");}  //leesbaar indien de tekst-app gebruikt wordt
    if (a=='1'){  //vooruit ^
      va = af;
      lr = 0;
      BT.println("vooruit");}
    if (a=='2'){  //achteruit \/
      va = -af;
      lr = 0;
      BT.println("achteruit");}
    if (a=='3'){  //links <
      lr = af;
      va = 0;
      setacl = 412;
      BT.println("links");}
    if (a=='4'){  //rechts >
      lr = -af;
      va = 0;
      setacl = 412;
      BT.println("rechts");}
    if (a=='5'){  //[]
      Kd = Kd / 10;
      BT.println("vierkantje");}
    if (a=='6'){  //X
      Kp = Kp - 1;
      BT.println("X");}
    if (a=='7'){  //driehoekje
      Kp = Kp + 1;
      BT.println("driehoekje");}
    if (a=='8'){  //O
      Kd = Kd * 10;
      BT.println("O");}
    if (a=='9'){  //start
      BT.println("start");}
  } 
  setacl = setacl + va + lr;    //verander setwaarde zodat er op comando naar voor gekanteld wordt
  int acl = analogRead(ACL)- setacl;
  int gyr = analogRead(GYR) - analogRead(GYRREF);
  v = acl * Kp + gyr * Kd;      //berken snelheid voor de motoren
  if(v > 256){                  //Stel een maximum limiet in voor de pwm uitganswaarde (256 = 5V)
    v = 256;
  }
  if(v < -256){                 //Stel een minimum limiet in voor de pwm uitganswaarde (256 = 5V)
    v = -256;
  }
  if(v>0){                      //Als de aangegeven snelheid positief is, laat dan de motoren voorwaarts draaien
    digitalWrite(IN1, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
  }
  if(v<0){                      //Als de aangegeven snelheid negatief is, laat dan de motoren achterwaarts draaien
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
  }
  if(lr = 0){                   //Als links of rechts niet actief is stuur dan de twee motoren aan met snelheid v
    analogWrite(ENA, abs(v));
    analogWrite(ENB, abs(v));
  }
  if(lr < 0){                   //Blokeer rechtse wiel om naar rechts te kunnen draaien, stuur het andere wiel aan met snelheid v
    analogWrite(ENA, abs(v));
    analogWrite(ENB, 0);
  }
  if(lr > 0){                   //Blokeer linkse wiel om naar links te kunnen draaien, stuur het andere wiel aan met snelheid v
    analogWrite(ENA, 0);
    analogWrite(ENB, abs(v));
  }
}
