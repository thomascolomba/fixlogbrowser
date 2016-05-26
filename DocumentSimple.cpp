#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include "MsgFixBounds.h"
#include "AbstractDocumentHelper.h"
#include "DocumentSimple.h"



DocumentSimple::DocumentSimple(){
	lines = new std::vector<char*>;
}

DocumentSimple::DocumentSimple(std::vector<char*>* a_lines){
	setLines(a_lines);
}

void DocumentSimple::setLines(std::vector<char*>* a_lines){
	if(a_lines != NULL){//free memory
		free(lines);
	}
	if(a_lines == NULL){
		lines = new std::vector<char*>;
	} else {
		lines = a_lines;
	}
}

std::vector<char*>* DocumentSimple::getLines(){
	return lines;
}

DocumentSimple::~DocumentSimple(){
	free(lines);
}

long DocumentSimple::getNbLines()const{
	return lines->size();
}

char* DocumentSimple::getLine(long a_lineNumber)const{
	return lines->at(a_lineNumber);
}

long DocumentSimple::getLineSize(long a_lineNumber)const{
	return strlen(lines->at(a_lineNumber));
}

long DocumentSimple::getStartPositionOfLine(long a_indexLine)const{
	if(0 > a_indexLine || a_indexLine >= getNbLines()){
			return -1;
	}
	long l_startPositionOfLine = 0;
	for(int b_indexLine = 0; b_indexLine < a_indexLine; b_indexLine++){
		l_startPositionOfLine = l_startPositionOfLine + getLineSize(b_indexLine);
	}
	return l_startPositionOfLine;
}

MsgFixBounds* DocumentSimple::extractMsgFixFromLine(long a_indexLine)const{
	long l_startPositionOfLine = getStartPositionOfLine(a_indexLine);
	MsgFixBounds* l_msgFixBounds = AbstractDocumentHelper::extractMsgFixBoundsFromLine(getLine(a_indexLine), l_startPositionOfLine);
	return l_msgFixBounds;
}

std::vector<MsgFixBounds*>* DocumentSimple::extractAllMsgFixFromDocument(){//Fix messages should be returned in the same order they appear in document
	std::vector<MsgFixBounds*>* l_fixMessages = new std::vector<MsgFixBounds*>;
	for(unsigned long b_indexLine = 0; b_indexLine < lines->size(); b_indexLine++){
		l_fixMessages->push_back(extractMsgFixFromLine(b_indexLine));
	}
	return l_fixMessages;
}

char* DocumentSimple::getMsgFixContent(MsgFixBounds* a_msgFixBounds)const{
	char* l_result = new char[a_msgFixBounds->getLength()+1];
	//todo thomas find the line where the MsgFixBounds starts
	long b_startPositionOfCurrentLine = 0;
	for(unsigned long b_indexLine = 0; b_indexLine < lines->size(); b_indexLine++){
		long b_endPositionOfCurrentLine = b_startPositionOfCurrentLine + getLineSize(b_indexLine);
		if(b_startPositionOfCurrentLine <= a_msgFixBounds->getStartPosition() && a_msgFixBounds->getEndPosition() <= b_endPositionOfCurrentLine){
			//we found the line containing specified MsgFixBounds
			long l_startPostionOnCurrentLine = a_msgFixBounds->getStartPosition() - b_startPositionOfCurrentLine;
			strncpy(l_result, lines->at(b_indexLine)+l_startPostionOnCurrentLine, a_msgFixBounds->getLength());
			l_result[a_msgFixBounds->getLength()] = '\0';
			return l_result;
		}
		b_startPositionOfCurrentLine = b_startPositionOfCurrentLine + getLineSize(b_indexLine);
	}
	return NULL;
}

MsgFixBounds* DocumentSimple::getNextMsgFixOfSameType(MsgFixBounds* a_currentMsgFix){return NULL;}//returns next fix message with same type (field 35 identical) according to the current fix message, QuoteRequest -> QuoteRequest, MarketDataRequest -> MarketDataRequest..., if such message exists, NULL otherwise
