/*
* Copyright 2021-2022 CLO-Vise. All rights reserved
*
* @file AddNewBom.cpp
*
* @brief Class implementation for create Bom table on tab.
* This class has all the variable and methods implementation which are used to create bom table and publish bom lines from CLO to PLM.
*
* @author GoVise
*
* @date 10-OCT-2021
*/
#include "AddNewBom.h"

#include <cstring>

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include "qtreewidget.h"
#include <QFile>
#include "qdir.h"

#include "CLOAPIInterface.h"
#include "CLOVise/PLM/Helper/Util/Helper.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Inbound/Document/DocumentConfig.h"
#include "CLOVise/PLM/Helper/Util/FlexTypeHelper.h"
#include "CLOVise/PLM/Helper/UIHelper/UIHelper.h"
#include "CLOVise/PLM/Helper/Util/Logger.h"
#include "CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include "CLOVise/PLM/Helper/UIHelper/CVHoverDelegate.h"
#include "CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include "CLOVise/PLM/Inbound/Color/ColorConfig.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/Section.h"
#include "CLOVise/PLM/Outbound/PublishToPLM/CreateProduct.h"
#include "CLOVise/PLM/Inbound/Material/PLMMaterialSearch.h"
#include "CLOVise/PLM/Inbound/Material/MaterialConfig.h"

using namespace std;

namespace CLOVise
{

}