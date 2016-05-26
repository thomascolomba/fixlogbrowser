#ifndef MSGFIXBOUNDS_H
#define MSGFIXBOUNDS_H

#include <vector>

/*typedef long Sci_PositionCR;
struct Sci_CharacterRange {
	Sci_PositionCR cpMin;
	Sci_PositionCR cpMax;
};
struct Sci_TextRange {
	struct Sci_CharacterRange chrg;
	char *lpstrText;
};*/



//a fix message is "8=FIX...|key=value|key=value|...|10=...|", keys and values can't contain neither '|' nor '='. keys and values can't be empty too.

class MsgFixBounds {
	private :
		long startPosition;//included
		long endPosition;//included
	public :
		MsgFixBounds(long a_startPosition, long a_endPosition);
		MsgFixBounds(const MsgFixBounds&);
		long getStartPosition();
		void setStartPosition(long a_startPosition);
		long getEndPosition();
		void setEndPosition(long a_endPosition);
		long getLength();
		inline bool operator==(const MsgFixBounds& a_other){return this->startPosition == a_other.startPosition && this->endPosition == a_other.endPosition;}
		inline bool operator!=(const MsgFixBounds& a_other){return !(*this==a_other);}

		static void printToConsole(MsgFixBounds*);//implementation may change
		static void printToConsole(std::vector<MsgFixBounds*>*);//implementation may change

		static char* g_fixHeader;
		static char g_fixKeyValueSeparator;
		static char g_fixFieldSeparator;
		static char* g_tagChecksum;
		static char* MDReqID_key;
		static char* MDEntryID_key;
		static char* QuoteReqID_key;
		static char* QuoteID_key;
		static char* OrderID_key;
		static char* clOrdID_key;
		static char* msgType_key;
		static char* subscriptionRequestType_key;
		static char* symbol_key;

		static char* marketDataRequest_msgType;
		static char* cancelMarketDataRequest_msgType;
		static char* marketDataRequestReject_msgType;
		static char* marketDataFullSnapshot_msgType;
		static char* marketDataIncrementalSnapshot_msgType;
		static char* marketDataResponse_msgType;
		static char* newOrderSingle_msgType;
		static char* executionReport_msgType;
};

#endif
