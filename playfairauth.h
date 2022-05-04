#ifndef PLAYFAIRAUTH_H
#define PLAYFAIRAUTH_H

#define DEBUG_MODE
#ifdef DEBUG_MODE
    #include <QDebug>
#endif

#include <QMainWindow>
#include <QWidget>
#include <QString>
//#include <QVector>
#include <QCloseEvent>
#include <QRandomGenerator>
#include <QLCDNumber>
#include <QTabWidget>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class CKeyEditor;
    class CClient;
    class CAuthMaster;
}
QT_END_NAMESPACE

class CPlayfairCipher;
class CKeyEditor;
class CClient;
class CAuthMaster;

static QVector<QString> responseParser(QString);

///@brief Класс шифровальщика
class CPlayfairCipher {
    private:
        QString *keyword, *key;
        QString *alphabet;
        int *rows, *columns;
    public:
        CPlayfairCipher(QString keyword = "WATSON");
        ~CPlayfairCipher();
        QString setKeyWord(QString);
        QString getKey();
        QString getKeyWord();
        QString getAlphabet();
        int getRowCount();
        int getColumnCount();
        QString encode(QString);
        QString decode(QString, bool clearX = false);
};

///@brief Класс UI редактора ключа
class CKeyEditor : public QWidget {
    Q_OBJECT
    private:
        Ui::CKeyEditor *ui;
        CPlayfairCipher *cipher;
    private:
        void closeEvent(QCloseEvent *) override;
    public:
        CKeyEditor(QWidget *parent = nullptr);
        ~CKeyEditor() override;
        void applyKey();
//        CPlayfairCipher *getCipher();
    signals:
        void closed();
    friend class CClient;
};

///@brief Класс UI клиента
class CClient : public QWidget {
    Q_OBJECT
    private:
        Ui::CClient *ui;
        int         sessionRandom, index;
        QString     *clientName;
        CKeyEditor  *keyEditor;
        CClient     *partner;
        CAuthMaster *master;
        bool        eveFlag = false;
//        bool isSessionMaster = false;
    private:
        void updateClientsCombo(QVector<CClient *>);
        void closeEvent(QCloseEvent *) override;
        void setAuthControlEnabled(bool);
    public:
        CClient(CAuthMaster *, bool isEve = false, QWidget *parent = nullptr);
        ~CClient() override;
        void setClientName(QString, bool emitSignal = true);
        QString getClientName();
        void generateSessionRandom();
        void startSession();
        bool isEve();
    public slots:
        QString sessionRequest(QString);
        bool sessionResponse(QString);
        void fall();
    signals:
        void closed(CClient *);
        void nameChanged(CClient *);
};

//class CEve : public CClient {
//    Q_OBJECT
//    private:
//        Ui::CClient *ui;
//    public:
//        CEve(CAuthMaster *, QWidget *parent = nullptr);
//        ~CEve() override;
//};

///@brief Класс UI QMainWindow
class CAuthMaster : public QMainWindow {
    Q_OBJECT
    private:
        Ui::CAuthMaster *ui;
        QVector<CClient *> *clients;
        void updateClientsCombo();
        void closeEvent(QCloseEvent *) override;
    public:
        CAuthMaster(QWidget *parent = nullptr);
        ~CAuthMaster() override;
        void createClient(bool isEve = false);
        void removeClient(CClient *);
        void updateClient(CClient *);
        QVector<CClient *> getClientsList();
    signals:
        void clientsUpdated(QVector<CClient *>);
//        void clientCreated(QString);
//        void clientRemoved(int);
//    friend class CClient;
};
#endif // PLAYFAIRAUTH_H
