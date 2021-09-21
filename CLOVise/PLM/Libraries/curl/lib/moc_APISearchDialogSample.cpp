/****************************************************************************
** Meta object code from reading C++ file 'APISearchDialogSample.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../classes/APISearchDialogSample.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'APISearchDialogSample.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLOAPISample__APISearchDialogSample_t {
    QByteArrayData data[6];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLOAPISample__APISearchDialogSample_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLOAPISample__APISearchDialogSample_t qt_meta_stringdata_CLOAPISample__APISearchDialogSample = {
    {
QT_MOC_LITERAL(0, 0, 35), // "CLOAPISample::APISearchDialog..."
QT_MOC_LITERAL(1, 36, 8), // "OnAccept"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 8), // "OnReject"
QT_MOC_LITERAL(4, 55, 25), // "OnServiceDivisionComboBox"
QT_MOC_LITERAL(5, 81, 6) // "_index"

    },
    "CLOAPISample::APISearchDialogSample\0"
    "OnAccept\0\0OnReject\0OnServiceDivisionComboBox\0"
    "_index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLOAPISample__APISearchDialogSample[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    1,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void CLOAPISample::APISearchDialogSample::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<APISearchDialogSample *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnAccept(); break;
        case 1: _t->OnReject(); break;
        case 2: _t->OnServiceDivisionComboBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CLOAPISample::APISearchDialogSample::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLOAPISample__APISearchDialogSample.data,
    qt_meta_data_CLOAPISample__APISearchDialogSample,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLOAPISample::APISearchDialogSample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLOAPISample::APISearchDialogSample::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLOAPISample__APISearchDialogSample.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::DialogSearch"))
        return static_cast< Ui::DialogSearch*>(this);
    return QDialog::qt_metacast(_clname);
}

int CLOAPISample::APISearchDialogSample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
