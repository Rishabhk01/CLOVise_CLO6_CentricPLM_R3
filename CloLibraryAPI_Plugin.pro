message("CloLibraryAPI_Plugin.pro build start")

build-debug {
	BUILD_DIR		= $$PWD/mac_release/CloLibraryAPI_Plugin/build
	BUILD_HOME_PATH = $$PWD
	MAC_RELEASE_PATH = $$PWD/mac_release
}

build-release {
	BUILD_DIR		= build
	BUILD_HOME_PATH = ../..
	MAC_RELEASE_PATH = ..
}

TARGET		= CloLibraryAPI_Plugin
TEMPLATE		= lib
DESTDIR		= $$BUILD_DIR
OBJECTS_DIR	= $$BUILD_DIR/object
MOC_DIR		= $$BUILD_DIR/moc
UI_DIR		= $$BUILD_DIR/ui
RCC_DIR		= $$BUILD_DIR/rcc

CONFIG		+= qt opengl cocoa x86_64 framework thread
CONFIG 		+= silent

DEFINES		+= CLO_API_INTERFACE_STATIC_BUILD

INCLUDEPATH += . \
	CLOAPIInterface \
	CLOAPIInterface/Lib \
	CLOAPIInterface/Samples/LibraryWindowImplementation \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/curl_Mac/include \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/include \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/zipper \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/minizip
 
	
LIBS += ../../CLOAPIInterface/Lib/libCLOAPIInterface.dylib \
    ../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/curl_Mac/lib/libcurl.4.dylib \
    ../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/lib/libz.1.2.11.dylib \
    ../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/lib/libminizip.1.dylib \
    ../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/lib/libZipper.1.0.1.dylib 


FORMS += CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISearchDialogSample.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISignInDialogSample.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/PLMSettingsSample.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Authentication/CLOPLMSignIn.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/CLOViseSuite/CLOViseSuite.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorResults.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialResult.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentResults.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductResults.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleResult.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintSearch.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintResults.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateImageIntent.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateImageIntent.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/AddNewBom.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterial.ui \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterial.ui \

HEADERS += CLOAPIInterface/LibraryWindowInterface.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/LibraryWindowImplPlugin.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVDialog.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVGroupBox.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVGroupFrame.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVOutterFrame.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVSubFrame.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVSubInnerFrame.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTableWidget.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTitleBar.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTreeWidget.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVWidget.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISearchDialogSample.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISignInDialogSample.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/PLMSettingDataSample.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/PLMSettingsSample.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APIStorage.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APIUtility.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/ComboBoxItem.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/Configuration.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/DirectoryUtil.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/Definitions.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/RestAPIUtility.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/Helper.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/DataUtility.h \
	CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CVLicenseHelper.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/FlexTypeHelper.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/FormatHelper.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CVErrorMessage.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CustomDoubleSpinBox.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CustomSpinBox.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/Logger.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/UIHelper.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVTableWidgetItem.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVTitleBar.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVQDateEdit.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVWidgets_export.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/json.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Authentication/CLOPLMImplementation.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Authentication/CLOPLMSignIn.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/CLOViseSuite/CLOViseSuite.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorResults.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/ColorConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/MaterialConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialResult.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/DocumentConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentResults.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/ProductConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductResults.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/SampleConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleResult.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintSearch.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintResults.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PrintConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateImageIntent.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateImageIntent.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/AddNewBom.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/Section.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterial.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterialConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterialConfig.h \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterial.h \

    
SOURCES += CLOAPIInterface/Samples/LibraryWindowImplementation/LibraryWindowImplPlugin.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVDialog.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVGroupBox.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVGroupFrame.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVOutterFrame.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVSubFrame.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVSubInnerFrame.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTableWidget.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTitleBar.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVTreeWidget.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/classes/widgets/MVWidget.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISearchDialogSample.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APISignInDialogSample.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/PLMSettingDataSample.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/PLMSettingsSample.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APIStorage.cpp \
	CLOAPIInterface/Samples/LibraryWindowImplementation/classes/APIUtility.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/ComboBoxItem.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/Configuration.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/DirectoryUtil.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/DataUtility.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CVLicenseHelper.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/DataHelper.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CustomDoubleSpinBox.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/Util/CustomSpinBox.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Authentication/CLOPLMImplementation.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Authentication/CLOPLMSignIn.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/CLOViseSuite/CLOViseSuite.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVDisplayMessageBox.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVWidgetGenerator.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVTitleBar.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Helper/UIHelper/CVQDateEdit.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/PLMColorResults.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Color/ColorConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/MaterialConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Material/PLMMaterialResult.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/DocumentConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Document/PLMDocumentResults.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/ProductConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Product/PLMProductResults.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/SampleConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Sample/PLMSampleResult.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintSearch.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PLMPrintResults.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Inbound/Print/PrintConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/PublishToPLMData.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateProduct.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CopyProduct.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/CreateImageIntent.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/UpdateImageIntent.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/AddNewBom.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/PublishToPLM/Section.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterial.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/CraeteMaterial/CreateMaterialConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterialConfig.cpp \
    CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Outbound/UpdateMaterial/UpdateMaterial.cpp \
    

RESOURCES += CLOAPIInterface/Samples/LibraryWindowImplementation/classes/CLOFormat.qrc \
    CLOAPIInterface/Samples/LibraryWindowImplementation/PLMResource.qrc
    
# Disable Warnings
QMAKE_CFLAGS += -w
QMAKE_CXXFLAGS += -w

# Essential Options
QMAKE_CFLAGS += -fno-builtin-function -fno-inline -finput-charset=UTF-8
QMAKE_CXXFLAGS += -fno-builtin-function -fno-inline -finput-charset=UTF-8

# C++ only
QMAKE_CXXFLAGS += -fpermissive

CONFIG += c++11

QMAKE_MACOSX_DEPLOYMENT_TARGET	= 10.12
QMAKE_CXXFLAGS					+= -mmacosx-version-min=10.12
QMAKE_LFLAGS						+= -mmacosx-version-min=10.12

QMAKE_CFLAGS_X86_64		-=  -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.12
QMAKE_CXXFLAGS_X86_64	-=  -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.12
QMAKE_LFLAGS_X86_64		-=  -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.12

QMAKE_LFLAGS		+= -stdlib=libc++ 
QMAKE_CXXFLAGS	+= -stdlib=libc++ -std=c++11

QMAKE_CC		= /Applications/Xcode.app/Contents/Developer/usr/bin/gcc
QMAKE_CXX	= /Applications/Xcode.app/Contents/Developer/usr/bin/g++

QMAKE_CFLAGS		+= -ferror-limit=0
QMAKE_CXXFLAGS	+= -ferror-limit=0
	
LIBS += -framework QtCore -framework QtNetwork -framework AGL -framework Cocoa
QT	+= core gui
QT  += widgets
QT  += network

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

option-qt5 {
	QT 		+= widgets
}

option-set-osx-basesdk {
	QMAKE_MAC_SDK = macosx10.14
}

INCLUDEPATH += . \
	/System/Library/Frameworks
#	/usr/include

LIBS += 	-L/usr/lib \
    -L../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/curl_Mac/lib \
    -L../../CLOAPIInterface/Samples/LibraryWindowImplementation/CLOVise/PLM/Libraries/zlib/lib

message("CloLibraryAPI_Plugin.pro compiler path...")
message($$QMAKE_CC)
message($$QMAKE_CXX)
