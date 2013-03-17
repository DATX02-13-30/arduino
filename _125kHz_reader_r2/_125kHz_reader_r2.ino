void setup(){
  Serial.flush();      //Flushing serial for correct first read.
  Serial.begin(9600);
}

//function to read tag, we recieve the array we write the info to
int* readTag(){
  //array where we add tag data.
  int tag[14];
  
  //Delay for data to arrive into the serial buffer.
   delay(200);
    //Transfering data from serial to the tag in the loop
    for(int i = 0; i < 14; i++){
      // read the incoming number on serial RX
      tag[i] = Serial.read();
      //transfering it to the array.
    
  }
  //we flush the serial so we dont have residual data when we read it again.
  Serial.flush();
  //Return the tag array.
  return tag;
}

void printTag(int* inputTag ){
  int* tag = inputTag;
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
    Serial.println("\nEnd of Tag");
}

boolean crcCheck(int* inputTag) {
  int* tag = inputTag;
  int checkValue1 = 0;
  int checkValue2 = 0;
  for(int i=1;i<11; i+=2) {
    checkValue1 = checkValue1 ^ tag[i];
  }
  for(int i=2;i<11; i+=2) {
    checkValue2 = checkValue2 ^ tag[i];
  }
  return (checkValue1 == tag[11])&&(checkValue2 == tag[12]);
}

void loop() 
{
  if(Serial.available()){
    int* tag = readTag();
    //if(crcCheck(tag)){
    printTag(tag);
  }
}





