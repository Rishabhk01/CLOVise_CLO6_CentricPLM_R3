#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Definitions.h
*
* @brief Class defining variable.
* This class has all the variable and function declaration which are used defining variable.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <string>

#include <QString>
#include <QVariant>
#include <QColor>

#include "CLOVise/PLM/Helper/Util/CVErrorMessage.h"

using namespace std;

enum currentScreenSelected {COLOR_SEARCH_CLICKED, SEARCH_PRODUCT_CLICKED, COPY_PRODUCT_CLICKED, CREATE_PRODUCT_CLICKED, UPDATE_PRODUCT_CLICKED, CREATE_MATERIAL_CLICKED, SEARCH_MATERIAL_CLICKED, UPDATE_MATERIAL_CLICKED, PRINT_SEARCH_CLICKED};
const QString PLM_NAME = "Centric 8";

#define FRONT_VIEW 0
#define RIGHT_VIEW 1
#define BACK_VIEW 2
#define LEFT_VIEW 3

#define STARTING_INDEX 0
#define FIRST_INDEX 1
#define DIVISION_FACTOR 2

//Colorway table columns

#define CHECKBOX_COLUMN 0
#define UPDATE_BTN_COLUMN 0
#define ASSOCIATE_COLOR_COLUMN 0
#define PLM_COLORWAY_COLUMN 1
#define CLO_COLORWAY_COLUMN 2
#define COLOR_CHIP_COLUMN 3
#define COLOR_NAME_COLUMN 4
#define COLOR_CODE_COLUMN 5
#define PANTONE_CODE_COLUMN 6
#define UNI_2_DIGIT_CODE_COLUMN 7
#define DESCRIPTION_COLUMN 8

//image intents table columns
#define IMAGE_INTENT_COLUMN		0
#define COLORWAY_COLUMN			1
#define IMAGE_VIEW_COLUMN		2
#define LABELS_COLUMN			3
#define DEFAULT_COLUMN			4
#define EDIT_COLUMN				5
#define DELETE_COLUMN			6

//image intent queue table columns
#define COLORWAY_NAME_COLUMN  0
#define VIEW_COLUMN  1
#define IMAGE_LABLE_COLUMN  2
#define DEFAULT_IMAGE_COLUMN  3


const QString SPLASH_IMAGE_PATH = ":/CLOVise/PLM/Images/Splash.png";
const QString CLOVISE_LOGO_PATH = ":/CLOVise/PLM/Images/img_bi_black_CLOVise.svg";
const QString PLM_IMAGE_PATH = ":/CLOVise/PLM/Images/img_bi_black_flexPLM.png";
const QString POWERED_BY_IMAGE_PATH = ":/CLOVise/PLM/Images/img_bi_black_poweredby.svg";
const QString CLOVISE_LOGO_WHITE_PATH = ":/CLOVise/PLM/Images/img_bi_white_CLOVise.svg";
const QString PLM_IMAGE_WHITE_PATH = ":/CLOVise/PLM/Images/img_bi_white_flexPLM.svg";
const QString POWERED_BY_IMAGE_WHITE_PATH = ":/CLOVise/PLM/Images/img_bi_white_poweredby.svg";

const QString FABRIC_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_fabric_over.svg";
const QString COLOR_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_color_over.svg";
const QString PRINT_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_print_over.svg";
const QString DOCUMENT_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_3d_over.svg";
const QString PRODUCT_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_product_over.svg";
const QString PUBLISH_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_goto_over.svg";
const QString SAMPLE_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_sample_over.svg";
const QString SUBMIT_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_submit_over.svg";
const QString LOGOUT_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/ui_exit.svg";
const QString OK_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_ok_over.svg";
const QString CANCEL_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_cancel_over.svg";
const QString BACK_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_back_over.svg";
const QString DOWNLOAD_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_down_over.svg";
const QString VIEW_THUMB_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/btn_icon_viewthumb_over.svg";
const QString VIEW_LIST_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/btn_icon_viewlist_over.svg";
const QString BACK_HOVER_PATH = ":/CLOVise/PLM/Images/icon_back_over.svg";
const QString ADD_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_add_over.svg";
const QString UPDATE_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_update_over.svg";
const QString SEARCH_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_search_over.svg";
const QString SAVE_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_save_over.svg";
const QString COPY_HOVER_ICON_PATH = ":/CLOVise/PLM/Images/icon_copy_over.svg";

const QString FABRIC_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_fabric_none.svg";
const QString COLOR_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_color_none.svg";
const QString DOCUMENT_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_3d_none.svg";
const QString PRODUCT_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_product_none.svg";
const QString PUBLISH_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_goto_none.svg";
const QString SAMPLE_NONE_ICON_PATH = ":/CLOVise/PLM/Images/Sample_none.svg";
const QString SUBMIT_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_submit_none.svg";
const QString LOGOUT_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_exit_none.svg";
const QString OK_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_ok_none.svg";
const QString ADD_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_add_none.svg";
const QString UPDATE_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_update_none.svg";
const QString SAVE_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_save_none.svg";
const QString COPY_NONE_ICON_PATH = ":/CLOVise/PLM/Images/icon_copy_none.svg";


