//
//  XBeeXtnd.h
//  
//
//  Created by steve daniels on 05-01-14.
//  Copyright (c) 2014 __ribosome.ca__. All rights reserved.
//


#ifndef XBeeXtnd_h
#define XBeeXtnd_h


#include <Arduino.h>
#include <WString.h>        // String
#include <XBee.h>
#include <LinkedList.h>    //Link: http://github.com/ivanseidel/LinkedList
#include <LiquidCrystal.h> 


#define ERR 0
#define AT 3
#define ZB 1
#define ZBSR 2



class XBeeXtnd 

{
private:
	
	// no pass xbee
	
	//XBee xbee;
	XBee *xbee;
	

	
	uint8_t DEBUG_STATE;
	uint8_t STORE_NODE_ADDRESS;
	
	AtCommandRequest  atRequest;  //= AtCommandRequest();
	AtCommandResponse atResponse; //= AtCommandResponse();

	ZBRxResponse zbRx;//ZBRxResponse();
	ZBTxRequest  zbTx; //ZBTxRequest();

	ZBTxStatusResponse zbSR; //= ZBTxStatusResponse();

	XBeeAddress64 dst; //= XBeeAddress64();
	XBeeAddress64 src; // = XBeeAddress64();
	
public:

	LinkedList<XBeeAddress64> nodeList; 
	
	LiquidCrystal *lcd;
	
	// storage
	uint8_t status;
	uint8_t tempAddress[8];
	String  zbRxString;
	String  zbTxString;
	int timeOut;
	String state;
	
	
	// constructor
	//void xtCreate( int,int, int, int, int, int  );
  void xtCreate( XBee &,  LiquidCrystal &  );
	// receive methods
	int  xtListen();
	
	// build API send command
	void xtSendAtCommand( uint8_t , uint8_t );
	void xtSendAtCommand( uint8_t * );
	void xtSendAtCommand( String );
	void xtSendAtCommand( AtCommandRequest );
	
	void sendZB( XBeeAddress64 , String);
	
	void xtDebug( int );
	
	// LinkedList stuff
	
	void clearList();
	void storeAddress();
	
	int getStatus();
	
	
	
	/// crystal shit
	void lcdPrint( String );
	void lcdPrintln( String );
	
	void heartBeat();
	void clearLine ( int );
	void clearLine ( int , int );
	void lcdMessage(char , int );
	void lcdMessage(char );
	
	void lcdMessage( String );

	void lcdMessage( String , int );
	
	// utilities
	void setTimeout( int ) ;
	
	
		


};
 // end of class

#endif