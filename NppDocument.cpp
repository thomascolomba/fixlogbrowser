#include "PluginInterface.h"
#include "MsgFixBounds.h"
#include "NppDocument.h"
#include "AbstractDocumentHelper.h"

NppDocument::NppDocument(){}//nope !
NppDocument::NppDocument(const NppDocument& a_NppDocument)
:nppData(a_NppDocument.nppData)
{}
NppDocument::NppDocument(const struct NppData& a_nppData)
:nppData(a_nppData)
{}
NppData NppDocument::getNppData(){
	return nppData;
}
void NppDocument::setNppData(const struct NppData a_nppData){
	nppData = a_nppData;
}
/*
MsgFixBounds* NppDocument::extractMsgFixFromLine(char* a_line){
	return NULL;//todo thomas implement
}
std::list<MsgFixBounds> NppDocument::extractAllMsgFixFromNppDocument(){
	std::list<MsgFixBounds> l_myList;
	return l_myList;;//todo thomas implement
}
*/

/**
  * you are responsible to release memory used by the returned value
  */
/*char* NppDocument::getMsgFixContent(MsgFixBounds* msgFix){
	long l_startPosition = msgFix->getStartPosition();
	long l_endPosition = msgFix->getEndPosition();
	char* l_content = new char[l_endPosition - l_startPosition + 1];
	TextRange tr;
	tr.chrg.cpMin = l_startPosition;
	tr.chrg.cpMax = l_endPosition;
	tr.lpstrText  = l_content;
	::SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	//ScintillaMsg(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	l_content[l_endPosition - l_startPosition] = '\0';
	return l_content;
}*/


//todo thomas implement these methods
long NppDocument::getNbLines()const{
	long l_lastPositionInNppDocument = ::SendMessage(nppData._scintillaMainHandle, SCI_GETLENGTH, 0, 0);
	long l_nbLinesInNppDocument = ::SendMessage(nppData._scintillaMainHandle, SCI_LINEFROMPOSITION, l_lastPositionInNppDocument, 0);
	return l_nbLinesInNppDocument+1;
}
/**
  * you are responsible to release memory used by the returned value
  */
char* NppDocument::getLine(long a_lineNumber)const{
	int l_lineLength = ::SendMessage(nppData._scintillaMainHandle, SCI_LINELENGTH, a_lineNumber, 0);
	char *l_line = new char[l_lineLength+1];
	::SendMessage(nppData._scintillaMainHandle, SCI_GETLINE, a_lineNumber, (LPARAM)l_line);//fills l_line
	l_line[l_lineLength] = '\0';
	return l_line;
}
long NppDocument::getLineSize(long a_lineNumber)const{
	return ::SendMessage(nppData._scintillaMainHandle, SCI_LINELENGTH, a_lineNumber, 0);
}
long NppDocument::getStartPositionOfLine(long a_line)const{
	 return ::SendMessage(nppData._scintillaMainHandle,  SCI_POSITIONFROMLINE , a_line, 0);
}
std::vector<MsgFixBounds*>* NppDocument::extractAllMsgFixFromDocument(){
	std::vector<MsgFixBounds*>* l_fixMessages = new std::vector<MsgFixBounds*>;
		for(long b_indexLine = 0; b_indexLine < getNbLines(); b_indexLine++){
			MsgFixBounds* l_msg = extractMsgFixFromLine(b_indexLine);
			if(l_msg != NULL){
				l_fixMessages->push_back(l_msg);
			}
		}
		return l_fixMessages;
}

char* NppDocument::getMsgFixContent(MsgFixBounds* msgFix)const{
	long l_startPosition = msgFix->getStartPosition();
	long l_endPosition = msgFix->getEndPosition();
	char* l_content = new char[msgFix->getLength() + 1];
	TextRange tr;
	tr.chrg.cpMin = l_startPosition;
	tr.chrg.cpMax = l_endPosition+1;//to include last character
	tr.lpstrText  = l_content;
	::SendMessage(nppData._scintillaMainHandle, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	//ScintillaMsg(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	l_content[msgFix->getLength()] = '\0';
	return l_content;
}
MsgFixBounds* NppDocument::getNextMsgFixOfSameType(MsgFixBounds* a_currentMsgFix){return NULL;}
MsgFixBounds* NppDocument::extractMsgFixFromLine(long a_indexLine)const{//todo thomas
	long l_startPositionOfLine = getStartPositionOfLine(a_indexLine);
		MsgFixBounds* l_msgFixBounds = AbstractDocumentHelper::extractMsgFixBoundsFromLine(getLine(a_indexLine), l_startPositionOfLine);
		return l_msgFixBounds;
}