const string TEXT_FILE_EXTENSION = ".log";
const string BASIC = "Basic ";
const string AUTHORIZATION = "Authorization";
const string CONTENTTYPE = "content-Type";
const string CHARSET_UTF8 = "charset: utf-8";
const string FORM_ENCODED_TYPE = "application/x-www-form-urlencoded";
const string APPLICATION_JSON_TYPE = "application/json";
const string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const string UNAUTHORIZED = "Unauthorized";
const string INTERNAL_SERVER_ERROR = "500 Internal Server";
const string TIMEOUT_ERROR = "504 Gateway Timeout";
const string REQUEST_TIMEOUT = "Request Timeout";
const string PROGRESSBAR_CLOSE_TIMER = "progressBarCloseTimer";
const string ENCRYPT_DATA_FILE_NAME = "data.txt";
const string JSON_START_INDEX = "{";

/* PLM Settings functionality Constants Start. */

const string PLMSERVER_JSON_KEY = "availablePLMServers";
const string PLMSERVERURL_JSON_KEY = "plmServerUrl";
const string SELECTEDPLMSERVER_JSON_KEY = "selectedPLMServer";
const string CLOPLMPLUGINVERSION_JSON_KEY = "plmClientVersion";
const string CLOPLUGINVERSION_JSON_KEY = "cloViseVersion";
const string COMPANY_NAME = "companyName";
const string COMPANY_API_KEY = "companyApiKey";
const string CLOVISE_LICENSE_URL = "cvLicenceURL";
const string SERVER_CONNECTED_STATUS = "Connected";
const string SERVER_NOTCONNECTED_STATUS = "Not Connected";
const string PLMSETTINGS_FILE_NAME = "PLMSettings.json";
const string PLMCONFIG_FILE_NAME = "cloplmpluginconfig.json";
const string RESULTS_PER_PAGE_KEY = "resultsPerPageOptions";
const string RESOLUTION_OPTIONS_KEY = "availableImagePageResolutionOptions";
const string NOOFIMAGES_OPTIONS_KEY = "availableImagePageNoOfImagesOptions";
const string SELECTED_NOOFIMAGES_KEY = "selectedImagePageNoOfImages";
const string TURNTABLE_IMAGE_RES_KEY = "turnTableImageResolution";
const string NOOFTURNTABLE_IMAGES_KEY = "noOfTurnTableImages";
const string SELECTED_RESULTS_PER_PAGE_KEY = "selectedResultsPerPage";
const string LOGGER_ENABLED_KEY = "loggerEnabled";
const string RESULTS_CLOSE_KEY = "closeDialoguePostDownload";
const string SELECT = "SELECT";
const string BLANK = "";
const string SUPPORTED_TRIM_FILE_FORMATS = "trm,btn,obj,zpac,sst,zsd,zip";
const string RESOLUTION_OPTION__KEY = "resolutionOptions";
const string RESOLUTION_DETAIL__KEY = "resolutionDetail";
const string RESOLUTION_KEY = "resolution";
const string NUMBER_OF_IMAGES_KEY = "numberOfImages";
const string IMAGE_HEIGHT_KEY = "imageHeight";
const string WIDTH_IMAGE_KEY = "imageWidth";
const string MAX_LOGFILE_SIZE = "maximumLogFileSize";
const string MAX_ZPRJ_UPLOAD_FILE_SIZE = "maxUploadFileSizeLimit";
const string DEBUG_LOGGER_ENABLED_KEY = "debugLoggerEnabled";
const string ERROR_LOGGER_ENABLED_KEY = "errorLoggerEnabled";
const string INFO_LOGGER_ENABLED_KEY = "infoLoggerEnabled";
const string PERFOMANCE_LOGGER_ENABLED_KEY = "perfomanceLoggerEnabled";
const string REST_LOGGER_ENABLED_KEY = "restAPILoggerEnabled";
const string USER_FULL_NAME_KEY = "fullName";
const string MAX_STYLE_SEARCH_LIMIT = "maximumStyleSearchResults";
const string MAX_MATERIAL_SEARCH_LIMIT = "maximumMaterialSearchResults";
const string MAX_COLOR_SEARCH_LIMIT = "maximumColorSearchResults";
const string MAX_PRINT_SEARCH_LIMIT = "maximumPrintSearchResults";
const string MAX_REF_ATT_SEARCH_LIMIT = "maximuRefAttributeValues";

