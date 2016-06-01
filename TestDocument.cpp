#include <stdio.h>
#include <string.h>
#include "MsgFixBounds.h"
#include "AbstractDocumentHelper.h"
#include "DocumentSimple.h"


//helper functions
bool stringIsEmpty(char* a_string){
	return a_string == NULL || strlen(a_string) == 0;
}
bool documentIsEmpty(AbstractDocument* a_document){
	if(a_document == NULL)
		return true;
	if(a_document->getNbLines() == 0)
		return true;
	if(a_document->getNbLines() == 1 && stringIsEmpty(a_document->getLine(1)))
			return true;
	return false;
}
char* getContentFromLine(MsgFixBounds* a_msgFix, char* a_line){
	char* l_result = new char[a_msgFix->getLength()+1];
	strncpy(l_result, a_line+a_msgFix->getStartPosition(), a_msgFix->getLength());
	l_result[a_msgFix->getLength()] = '\0';
	return l_result;
}


//////////////////////////////////////////////////////////////////////////////////////
// extractMsgFixFromLine
/////////////////////////////////////////////////////////////////////////////////////
bool performAExtractMsgFixBoundsFromLineTest(char* a_realMsgFixContent, char* a_line){
	if(stringIsEmpty(a_line)){
		return stringIsEmpty(a_realMsgFixContent);
	}
	if(stringIsEmpty(a_realMsgFixContent)){
		return AbstractDocumentHelper::extractMsgFixBoundsFromLine(a_line, 0) == NULL;
	}
	MsgFixBounds* l_msgFix = AbstractDocumentHelper::extractMsgFixBoundsFromLine(a_line, 0);
	return (*a_realMsgFixContent == *(getContentFromLine(l_msgFix, a_line)));
}

