
int data1 = 0;

//Just used until new code is done.
int tag[14];

int crc1;
int crc2;

void setup(){
  Serial.flush();      //Flushing serial for correct first read.
  Serial.begin(9600);
}


void loop() 
{
  int tag[14];
  if (Serial.available() > 0) {
    delay(200);          //Delay for data to arrive into the serial buffer.

    for(int i = 0; i < 14; i++){
      // read the incoming number on serial RX
      data1 = Serial.read();
      tag[i] = data1;
    }
    Serial.flush();

    
    Serial.println("New Tag");
    //printing data
    for(int i = 1; i < 11; i++){
      //Check if it's a number or letter
      if((47 < tag[i]) && (tag[i] < 58)){
        Serial.print((tag[i]-48), HEX);
        Serial.print(" ");
      }
      else{
        Serial.print((tag[i]-55), HEX);
        Serial.print(" ");
      }

    }
    Serial.println("");
    Serial.println("End of Tag");
  }
}