/* PLM search functionality Constants End. */
const string TYPENAME_JSON_KEY = "typeName";
const string AVAILABLE_VIEWS_JSON_KEY = "availableViews";
const string DEFAULT_VIEW_JSON_KEY = "defaultView";
const string TYPEDISPLAYNAME_JSON_KEY = "typeDisplayName";
const string HIERARCHY_JSON_KEY = "hierarchy";
const string SUB_FLEX_TYPE = "subType";
const string ROOTHIRARCHY_JSON_KEY = "rootLevelTypes";
const string FIRSTROOT_JSON_KEY = "firstLevelTypes";
const string SECONDROOT_JSON_KEY = "secondLevelTypes";
const string FIELDLIST_JSON_KEY = "fieldsList";
const string ATTSCOPE_JSON_KEY = "attScopes";
const string SUPPORTED_ATTYPE_JSON_KEY = "supportedAttTypes";
const string MODIFY_SUPPORTED_ATTYPE_JSON_KEY = "modifySupportedAttTypes";
const string RESULTS_SUPPORTED_ATTYPE_JSON_KEY = "resultsSupportedAttTypes";
const string PRESETLIST_JSON_KEY = "presetList";
const string ATTSCOPE_KEY = "attScope";
const string ATTRIBUTE_JSON_KEY = "attributes";
const string ATTRIBUTE_KEY = "attKey";
const string ATTRIBUTE_NAME_KEY = "attName";
const string ATTRIBUTE_REQUIRED_KEY = "isAttRequired";
const string ATTRIBUTE_EDITABLE_KEY = "isAttEditable";
const string ATTRIBUTE_DEFAULT_VALUE_KEY = "attDefaultValue";
const string ATTRIBUTE_TYPE_KEY = "attType";
const string ATTRIBUTE_VALUE_KEY = "attValue";
const string ATTRIBUTE_ATTSCOPE_KEY = "attScope";
const string IS_MULTISELECT_KEY = "isMultiselectable";
const string IS_CHECK_KEY = "isCheckable";
const string IS_SINGLE_INTERGER_KEY = "isSingleInterger";
const string IS_SINGLE_FLOAT_KEY = "isSingleFloat";
const string ATTRIBUTE_SEARCHABLE_KEY = "isAttSearchable";
const string ATTRIBUTE_SETTABLE_KEY = "isAttSettable";
const string ATTRIBUTE_UPDATABLE_KEY = "isAttUpdatable";
const string ATTRIBUTE_ENABLED_KEY = "isAttEnabled";
const string ATTRIBUTE_CURENCY_CODE_KEY = "attCurrencyCode";
const string ATTRIBUTE_DECIMALS_KEY = "attDecimalFigures";
const string ENUMLIST_JSON_KEY = "enumList";
const string ENUM_KEY = "enumKey";
const string ENUM_VALUE_KEY = "enumValue";
const string TYPENAME_KEY = "typeName";
const string COMP3D_KEY = "3dCompatibilityKey";
const string MAX_RESULTS_LIMIT_KEY = "maxResultsLimit";
const string CLOSOLIDCOLOR_KEY = "cloSolidColorKey";
const string CLOPRINTANDPATTERN_KEY = "cloPrintAndPatternKey";
const string RESULTFOUND_KEY = "resultFound";
const string INCLUDEPALETTE_KEY = "includePalette";
const string INCLUDESEASON_KEY = "includeSeason";
const string RESULTFIELDLIST_KEY = "fieldsList";
const string SEARCHRESULTS_KEY = "SearchResults";
const string ATTACHMENTS_KEY = "attachments";
const string _3DMODEL_METADATALIST_KEY = "3DModelMetaDataList";
const string ATTACHMENT_KEY = "attachment";
const string HEADERLIST_KEY = "headerList";
const string CURRENCY_CODE = "attCurrencyCode";
const string DECIMAP_FIGURES = "attDecimalFigures";
const string NONCE_KEY = "nonce_key";
const string NONCE_VALUE = "nonce";
const string ATTRIBUTE_TYPE = "value_type";
const string ATTRIBUTE_NAME = "node_name";
const string ATTRIBUTE_DISPLAY_NAME = "display_name";
const string ATTRIBUTE_REQUIRED = "required";
const string ATTRIBUTE_EDITABLE = "restrict_edit";
const string ATTRIBUTE_ID = "id";
const string ATTRIBUTE_HEIRARCHY = "hierarchy";
const string CODE_KEY = "code";
const string PANTONE_KEY = "pantone";
const string LATEST_REVISION_KEY = "latest_revision";
const string ATTRIBUTE_DISPLAY_NAMES = "display_names";
const string ATTRIBUTE_INTERNAL_NAME = "rest_api_name";
const string ATTRIBUTE_REST_API_EXPOSED = "rest_api_exposed";
const string ATTRIBUTE_INITIAL_VALUE = "initial_value";
const string COLOR_SPEC_ID = "color_specification";
const string ATTRIBUTE_CREATE_ONLY = "create_only";
const string ENUM_ATTRIBUTE_FORMAT_KEY = "formatKey";




const string FILTERSLIST_JSON_KEY = "filtersList";
const string FILTERS_JSON_KEY = "filters";

const string FIELDS_LIST_JSON_KEY = "fieldsList";
const string SUPPORTED_ATT_KEY = "supportedAttTypes";

const string VIEWLIST_JSON_KEY = "viewsList";
const string VIEWS_JSON_KEY = "views";
const string VIEW_NAME_JSON_KEY = "viewName";
const string VIEW_ID = "viewId";
const string ISVENDOR_KEY = "isVendor";
const string PLM_VERSION_JSON_KEY = "plmVersion";

