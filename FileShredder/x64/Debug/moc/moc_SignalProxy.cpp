/****************************************************************************
** Meta object code from reading C++ file 'SignalProxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../SignalProxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SignalProxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SignalProxy_t {
    QByteArrayData data[15];
    char stringdata0[188];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SignalProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SignalProxy_t qt_meta_stringdata_SignalProxy = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SignalProxy"
QT_MOC_LITERAL(1, 12, 20), // "signalUpdateListView"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 4), // "item"
QT_MOC_LITERAL(4, 39, 3), // "tag"
QT_MOC_LITERAL(5, 43, 16), // "signalMessageBox"
QT_MOC_LITERAL(6, 60, 5), // "title"
QT_MOC_LITERAL(7, 66, 4), // "text"
QT_MOC_LITERAL(8, 71, 4), // "type"
QT_MOC_LITERAL(9, 76, 21), // "signalSetListViewTags"
QT_MOC_LITERAL(10, 98, 10), // "currentTag"
QT_MOC_LITERAL(11, 109, 24), // "sendSignalUpdateListView"
QT_MOC_LITERAL(12, 134, 6), // "string"
QT_MOC_LITERAL(13, 141, 20), // "sendSignalMessageBox"
QT_MOC_LITERAL(14, 162, 25) // "sendSignalSetListViewTags"

    },
    "SignalProxy\0signalUpdateListView\0\0"
    "item\0tag\0signalMessageBox\0title\0text\0"
    "type\0signalSetListViewTags\0currentTag\0"
    "sendSignalUpdateListView\0string\0"
    "sendSignalMessageBox\0sendSignalSetListViewTags"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SignalProxy[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    3,   49,    2, 0x06 /* Public */,
       9,    2,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    2,   61,    2, 0x0a /* Public */,
      13,    3,   66,    2, 0x0a /* Public */,
      14,    2,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    4,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,    3,    4,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12, 0x80000000 | 12,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,    4,   10,

       0        // eod
};

void SignalProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SignalProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalUpdateListView((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->signalMessageBox((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->signalSetListViewTags((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->sendSignalUpdateListView((*reinterpret_cast< const string(*)>(_a[1])),(*reinterpret_cast< const string(*)>(_a[2]))); break;
        case 4: _t->sendSignalMessageBox((*reinterpret_cast< const string(*)>(_a[1])),(*reinterpret_cast< const string(*)>(_a[2])),(*reinterpret_cast< const string(*)>(_a[3]))); break;
        case 5: _t->sendSignalSetListViewTags((*reinterpret_cast< const string(*)>(_a[1])),(*reinterpret_cast< const string(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SignalProxy::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SignalProxy::signalUpdateListView)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SignalProxy::*)(const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SignalProxy::signalMessageBox)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SignalProxy::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SignalProxy::signalSetListViewTags)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SignalProxy::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SignalProxy.data,
    qt_meta_data_SignalProxy,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SignalProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SignalProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SignalProxy.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SignalProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SignalProxy::signalUpdateListView(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SignalProxy::signalMessageBox(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SignalProxy::signalSetListViewTags(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
