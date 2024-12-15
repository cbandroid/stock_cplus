#include "mainwindow.h"
#include <Windows.h>
//#include <DbgHelp.h>
//#include <QMessageBox>
#include <QNetworkInterface>
#include <QApplication>
#include <QSharedMemory>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

//LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
//{
//    HANDLE hDumpFile = CreateFile(L"crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//    if(hDumpFile != INVALID_HANDLE_VALUE)
//    {
//        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
//        dumpInfo.ExceptionPointers = pException;
//        dumpInfo.ThreadId = GetCurrentThreadId();
//        dumpInfo.ClientPointers = TRUE;
//        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
//    }
//    QMessageBox msgBox;
//    msgBox.setText("application crash!");
//    msgBox.exec();

//    return EXCEPTION_EXECUTE_HANDLER;
//}
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    static QSharedMemory *shareMem = new QSharedMemory( "oneStock",&a);
    if ( !shareMem->create( 1 ) )
    {
        qApp->quit();
        return -1;
    }

    Q_UNUSED(a);
    a.setQuitLockEnabled(false);

    // 确保OpenSSL的线程安全性
    OpenSSL_add_all_algorithms();

    MainWindow *w = new MainWindow ;
    
    a.installEventFilter(w);
    w->showMaximized();

    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

    return a.exec();

}
