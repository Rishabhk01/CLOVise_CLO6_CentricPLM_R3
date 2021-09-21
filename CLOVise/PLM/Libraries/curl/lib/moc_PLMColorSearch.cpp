/****************************************************************************
** Meta object code from reading C++ file 'PLMColorSearch.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../CLOVise/PLM/Inbound/Color/PLMColorSearch.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PLMColorSearch.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLOVise__PLMColorSearch_t {
    QByteArrayData data[15];
    char stringdata0[218];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLOVise__PLMColorSearch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLOVise__PLMColorSearch_t qt_meta_stringdata_CLOVise__PLMColorSearch = {
    {
QT_MOC_LITERAL(0, 0, 23), // "CLOVise::PLMColorSearch"
QT_MOC_LITERAL(1, 24, 14), // "onHideHirarchy"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 5), // "_hide"
QT_MOC_LITERAL(4, 46, 21), // "onClickedSubmitButton"
QT_MOC_LITERAL(5, 68, 19), // "onClickedBackButton"
QT_MOC_LITERAL(6, 88, 15), // "onFilterChanged"
QT_MOC_LITERAL(7, 104, 5), // "_item"
QT_MOC_LITERAL(8, 110, 15), // "onSeasonChanged"
QT_MOC_LITERAL(9, 126, 17), // "onCheckBoxChecked"
QT_MOC_LITERAL(10, 144, 8), // "_checked"
QT_MOC_LITERAL(11, 153, 17), // "onTreeNodeClicked"
QT_MOC_LITERAL(12, 171, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(13, 188, 7), // "_column"
QT_MOC_LITERAL(14, 196, 21) // "onResetDateEditWidget"

    },
    "CLOVise::PLMColorSearch\0onHideHirarchy\0"
    "\0_hide\0onClickedSubmitButton\0"
    "onClickedBackButton\0onFilterChanged\0"
    "_item\0onSeasonChanged\0onCheckBoxChecked\0"
    "_checked\0onTreeNodeClicked\0QTreeWidgetItem*\0"
    "_column\0onResetDateEditWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLOVise__PLMColorSearch[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x08 /* Private */,
       4,    0,   57,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    1,   59,    2, 0x08 /* Private */,
       8,    1,   62,    2, 0x08 /* Private */,
       9,    1,   65,    2, 0x08 /* Private */,
      11,    2,   68,    2, 0x08 /* Private */,
      14,    0,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,    7,   13,
    QMetaType::Void,

       0        // eod
};

void CLOVise::PLMColorSearch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PLMColorSearch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onHideHirarchy((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->onClickedSubmitButton(); break;
        case 2: _t->onClickedBackButton(); break;
        case 3: _t->onFilterChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onSeasonChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onCheckBoxChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->onTreeNodeClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->onResetDateEditWidget(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CLOVise::PLMColorSearch::staticMetaObject = { {
    QMetaObject::SuperData::link<MVDialog::staticMetaObject>(),
    qt_meta_stringdata_CLOVise__PLMColorSearch.data,
    qt_meta_data_CLOVise__PLMColorSearch,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLOVise::PLMColorSearch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLOVise::PLMColorSearch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLOVise__PLMColorSearch.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::ColorSearch"))
        return static_cast< Ui::ColorSearch*>(this);
    return MVDialog::qt_metacast(_clname);
}

int CLOVise::PLMColorSearch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MVDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
