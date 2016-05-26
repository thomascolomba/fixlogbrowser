#variables to custom for each developer
NOTEPAD_EXECUTABLE=C:\Users\thomas\Desktop\notepad++\notepad++.exe
PATH_TO_NOTEPAD_PLUGINS_DIRECTORY=C:\Users\thomas\Desktop\notepad++\plugins\

CXX=mingw32-c++.exe
CXXFLAGS=-W -Wall -Wno-write-strings -DUNICODE
LDFLAGS=-shared -Wl,--out-implib,my_plugin_dll.a
LDLIBS=
DLL=my_plugin.dll
  
all:deploy

my_plugin.dll: NppPluginDemo.o PluginDefinition.o MsgFixBounds.o NppDocument.o AbstractDocumentHelper.o
	$(CXX)  -o $@ $^ $(LDFLAGS)

NppPluginDemo.o: NppPluginDemo.cpp PluginDefinition.h
	$(CXX) -o $@ -c NppPluginDemo.cpp $(CXXFLAGS)
	
PluginDefinition.o: PluginDefinition.cpp PluginDefinition.h menuCmdID.h AbstractDocumentHelper.h
	$(CXX) -o $@ -c PluginDefinition.cpp $(CXXFLAGS)
	
MsgFixBounds.o: MsgFixBounds.cpp MsgFixBounds.h
	$(CXX) -o $@ -c MsgFixBounds.cpp $(CXXFLAGS)

AbstractDocumentHelper.o: AbstractDocumentHelper.cpp AbstractDocumentHelper.h AbstractDocument.h DocumentSimple.h DocumentSimple.cpp
	$(CXX) -o $@ -c AbstractDocumentHelper.cpp $(CXXFLAGS)

DocumentSimple.o: DocumentSimple.cpp DocumentSimple.h AbstractDocument.h
	$(CXX) -o $@ -c DocumentSimple.cpp $(CXXFLAGS)

.PHONY: clean mrproper deploy execute unit_test.exe

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(DLL) my_plugin_dll.a

deploy:$(DLL)
	cp $(DLL) $(PATH_TO_NOTEPAD_PLUGINS_DIRECTORY)

execute:
	$(NOTEPAD_EXECUTABLE)

unit_test.exe: TestDocument.o MsgFixBounds.o AbstractDocumentHelper.o DocumentSimple.o
	$(CXX)  -o $@ TestDocument.o MsgFixBounds.o AbstractDocumentHelper.o DocumentSimple.o
	chmod 711 $@
	$@
	rm $@

TestDocument.o: TestDocument.cpp
	$(CXX)  -o TestDocument.o -c TestDocument.cpp $(CXXFLAGS)