const string COLOR_IMAGE_DISPLAY_NAME = "Color Image";
const string SOLID_IMAGE_DISPLAY_NAME = "Solid Image";
const string DEFAULT_VIEW_KEY = "defaultView";
const string IMAGE_DISPLAY_NAME = "Image";

const string PLMPALETTE_FILE_NAME = "PLMPaletteSearch.json";
const string ACTIVESEASONS_JSON_KEY = "activeSeasons";
const string PALETTEID_KEY = "paletteId";
const string PRODUCTID_KEY = "productId";
const string PRODUCT_NAME_KEY = "productName";
const string PRODUCT_STATUS_KEY = "productStatus";
const string SEASONID_KEY = "seasonId";
const string SEASONID_VERSION_KEY = "seasonVersionId";
const string PRODUCTNAME_KEY = "productName";
const string QUICKSEARCH_KEY = "quickSearchCriteria";
const string PALETTENAME_KEY = "paletteName";
const string SUB_PALETTENAME_KEY = "paletteName";
const string SEASONNAME_KEY = "seasonName";
const string FILTER_NAME_KEY = "filterName";
const string FILTER_ID_KEY = "filterId";
const string FILTER_TYPE_KEY = "filterType";
const string FILTER_ATTRIBUTES_KEY = "attributes";
const string ATTRIBUTES_KEY = "attributes";
const string NO_FILTER_TEMPLATE_KEY = "NOFILTER";
const string DEFAULT_FILTER_KEY = "defaultFilter";
const string SUBPALETTE_JSON_KEY = "subPalettes";


const string ZPRJ = ".zprj";
const string PNG = ".png";
const string ZIP = ".zip"; 
const string GLB = ".glb";

const string DELIMITER_NEGATION = "|~*~|";
const string DELIMITER_COMMA = ", ";
const string DELIMITER_HYPHAN = "|---|";

const string IconViewKey = "iconView";
const QString ICON_VIEW = "Icon view";
const string TableViewKey = "tableView";
const QString TABULAR_VIEW = "Tabular view";
const string TrueValue = "true";
const string FalseValue = "false";
const string TrueValueInt = "1";
const string FalseValueInt = "0";
const string PaletteDisplayValue = "Palette";
const string RGBCODE_KEY = "rgbCode";
const string PaletteKey = "palette";
const string SEASON_KEY = "season";
const string YesDisplayValue = "Yes";
const string NoDisplayValue = "No";

const string NAME_KEY = "name";
const string OBJECT_NAME_KEY = "objectName";
const string OBJECT_STATUS_KEY = "objectStatus";
const string Null_Value = "NULL";
const string QUICK_SEARCK_CRITERIA_KEY = "quickSearchCriteria";
const string FLEX_TYPE_STRING_KEY = "flexTypeString";
const string INCLUDE_SUPPLIER_STRING_KEY = "includeSupplier";
const string SEARCHBY_SUPPLIER_STRING_KEY = "searchBySupplier";
const string REQ_FROM_CLO_KEY = "requestfromCLO";
const string DOCUMENT_METADATA_LIST_KEY = "3DModelMetaDataList";

const string API_META_DATA = "apiMetaData";
const string COLOR_MODULE_NAME = "Color";
const string TEMPLATE_KEY = "TEMPLATE_FILTER_TYPE";
const string CURRENCY_ATT_TYPE_KEY = "currency";
const string INTEGER_ATT_TYPE_KEY = "integer";
const string CONSTANT_ATT_TYPE_KEY = "constant";
const string SEQUENCE_ATT_TYPE_KEY = "sequence";
const string FLOAT_ATT_TYPE_KEY = "float";
const string DOUBLE_ATT_TYPE_KEY = "double";
const string CHOICE_ATT_TYPE_KEY = "choice";
const string REF_ATT_TYPE_KEY = "ref";
const string REFLIST_ATT_TYPE_KEY = "reflist";
const string STRING_ATT_TYPE_KEY = "string";
const string ENUM_ATT_TYPE_KEY = "enum";
const string MOA_LIST_ATT_TYPE_KEY = "moaList";
const string DRIVEN_ATT_TYPE_KEY = "driven";
const string MOA_ENTRY_ATT_TYPE_KEY = "moaEntry";
const string TEXT_AREA_ATT_TYPE_KEY = "Textarea";
const string TEXT_ATT_TYPE_KEY = "text";
const string DERIVEDSTRING_ATT_TYPE_KEY = "derivedString";
const string DATE_ATT_TYPE_KEY = "date";
const string TIME_ATT_TYPE_KEY = "time";
const string BOOLEAN_ATT_TYPE_KEY = "boolean";
const string USER_LIST_ATT_TYPE_KEY = "userList";

const string COLOR_ID_KEY = "colorId";
const string OBJECT_ID_KEY = "objectId";
const string COLOR_TYPE_KEY = "colorType";
const string COLOR_NAME_KEY = "colorName";
const string RGB_VALUE_KEY = "rgb_triple";
const string THUMBNAIL_KEY = "thumbnail";
const string ATTACHMENT_URL_KEY = "attachmentURL";
const string ATTACHMENT_NAME_KEY = "attachmentName";
const string MATERIAL_MODULE = "Material";
const string MATERIAL_ROOT_TYPE = "Material";
const string MATERIAL_SEARCH_ACTIVITY = "FIND_ADVANCED_MATERIAL";
static string INVALID_STRINGS_CHARS = "/:*?\"<>|";
const string IMAGE_LABEL_KEY = "labelNodeName";


