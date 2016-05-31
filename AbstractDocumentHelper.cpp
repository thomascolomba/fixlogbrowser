#include <string.h>
#include <stdio.h>
#include "MsgFixBounds.h"
#include "AbstractDocument.h"
#include "AbstractDocumentHelper.h"

MsgFixBounds* AbstractDocumentHelper::extractMsgFixBoundsFromLine(char* a_line, long a_positionofLineBeginning){
	//printf("a_line = %s\n",a_line);
	if(a_line == NULL){
		return NULL;
	}
	//looking for fix header (only the first for now)
	char* l_fixHeaderPosition = strstr(a_line, MsgFixBounds::g_fixHeader);
	if(l_fixHeaderPosition == NULL){
		return NULL;
	}
	bool l_isMsgFixFound = false;
	long l_fixHeaderIndex = l_fixHeaderPosition - a_line;
	//printf("l_fixHeaderIndex = %li\n",l_fixHeaderIndex);
	long l_lineLength = strlen(a_line);
	char* l_lastCharacterOfLinePosition = a_line+l_lineLength;
	char* b_startPositionField = l_fixHeaderPosition;
	char* l_lastMsgFixCharacterPosition = NULL;
	while(b_startPositionField < l_lastCharacterOfLinePosition){
		//printf("b_startPositionField < l_lastCharacterOfLinePosition : %i < %i\n", b_startPositionField, l_lastCharacterOfLinePosition);
	
		//searching key
		char* b_currentFixKeyValueSeparator = strchr(b_startPositionField, MsgFixBounds::g_fixKeyValueSeparator);// '='
		if(b_currentFixKeyValueSeparator == NULL){
			return NULL;
		}
		long l_keyLength = (long)(b_currentFixKeyValueSeparator - b_startPositionField);
		//printf("l_keyLength =  %i\n", l_keyLength);
		char* l_key = new char[l_keyLength+1];
		strncpy(l_key, b_startPositionField, l_keyLength);
		if(l_key == NULL){
			return NULL;
		}
		l_key[l_keyLength] = '\0';

		if(strstr(l_key, &MsgFixBounds::g_fixFieldSeparator) != NULL){//if key contains | then a previous = is missing
			return NULL;
		}
		//printf("l_key = %s, keyLength = %i\n",l_key, l_keyLength);
		//searching value
		char* b_currentFixFieldSeparator = strchr(b_currentFixKeyValueSeparator+1, MsgFixBounds::g_fixFieldSeparator);
		if(b_currentFixFieldSeparator == NULL){
			return NULL;
		}
		long l_valueLength = (long)(b_currentFixFieldSeparator - b_currentFixKeyValueSeparator - 1);
		char* l_value = new char[l_valueLength+1];
		strncpy(l_value, b_currentFixKeyValueSeparator+1, l_valueLength);
		if(l_value == NULL){
			return NULL;
		}
		l_value[l_valueLength] = '\0';
		//printf("l_value = %s\n",l_value);
		
		b_startPositionField = b_currentFixFieldSeparator + 1;

		if(strcmp(l_key,MsgFixBounds::g_tagChecksum) == 0){
			l_isMsgFixFound = true;
			l_lastMsgFixCharacterPosition = b_currentFixFieldSeparator;
			break;
		}
		

	}
	if(l_isMsgFixFound){
		MsgFixBounds* msgFixFound = new MsgFixBounds(l_fixHeaderIndex + a_positionofLineBeginning, (l_lastMsgFixCharacterPosition-a_line) + a_positionofLineBeginning);
		return msgFixFound;
	}
	return NULL;
}

std::vector<MsgFixBounds*>* AbstractDocumentHelper::extractAllMsgFixFromDocument(const AbstractDocument& a_abstractDocument){//Fix messages should be returned in the same order they appear in document
	std::vector<MsgFixBounds*>* l_fixMessages = new std::vector<MsgFixBounds*>;
	for(long b_indexLine = 0; b_indexLine < a_abstractDocument.getNbLines(); b_indexLine++){
		MsgFixBounds* b_msg = a_abstractDocument.extractMsgFixFromLine(b_indexLine);
		if(b_msg != NULL){
			l_fixMessages->push_back(b_msg);
		}
	}
	return l_fixMessages;
}

