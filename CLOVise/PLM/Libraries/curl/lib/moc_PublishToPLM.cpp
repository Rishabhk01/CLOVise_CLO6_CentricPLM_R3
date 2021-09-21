/****************************************************************************
** Meta object code from reading C++ file 'PublishToPLM.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../CLOVise/PLM/Outbound/PublishToPLM/PublishToPLM.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PublishToPLM.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLOVise__PublishToPLM_t {
    QByteArrayData data[8];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLOVise__PublishToPLM_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLOVise__PublishToPLM_t qt_meta_stringdata_CLOVise__PublishToPLM = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CLOVise::PublishToPLM"
QT_MOC_LITERAL(1, 22, 21), // "onCancelWindowClicked"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 19), // "onResolutionChanged"
QT_MOC_LITERAL(4, 65, 13), // "_changedValue"
QT_MOC_LITERAL(5, 79, 21), // "onPublishToPLMClicked"
QT_MOC_LITERAL(6, 101, 30), // "onImagePageCurrentIndexChanged"
QT_MOC_LITERAL(7, 132, 21) // "onResetDateEditWidget"

    },
    "CLOVise::PublishToPLM\0onCancelWindowClicked\0"
    "\0onResolutionChanged\0_changedValue\0"
    "onPublishToPLMClicked\0"
    "onImagePageCurrentIndexChanged\0"
    "onResetDateEditWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLOVise__PublishToPLM[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       5,    0,   43,    2, 0x08 /* Private */,
       6,    1,   44,    2, 0x08 /* Private */,
       7,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

       0        // eod
};

void CLOVise::PublishToPLM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PublishToPLM *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onCancelWindowClicked(); break;
        case 1: _t->onResolutionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->onPublishToPLMClicked(); break;
        case 3: _t->onImagePageCurrentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onResetDateEditWidget(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CLOVise::PublishToPLM::staticMetaObject = { {
    QMetaObject::SuperData::link<MVDialog::staticMetaObject>(),
    qt_meta_stringdata_CLOVise__PublishToPLM.data,
    qt_meta_data_CLOVise__PublishToPLM,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLOVise::PublishToPLM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLOVise::PublishToPLM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLOVise__PublishToPLM.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::PublishToPLM"))
        return static_cast< Ui::PublishToPLM*>(this);
    return MVDialog::qt_metacast(_clname);
}

int CLOVise::PublishToPLM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MVDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
