#include <stdio.h>
#include "MsgFixBounds.h"

MsgFixBounds::MsgFixBounds(long a_startPosition, long a_endPosition)
:startPosition(a_startPosition), endPosition(a_endPosition)
{}

long MsgFixBounds::getStartPosition(){
	return startPosition;
}
void MsgFixBounds::setStartPosition(long a_startPosition){
	startPosition = a_startPosition;
}
long MsgFixBounds::getEndPosition(){
	return endPosition;
}
void MsgFixBounds::setEndPosition(long a_endPosition){
	endPosition = a_endPosition;
}
/*
bool MsgFixBounds::operator==(const MsgFixBounds& a_first, const MsgFixBounds& a_second){
	return a_first.startPosition == a_second.startPosition && a_first.endPosition == a_second.endPosition;
}*/
/*
bool MsgFixBounds::operator!=(const MsgFixBounds& a_first, const MsgFixBounds& a_second){
	return !(a_first == a_second);
}*/
/**
  * could be improved by using SCI_GETTEXTRANGE
  * here is some code found on scintilla.org to do it : 
  */
/*char* MsgFix::getContent(struct NppData nppData){
	Sci_TextRange l_tr;
	l_tr.chrg.cpMin = startPosition;
	l_tr.chrg.cpMax = endPosition;
	l_tr.lpstrText = new char[endPosition-startPosition+1];
	::SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&l_tr));
	return l_tr.lpstrText;	
}*/

long MsgFixBounds::getLength(){
	return endPosition - startPosition + 1;
}

void MsgFixBounds::printToConsole(MsgFixBounds* a_msg){
	printf("[%li, %li]\n", a_msg->startPosition, a_msg->endPosition);
}
void MsgFixBounds::printToConsole(std::vector<MsgFixBounds*>* a_msgList){
	for(unsigned long b_indexMsgList = 0; b_indexMsgList < a_msgList->size(); b_indexMsgList++){
		printToConsole(a_msgList->at(b_indexMsgList));
	}
}

char* MsgFixBounds::g_fixHeader = "8=FIX";
char MsgFixBounds::g_fixKeyValueSeparator = '=';
char MsgFixBounds::g_fixFieldSeparator = '|';//could be SOH
char* MsgFixBounds::g_tagChecksum = "10";

char* MsgFixBounds::MDReqID_key = "262";
char* MsgFixBounds::MDEntryID_key = "278";
char* MsgFixBounds::QuoteReqID_key = "131";
char* MsgFixBounds::QuoteID_key = "117";
char* MsgFixBounds::OrderID_key = "37";
char* MsgFixBounds::clOrdID_key = "11";
char* MsgFixBounds::msgType_key = "35";
char* MsgFixBounds::subscriptionRequestType_key = "263";
char* MsgFixBounds::symbol_key = "55";

char* MsgFixBounds::marketDataRequest_msgType = "V";
char* MsgFixBounds::cancelMarketDataRequest_msgType = "V";
char* MsgFixBounds::marketDataRequestReject_msgType = "Y";
char* MsgFixBounds::marketDataFullSnapshot_msgType = "W";
char* MsgFixBounds::marketDataIncrementalSnapshot_msgType = "X";
char* MsgFixBounds::marketDataResponse_msgType = "AJ";//or AG -> todo check;
char* MsgFixBounds::newOrderSingle_msgType = "D";
char* MsgFixBounds::executionReport_msgType = "8";

/*TextRange tr;
  tr.chrg.cpMin = start;
  tr.chrg.cpMax = end;
  tr.lpstrText  = text;
  ScintillaMsg(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));*/