/*PLMColor */
const string PLM_COLOR_FILE_NAME = "PLM_COLOR_SEARCH.json";
const string PLM_COLOR_RESULT_FILE = "PLM_COLOR_RESULTS.json";
const string PLM_COLOR_FILTER_FILE = "PLM_COLOR_FILTER.json";
const string PLM_COLOR_VIEW_FILE = "PLM_COLOR_VIEW.json";
const string PLM_COLOR_PALETTE_FILE = "PLM_COLOR_PALETTE.json";
const string PLM_COLOR_ATTACHMENT_FILE_NAME = "PLMColorAttachment.json";
const string COLOR_ROOT_TYPE = "Color"; 
const string COLOR_MODULE = "Color";
const string COLOR_SEARCH_ACTIVITY = "FIND_COLOR";
const string COLOR_SWATCH_NAME = "PLM Solid Colors";
const QString RGB_TOOLTIP_KEY = "RGB Value: ";

/*PLMPrint */
const string PLM_PRINT_FILE_NAME = "PLM_PRINT_SEARCH.json";
const string PLM_PRINT_RESULT_FILE = "PLM_PRINT_RESULTS.json";
const string PLM_PRINT_VIEW_FILE = "PLM_PRINT_VIEW.json";
const string PRINT_ROOT_TYPE = "Print";
const string PRINT_MODULE = "Print";
const string PRINT_SEARCH_ACTIVITY = "FIND_PRINT";


/*PLMMaterial */
const string PLMMATERIAL_FILE_NAME = "PLMMaterialFilter.json";
const string PLM_MATERIAL_SEARCH_FILE_NAME = "PLM_MATERIAL_SEARCH.json";
const string PLM_MATERIAL_CREATE_FILE_NAME = "PLM_MATERIAL_CREATE.json";
const string PLM_MATERIAL_UPDATE_FILE_NAME = "PLM_MATERIAL_UPDATE.json";
const string PLM_MATERIAL_FILTER = "PLM_MATERIAL_FILTER.json";
const string PLM_MATERIAL_VIEW = "PLM_MATERIAL_VIEW.json";
const string PLM_MATERIAL_PALETTE = "PLM_MATERIAL_PALETTE.json";
const string PLM_MATERIAL_RESULTS = "PLM_MATERIAL_RESULTS.json";
const string VIEWS_LIST_JSON_KEY = "viewsList";
const string MATERIAL_ID_KEY = "materialId";
const string MATERIAL_TYPE_KEY = "materialType";
const string MATERIAL_NAME_KEY = "materialName";
const string MODULE_KEY = "module";
const string ROOT_TYPE_KEY = "rootType";
const string ACTIVITY_KEY = "activity";
const string CHECKED_IDS_KEY = "checkedIds";
const QString FABRICS_FILE_PATH = "C:/Users/Public/Documents/CLO/Assets/Materials/Fabric/";

/*PLMProduct */
const string PLM_PRODUCT_FILE_NAME = "PLM_PRODUCT_SEARCH.json";
const string PLM_PRODUCT_RESULT_FILE = "PLM_PRODUCT_RESULTS.json";
const string PLM_PRODUCT_FILTER_FILE = "PLM_PRODUCT_FILTER.json";
const string PLM_PRODUCT_VIEW_FILE = "PLM_PRODUCT_VIEW.json";
const string PLM_PRODUCT_PALETTE_FILE = "PLM_PRODUCT_PALETTE.json";
const string PLM_PRODUCT_ATTACHMENT_FILE_NAME = "PLMProductAttachment.json";
const string PRODUCT_ROOT_TYPE = "Product";
const string PRODUCT_MODULE = "Product";
const string PRODUCT_SEARCH_ACTIVITY = "FIND_PRODUCT";
const string PRODUCT_SEASON_TYPE_KEY = "productType";

/*PLMDocument */
const string PLM_DOCUMENT_FILE_NAME = "PLM_DOCUMENT_SEARCH.json";
const string PLM_DOCUMENT_RESULT_FILE = "PLM_DOCUMENT_RESULTS.json";
const string PLM_DOCUMENT_FILTER_FILE = "PLM_DOCUMENT_FILTER.json";
const string PLM_DOCUMENT_VIEW_FILE = "PLM_DOCUMENT_VIEW.json";
const string PLM_DOCUMENT_PALETTE_FILE = "PLM_DOCUMENT_PALETTE.json";
const string PLM_DOCUMENT_ATTACHMENT_FILE_NAME = "PLMDocumentAttachment.json";
const string DOCUMENT_ROOT_TYPE = "Document\\3dModel";
const string DOCUMENT_MODULE = "Document";
const string DOCUMENT_SEARCH_ACTIVITY = "FIND_DOCUMENT";
const string CLODOCUMENT_KEY = "cloDocumentKey";
const string PRODUCT_ID_EXIST_KEY = "productIdKeyExist";
const string GLB_EXPORT_KEY = "glbExport";

