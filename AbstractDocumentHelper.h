#ifndef ABSTRACT_DOCUMENT_HELPER_H
#define ABSTRACT_DOCUMENT_HELPER_H

#include <vector>
#include "MsgFixBounds.h"
#include "AbstractDocument.h"

class AbstractDocumentHelper {
public:
	static MsgFixBounds* extractMsgFixBoundsFromLine(char* a_line, long a_positionofLineBeginning);
	static std::vector<MsgFixBounds*>* extractAllMsgFixFromDocument(const AbstractDocument& a_abstractDocument);
	static std::vector<MsgFixBounds*>* extractMsgFixFromDocumentByMsgType(const AbstractDocument& a_abstractDocument, char* a_msgType);
	static char* getMsgType(MsgFixBounds* a_msgFix, const AbstractDocument& a_abstractDocument);
	static char* getFieldValue(MsgFixBounds* a_msgFix, const AbstractDocument& a_abstractDocument, char* a_fieldId);
	static unsigned long getTotalLength(std::vector<MsgFixBounds*>* a_msgFixList);
	static std::vector<char*>* getMsgTypesOfFollowingMsgTypeInWorkflow(char* a_msgType);
	static bool isMarketDataRequestAnUnsubscription(MsgFixBounds* a_marketDataRequestFix, const AbstractDocument& a_document);
	static unsigned int* getFollowingRelevantMsgFix(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message according to the user, if such message exists, NULL otherwise
	//todo thomas static MsgFixBounds* getPreviousRelevantMsgFix(MsgFixBounds* a_currentMsgFix, const AbstractDocument& a_document);//returns the previous relevant fix message according to the user, if such message exists, NULL otherwise
	static unsigned int* getFollowingRelevantMsgFixAfterMarketDataRequest(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message after the current market data request according to the user, if such message exists, NULL otherwise
	static unsigned int* getFollowingRelevantMsgFixAfterCancelMarketDataRequest(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message after the current market data request according to the user, if such message exists, NULL otherwise
	static unsigned int* getFollowingRelevantMsgFixAfterMarketDataRequestReject(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message after the current market data request according to the user, if such message exists, NULL otherwise
	static unsigned int* getFollowingRelevantMsgFixAfterMarketDataSnapshot(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message after the current market data request according to the user, if such message exists, NULL otherwise
	static unsigned int* getFollowingRelevantMsgFixAfterNewOrderSingle(std::vector<MsgFixBounds*>* a_msgFixList, int a_currentMsgFixIndex, const AbstractDocument& a_document);//returns the following relevant fix message after the current market data request according to the user, if such message exists, NULL otherwise

};

#endif
