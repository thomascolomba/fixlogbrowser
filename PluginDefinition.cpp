//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>
#include <vector>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "PluginInterface.h"
#include "MsgFixBounds.h"
#include "AbstractDocument.h"
#include "NppDocument.h"
#include "AbstractDocumentHelper.h"

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[12];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
printf("plugin being loaded...");
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//print fix messages into a new document, if there is no messages, a messagebox print an error message instead
void openNewFile(std::vector<MsgFixBounds*>* a_lines, AbstractDocument* a_document, char* a_msgType){
	if(a_lines == NULL || a_lines->size() == 0){
		::MessageBox(NULL, TEXT("There is no such fix messages to print"), TEXT("No fix message to print"), MB_OK);
	} else {
		unsigned long l_textLength = AbstractDocumentHelper::getTotalLength(a_lines) + a_lines->size() + 1;//+a_lines->size() to get memory for each end of line, +1 for \0
		char* l_textToPrint = new char[l_textLength];
		strcpy(l_textToPrint, "");
		for(unsigned int b_indexFixMsgList = 0; b_indexFixMsgList < a_lines->size(); b_indexFixMsgList++){
			char* b_fixMsgContent = a_document->getMsgFixContent(a_lines->at(b_indexFixMsgList));
			strcat(l_textToPrint, b_fixMsgContent);
			strcat(l_textToPrint, "\n");
		}
		::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

		// Get the current scintilla
		int which = -1;
		::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
		if (which == -1)
			return;
		HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;


		// Scintilla control has no Unicode mode, so we use (char *) here
		::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)l_textToPrint);

		delete l_textToPrint;
	}
}

/*void deplaceCursorAtLine(int a_columnNumber, int a_lineNumber){
	::SendMessage(nppData._scintillaMainHandle, SCI_GOTOLINE, a_lineNumber, 0);
}*/

//select text and will scroll to it if needed
void highlightText(int a_startPosition, int a_endPosition){
	::SendMessage(nppData._scintillaMainHandle, SCI_SETSEL, a_startPosition, a_endPosition);
}

void highlightMsgFix(MsgFixBounds* a_msgFixBounds){
	highlightText(a_msgFixBounds->getStartPosition(), a_msgFixBounds->getEndPosition()+1);
}

void highlighting(){
	highlightText(140, 145);
}

void extractMessage(char* a_msgType)
{
	//if(a_msgType == NULL || a_msgType == ""){
	//	return;//todo print message box
	//}

	int cursorPosition = ::SendMessage(nppData._scintillaMainHandle, SCI_GETCURRENTPOS, 0, 0);
	int currentLine = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);// Line position of the cursor in the editor
	int currentColumn = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTCOLUMN, 0, 0);// Column position of the cursor in the editor
	//printf("pos = %i, line = %i, col=%i\n", cursorPosition, currentLine, currentColumn);

	AbstractDocument* l_document = new NppDocument(nppData);

	std::vector<MsgFixBounds*>* l_fixMsgList = AbstractDocumentHelper::extractMsgFixFromDocumentByMsgType(*l_document, a_msgType);
	if(l_fixMsgList == NULL){
		printf("null\n");
		return;
	}
	//MsgFixBounds::printToConsole(l_fixMsgList);
	//for(unsigned long b_indexMsgFixList = 0; b_indexMsgFixList < l_fixMsgList->size(); b_indexMsgFixList++){
	//	printf("msg : %s\n",l_document->getMsgFixContent(l_fixMsgList->at(b_indexMsgFixList)));
	//}

	openNewFile(l_fixMsgList, l_document, a_msgType);

////

    // Open a new document
    /*::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");*/
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void extractMDRAndCMDR(){
	extractMessage("V");
}

void extractMDSFR(){
	extractMessage("W");
}

void extractMDRR(){
	extractMessage("Y");
}

void extractQR(){
	extractMessage("R");
}

void extractS(){
	extractMessage("S");
}

void extractAJ(){
	extractMessage("AJ");
}

void extractD(){
	extractMessage("D");
}

void extract8(){
	extractMessage("8");
}

void extractQ(){
	extractMessage("Q");
}

void extractOT(){
	extractMessage("OT");
}

void goToFollowingRelevantMessage(){
	//1)compute fix message List
	AbstractDocument* l_document = new NppDocument(nppData);
	std::vector<MsgFixBounds*>* l_msgFixList = AbstractDocumentHelper::extractAllMsgFixFromDocument(*l_document);
	if(l_msgFixList == NULL){
		printf("No fix msg on this document");
		return;
	}
	//2)is there a fix msg on the current line ?
	int currentLine = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);// Line position of the cursor in the editor
	long startPositionOfTheCurrentLine = l_document->getStartPositionOfLine(currentLine);
	//printf("---currentLine = %i\n", currentLine);
	//printf("---startPositionOfTheCurrentLine = %i\n", startPositionOfTheCurrentLine);
	char* l_lineContent = l_document->getLine(currentLine);
	//printf("---l_lineContent = %s\n", l_lineContent);
	//MsgFixBounds* l_currentMsgFix = AbstractDocumentHelper::extractMsgFixBoundsFromLine(l_lineContent, (long)currentLine);//(long)startPositionOfTheCurrentLine);
	MsgFixBounds* l_currentMsgFix = AbstractDocumentHelper::extractMsgFixBoundsFromLine(l_lineContent, (long)startPositionOfTheCurrentLine);
	if(l_currentMsgFix == NULL){
		printf("No fix message on the current line");
		return;
	}
	MsgFixBounds::printToConsole(l_currentMsgFix);
	//3) search index of current fix message among fix messages list
	unsigned int l_indexOfCurrentMsg = 0;
	for(unsigned int b_indexFixMsg = 0; b_indexFixMsg < l_msgFixList->size(); b_indexFixMsg++){
		if((*l_currentMsgFix)==*(l_msgFixList->at(b_indexFixMsg))){
			l_indexOfCurrentMsg = b_indexFixMsg;
			//printf("corresponding msg found : %s\n", l_document->getMsgFixContent(l_msgFixList->at(b_indexFixMsg)));
		}
	}
	//4)search following relevant msg
	unsigned int*l_indexFollowingRelevantMsg = AbstractDocumentHelper::getFollowingRelevantMsgFix(l_msgFixList, l_indexOfCurrentMsg, *l_document);
	if(l_indexFollowingRelevantMsg == NULL){
		printf("No fix msg on this document");
		return;
	}
	//printf("l_indexFollowingRelevantMsg = %i\n",*l_indexFollowingRelevantMsg);
	printf("following relevant msg : %s\n",l_document->getMsgFixContent(l_msgFixList->at(*l_indexFollowingRelevantMsg)));
	//highlighting next message
	highlightMsgFix(l_msgFixList->at(*l_indexFollowingRelevantMsg));
	printf("end of highlighting\n");

}

