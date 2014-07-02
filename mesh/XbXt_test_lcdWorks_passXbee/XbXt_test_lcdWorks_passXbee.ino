// this one is for an uno 


#include <XBee.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>
#include <LiquidCrystal.h> 
#include <XBeeXtnd.h>

// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);
LiquidCrystal  lcd(12,11,5,4,3,2);
XBee xbee = XBee();
XBeeXtnd xbxt; 

boolean  LIST_ACQUIRED = false;
boolean  nodeFound     = false; 

long addressPrefix = 0x13A200;

// node to find 
XBeeAddress64 nodeOfInterest = XBeeAddress64(0x13A200, 0x4054F0F7);


int oldL;

#define timeInterval 15000
long timeOut = 5000;

void setup() {
  // start the LCD
  lcd.begin(16, 2);
  
  Serial.begin(38400);
  xbee.begin(Serial);
  
  // create and extended object with LCD and radio 
  xbxt.xtCreate(xbee, lcd);
  

  // start a software serial port as well (rendundancy is good) 
  nss.begin(9600);
  delay(2000);
  
  // start code 
  nss.println("===start ( XbXT test_LCD ) ===");
  xbxt.xtSendAtCommand('N', 'I');
  nss.println("=== sent ===");
 
 // check for the AT response to the NI COMMAND
  if ( xbxt.xtListen() == 3 ){
       nss.print( " AT command success :: " );
       nss.println(xbxt.zbRxString);
  }

  // set up LCD via the xtended object
  xbxt.clearLine(1);
  xbxt.lcdMessage("reset done");
  delay(1000);
   
}

void loop() {
  
  // put your main code here, to run repeatedly: 
  // listend for any and all meassages -- this is needed for node  list calls too. 
 
 int LISTEN = xbxt.xtListen();
   // nss.print( "l :: ");
   // nss.println( L );
   
   if ( LISTEN > 0  ) {

     // when I get a message from another ZB this is the resposne
      if (LISTEN == ZB ) {
         nss.print(" received xb message " );
         String s = xbxt.zbRxString;
         nss.println(s);
        
            // node discover is true you need to figure out how to manage time outs etc. 
      }
      
      // don't know why I have this
      if ( LISTEN == ZBSR ) {
         nss.print("Status response :: " );
         nss.println( xbxt.getStatus() );
      }
      
      // when I send a local AT command this is the response 
      if ( LISTEN == AT ) {
       nss.print( " AT command success :: " );
       nss.println(xbxt.zbRxString);
      }
      
  }  // end sort messages 
  
  if (  (LISTEN == ZB ) && (xbxt.zbRxString.charAt(4) == 'N') && (xbxt.zbRxString.charAt(5) == 'D')   ) {
     // do the thing where you do stuff becasue you were told to do a thing. 
     
     // not this is a node discovery and it will block for the duration of timeout  
    //  incoming messages will be lost during this process.    
     getAllNodes(20000);   
     
  }
  
 //  if ((  xbxt.nodeList.size() > 1  ) && ( (millis()-timeOut) > timeInterval) ) {
  if (( LIST_ACQUIRED ) && ( (millis()-timeOut) > timeInterval) ) {
   
        printNodeList();
        
        //  check for the node -- havea time out on the search -- or limit by scans?
        int lookForNode = checkForNode(nodeOfInterest);
        
        nss.print("Looking for 0x");
        nss.print(nodeOfInterest.getMsb(),HEX);
        nss.print("\t0x");
        nss.print(nodeOfInterest.getLsb(),HEX); 
        if ( lookForNode == 0 ) { nss.print(" ... not");}
        nss.println(" found.");
        LIST_ACQUIRED = false;
        timeOut = millis();
  }
}


void printNodeList() {
 
  nss.println("Address list");
  for(int i = 0; i < xbxt.nodeList.size(); i++){ 
        XBeeAddress64 node = xbxt.nodeList.get(i);
        nss.print("0x");
        nss.print(node.getMsb(),HEX);
        nss.print("\t0x");
        nss.println(node.getLsb(),HEX);
                
   }
  
} // end print 


// // put your main code here, to run repeatedly: 
//  int L = xbxt.xtListen();
//   xbxt.nssPrint( "l :: ");
//   String LL= "" + L;
//   xbxt.nssPrintln( LL );
//  if ( L > 0  ) {
//  
//      if (L == ZB ) {
//         xbxt.nssPrint(" received xb message " );
//         xbxt.nssPrintln(xbxt.zbRxString);
//      }
//      
//      if ( L == ZBSR ) {
//         xbxt.nssPrint("Status response :: " );
//         String LL= "" + xbxt.getStatus();
//         xbxt.nssPrintln( LL );
//      }
//      
//      if ( L == AT ) {
//       xbxt.nssPrint( " AT command success :: " );
//       xbxt.nssPrintln(xbxt.zbRxString);
//      }
//  }