std::vector<MsgFixBounds*>* AbstractDocumentHelper::extractMsgFixFromDocumentByMsgType(const AbstractDocument& a_abstractDocument, char* a_msgType){
	std::vector<MsgFixBounds*>* l_allMsgFix = AbstractDocumentHelper::extractAllMsgFixFromDocument(a_abstractDocument);
	if(l_allMsgFix == NULL){
		return NULL;
	}
	std::vector<MsgFixBounds*>* l_result = new std::vector<MsgFixBounds*>;
	for(std::vector<MsgFixBounds*>::iterator b_msgFixIter =  l_allMsgFix->begin(); b_msgFixIter != l_allMsgFix->end(); ++b_msgFixIter){//this iteration don't work
		char* b_msgType = AbstractDocumentHelper::getMsgType(*b_msgFixIter, a_abstractDocument);
		if(b_msgType != NULL && (strcmp(b_msgType, a_msgType) == 0)){
			l_result->push_back(*b_msgFixIter);
		}
	}
	return l_result;
}

char* AbstractDocumentHelper::getMsgType(MsgFixBounds* a_msgFix, const AbstractDocument& a_document){
	return AbstractDocumentHelper::getFieldValue(a_msgFix, a_document, MsgFixBounds::msgType_key);
}

char* AbstractDocumentHelper::getFieldValue(MsgFixBounds* a_msgFix, const AbstractDocument& a_document, char* a_fieldId){
	char* l_msgFixContent = a_document.getMsgFixContent(a_msgFix);
		if(l_msgFixContent == NULL){
			return NULL;
		}
		char l_separatorAndMsgType[4];
		strcpy(l_separatorAndMsgType, &MsgFixBounds::g_fixFieldSeparator);
		strcat(l_separatorAndMsgType, a_fieldId);
		l_separatorAndMsgType[3] = '\0';
		char* l_endOfFieldBeforeMsgType = strstr(l_msgFixContent, l_separatorAndMsgType);

		if(l_endOfFieldBeforeMsgType == NULL){
			return NULL;
		}
		//now we look for the next separator
		char* l_characterAfterMsgType = strchr(l_endOfFieldBeforeMsgType+3, MsgFixBounds::g_fixFieldSeparator);
		if(l_characterAfterMsgType == NULL){
			return NULL;
		}
		long l_length = ((long)l_characterAfterMsgType - (long)l_endOfFieldBeforeMsgType)-4;
		char* l_msgType = new char[l_length+1];
		strncpy(l_msgType, l_endOfFieldBeforeMsgType+4, l_length);
		l_msgType[l_length] = '\0';
		return l_msgType;
}

unsigned long AbstractDocumentHelper::getTotalLength(std::vector<MsgFixBounds*>* a_msgFixList){
	unsigned long l_totalSize = 0;
	if(a_msgFixList != NULL && a_msgFixList->size() != 0){
		for(unsigned int b_indexMsgFixList = 0; b_indexMsgFixList < a_msgFixList->size(); b_indexMsgFixList++){
			l_totalSize = l_totalSize + a_msgFixList->at(0)->getLength();
		}
	}
	return l_totalSize;
}

bool AbstractDocumentHelper::isMarketDataRequestAnUnsubscription(MsgFixBounds* a_marketDataRequestFix, const AbstractDocument& a_document){
	char* l_fieldValue = getFieldValue(a_marketDataRequestFix, a_document, MsgFixBounds::subscriptionRequestType_key);
	if(l_fieldValue == NULL){
		return false;
	}
	return (strcmp(l_fieldValue, "2") == 0);
}


