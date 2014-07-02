

void getAllNodes ( int timeOut ) {
  
       // set up the LCD display
       xbxt.clearLine(1);
       xbxt.lcdMessage("ND :: node discover");
       Console.println( "\n\n ND :: node discover -- this is a timed out blocking function" );
       xbxt.clearList();

       // actually send the discover command
       xbxt.xtSendAtCommand('N','D');
       long startTime = millis();
       long timeNow = millis();
       Console.println(timeOut);
       while ( (timeNow - startTime) < timeOut) { 
        
           // listen for AT responses
           if ( xbxt.xtListen() == AT ){
              Console.print( " AT command success :: " );
              Console.println(xbxt.zbRxString);
           }
         delay(5);
         timeNow = millis();
        
       } // end time out 
       Console.println("\n search done.");
  LIST_ACQUIRED = true;
}


int checkForNode(XBeeAddress64 testNode) {
  
   int testLsb = testNode.getLsb();
 
   for(int i = 0; i < xbxt.nodeList.size(); i++){ 
        XBeeAddress64 node = xbxt.nodeList.get(i);     
        int currentLsb = node.getLsb();
        if ( testLsb == currentLsb ) return 1;
   } 

   
   return 0;           
}

