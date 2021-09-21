/****************************************************************************
** Meta object code from reading C++ file 'PLMMaterialResult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../CLOVise/PLM/Inbound/Material/PLMMaterialResult.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PLMMaterialResult.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLOVise__PLMMaterialResult_t {
    QByteArrayData data[15];
    char stringdata0[313];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLOVise__PLMMaterialResult_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLOVise__PLMMaterialResult_t qt_meta_stringdata_CLOVise__PLMMaterialResult = {
    {
QT_MOC_LITERAL(0, 0, 26), // "CLOVise::PLMMaterialResult"
QT_MOC_LITERAL(1, 27, 26), // "onClickedDeselectAllButton"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 19), // "onClickedBackButton"
QT_MOC_LITERAL(4, 75, 19), // "onClickedNextButton"
QT_MOC_LITERAL(5, 95, 23), // "onClickedPreviousButton"
QT_MOC_LITERAL(6, 119, 34), // "onResultPerPageCurrentIndexCh..."
QT_MOC_LITERAL(7, 154, 24), // "onResultViewIndexChanged"
QT_MOC_LITERAL(8, 179, 17), // "onIconViewClicked"
QT_MOC_LITERAL(9, 197, 16), // "onTabViewClicked"
QT_MOC_LITERAL(10, 214, 23), // "onTableCheckBoxSelected"
QT_MOC_LITERAL(11, 238, 22), // "onIconCheckBoxSelected"
QT_MOC_LITERAL(12, 261, 17), // "onDownloadClicked"
QT_MOC_LITERAL(13, 279, 25), // "onHorizontalHeaderClicked"
QT_MOC_LITERAL(14, 305, 7) // "_column"

    },
    "CLOVise::PLMMaterialResult\0"
    "onClickedDeselectAllButton\0\0"
    "onClickedBackButton\0onClickedNextButton\0"
    "onClickedPreviousButton\0"
    "onResultPerPageCurrentIndexChanged\0"
    "onResultViewIndexChanged\0onIconViewClicked\0"
    "onTabViewClicked\0onTableCheckBoxSelected\0"
    "onIconCheckBoxSelected\0onDownloadClicked\0"
    "onHorizontalHeaderClicked\0_column"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLOVise__PLMMaterialResult[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    0,   77,    2, 0x08 /* Private */,
       6,    1,   78,    2, 0x08 /* Private */,
       7,    1,   81,    2, 0x08 /* Private */,
       8,    0,   84,    2, 0x08 /* Private */,
       9,    0,   85,    2, 0x08 /* Private */,
      10,    0,   86,    2, 0x08 /* Private */,
      11,    0,   87,    2, 0x08 /* Private */,
      12,    0,   88,    2, 0x08 /* Private */,
      13,    1,   89,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,

       0        // eod
};

void CLOVise::PLMMaterialResult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PLMMaterialResult *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onClickedDeselectAllButton(); break;
        case 1: _t->onClickedBackButton(); break;
        case 2: _t->onClickedNextButton(); break;
        case 3: _t->onClickedPreviousButton(); break;
        case 4: _t->onResultPerPageCurrentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onResultViewIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onIconViewClicked(); break;
        case 7: _t->onTabViewClicked(); break;
        case 8: _t->onTableCheckBoxSelected(); break;
        case 9: _t->onIconCheckBoxSelected(); break;
        case 10: _t->onDownloadClicked(); break;
        case 11: _t->onHorizontalHeaderClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CLOVise::PLMMaterialResult::staticMetaObject = { {
    QMetaObject::SuperData::link<MVDialog::staticMetaObject>(),
    qt_meta_stringdata_CLOVise__PLMMaterialResult.data,
    qt_meta_data_CLOVise__PLMMaterialResult,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLOVise::PLMMaterialResult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLOVise::PLMMaterialResult::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLOVise__PLMMaterialResult.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::materialResult"))
        return static_cast< Ui::materialResult*>(this);
    return MVDialog::qt_metacast(_clname);
}

int CLOVise::PLMMaterialResult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MVDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