/*std::vector<char*>* AbstractDocumentHelper::getMsgTypesOfFollowingMsgTypeInWorkflow(char* a_msgType){
	std::vector<char*>* l_msgTypes = new std::vector<char*>;
	if(strcmp(a_msgType, MsgFixBounds::marketDataRequest_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::cancelMarketDataRequest_msgType);//unsubscribing
		l_msgTypes.push_back(MsgFixBounds::marketDataSnapshot_msgType);//prices sent
		l_msgTypes.push_back(MsgFixBounds::marketDataRequestReject_msgType);//request rejected
	} else if(strcmp(a_msgType, MsgFixBounds::cancelMarketDataRequest_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::marketDataRequest_msgType);//resubscribing
		l_msgTypes.push_back(MsgFixBounds::marketDataRequestReject_msgType);//confirming cancel
	} else if(strcmp(a_msgType, MsgFixBounds::marketDataRequestReject_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::cancelMarketDataRequest_msgType);//confirming reject
		l_msgTypes.push_back(MsgFixBounds::marketDataRequest_msgType);//subscribing again
	}else if(strcmp(a_msgType, MsgFixBounds::marketDataSnapshot_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::cancelMarketDataRequest_msgType);//unsubscribing
		l_msgTypes.push_back(MsgFixBounds::marketDataRequest_msgType);//subscribing
		l_msgTypes.push_back(MsgFixBounds::marketDataResponse_msgType);//response to trade for example
		l_msgTypes.push_back(MsgFixBounds::newOrderSingle_msgType);//a new order
	}else if(strcmp(a_msgType, MsgFixBounds::marketDataResponse_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::newOrderSingle_msgType);//a new order
	}else if(strcmp(a_msgType, MsgFixBounds::newOrderSingle_msgType) == 0){
		l_msgTypes.push_back(MsgFixBounds::executionReport_msgType);//a report
	}else {
		//nothing
	}
	return l_msgTypes;
	return NULL;
}*/

//returns the following relevant message based on message type if it exists, else it returns the following message refering to the same symbol if it exists, else it returns NULL
unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFix(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	char* l_currentMsgType = AbstractDocumentHelper::getMsgType(a_msgFixList->at(a_currentMsgFixIndex), a_document);
	unsigned int* l_nextRelevantMsgFix = NULL;
	if(l_currentMsgType != NULL){//search next relevant message based on message type
		if(strcmp(l_currentMsgType, MsgFixBounds::marketDataRequest_msgType) == 0){//market data request
			printf("CASE market data request\n");
			l_nextRelevantMsgFix = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataRequest(a_msgFixList, a_currentMsgFixIndex, a_document);
		} else if(strcmp(l_currentMsgType, MsgFixBounds::marketDataRequestReject_msgType) == 0){
			l_nextRelevantMsgFix = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataRequestReject(a_msgFixList, a_currentMsgFixIndex, a_document);
		} else if(strcmp(l_currentMsgType, MsgFixBounds::marketDataFullSnapshot_msgType) == 0){
			l_nextRelevantMsgFix = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataSnapshot(a_msgFixList, a_currentMsgFixIndex, a_document);
		} else if(strcmp(l_currentMsgType, MsgFixBounds::newOrderSingle_msgType) == 0){
			l_nextRelevantMsgFix = AbstractDocumentHelper::getFollowingRelevantMsgFixAfterNewOrderSingle(a_msgFixList, a_currentMsgFixIndex, a_document);
		}
	}
	if(l_nextRelevantMsgFix != NULL){
		return l_nextRelevantMsgFix;
	}
	l_nextRelevantMsgFix = AbstractDocumentHelper::getFollowingMsgFixBasedOnSymbol(a_msgFixList, a_currentMsgFixIndex, a_document);
	if(l_nextRelevantMsgFix != NULL){
		return l_nextRelevantMsgFix;
	}
	return NULL;
}

unsigned int* AbstractDocumentHelper::getFollowingMsgFixBasedOnSymbol(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	char* l_symbol = getFieldValue(a_msgFixList->at(a_currentMsgFixIndex), a_document, MsgFixBounds::symbol_key);
	if(l_symbol == NULL){
		return NULL;
	}
	char* b_symbol = NULL;
	for(unsigned int b_msgFixIndex = a_currentMsgFixIndex+1; b_msgFixIndex < a_msgFixList->size(); b_msgFixIndex++){
		b_symbol = getFieldValue(a_msgFixList->at(b_msgFixIndex), a_document, MsgFixBounds::symbol_key);
		if(b_symbol != NULL && strcmp(l_symbol, b_symbol) == 0){
			return new unsigned int(b_msgFixIndex);
		}
	}
	return NULL;
}

