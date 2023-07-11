#include <SoftwareSerial.h>

uint8_t inInts[150];   // string to hold input
float vol=0,cur=0;
int soc=0,temp=0;
SoftwareSerial MySerial(10,11 ); // RX | TX

void setup() {

  
    Serial.begin(9600);
    MySerial.begin(19200); 
 
}

void loop() {
getdata();
delay(500);
}


void getdata()
{

int ii=0;
int sum=0,sum1=0;

uint8_t data1[6] = { 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00 };  MySerial.write(data1, 6);
delay(10);
 while (MySerial.available()) {int bt = MySerial.read(); 
                               inInts[ii] = "";  
                               inInts[ii] = bt; 
                               ii++;      
                               delayMicroseconds(30); }
  
  for (int i = 0; i < ii; i++){  if ((i >=4) && (i<= 137)){sum=sum+inInts[i];}  }
  sum1= inInts[138]*256+ inInts[139];  // check sum, to check the error while transmission. 
                               

if(sum==sum1){
//Serial.println("");
//for (int i = 0; i < ii; i++){ Serial.print(inInts[i],HEX); Serial.print(", "); }
//Serial.println("");
  vol=((inInts[4]<<8)|inInts[5])*0.1; //bitwise or and left shift operator(which is basically multiplication by 2^shiftingvalue).
  soc=inInts[74];
  
  temp=(inInts[97]<<8)|inInts[98];
  cur=((inInts[72]<<8)|inInts[73])*0.1;
      Serial.print(vol);Serial.print("x"); 
     Serial.print(cur);Serial.print("x");  
     Serial.print(soc);Serial.print("x");   
     Serial.print(temp);Serial.print("x\n");         
    
            
      
              
              }

}
