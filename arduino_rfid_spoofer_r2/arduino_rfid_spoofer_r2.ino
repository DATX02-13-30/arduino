/* Pin to connect to the circuit
 * Setting the pin LOW will tune the coil,
 * meaning it will respond as a high signal to the reader
 * Setting the pin to HIGH will detune coil
 * meaning the reader will see it as a low signal
 */
  
int coil_pin = 2;

void setup()
{
  // Configururation of terminal
  Serial.begin(9600);
  
  // Set pin as output
  pinMode(coil_pin, OUTPUT);
    
  // Start pin as low
  digitalWrite(coil_pin, LOW);
}

// Manchester encoding for signal and sets pins.
// Needs clock and signal to do encoding
void set_pin_manchester(int clock_half, int signal)
{
  // Manchester encoding is xoring the clock with the signal
  int man_encoded = clock_half ^ signal;
  
  // If 1, set the pin LOW (this will tune the antenna and the reader sees this as a high signal)
  // If 0, set the pin to HIGH  (this will detune the antenna and the reader sees this as a low signal)
  if(man_encoded == 1)
  {
     digitalWrite(coil_pin, LOW);
  }
  else
  {
    digitalWrite(coil_pin, HIGH);
  }
}

// Use this function to do the sending of the spoofed tag.
// It's recieves the array of what it is supposed to send.
void transmitt_tag(int* tagg_pointer){
  // Loop and call on set_pin_manchester.
  for(int i = 0; i < 64; i++)
  {
    set_pin_manchester(0, tagg_pointer[i]);
    delayMicroseconds(256);
    
    set_pin_manchester(1, tagg_pointer[i]);
    delayMicroseconds(256); 
  }
}

// Writes the given hex number in binary in the message array
void write_in_HEX(byte number, int place, int* message){
  // Calculate where in the message to write the number.
  // Will point to the least-significant bit.
  int index = 12 + (place * 5);
  // Write in the bits.
  for(int i=0; i<4; i++){
    // Read each bit from the given number, starting with least-significant.
    int Bit = bitRead(number,i);
    // Write the bit to the message, writing the least-significant first.
    message[index-i] = Bit;
  }
}

// Used to load the UID into the array.
void load_UID (byte* tag, int* message){
  // Go through the UID and load them into the message array.
  for(int i=0; i<10; i++){
    // Read in a UID number.
    byte UID_num = tag[i];
    // Calling on the write function, it takes the hex number, where to write it and the message.
    write_in_HEX(tag[i], i, message);
  }
  // The UID is now inserted into the message.
}

// Decides the even parity for a given number.
int number_parity(byte number){
  // Counter for number of 1 bits found
  int ones = 0;
  // Go through the bits of the given number
  for(int i=0; i<4; i++){
    // If we get a 1, we increment.
    if(bitRead(number,i) == 1){
      ones++;
    }
  }
  // With the number of ones counted and modulo we can say what the parity is.
  return (ones % 2);
}

// Calculates the parity of a column.
int column_parity(int column, byte* tag){
  // Counter for number of 1 bits found
  int ones = 0;  
  // Step through the numbers and check a specific bit in each number.
  for(int i=0; i<10; i++){
    // If it is a 1 from the bit read then we increments ones.
    if(bitRead(tag[i], column)){
      ones++;
    }
  }
  // Return what the parity should be.
  return (ones % 2);
}

// Function goes through the binary_message and adds parity to it.
void create_parity(byte* tag, int* binary_message){
  // This index to the parity for the first number.
  int index = 13;
  
  // This loop sets the parity of the numbers.
  for(int i=0; i<10; i++){
    // Number_parity will return the parity. Insert it directly into the message.
    binary_message[index+(5*i)] = number_parity(tag[i]);
  }
  
  // Setting the index to the last column parity bit in the array.
  index = 62;
  // Column parity.
  for(int i=0; i<4; i++){
    // On each iteration, evaluate the parity of a column.
    binary_message[index-i] = column_parity(i, tag);
  }
}

void loop()
{
  // Hex format of the tags UID that we want to spoof.
  byte tag_to_spoof[10] = {0x0,0x4,0x1,0x5,0xD,0x8,0xF,0xD,0xF,0x6};  
  // This array is used to construct the tagg message we want to send, this array will be modified.
  int binary_message[64] = {1,1,1,1,1,1,1,1,1, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0};
  // The UID gets inserted into the binary_message in this array.
  load_UID(tag_to_spoof, binary_message);
  // Create the parity of the message. Number parity and column parity.
  create_parity(tag_to_spoof, binary_message);
  
  
  /*Printing tag on terminal.
  for(int i = 0; i < 64; i++){
    Serial.print(" ");
    Serial.print(binary_message[i]);
  }
  
  Serial.println("\n New Line");
  // Using delay to reduce in-terminal spam.
  delay(10000);
  */
  
  // Calling the transmitt function.
  transmitt_tag(binary_message);
  
}
