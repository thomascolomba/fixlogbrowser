#ifndef ABSTRACT_DOCUMENT_H
#define ABSTRACT_DOCUMENT_H

#include <vector>
#include "MsgFixBounds.h"

/**
 * should be implemented for :
 * 1) test
 * 2) Notepad++ document
 * 3) Any else document...
 *
 * It is assumed one line contains 0 or 1 fix message.
 */
class AbstractDocument {
public:
	virtual long getNbLines()const=0;
	virtual char* getLine(long a_lineNumber)const = 0;
	virtual long getLineSize(long a_lineNumber)const = 0;// \0 not included
	virtual long getStartPositionOfLine(long a_line)const = 0;
	virtual MsgFixBounds* extractMsgFixFromLine(long a_line)const = 0;
	virtual std::vector<MsgFixBounds*>* extractAllMsgFixFromDocument() = 0;//Fix messages should be returned in the same order they appear in document
	virtual char* getMsgFixContent(MsgFixBounds*)const = 0;

	virtual MsgFixBounds* getNextMsgFixOfSameType(MsgFixBounds* a_currentMsgFix) = 0;//returns next fix message with same type (field 35 identical) according to the current fix message, QuoteRequest -> QuoteRequest, MarketDataRequest -> MarketDataRequest..., if such message exists, NULL otherwise
	virtual inline ~AbstractDocument(){}
};


#endif
