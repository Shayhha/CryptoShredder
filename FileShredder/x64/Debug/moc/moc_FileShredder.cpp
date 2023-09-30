/****************************************************************************
** Meta object code from reading C++ file 'FileShredder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../FileShredder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileShredder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileShredder_t {
    QByteArrayData data[21];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileShredder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileShredder_t qt_meta_stringdata_FileShredder = {
    {
QT_MOC_LITERAL(0, 0, 12), // "FileShredder"
QT_MOC_LITERAL(1, 13, 9), // "wipeFiles"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "cancelWipe"
QT_MOC_LITERAL(4, 35, 14), // "openFileDialog"
QT_MOC_LITERAL(5, 50, 13), // "clearContents"
QT_MOC_LITERAL(6, 64, 17), // "addItemToListView"
QT_MOC_LITERAL(7, 82, 4), // "item"
QT_MOC_LITERAL(8, 87, 14), // "updateListView"
QT_MOC_LITERAL(9, 102, 3), // "tag"
QT_MOC_LITERAL(10, 106, 12), // "checkThreads"
QT_MOC_LITERAL(11, 119, 15), // "setListViewTags"
QT_MOC_LITERAL(12, 135, 10), // "currentTag"
QT_MOC_LITERAL(13, 146, 17), // "doubleClickedFile"
QT_MOC_LITERAL(14, 164, 11), // "QModelIndex"
QT_MOC_LITERAL(15, 176, 5), // "index"
QT_MOC_LITERAL(16, 182, 14), // "showMessageBox"
QT_MOC_LITERAL(17, 197, 27), // "QMessageBox::StandardButton"
QT_MOC_LITERAL(18, 225, 5), // "title"
QT_MOC_LITERAL(19, 231, 4), // "text"
QT_MOC_LITERAL(20, 236, 4) // "type"

    },
    "FileShredder\0wipeFiles\0\0cancelWipe\0"
    "openFileDialog\0clearContents\0"
    "addItemToListView\0item\0updateListView\0"
    "tag\0checkThreads\0setListViewTags\0"
    "currentTag\0doubleClickedFile\0QModelIndex\0"
    "index\0showMessageBox\0QMessageBox::StandardButton\0"
    "title\0text\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileShredder[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    1,   73,    2, 0x08 /* Private */,
       8,    2,   76,    2, 0x08 /* Private */,
      10,    0,   81,    2, 0x08 /* Private */,
      11,    2,   82,    2, 0x08 /* Private */,
      11,    1,   87,    2, 0x28 /* Private | MethodCloned */,
      13,    1,   90,    2, 0x08 /* Private */,
      16,    3,   93,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   12,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, 0x80000000 | 14,   15,
    0x80000000 | 17, QMetaType::QString, QMetaType::QString, QMetaType::QString,   18,   19,   20,

       0        // eod
};

void FileShredder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileShredder *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wipeFiles(); break;
        case 1: _t->cancelWipe(); break;
        case 2: _t->openFileDialog(); break;
        case 3: _t->clearContents(); break;
        case 4: _t->addItemToListView((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->updateListView((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->checkThreads(); break;
        case 7: _t->setListViewTags((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->setListViewTags((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->doubleClickedFile((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 10: { QMessageBox::StandardButton _r = _t->showMessageBox((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QMessageBox::StandardButton*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileShredder::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_FileShredder.data,
    qt_meta_data_FileShredder,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileShredder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileShredder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileShredder.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int FileShredder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