/*PLMStyle */
const string PLM_STYLE_FILE_NAME = "PLM_STYLE_SEARCH.json";
const string PLM_CREATE_STYLE_FILE_NAME = "CentricCreateStyleHierarchy.json";
const string PLM_STYLE_RESULT_FILE = "PLM_STYLE_RESULTS.json";
const string PLM_STYLE_FILTER_FILE = "PLM_STYLE_FILTER.json";
const string PLM_STYLE_VIEW_FILE = "PLM_STYLE_VIEW.json";
const string PLM_STYLE_PALETTE_FILE = "PLM_STYLE_PALETTE.json";
const string PLM_STYLE_ATTACHMENT_FILE_NAME = "PLMStyleAttachment.json";
const string STYLE_ROOT_TYPE = "Style";
const string STYLE_MODULE = "Style";
const string STYLE_SEARCH_ACTIVITY = "FIND_STYLE";
const string STYLE_ID_EXIST_KEY = "styleIdKeyExist";

/* UI Definitions */
const int VERTICAL_HEADER = 80;
const int HORIZONTAL_HEADER_HEIGHT = 30;
const int COLORWAY_HORIZONTAL_HEADER_HEIGHT = 50;
const int HORIZONTAL_HEADER_WIDTH = 100;
const int COLUMN_SIZE = 200;
const QString TOOLTIP_STYLE = "QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const QString PREVIOUS_BUTTON_STYLE = "#previousButton { qproperty-icon: none; image: url(:/CLOVise/PLM/Images/previous_none.svg); } #previousButton:hover { image: url(:/CLOVise/PLM/Images/previous_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const QString NEXT_BUTTON_STYLE = "#nextButton { qproperty-icon: none; image: url(:/CLOVise/PLM/Images/next_none.svg); } #nextButton:hover { image: url(:/CLOVise/PLM/Images/next_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const int DESIGNSUITE_ICONHEIGHT = 24;
const string ZERO_RESOLUTION = "0";
const int DESIGNSUITE_ICONWIDTH = 24;
const int ICONHEIGHT = 18;
const int ICONWIDTH = 18;
const int SPINBOX_HEIGHT = 25;
const int RESET_BUTTON_ICONHEIGHT_MAC = 28;
const int RESET_BUTTON_ICONWIDTH_MAC = 28;
const int RESET_BUTTON_ICONHEIGHT = 14;
const int RESET_BUTTON_ICONWIDTH = 14;
const int DESIGNSUITEBUTTONWIDTH = 300;
const int DESIGNSUITEBUTTONHEIGHT = 40;
const int LABEL_WIDTH = 170;
const int RADIOBUTTON_COLUMN = 0;
const int IMAGE_COLUMN = 1;
const QString DOWNLOAD_TEXT = "Download";
const QString PREVIOUS = "Previous";
const QString NEXT = "Next";
const QString DATE_FORMAT_TEXT = "YYYY-MM-DD";

/* Publish definitions */
const string DOCUMENTID_KEY = "documentId";
const string _3DMODEL_TYPE_KEY = "3dModelType";
const string _3D_MODEL_FILE_FIELD_NAME = "3D Model File Name";
const string RESOLUTION_FIELD_NAME = "Resolution";
const string NUMBER_OF_IMAGES = "# Images";
const string DOCUMENT_DETAIL_KEY = "documentDetails";
const string THUMBNAIL_DETAIL_KEY = "thumbnailDetails";
const string THUMBNAIL_NAME_KEY = "thumbnailName";
const string THREE_D_MODEL_DETAIL_KEY = "3dModelDetails";
const string THREE_D_MODEL_NAME_KEY = "3dModelName";
const string THREE_D_MODEL_TYPE_KEY = "3dModelType";
const string PRODUCT_DETAIL_KEY = "productDetails";
const string IMAGEPAGE_DETAIL_KEY = "imagePageDetails";
const string NAME_OF_IMAGEPAGE_KEY = "nameOfImagePage";
const string IMAGEPAGE_NAME_SUFFIX = "";
const string TURNTABLE_NAME_SUFFIX = " Turntable Visuals";
const string ZIP_NAME_KEY = "zipName";
const string TURNTABLE_IMAGEPAGE_DETAIL_KEY = "turnTableImagePageDetails";
const string ATTACHMENT_ZIP_KEY = "attachmentsZip";
const string OBJECT_ID = "Object Id";
const string OBJECT_NAME = "Object Name";
const string OBJECT_STATUS = "Oject Status";
const string PRODUCT_ID = "Product Id";
const string PRODUCT_OBJ_ID = "productObjectId";
const string PRODUCT_NAME = "Product Name";
const string PRODUCT_STATUS = "Product Status";
const string DEFAULT_3DMODEL_NAME = "Default_Modelist";
const string REQUIRED_THROW_KEY = " cannot be blank";