//we iterate on following messages
//will stop if corresponding mdrr is encountered, else returns the last corresponding market data snapshot
unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataRequest(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	unsigned int l_tmpLastRelevantMDSIndex = 0;
	bool isARelevantMDSFound = false;
	char* l_mdReqIdOfMDR = AbstractDocumentHelper::getFieldValue(a_msgFixList->at(a_currentMsgFixIndex), a_document, MsgFixBounds::MDReqID_key);
	for(unsigned int b_msgFixIndex = a_currentMsgFixIndex+1; b_msgFixIndex < a_msgFixList->size(); b_msgFixIndex++){
		MsgFixBounds* b_msgFix = a_msgFixList->at(b_msgFixIndex);
		char* b_msgType = AbstractDocumentHelper::getMsgType(b_msgFix, a_document);
		if(b_msgType == NULL){
			continue;
		}
		if(strcmp(b_msgType, MsgFixBounds::marketDataRequestReject_msgType) == 0 && strcmp(AbstractDocumentHelper::getFieldValue(a_msgFixList->at(b_msgFixIndex), a_document, MsgFixBounds::MDReqID_key), l_mdReqIdOfMDR) == 0){
			//case mdrr
			return new unsigned int(b_msgFixIndex);
		} else if(strcmp(b_msgType, MsgFixBounds::marketDataFullSnapshot_msgType) == 0 && strcmp(AbstractDocumentHelper::getFieldValue(a_msgFixList->at(b_msgFixIndex), a_document, MsgFixBounds::MDReqID_key), l_mdReqIdOfMDR) == 0 && isARelevantMDSFound == false) {
			//case mds
			l_tmpLastRelevantMDSIndex = b_msgFixIndex;
			isARelevantMDSFound = true;
		}
	}
	if(isARelevantMDSFound){
		return new unsigned int(l_tmpLastRelevantMDSIndex);
	}

	return NULL;
	//isMarketDataRequestAnUnsubscription
	//todo write getNextMsgFix

}

unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFixAfterCancelMarketDataRequest(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	return NULL;
}

//will always return NULL, no fix message is relevant after a market data request reject message.
unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataRequestReject(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	return NULL;
}

//will return corresponding market data reject request if it exists
unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFixAfterMarketDataSnapshot(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	char* l_origMDReqID = AbstractDocumentHelper::getFieldValue(a_msgFixList->at(a_currentMsgFixIndex), a_document, MsgFixBounds::MDReqID_key);
	if(l_origMDReqID == NULL){
		return NULL;
	}
	for(unsigned int b_msgFixIndex = a_currentMsgFixIndex+1; b_msgFixIndex < a_msgFixList->size(); b_msgFixIndex++){
		MsgFixBounds* b_msgFix = a_msgFixList->at(b_msgFixIndex);
		char* b_msgType = AbstractDocumentHelper::getMsgType(b_msgFix, a_document);
		if(b_msgType == NULL){
			continue;
		}
		if((strcmp(b_msgType, MsgFixBounds::marketDataRequestReject_msgType) == 0) && (strcmp(l_origMDReqID, AbstractDocumentHelper::getFieldValue(a_msgFixList->at(b_msgFixIndex), a_document, MsgFixBounds::MDReqID_key)) == 0)){
			return new unsigned int(b_msgFixIndex);
		}
	}
	return NULL;
}

//will return the corresponding execution report if it exists
unsigned int* AbstractDocumentHelper::getFollowingRelevantMsgFixAfterNewOrderSingle(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document){
	char* l_ordId = AbstractDocumentHelper::getFieldValue(a_msgFixList->at(a_currentMsgFixIndex), a_document, MsgFixBounds::OrderID_key);
	for(unsigned int b_msgFixIndex = a_currentMsgFixIndex+1; b_msgFixIndex < a_msgFixList->size(); b_msgFixIndex++){
		MsgFixBounds* b_msgFix = a_msgFixList->at(b_msgFixIndex);
		char* b_msgType = AbstractDocumentHelper::getMsgType(b_msgFix, a_document);
		if(b_msgType == NULL){
			continue;
		}
		if((strcmp(b_msgType, MsgFixBounds::executionReport_msgType) == 0) && (strcmp(l_ordId, AbstractDocumentHelper::getFieldValue(a_msgFixList->at(b_msgFixIndex), a_document, MsgFixBounds::clOrdID_key)) == 0)){
			return new unsigned int(b_msgFixIndex);
		}
	}
	return NULL;
}


