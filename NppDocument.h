#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <list>
#include "pluginInterface.h"
#include "MsgFixBounds.h"
#include "NppDocument.h"
#include "AbstractDocument.h"

class NppDocument : public AbstractDocument {
	private :
		struct NppData nppData;
		NppDocument();//nope !
	public :
		NppDocument(const struct NppDocument& a_document);
		NppDocument(const struct NppData& a_nppData);
		NppData getNppData();
		void setNppData(const struct NppData a_nppData);


		long getNbLines()const;
		char* getLine(long a_lineNumber)const;
		long getLineSize(long a_lineNumber)const;
		long getStartPositionOfLine(long a_line)const;
		std::vector<MsgFixBounds*>* extractAllMsgFixFromDocument();//Fix messages should be returned in the same order they appear in document
		char* getMsgFixContent(MsgFixBounds*)const;
		MsgFixBounds* getNextMsgFixOfSameType(MsgFixBounds* a_currentMsgFix);//returns next fix message with same type (field 35 identical) according to the current fix message, QuoteRequest -> QuoteRequest, MarketDataRequest -> MarketDataRequest..., if such message exists, NULL otherwise
		MsgFixBounds* extractMsgFixFromLine(long a_indexLine)const;
		inline ~NppDocument(){}
};

#endif

