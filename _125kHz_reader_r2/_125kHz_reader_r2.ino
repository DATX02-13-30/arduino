
int data1 = 0;

//Just used until new code is done.
int tag[14];

int crc1;
int crc2;

void setup(){
  Serial.flush();      //Flushing serial for correct first read.
  Serial.begin(9600);
}

//function to read tag, we recieve the array we write the info to
int* readTag(){
  //Delay for data to arrive into the serial buffer.
  delay(200);
  //array where we add tag data.
  int arrayTag[14];
  //Used in the loop.
  int tagData;
  //Transfering data from serial to the tag in the loop
  for(int i = 0; i < 14; i++){
    // read the incoming number on serial RX
    tagData = Serial.read();
    //transfering it to the array.
    arrayTag[i] = tagData;
  }
  //we flush the serial so we dont have residual data when we read it again.
  Serial.flush();
  //Return the tag array.
  return arrayTag;
}

void printTag(){
  
}

void loop() 
{
  

  if(Serial.available() > 0) {  
    //we call on the read function and save the array to a pointer.
    int* tag;
    tag = readTag();
    
    
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