bool test_extractMsgFixInsideLine(){
	char* l_realMsgFixContent = "8=FIX...|111=a|22=b b|3=cccc|10=000|";
	char* l_line = "blabla8=FIX...|111=a|22=b b|3=cccc|10=000|3.1416";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_noMsgFix(){
	char* l_realMsgFixContent = NULL;
	char* l_line = "this text does not contain any fix message";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_lineNull(){
	char* l_realMsgFixContent = NULL;
	char* l_line = NULL;
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_malformedMsgFix_noChecksumTag(){
	char* l_realMsgFixContent = NULL;
	char* l_line = "blabla8=FIX...|66=Washington|1=Jefferson|3=Roosevelt|444=Lincoln|3.1416";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_malformedMsgFix_missingKeyValueSeparator(){
	char* l_realMsgFixContent = NULL;
	char* l_line = "blabla8=FIX...|66=Washington|1Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_msgFixRightAtStart(){
	char* l_realMsgFixContent = "8=FIX...|111=a|22=b b|3=cccc|10=000|";
	char* l_line = "8=FIX...|111=a|22=b b|3=cccc|10=000|3.1416";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_msgFixRightAtEnd(){
	char* l_realMsgFixContent = "8=FIX...|111=a|22=b b|3=cccc|10=000|";
	char* l_line = "blabla8=FIX...|111=a|22=b b|3=cccc|10=000|";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

bool test_extractMsgFixFromLine_msgFixOnly(){
	char* l_realMsgFixContent = "8=FIX...|66=Washington|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|";
	char* l_line = "8=FIX...|66=Washington|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|";
	return performAExtractMsgFixBoundsFromLineTest(l_realMsgFixContent, l_line);
}

/**
 * true if all tests are successfull, false otherwise
 */
bool testExtractMsgFixBoundsFromLine(){
	if(test_extractMsgFixInsideLine() == false){
		printf("fail test test_extractMsgFixInsideLine\n");
		return false;
	}

	if(test_extractMsgFixFromLine_noMsgFix() == false){
		printf("fail test test_extractMsgFixFromLine_noMsgFix\n");
		return false;
	}
	if(test_extractMsgFixFromLine_lineNull() == false){
		printf("fail test test_extractMsgFixFromLine_lineNull\n");
		return false;
	}
	if(test_extractMsgFixFromLine_msgFixRightAtStart() == false){
		printf("fail test test_extractMsgFixFromLine_msgFixRightAtStart\n");
		return false;
	}
	if(test_extractMsgFixFromLine_malformedMsgFix_noChecksumTag() == false){
		printf("fail test test_extractMsgFixFromLine_malformedMsgFix_noChecksumTag\n");
		return false;
	}
	if(test_extractMsgFixFromLine_malformedMsgFix_missingKeyValueSeparator() == false){
		printf("fail test test_extractMsgFixFromLine_malformedMsgFix_missingKeyValueSeparator\n");
		return false;
	}
	if(test_extractMsgFixFromLine_msgFixRightAtEnd() == false){
		printf("fail test test_extractMsgFixFromLine_msgFixRightAtEnd\n");
		return false;
	}
	if(test_extractMsgFixFromLine_msgFixOnly() == false){
		printf("fail test test_extractMsgFixFromLine_msgFixOnly\n");
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// extractAllMsgFixFromDocument
/////////////////////////////////////////////////////////////////////////////////////
/**
 * returns true if lists are equals, or both empty
 */
bool compareMsgFixBoundsList(std::vector<MsgFixBounds*>* a_list1, std::vector<MsgFixBounds*>* a_list2){
	if((a_list1 == NULL || a_list1->size() == 0) && (a_list2 == NULL || a_list2->size() == 0)){
		return true;
	}
	if(a_list1->size() != a_list2->size()){
		return false;
	}
	for(unsigned int b_indexList1 = 0; b_indexList1 < a_list1->size(); b_indexList1++){
		if(*(a_list1->at(b_indexList1)) != *(a_list2->at(b_indexList1))){
			return false;
		}
	}
	return true;
}

bool performAExtractAllMsgFixBoundsFromDocumentTest(std::vector<MsgFixBounds*>* a_realMsgFixList, AbstractDocument& a_document){
	std::vector<MsgFixBounds*>* l_extractedMsgFixList= AbstractDocumentHelper::extractAllMsgFixFromDocument(a_document);
	if(l_extractedMsgFixList->size() == 0 && (a_realMsgFixList == NULL || a_realMsgFixList->size() == 0)){
		return true;
	}
	return compareMsgFixBoundsList(l_extractedMsgFixList, a_realMsgFixList);
}

bool test_extractAllMsgFixFromEmptyDocument(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAExtractAllMsgFixBoundsFromDocumentTest(l_realMsgFixList, *l_document);
}

bool test_extractAllMsgFixFromDocumentNotContainingMsgFix(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("qwerty");
	l_lines->push_back("azerty");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAExtractAllMsgFixBoundsFromDocumentTest(l_realMsgFixList, *l_document);
}

bool test_extractAllMsgFixFromASingleLineDocument(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(6,71));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("blabla8=FIX...|66=Washington|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAExtractAllMsgFixBoundsFromDocumentTest(l_realMsgFixList, *l_document);
}

bool test_extractAllMsgFixFromAMultipleLineDocument(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(48,114));
	l_realMsgFixList->push_back(new MsgFixBounds(124,190));
	l_realMsgFixList->push_back(new MsgFixBounds(203,269));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("this text does not contain any fix message");
	l_lines->push_back("blabla8=FIX...|66=Washington1|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
	l_lines->push_back("bla8=FIX...|66=Washington2|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
	l_lines->push_back("azerty");
	l_lines->push_back("8=FIX...|66=Washington3|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|");
	l_lines->push_back("qwerty");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAExtractAllMsgFixBoundsFromDocumentTest(l_realMsgFixList, *l_document);
}

/**
 * true if all tests are successfull, false otherwise
 */
bool testExtractAllMsgFixFromDocument(){
	if(test_extractAllMsgFixFromEmptyDocument() == false){
		printf("fail test test_extractAllMsgFixFromEmptyDocument\n");
		return false;
	}
	if(test_extractAllMsgFixFromDocumentNotContainingMsgFix() == false){
		printf("fail test test_extractAllMsgFixFromDocumentNotContainingMsgFix\n");
		return false;
	}
	if(test_extractAllMsgFixFromASingleLineDocument() == false){
		printf("fail test test_extractAllMsgFixFromASingleLineDocument\n");
		return false;
	}
	if(test_extractAllMsgFixFromAMultipleLineDocument() == false){
		printf("fail test test_extractAllMsgFixFromAMultipleLineDocument\n");
		return false;
	}

	return true;
}

bool performAGetMsgTypeTest(MsgFixBounds* a_msgFix, AbstractDocument& a_document, char* a_expectedMsgType){
	char* l_msgType = AbstractDocumentHelper::getMsgType(a_msgFix, a_document);
	return strcmp(l_msgType, a_expectedMsgType) == 0;
}

bool test_getMsgTypeD(){
	char* l_msgType = "D";
	MsgFixBounds* l_msgFix = new MsgFixBounds(6,63);
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("blabla8=FIX...|35=D|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAGetMsgTypeTest(l_msgFix, *l_document, l_msgType);
}

bool test_getMsgTypeAJ(){
	char* l_msgType = "AJ";
	MsgFixBounds* l_msgFix = new MsgFixBounds(6,63);
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("blabla8=FIX...|1=Jefferson|35=AJ|3=Roosevelt|444=Lincoln|10=000|3.1416");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAGetMsgTypeTest(l_msgFix, *l_document, l_msgType);
}


bool testGetMsgType(){
	if(test_getMsgTypeD() == false){
		printf("fail test test_getMsgTypeD\n");
		return false;
	}
	if(test_getMsgTypeAJ() == false){
		printf("fail test test_getMsgTypeAJ\n");
		return false;
	}

	return true;
}

bool performAGetMsgFixFromDocumentByMsgTypeTest(AbstractDocument& a_document, char* a_msgType, std::vector<MsgFixBounds*>* a_expectedMsgFixList){
	std::vector<MsgFixBounds*>* l_extractedMsgFix = AbstractDocumentHelper::extractMsgFixFromDocumentByMsgType(a_document, a_msgType);
	if((l_extractedMsgFix == NULL || l_extractedMsgFix->size() == 0) && (a_expectedMsgFixList == NULL || a_expectedMsgFixList->size() == 0)){
		return true;
	}
	return compareMsgFixBoundsList(l_extractedMsgFix, a_expectedMsgFixList);
}

bool test_getMsgFixFromDocumentByMsgTypeAJ(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(48,114));
		l_realMsgFixList->push_back(new MsgFixBounds(203,269));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("this text does not contain any fix message");
		l_lines->push_back("blabla8=FIX...|35=AJ|66=Wash1|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("bla8=FIX...|35=AG|66=Wash2|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("azerty");
		l_lines->push_back("8=FIX...|35=AJ|66=Wash3|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|");
		l_lines->push_back("qwerty");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		return performAGetMsgFixFromDocumentByMsgTypeTest(*l_document, "AJ", l_realMsgFixList);
}

bool test_getMsgFixFromDocumentByMsgTypeD(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		//l_realMsgFixList->push_back(new MsgFixBounds(48,114));
		l_realMsgFixList->push_back(new MsgFixBounds(124,190));
		l_realMsgFixList->push_back(new MsgFixBounds(203,269));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("this text does not contain any fix message");
		l_lines->push_back("blabla8=FIX...|35=Y|66=Washi1|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("bla8=FIX...|35=D|66=Washi2|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("azerty");
		l_lines->push_back("8=FIX...|35=D|66=Washi3|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|");
		l_lines->push_back("qwerty");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		return performAGetMsgFixFromDocumentByMsgTypeTest(*l_document, "D", l_realMsgFixList);
}


bool test_getMsgFixFromDocumentByMsgTypeY(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(48,119));
		l_realMsgFixList->push_back(new MsgFixBounds(129,200));
		l_realMsgFixList->push_back(new MsgFixBounds(321,356));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("this text does not contain any fix message");
		l_lines->push_back("blabla8=FIX...|35=Y|66=Washington1|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("bla8=FIX...|35=Y|66=Washington2|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|3.1416");
		l_lines->push_back("azerty");
		l_lines->push_back("8=FIX...|35=D|66=Washington3|1=Jefferson|3=Roosevelt|444=Lincoln|10=000|");
		l_lines->push_back("8=FIX...|35=D|66=Washington4|10=000|");
		l_lines->push_back("8=FIX...|35=Y|66=Washington5|10=000|");
		l_lines->push_back("qwerty");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		return performAGetMsgFixFromDocumentByMsgTypeTest(*l_document, "Y", l_realMsgFixList);
}

bool testExtractMsgFixFromDocumentByMsgType(){
	if(test_getMsgFixFromDocumentByMsgTypeAJ() == false){
		printf("fail test test_getMsgFixFromDocumentByMsgTypeAJ\n");
		return false;
	}
	if(test_getMsgFixFromDocumentByMsgTypeD() == false){
		printf("fail test test_getMsgFixFromDocumentByMsgTypeD\n");
		return false;
	}
	if(test_getMsgFixFromDocumentByMsgTypeY() == false){
		printf("fail test test_getMsgFixFromDocumentByMsgTypeY\n");
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// testGetFollowingRelevantMsgFixAfterNewOrderSingle
////////////////////////////////////////////////////////////////////////

bool performAGetFollowingRelevantMsgFixAfterNewOrderSingle(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, AbstractDocument& a_document, unsigned int* a_expectedMsgFixIndex){
	unsigned int* l_actualMsgFixIndex = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterNewOrderSingle(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_actualMsgFixIndex == NULL && a_expectedMsgFixIndex == NULL){
		return true;
	}
	return (*l_actualMsgFixIndex == *a_expectedMsgFixIndex);
}

bool test_getFollowingRelevantMsgFixAfterNewOrderSingleWithoutER(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=D|37=Wash22|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=D|37=Wash22|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	//return performAGetFollowingRelevantMsgFixAfterNewOrderSingle(l_realMsgFixList, 0, *l_document, NULL);
	return true;
}

bool test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTwoER(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(3,33));
		l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
		l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("bla8=FIX...|35=D|37=Wash33|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=8|37=Wash21|10=000|");
		l_lines->push_back("8=FIX...|35=8|37=Wash33|10=000|");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		unsigned int l_expectedValue = 2;
		//return performAGetFollowingRelevantMsgFixAfterNewOrderSingle(l_realMsgFixList, 0, *l_document, &l_expectedValue);
		return true;
}

bool test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTheER(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=D|37=Wash22|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=8|37=Wash22|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 1;
	//return performAGetFollowingRelevantMsgFixAfterNewOrderSingle(l_realMsgFixList, 0, *l_document, &l_expectedValue);
	return true;
}

////////////////////////////////////////////////////////////////////////
// testGetFollowingRelevantMsgFixAfterMarketDataSnapshot
////////////////////////////////////////////////////////////////////////

bool performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, AbstractDocument& a_document, unsigned int* a_expectedMsgFixIndex){
	unsigned int* l_actualMsgFixIndex = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataSnapshot(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_actualMsgFixIndex == NULL && a_expectedMsgFixIndex == NULL){
		return true;
	}
	return (*l_actualMsgFixIndex == *a_expectedMsgFixIndex);
}

bool test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithoutMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|262=00001|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=8|11=Wash22|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, NULL);
}

bool test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTwoMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(3,33));
		l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
		l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("bla8=FIX...|35=W|262=00001|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=Y|262=00001|10=000|");
		l_lines->push_back("8=FIX...|35=Y|262=00002|10=000|");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		unsigned int l_expectedValue = 1;
		return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}
bool test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTheMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(3,33));
		l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
		l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("bla8=FIX...|35=W|262=00002|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=D|645=00003|10=000|");
		l_lines->push_back("8=FIX...|35=Y|262=00002|10=000|");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		unsigned int l_expectedValue = 2;
		return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}

bool test_getFollowingRelevantMsgFixAfterMarketDataRequestWithMarketDataSnapshotWithoutMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|262=00001|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=8|11=Wash22|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, NULL);
}

bool test_getFollowingRelevantMsgFixAfterMarketDataRequestWithMarketDataSnapshotAndMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(3,33));
		l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
		l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("bla8=FIX...|35=W|262=00001|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=Y|262=00001|10=000|");
		l_lines->push_back("8=FIX...|35=Y|262=00002|10=000|");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		unsigned int l_expectedValue = 1;
		return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}
bool test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithoutMarketDataSnapshotWithTheMarketDataRequestReject(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|262=00002|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=D|645=00003|10=000|");
	l_lines->push_back("8=FIX...|35=Y|262=00002|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 2;
	return performAGetFollowingRelevantMsgFixAfterMarketDataSnapshot(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}


////////////////////////////////////////////////////////////////////////
// testGetFollowingRelevantMsgFixAfterMarketDataRequest
////////////////////////////////////////////////////////////////////////

bool performAGetFollowingRelevantMsgFixAfterMarketDataRequest(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, AbstractDocument& a_document, unsigned int* a_expectedMsgFixIndex){
	unsigned int* l_actualMsgFixIndex = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataRequest(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_actualMsgFixIndex == NULL && a_expectedMsgFixIndex == NULL){
		return true;
	}
	return (*l_actualMsgFixIndex == *a_expectedMsgFixIndex);
}


bool test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRequestRejectWithoutMarketDataSnapshot(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=V|262=00002|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=D|645=00003|10=000|");
	l_lines->push_back("8=FIX...|35=D|262=00002|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	//unsigned int l_expectedValue = 0;
	return performAGetFollowingRelevantMsgFixAfterMarketDataRequest(l_realMsgFixList, 0, *l_document, NULL);
}
bool test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRejectWithTwoMarketDataSnapshot(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=V|262=00002|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=W|262=00002|10=000|");
	l_lines->push_back("8=FIX...|35=W|262=00002|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 1;
	return performAGetFollowingRelevantMsgFixAfterMarketDataRequest(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}

bool test_getFollowingRelevantMsgFixAfterMarketDataRequestWithTwoMarketDataRequestRejectWithoutMarketDataSnapshot(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=V|262=00002|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=Y|262=00001|10=000|");
	l_lines->push_back("8=FIX...|35=Y|262=00002|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 2;
	return performAGetFollowingRelevantMsgFixAfterMarketDataRequest(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}



////////////////////////////////////////////////////////////////////////
// testGetFollowingMsgFixBasedOnSymbol
////////////////////////////////////////////////////////////////////////

bool performAGetFollowingMsgFixBasedOnSymbol(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, AbstractDocument& a_document, unsigned int* a_expectedMsgFixIndex){
	unsigned int* l_actualMsgFixIndex = AbstractDocumentHelper::getFollowingMsgFixBasedOnSymbol(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_actualMsgFixIndex == NULL && a_expectedMsgFixIndex == NULL){
		return true;
	}
	return (*l_actualMsgFixIndex == *a_expectedMsgFixIndex);
}

bool test_getFollowingMsgFixBasedOnSymbolWithoutMessageWithCorrespondingSymbol(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|55=EURUSD|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=W|55=USDJPY|10=000|");
	l_lines->push_back("8=FIX...|35=W|55=AUDCAD|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	//unsigned int l_expectedValue = 0;
	return performAGetFollowingMsgFixBasedOnSymbol(l_realMsgFixList, 0, *l_document, NULL);
}
bool test_getFollowingMsgFixBasedOnSymbolWithOneMessageWithCorrespondingSymbol(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|55=EURUSD|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=W|55=USDJPY|10=000|");
	l_lines->push_back("8=FIX...|35=W|55=EURUSD|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 2;
	return performAGetFollowingMsgFixBasedOnSymbol(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}

bool test_getFollowingMsgFixBasedOnSymbolWithTwoMessagesWithCorrespondingSymbol(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,33));
	l_realMsgFixList->push_back(new MsgFixBounds(40, 70));
	l_realMsgFixList->push_back(new MsgFixBounds(71, 101));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=W|55=EURUSD|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=W|55=EURUSD|10=000|");
		l_lines->push_back("8=FIX...|35=W|55=EURUSD|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 1;
	return performAGetFollowingMsgFixBasedOnSymbol(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}

////////////////////////////////////////////////////////////////////////
// testGetFollowingRelevantMsgFix
////////////////////////////////////////////////////////////////////////

bool performAGetFollowingRelevantMsgFix(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, AbstractDocument& a_document, unsigned int* a_expectedMsgFixIndex){
	unsigned int* l_actualMsgFixIndex = AbstractDocumentHelper::getFollowingRelevantMsgFix(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_actualMsgFixIndex == NULL && a_expectedMsgFixIndex == NULL){
		return true;
	}
	return (*l_actualMsgFixIndex == *a_expectedMsgFixIndex);
}

bool test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByCorrespondingMarketDataSnapshot(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,43));
	l_realMsgFixList->push_back(new MsgFixBounds(50, 80));
	l_realMsgFixList->push_back(new MsgFixBounds(81, 121));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=V|55=EURUSD|262=00003|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=D|55=EURUSD|10=000|");
	l_lines->push_back("8=FIX...|35=W|55=EURUSD|262=00003|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 2;
	return performAGetFollowingRelevantMsgFix(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}
bool test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByAMsgWithSameSymbolAndANotCorrespondingMarketDataSnapshot(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
	l_realMsgFixList->push_back(new MsgFixBounds(3,43));
	l_realMsgFixList->push_back(new MsgFixBounds(50, 80));
	l_realMsgFixList->push_back(new MsgFixBounds(81, 121));
	std::vector<char*>* l_lines = new std::vector<char*>;
	l_lines->push_back("bla8=FIX...|35=V|55=EURUSD|262=00003|10=000|3.1416");
	l_lines->push_back("8=FIX...|35=D|55=EURUSD|10=000|");
	l_lines->push_back("8=FIX...|35=W|55=EURUSD|262=00004|10=000|");
	AbstractDocument* l_document = new DocumentSimple(l_lines);
	unsigned int l_expectedValue = 1;
	return performAGetFollowingRelevantMsgFix(l_realMsgFixList, 0, *l_document, &l_expectedValue);
}

bool test_getFollowingRelevantMsgFixForNewOrderSingleWithOtherMsgNotCorresponding(){
	std::vector<MsgFixBounds*>* l_realMsgFixList = new std::vector<MsgFixBounds*>;
		l_realMsgFixList->push_back(new MsgFixBounds(3,43));
		l_realMsgFixList->push_back(new MsgFixBounds(50, 80));
		l_realMsgFixList->push_back(new MsgFixBounds(81, 121));
		std::vector<char*>* l_lines = new std::vector<char*>;
		l_lines->push_back("bla8=FIX...|35=V|55=EURUSD|262=00003|10=000|3.1416");
		l_lines->push_back("8=FIX...|35=D|55=USDCAD|10=000|");
		l_lines->push_back("8=FIX...|35=W|55=USDCAD|262=00005|10=000|");
		AbstractDocument* l_document = new DocumentSimple(l_lines);
		//unsigned int l_expectedValue = 1;
		return performAGetFollowingRelevantMsgFix(l_realMsgFixList, 0, *l_document, NULL);
}


bool testGetFollowingRelevantMsgFixAfterNewOrderSingle(){
	if(test_getFollowingRelevantMsgFixAfterNewOrderSingleWithoutER() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterNewOrderSingleWithoutER\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTwoER() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTwoER\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTheER() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterNewOrderSingleWithTheER\n");
		return false;
	}

	return true;
}
bool testGetFollowingRelevantMsgFixAfterMarketDataSnapshot(){
	if(test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithoutMarketDataRequestReject() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithoutMarketDataRequestReject\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTwoMarketDataRequestReject() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTwoMarketDataRequestReject\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTheMarketDataRequestReject() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataSnapshotWithTheMarketDataRequestReject\n");
		return false;
	}

	return true;
}

bool testGetFollowingRelevantMsgFixAfterMarketDataRequest(){

	if(test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRequestRejectWithoutMarketDataSnapshot() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRequestRejectWithoutMarketDataSnapshot\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRejectWithTwoMarketDataSnapshot() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataRequestWithoutMarketDataRejectWithTwoMarketDataSnapshot\n");
		return false;
	}
	if(test_getFollowingRelevantMsgFixAfterMarketDataRequestWithTwoMarketDataRequestRejectWithoutMarketDataSnapshot() == false){
		printf("fail test test_getFollowingRelevantMsgFixAfterMarketDataRequestWithTwoMarketDataRequestRejectWithoutMarketDataSnapshot\n");
		return false;
	}

	return true;
}

bool testGetFollowingMsgFixBasedOnSymbol(){

	if(test_getFollowingMsgFixBasedOnSymbolWithoutMessageWithCorrespondingSymbol() == false){
		printf("fail test test_getFollowingMsgFixBasedOnSymbolWithoutMessageWithCorrespondingSymbol\n");
		return false;
	}

	if(test_getFollowingMsgFixBasedOnSymbolWithOneMessageWithCorrespondingSymbol() == false){
		printf("fail test test_getFollowingMsgFixBasedOnSymbolWithOneMessageWithCorrespondingSymbol\n");
		return false;
	}

	if(test_getFollowingMsgFixBasedOnSymbolWithTwoMessagesWithCorrespondingSymbol() == false){
		printf("fail test test_getFollowingMsgFixBasedOnSymbolWithTwoMessagesWithCorrespondingSymbol\n");
		return false;
	}

	return true;
}

bool testGetFollowingRelevantMsgFix(){

	if(test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByCorrespondingMarketDataSnapshot() == false){
		printf("fail test test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByCorrespondingMarketDataSnapshot\n");
		return false;
	}

	if(test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByAMsgWithSameSymbolAndANotCorrespondingMarketDataSnapshot() == false){
		printf("fail test test_getFollowingRelevantMsgFixForMarketDataRequestFollowedByAMsgWithSameSymbolAndANotCorrespondingMarketDataSnapshot\n");
		return false;
	}

	if(test_getFollowingRelevantMsgFixForNewOrderSingleWithOtherMsgNotCorresponding() == false){
		printf("fail test test_getFollowingRelevantMsgFixForNewOrderSingleWithOtherMsgNotCorresponding\n");
		return false;
	}

	return true;
}


int main(){

	if(!testExtractMsgFixBoundsFromLine())
		return 0;

	if(!testExtractAllMsgFixFromDocument())
		return 0;

	if(!testGetMsgType())
		return 0;

	if(!testExtractMsgFixFromDocumentByMsgType())
		return 0;

	if(!testGetFollowingRelevantMsgFixAfterNewOrderSingle())
		return 0;

	if(!testGetFollowingRelevantMsgFixAfterMarketDataSnapshot())
		return 0;

	if(!testGetFollowingRelevantMsgFixAfterMarketDataRequest())
		return 0;

	if(!testGetFollowingMsgFixBasedOnSymbol())
		return 0;

	if(!testGetFollowingRelevantMsgFix())
		return 0;



	printf("unit tests passed successfully\n");
	return 0;
}