/*void goToLine(){
	deplaceCursorAtLine(60);
}*/


//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    printf("TCO on initialise les commandes");

    ShortcutKey* shortcutForFeat1 = new ShortcutKey;
    shortcutForFeat1->_isAlt = true;
    shortcutForFeat1->_isCtrl = false;
    shortcutForFeat1->_isShift = false;
    shortcutForFeat1->_key = 0x56;//V
    setCommand(0, TEXT("Extract MDR and CMDR"), extractMDRAndCMDR, shortcutForFeat1, false);

    ShortcutKey* shortcutForFeat2 = new ShortcutKey;
    shortcutForFeat2->_isAlt = true;
    shortcutForFeat2->_isCtrl = false;
    shortcutForFeat2->_isShift = false;
    shortcutForFeat2->_key = 0x57;//W
    setCommand(1, TEXT("Extract MDSFR"), extractMDSFR, shortcutForFeat2, false);

    ShortcutKey* shortcutForFeat3 = new ShortcutKey;
    shortcutForFeat3->_isAlt = true;
    shortcutForFeat3->_isCtrl = false;
    shortcutForFeat3->_isShift = false;
    shortcutForFeat3->_key = 0x59;//Y
	setCommand(2, TEXT("Extract MDRRR"), extractMDRR, shortcutForFeat3, false);

	ShortcutKey* shortcutForFeat4 = new ShortcutKey;
	shortcutForFeat4->_isAlt = true;
	shortcutForFeat4->_isCtrl = false;
	shortcutForFeat4->_isShift = false;
	shortcutForFeat4->_key = 0x52;//R
	setCommand(3, TEXT("Extract QR"), extractQR, shortcutForFeat4, false);

	ShortcutKey* shortcutForFeat5 = new ShortcutKey;
	shortcutForFeat5->_isAlt = true;
	shortcutForFeat5->_isCtrl = false;
	shortcutForFeat5->_isShift = false;
	shortcutForFeat5->_key = 0x53;//S
	setCommand(4, TEXT("Extract S"), extractS, shortcutForFeat5, false);

	setCommand(5, TEXT("Extract AJ"), extractAJ, NULL, false);

	ShortcutKey* shortcutForFeat6 = new ShortcutKey;
	shortcutForFeat6->_isAlt = true;
	shortcutForFeat6->_isCtrl = false;
	shortcutForFeat6->_isShift = false;
	shortcutForFeat6->_key = 0x44;//D
	setCommand(6, TEXT("Extract D"), extractD, shortcutForFeat6, false);

	ShortcutKey* shortcutForFeat7 = new ShortcutKey;
	shortcutForFeat7->_isAlt = true;
	shortcutForFeat7->_isCtrl = false;
	shortcutForFeat7->_isShift = false;
	shortcutForFeat7->_key = 0x38;//8
	setCommand(7, TEXT("Extract 8"), extract8, shortcutForFeat7, false);

	ShortcutKey* shortcutForFeat8 = new ShortcutKey;
	shortcutForFeat8->_isAlt = true;
	shortcutForFeat8->_isCtrl = false;
	shortcutForFeat8->_isShift = false;
	shortcutForFeat8->_key = 0x51;//Q
	setCommand(8, TEXT("Extract Q"), extractQ, shortcutForFeat8, false);

	setCommand(9, TEXT("Extract OT"), extractOT, NULL, false);

	/*ShortcutKey* shortcutForFeat9 = new ShortcutKey;
	shortcutForFeat9->_isAlt = true;
	shortcutForFeat9->_isCtrl = false;
	shortcutForFeat9->_isShift = false;
	shortcutForFeat9->_key = 0x51;//Q*/
	//setCommand(10, TEXT("highlighting"), highlighting, NULL, false);

	ShortcutKey* shortcutForFeat11 = new ShortcutKey;
	shortcutForFeat11->_isAlt = true;
	shortcutForFeat11->_isCtrl = false;
	shortcutForFeat11->_isShift = false;
	shortcutForFeat11->_key = 0x4E;//N
	setCommand(10, TEXT("Go to following relevant message"), goToFollowingRelevantMessage, shortcutForFeat11, false);
    
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
	delete funcItem[0]._pShKey;
	delete funcItem[1]._pShKey;
	delete funcItem[2]._pShKey;
	delete funcItem[3]._pShKey;
	delete funcItem[4]._pShKey;
	delete funcItem[5]._pShKey;
	delete funcItem[6]._pShKey;
	delete funcItem[7]._pShKey;
	delete funcItem[8]._pShKey;
	delete funcItem[9]._pShKey;
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}


void helloDlg()
{
    ::MessageBox(NULL, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}
