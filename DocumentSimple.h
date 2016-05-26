#ifndef DOCUMENT_TEST_IMPL_H
#define DOCUMENT_TEST_IMPL_H

#include <vector>
#include "MsgFixBounds.h"
#include "AbstractDocument.h"

class DocumentSimple : public AbstractDocument {
private:
	std::vector<char*>* lines;//each line should ends with '\0'
public:
	DocumentSimple();
	DocumentSimple(std::vector<char*>* a_lines);
	void setLines(std::vector<char*>* a_lines);
	std::vector<char*>* getLines();
	virtual ~DocumentSimple();

	long getNbLines()const;
	char* getLine(long a_lineNumber)const;
	long getLineSize(long a_lineNumber)const;
	long getStartPositionOfLine(long a_line)const;
	std::vector<MsgFixBounds*>* extractAllMsgFixFromDocument();//Fix messages should be returned in the same order they appear in document
	char* getMsgFixContent(MsgFixBounds*)const;
	MsgFixBounds* getNextMsgFixOfSameType(MsgFixBounds* a_currentMsgFix);//returns next fix message with same type (field 35 identical) according to the current fix message, QuoteRequest -> QuoteRequest, MarketDataRequest -> MarketDataRequest..., if such message exists, NULL otherwise
	MsgFixBounds* extractMsgFixFromLine(long a_indexLine)const;
};

#endif
