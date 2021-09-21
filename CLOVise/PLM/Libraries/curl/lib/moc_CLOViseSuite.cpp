/****************************************************************************
** Meta object code from reading C++ file 'CLOViseSuite.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../CLOVise/PLM/CLOViseSuite/CLOViseSuite.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CLOViseSuite.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLOVise__CLOViseSuite_t {
    QByteArrayData data[9];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLOVise__CLOViseSuite_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLOVise__CLOViseSuite_t qt_meta_stringdata_CLOVise__CLOViseSuite = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CLOVise::CLOViseSuite"
QT_MOC_LITERAL(1, 22, 16), // "onMaterialSearch"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 19), // "onClickedSearchProd"
QT_MOC_LITERAL(4, 60, 21), // "onClickedSearchColors"
QT_MOC_LITERAL(5, 82, 23), // "onClickedSearchDocument"
QT_MOC_LITERAL(6, 106, 21), // "onClickedPublishToPLM"
QT_MOC_LITERAL(7, 128, 15), // "onClickedLogout"
QT_MOC_LITERAL(8, 144, 17) // "onClickedSettings"

    },
    "CLOVise::CLOViseSuite\0onMaterialSearch\0"
    "\0onClickedSearchProd\0onClickedSearchColors\0"
    "onClickedSearchDocument\0onClickedPublishToPLM\0"
    "onClickedLogout\0onClickedSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLOVise__CLOViseSuite[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CLOVise::CLOViseSuite::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CLOViseSuite *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onMaterialSearch(); break;
        case 1: _t->onClickedSearchProd(); break;
        case 2: _t->onClickedSearchColors(); break;
        case 3: _t->onClickedSearchDocument(); break;
        case 4: _t->onClickedPublishToPLM(); break;
        case 5: _t->onClickedLogout(); break;
        case 6: _t->onClickedSettings(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CLOVise::CLOViseSuite::staticMetaObject = { {
    QMetaObject::SuperData::link<MVDialog::staticMetaObject>(),
    qt_meta_stringdata_CLOVise__CLOViseSuite.data,
    qt_meta_data_CLOVise__CLOViseSuite,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLOVise::CLOViseSuite::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLOVise::CLOViseSuite::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLOVise__CLOViseSuite.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::CLOViseSuite"))
        return static_cast< Ui::CLOViseSuite*>(this);
    return MVDialog::qt_metacast(_clname);
}

int CLOVise::CLOViseSuite::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MVDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
