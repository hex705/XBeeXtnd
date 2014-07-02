//for the yun 
// keep baud rate low 9600




// challenge can't use pins 0,1 with xBee
// should be able to use software serial.
// but it is blocking and non-buffered

#include <Bridge.h>
#include <AltSoftSerial.h>
#include <XBee.h>
//  #include <SoftwareSerial.h>
#include <LinkedList.h>
#include <LiquidCrystal.h> 
#include <XBeeXtnd.h>

// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
//uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
//uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
//SoftwareSerial nss(ssRX, ssTX);
LiquidCrystal  lcd(12,11,6,4,3,2);
XBee xbee = XBee();
XBeeXtnd xbxt; 
AltSoftSerial altSerial;

boolean  LIST_ACQUIRED = false;
boolean  nodeFound     = false; 

long addressPrefix = 0x13A200;

// node to find 
XBeeAddress64 nodeOfInterest = XBeeAddress64(0x13A200, 0x4054F0F7);
XBeeAddress64 dst  = XBeeAddress64(addressPrefix, 0x40311E08);

int oldL;
#define timeInterval 15000
long timeOut = 5000;
boolean firstTime = true;

void setup() {
  // start the LCD
  lcd.begin(16, 2);
  altSerial.begin(9600); // no longer 9600
  //Serial.begin(38400);
  xbee.begin(altSerial);
  
  // create and extended object with LCD and radio 
  xbxt.xtCreate(xbee, lcd);
  xbxt.setTimeout(5);

  // start a software serial port as well (rendundancy is good) 
 
  delay(2000);
   // initialize serial communication -- need these to see stuff in arduino
  Bridge.begin();
  Console.begin(); 
  
  
  while (!Console){
    ; // wait for Console port to connect.
  }
  
  Console.println("You're connected to the Console!!!!");
  
  
  // start code 
  Console.println("===start ( XbXT test_LCD ) ===");
  xbxt.xtSendAtCommand('N', 'I');
  Console.println("=== sent ===");
 
}

void loop() {
  
  
    if (firstTime) {
       // check for the AT response to the NI COMMAND
 int x = xbxt.xtListen();

if (x == 3 ) {
       Console.print( " AT command success :: " );
       String s = xbxt.zbRxString;
       Console.println(s);
       xbxt.clearLine(0);
       xbxt.lcdMessage(s,0);
       
}
  
 delay(3000);
  // set up LCD via the xtended object
  xbxt.clearLine(1);
  xbxt.lcdMessage("firstTime done");
  Console.println("firstTime done");
  delay(1000);
   xbxt.lcdMessage("setup done");
   firstTime = false; 
}
    
  // put your main code here, to run repeatedly: 
  // listend for any and all meassages -- this is needed for node  list calls too. 
// xbxt.sendZB( dst, "trying to do a send");
 int LISTEN = xbxt.xtListen();
   // Console.print( "l :: ");
   // Console.println( L );
   
   if ( LISTEN > 0  ) {

     // when I get a message from another ZB this is the resposne
      if (LISTEN == ZB ) {
         Console.print(" received xb message " );
         String s = xbxt.zbRxString;
         Console.println(s);
        
            // node discover is true you need to figure out how to manage time outs etc. 
      }
      
      // don't know why I have this
      if ( LISTEN == ZBSR ) {
         Console.print("Status response :: " );
         Console.println( xbxt.getStatus() );
      }
      
      // when I send a local AT command this is the response 
      if ( LISTEN == AT ) {
       Console.print( " AT command success :: " );
       Console.println(xbxt.zbRxString);
      }
      
  }  // end sort messages 
  
  if (  (LISTEN == ZB ) && (xbxt.zbRxString.charAt(4) == 'N') && (xbxt.zbRxString.charAt(5) == 'D')   ) {
     // do the thing where you do stuff becasue you were told to do a thing. 
     
     // not this is a node discovery and it will block for the duration of timeout  
    //  incoming messages will be lost during this process.    
     getAllNodes(15000);   
     
  }
  
 //  if ((  xbxt.nodeList.size() > 1  ) && ( (millis()-timeOut) > timeInterval) ) {
  if (( LIST_ACQUIRED ) && ( (millis()-timeOut) > timeInterval) ) {
   
        printNodeList();
        
        //  check for the node -- havea time out on the search -- or limit by scans?
        int lookForNode = checkForNode(nodeOfInterest);
        
        Console.print("Looking for 0x");
        Console.print(nodeOfInterest.getMsb(),HEX);
        Console.print("\t0x");
        Console.print(nodeOfInterest.getLsb(),HEX); 
        if ( lookForNode == 0 ) { Console.print(" ... not");}
        Console.println(" found.");
        LIST_ACQUIRED = false;
        timeOut = millis();
  }
}


void printNodeList() {
 
  Console.println("Address list");
  for(int i = 0; i < xbxt.nodeList.size(); i++){ 
        XBeeAddress64 node = xbxt.nodeList.get(i);
        Console.print("0x");
        Console.print(node.getMsb(),HEX);
        Console.print("\t0x");
        Console.println(node.getLsb(),HEX);
                
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
