/****************************************************************************
** Meta object code from reading C++ file 'evita4_vent.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "evita4_vent.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'evita4_vent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Evita4_vent_t {
    QByteArrayData data[10];
    char stringdata0[176];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Evita4_vent_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Evita4_vent_t qt_meta_stringdata_Evita4_vent = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Evita4_vent"
QT_MOC_LITERAL(1, 12, 14), // "process_buffer"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "request_icc"
QT_MOC_LITERAL(4, 40, 14), // "request_dev_id"
QT_MOC_LITERAL(5, 55, 23), // "request_measurement_cp1"
QT_MOC_LITERAL(6, 79, 23), // "request_measurement_cp2"
QT_MOC_LITERAL(7, 103, 23), // "request_device_settings"
QT_MOC_LITERAL(8, 127, 21), // "request_text_messages"
QT_MOC_LITERAL(9, 149, 26) // "request_stop_communication"

    },
    "Evita4_vent\0process_buffer\0\0request_icc\0"
    "request_dev_id\0request_measurement_cp1\0"
    "request_measurement_cp2\0request_device_settings\0"
    "request_text_messages\0request_stop_communication"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Evita4_vent[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    0,   57,    2, 0x0a /* Public */,
       6,    0,   58,    2, 0x0a /* Public */,
       7,    0,   59,    2, 0x0a /* Public */,
       8,    0,   60,    2, 0x0a /* Public */,
       9,    0,   61,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Evita4_vent::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Evita4_vent *_t = static_cast<Evita4_vent *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->process_buffer(); break;
        case 1: _t->request_icc(); break;
        case 2: _t->request_dev_id(); break;
        case 3: _t->request_measurement_cp1(); break;
        case 4: _t->request_measurement_cp2(); break;
        case 5: _t->request_device_settings(); break;
        case 6: _t->request_text_messages(); break;
        case 7: _t->request_stop_communication(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Evita4_vent::staticMetaObject = {
    { &Device::staticMetaObject, qt_meta_stringdata_Evita4_vent.data,
      qt_meta_data_Evita4_vent,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Evita4_vent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Evita4_vent::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Evita4_vent.stringdata0))
        return static_cast<void*>(this);
    return Device::qt_metacast(_clname);
}

int Evita4_vent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Device::qt_metacall(_c, _id, _a);
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