const QColor hoverHighlightColor = "#24272a";
const QColor highlightColor = "#33414d";
const QColor highlightedTextColor = "#46c8ff"; // highlighted text color to set
const QString TAB_BG_COLOR = "#46C8FF";
const QString DEFAULT_TEXT_COLOR = "#FFFFFF";
const QString SELECTED_TAB_BG_COLOR = "#323234";
const QString DEFAULT_TAB_BG_COLOR = "#202021";

const QString LINEEDIT_STYLE = "QLineEdit { font-size: 10px; font-face: ArialMT; max-height 25px; min-width: 40px; }";
const QString DISABLED_LINEEDIT_STYLE = "QLineEdit { color:#868482; font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; }";
const QString HEADER_STYLE = "font-size: 12px; font-face: ArialMT; height: 20px;";
const QString BUTTON_STYLE = "QPushButton{font-size: 10px; font-face: ArialMT; height: 27px; padding-left:12px;padding-right:12px;icon-size:18px;}""QPushButton::hover{padding-left:12px;padding-right:12px;color: #46C8FF;}";
const QString PUSH_BUTTON_STYLE = "QPushButton{ font-size: 10px; font-face: ArialMT; height: 30px; padding-left: 8px; padding-right: 8px; } QPushButton:hover{ font-size: 10px; font-face: ArialMT; height: 30px; padding-left: 8px; padding-right: 8px; color: #46C8FF; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const QString CLOVISESUITE_BUTTON_STYLE = "QPushButton{ icon-size: 18px; font-size: 10px; font-face: ArialMT; height: 22px; width: 21px; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const QString TOOL_BUTTON_STYLE = "QToolButton { font-size: 10px; font-face: ArialMT; width: 30px; height: 30px; icon-size: 30px;}""QToolButton::hover { width: 30px; height: 30px; icon-size: 30px; color: #46C8FF; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }";
const QString LABEL_STYLE = "QLabel {font-size: 10px; font-face: ArialMT; height: 20px;}";
const QString USER_NAME_LABEL_STYLE = "QLabel { font-size: 9px; font-face: ArialMT; height: 25px; margin: 5px; padding: 5px; }""QToolTip { color: #46C8FF; background-color: #33414D; border: 1px #000000; }";


const QString FONT_STYLE = "font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px;";
const QString PUBLISH_LABEL_STYLE = "font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 160px;";
const QString VIEW_COMBOBOX_STYLE = "QComboBox { font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 150px; }";
const QString disabledComboTextStyle = "color:grey; selection-color: grey; font-family: ArialMT; combobox-popup: 0;padding: 1px 0px 1px 3px;";
const QString DISABLED_VIEW_COMBOBOX_STYLE = "QComboBox { color: #808080; font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 150px; padding: 1px 0px 1px 3px; }";
const QString DISABLED = "QLabel {color: #808080;font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px;}";
const QString COMBOBOX_STYLE = "QComboBox { font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; color: #FFFFFF; }";
const QString DISABLED_COMBOBOX_STYLE = "QComboBox { color: #808080; font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; padding: 1px 0px 1px 3px; }";
const QString DOUBLE_SPINBOX_STYLE = "QDoubleSpinBox { font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; }";
const QString DISABLED_DOUBLE_SPINBOX_STYLE = "QDoubleSpinBox { color: #808080; font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; }";
const QString SPINBOX_STYLE = "QSpinBox { font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; }""QSpinBox::up-button { max-height: 12px; }""QSpinBox::down-button { max-height: 12px; }";
const QString DISABLED_SPINBOX_STYLE = "QSpinBox { color: #808080; font-size: 10px; font-face: ArialMT; max-height: 25px; min-width: 40px; }""QSpinBox::up-button { height: 12px; }""QSpinBox::down-button { height: 12px; }";
const QString RESULTSPERPAGE_COMBOBOX_STYLE_SHEET = "QComboBox { font-size: 10px; font-face: ArialMT; max-height: 25px; min-width:40px; }";

const QString SCROLLBAR_STYLE = "QScrollBar::add-page:vertical { background: #000; }""QScrollBar::sub-page:vertical {background: #000;}""QScrollBar::add-page:horizontal { background: #000; }""QScrollBar::sub-page:horizontal {background: #000;}";
const QString DISABLED_TEXTEDIT_STYLE = "QTextEdit { border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; font-size: 10px; color: #080808; font-face: ArialMT; min-height: 65px; max-height: 70px; }";
const QString TEXTEDIT_STYLE = "QTextEdit { border-bottom: 1px inset #262626; border-left: 1px inset #0D0D0D; border-top: 1px inset #0D0D0D; border-right: 1px inset #262626; background-color: #222224; font-size: 10px; color: #ffffff; font-face: ArialMT; min-height: 65px; max-height: 70px; }";
const QString DATE_RESET_BUTTON_STYLE = "QToolButton { icon_size: 18px; image: url(:/CLOVise/PLM/Images/DateReset_none.svg); }""QToolButton:hover { image: url(:/CLOVise/PLM/Images/DateReset_over.svg); }""QToolTip{ color: #46C8FF; background-color: #33414D; border: 1px #000000; }";

/* CLOVise Licence constants start*/
const string LICENSE_BYPASS = "{\"result_code\": 1}";
const string LICENSE_ERROR_CODE_101 = "{\"result_code\": 101}";
const string LICENSE_ERROR_CODE_102 = "{\"result_code\": 102}";
const string LICENSED_USER = "X-User-Product";
const string CLOVISE = "clovise";
const string HTTP_POST = "HTTP Post";
/* CLOVise Licence constants end*/

/* Tooltip values */
const QString SEARCH_TOOLTIP = "Search";
const QString PUBLISH_TOOLTIP = "Publish";
const QString SUBMIT_TOOLTIP = "Submit";
const QString LOGOUT_TOOLTIP = "Logout";
const QString CREATE_TOOLTIP = "Create";
const QString UPDATE_TOOLTIP = "Update";
const QString COPY_TOOLTIP = "Copy";
/* Tooltip values End*/

/* Sample module start*/
const string PLM_SAMPLE_FILE_NAME = "PLM_SAMPLE_SEARCH.json";
const string PLM_SAMPLE_RESULT_FILE = "PLM_SAMPLE_RESULTS.json";
const string PLM_SAMPLE_FILTER_FILE = "PLM_SAMPLE_FILTER.json";
const string PLM_SAMPLE_VIEW_FILE = "PLM_SAMPLE_VIEW.json";
const string PLM_SAMPLE_PALETTE_FILE = "PLM_SAMPLE_PALETTE.json";
const string PLM_SAMPLE_ATTACHMENT_FILE_NAME = "PLMSampleAttachment.json";
const string SAMPLE_ROOT_TYPE = "Sample";
const string SAMPLE_MODULE = "Sample";
const string SAMPLE_SEARCH_ACTIVITY = "FIND_SAMPLE";
const string SAMPLE_NAME_KEY = "sampleName";
/* Sample module end*/


//const QStringList METADATA_SUPPORTED_FILE_LIST{ "zfab", "btn", "zsd" , "bth", "zpl", "zpac", "zsp", "zprj", "ai" };
const QStringList METADATA_SUPPORTED_FILE_LIST{ "zfab", "btn", "bth", "zpac", "zprj", "ai", "Zprj" };

/*Asset file configuration start*/
const string WRIGHT_SINGLE_SLASH = "/";
const string LEFT_DOUBLE_SLASH = "\\";
const QString SIDE_MENU_TRIM_ID = "TRIM";
const QString SIDE_MENU_COLOR_ID = "COLOR";
const QString SIDE_MENU_PRODUCT_ID = "PRODUCT";
const QString SIDE_MENU_MATERIAL_ID = "MATERIAL";
const QString SIDE_MENU_PRINT_ID = "PRINT";
const QString MATERIAL_DATA_ID_KEY = "Material ";
const QString COLOR_DATA_ID_KEY = "Color ";
const QString PRINT_DATA_ID_KEY = "Print & Designs ";
const QString PRODUCT_DATA_ID_KEY = "Product ";
const QString FABRIC_DATA_ID_KEY = "Fabric";
const QString DOCUMENT_DATA_ID_KEY = "Document";
const QString TRIM_DATA_ID_KEY = "Trim";
const QString PRINTS_AND_PATTERN_DATA_ID_KEY = "Print & Pattern";
const QStringList TRIM_SUPPORTING_LIST{ "btn", "zsd" , "bth", "zpl", "png", "jpg", "jpeg", "zpac", "zsp" };
const QStringList COLOR_SUPPORTING_LIST{ "png", "jpeg", "jpg", "ai" };
const QStringList PRODUCT_SUPPORTING_LIST{ "zprj", "Zprj" };
const QStringList MATERIAL_SUPPORTING_LIST{ "zfab" };

const string EXCLUDED_PREVIEW_FEILDS_KEY = "excludedPreviewFields";

/*Asset file configuration end*/

const string NODE_NAME_KEY = "node_name";
const string REST_API_NAME_KEY = "rest_api_name";
const string CREATE_DEFINITION_KEY = "create_form_definition";
const string VALUE_TYPE_KEY = "value_type";
const string ENABLED_KEY = "enabled";
const string ATT_FLAG_KEY = "attribute_flags";
const string DISPLAY_NAME_KEY = "display_name";
const string DFAULT_ASSET_KEY = "default_3d_asset";
const string PERFOMANCE_KEY = "CLO-VISE PERFORMANCE TESTING ->";
const string INITIAL_VALUE_KEY = "initial_value";
const string DEFAULT_IMAGE_KEY = "default_image";
const string CREATE_ONLY = "create_only";
const string SEARCH_MATERIAL = "Search Material";
const string UPDATE_MATERIAL = "Update Material";
const string CREATE_MATERIAL = "Create Material";
const string DOCUMENT_NAME = "docName";
const string ATT_RESTRICT_EDIT = "restrict_edit";
const string ALLOW_CREATE_COLOR = "allow_create_color_spec_on_colorway";
const string REFER_DEFAULT_IMAGE_ON_COLOR = "reference_default_image_on_colorway_create";
const QStringList MATERIAL_FILTER_LIST{ ".zfab", ".zsd", ".zpl", ".zpac", ".zsp", ".btn", ".bth", ".obj", ".sst" };