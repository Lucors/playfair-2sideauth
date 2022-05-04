#include "playfairauth.h"
#include "ui_keyeditor.h"
#include "ui_client.h"
#include "ui_authmaster.h"


static QVector<QString> responseParser(QString rawInput){
    QVector<QString> output;
    QString buffer;
    bool number = rawInput[0].isDigit();

    for (QChar ch : rawInput){
        if ((!number && !ch.isDigit()) || (number && ch.isDigit())){
            buffer.append(ch);
            continue;
        }

        if ((!number && ch.isDigit()) || (number && !ch.isDigit())){
            number = !number;
            output.append(buffer);
            buffer.clear();
            buffer.append(ch);
            continue;
        }
    }
    if (!buffer.isEmpty()){
        output.append(buffer);
    }
    return output;
}


CPlayfairCipher::CPlayfairCipher(QString keyword){
//    //English only
//    this->alphabet  = new QString("ABCDEFGHIKLMNOPQRSTUVWXYZ");
    this->alphabet = new QString("1234567890 "\
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                                 "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ");
    this->rows      = new int(7);
    this->columns   = new int(this->alphabet->size() / *this->rows);
    this->keyword   = new QString();
    this->key       = new QString();
//    this->key       = new QVector<QVector<QChar>>();
    this->setKeyWord(keyword);
}
CPlayfairCipher::~CPlayfairCipher(){
    delete this->alphabet;
    delete this->keyword;
    delete this->columns;
    delete this->rows;
    delete this->key;
}
QString CPlayfairCipher::setKeyWord(QString keyword){
    *this->keyword = keyword.toUpper();

    QString rawKey = *this->keyword + *this->alphabet;
    rawKey = rawKey.replace('J', 'J');
//            QString preparedKey{};
    this->key->clear();
    for (QChar ch : rawKey){
        if (this->key->contains(ch)){
            continue;
        }
        if (this->alphabet->contains(ch)){
            this->key->append(ch);
        }
    }

//            for (int i = 0, l= 0; i < *this->rows; i++){
//                for (int j = 0; j < *this->columns; j++, l++){
//                    this->key->operator[](i)[j] = this->k->at(l);
//                }
//            }

    return this->getKey();
}
QString CPlayfairCipher::getKey(){
    return *(this->key);
}
QString CPlayfairCipher::getKeyWord(){
    return *(this->keyword);
}
QString CPlayfairCipher::getAlphabet(){
    return *(this->alphabet);
}
int CPlayfairCipher::getRowCount(){
    return *(this->rows);
}
int CPlayfairCipher::getColumnCount(){
    return *(this->columns);
}
QString CPlayfairCipher::encode(QString rawInput){
    rawInput = rawInput.toUpper().trimmed();
    QString preparedInput{};
    for (int i = 0; i < rawInput.size(); i++){
        if (!this->alphabet->contains(rawInput[i])){
            continue;
        }
        preparedInput.append(rawInput[i]);
        if (i+1 < rawInput.size()){
            if (rawInput[i] == rawInput[i+1]){
                preparedInput.append('X');
            }
        }
    }
    if (preparedInput.size() % 2 != 0){
        preparedInput.append('X');
    }
//            qDebug() << preparedInput;

    QString encoded{};
    for (int i = 0; i < preparedInput.size(); i += 2){
        int indA = this->key->indexOf(preparedInput[i]);
        int indB = this->key->indexOf(preparedInput[i+1]);
        if (indA == -1 || indB == -1){
            continue;
        }
        int rowA    = static_cast<int>(indA / *this->columns);
        int columnA = indA % *this->columns;
        int rowB    = static_cast<int>(indB / *this->columns);
        int columnB = indB % *this->columns;

//        qDebug() << "CH[" << rawInput[i] << "] indx " << indA;
//        qDebug() << "CH[" << preparedInput[i] << "] at " << rowA << ' ' << columnA;
//        qDebug() << "CH[" << preparedInput[i+1] << "] at " << rowB << ' ' << columnB;
//        qDebug() << "CH[" << rawInput[i] << "] restored " << rowA*(*this->rows)+columnA;

        //Одна строка
        if (rowA == rowB){
            columnA = (columnA+1)%(*this->columns);
            rowA    = (rowA % (*this->rows)) * (*this->columns);
            encoded.append(this->key->at(rowA + columnA));

            columnB = (columnB+1)%(*this->columns);
            rowB    = (rowB % (*this->rows)) * (*this->columns);
            encoded.append(this->key->at(rowB + columnB));
            continue;
        }
        //Один столбец
        if (columnA == columnB){
            columnA = columnA % (*this->columns);
            rowA    = ((rowA + 1) % (*this->rows)) * (*this->columns);
            encoded.append(this->key->at(rowA + columnA));

            columnB = columnB % (*this->columns);
            rowB    = ((rowB + 1) % (*this->rows)) * (*this->columns);
            encoded.append(this->key->at(rowB + columnB));
            continue;
        }
        encoded.append(this->key->at(rowA*(*this->columns) + columnB % (*this->columns)));
        encoded.append(this->key->at(rowB*(*this->columns) + columnA % (*this->columns)));
    }

    return encoded;
}
QString CPlayfairCipher::decode(QString rawInput, bool clearX){
    QString decoded{};
    QString preparedInput{rawInput}; //Prepare raw?
    for (int i = 0; i < preparedInput.size(); i += 2){
        int indA = this->key->indexOf(preparedInput[i]);
        int indB = this->key->indexOf(preparedInput[i+1]);
        if (indA == -1 || indB == -1){
            continue;
        }
        int rowA    = static_cast<int>(indA / *this->columns);
        int columnA = indA % *this->columns;
        int rowB    = static_cast<int>(indB / *this->columns);
        int columnB = indB % *this->columns;

        //Одна строка
        if (rowA == rowB){
            if ((columnA-1) < 0){
                columnA = (*this->columns);
            }
            if ((columnB-1) < 0){
                columnB = (*this->columns);
            }
            decoded.append(this->key->at(rowA*(*this->columns) + (columnA-1)%(*this->columns)));
            decoded.append(this->key->at(rowB*(*this->columns) + (columnB-1)%(*this->columns)));
            continue;
        }
        //Один столбец
        if (columnA == columnB){
            if ((rowA-1) < 0){
                rowA = (*this->rows);
            }
            if ((rowB-1) < 0){
                rowB = (*this->rows);
            }

            columnA = columnA % (*this->columns);
            rowA    = ((rowA - 1) % (*this->rows)) * (*this->columns);
            decoded.append(this->key->at(rowA + columnA));

            columnB = columnB % (*this->columns);
            rowB    = ((rowB - 1) % (*this->rows)) * (*this->columns);
            decoded.append(this->key->at(rowB + columnB));
            continue;
        }
        decoded.append(this->key->at(rowA*(*this->columns) + columnB % (*this->columns)));
        decoded.append(this->key->at(rowB*(*this->columns) + columnA % (*this->columns)));
    }
    if (clearX){
        preparedInput = decoded;
        decoded.clear();
        for (int i = 0; i < preparedInput.size(); i++){
            if (preparedInput[i] == 'X'){
                if ((i-1 < 0) && (i+1 > preparedInput.size())){
                    decoded.append('X');
                    continue;
                }
                if (preparedInput[i-1] == preparedInput[i+1]){
                    continue;
                }
            }
            decoded.append(preparedInput[i]);
        }
    }
    return decoded;
}


CKeyEditor::CKeyEditor(QWidget *parent):QWidget(parent),
    ui(new Ui::CKeyEditor){
    ui->setupUi(this);
    this->cipher = new CPlayfairCipher();
    ui->leKeyInput->setText(this->cipher->getKeyWord());

    for (int i = 0, l = 0; i < this->cipher->getRowCount(); i++){
        for (int j = 0; j < this->cipher->getColumnCount(); j++, l++){
            ui->keyTable->setItem(i, j, new QTableWidgetItem("Null"));
        }
    }

    CKeyEditor::connect(ui->pbApplyKey, &QPushButton::clicked, this, &CKeyEditor::applyKey);
    this->applyKey();
}
CKeyEditor::~CKeyEditor(){
    delete ui;
    delete this->cipher;
}
void CKeyEditor::closeEvent(QCloseEvent *event){
    emit this->closed();
    event->accept();
//    this->destroy();
}
void CKeyEditor::applyKey(){
    this->cipher->setKeyWord(ui->leKeyInput->text());
    QString contentKey = this->cipher->getKey();
    for (int i = 0, l = 0; i < this->cipher->getRowCount(); i++){
        for (int j = 0; j < this->cipher->getColumnCount(); j++, l++){
            ui->keyTable->item(i, j)->setText(contentKey.at(l));
        }
    }
}
//CPlayfairCipher *CKeyEditor::getCipher(){
//    return this->cipher;
//}


CClient::CClient(CAuthMaster *master, bool isEve, QWidget *parent):QWidget(parent),
    ui(new Ui::CClient){
    ui->setupUi(this);
    this->eveFlag    = isEve;
    this->master     = master;
    this->clientName = new QString();
    this->keyEditor  = new CKeyEditor();
    if(!isEve){
        ui->lblEve->close();
    }
    this->setClientName("Клиент", false);
    generateSessionRandom();

    this->updateClientsCombo(master->getClientsList());
    CClient::connect(master, &CAuthMaster::clientsUpdated, this, &CClient::updateClientsCombo);
    CClient::connect(ui->pbAuth, &QPushButton::clicked, this, &CClient::startSession);
    CClient::connect(ui->pbShowKey, &QPushButton::clicked, this, [&](){
        this->keyEditor->show();
        this->setEnabled(false);
    });
    CClient::connect(this->keyEditor, &CKeyEditor::closed, this, [&](){
        this->setEnabled(true);
    });
    CClient::connect(ui->leClientName, &QLineEdit::editingFinished, this, [&](){
        this->setClientName(ui->leClientName->text());
    });
}
CClient::~CClient(){
    delete ui;
    delete this->clientName;
    delete this->keyEditor;
}
void CClient::updateClientsCombo(QVector<CClient *> clients){
    ui->cmbClients->clear();
    QString clientName{};
    for (int i = 0; i < clients.size(); i++){
        clientName.clear();
        if (clients.at(i) != const_cast<CClient *>(this)){
            if (clients.at(i)->isEve()){
                clientName.append("Eve:");
            }
            clientName.append(clients.at(i)->getClientName());
            ui->cmbClients->addItem(clientName);
            continue;
        }
        this->index = i;
        ui->cmbClients->addItem("This");
    }
}
void CClient::closeEvent(QCloseEvent *event){
    this->keyEditor->close();
    emit this->closed(this);
    event->accept();
}
void CClient::setAuthControlEnabled(bool enabled){
    ui->cmbClients->setEnabled(enabled);
    ui->pbAuth->setEnabled(enabled);
    ui->pbShowKey->setEnabled(enabled);
}
void CClient::setClientName(QString name, bool emitSignal){
    QString nameBuffer;
    for (QChar ch : name){
        if (!ch.isDigit()){
            nameBuffer.append(ch);
        }
    }
    *(this->clientName) = nameBuffer;
    ui->leClientName->setText(nameBuffer);
    this->setWindowTitle(nameBuffer);
    if (emitSignal){
        emit nameChanged(this);
    }
}
QString CClient::getClientName(){
    return *(this->clientName);
}
void CClient::generateSessionRandom(){
    this->sessionRandom = QRandomGenerator::global()->bounded(1, 500);
    ui->lcdSessionRandom->display(this->sessionRandom);
}
void CClient::startSession(){
//    this->isSessionMaster = true;
    if (ui->cmbClients->count() == 0){
        return;
    }
    if (this->index == ui->cmbClients->currentIndex()){
        return;
    }
    this->generateSessionRandom();
    this->setAuthControlEnabled(false);
    this->partner = this->master->getClientsList().operator[](ui->cmbClients->currentIndex());

    ui->leRequest->setText(QString::number(this->sessionRandom));
    QString rawResponse = this->partner->sessionRequest(QString::number(this->sessionRandom)); //r1 -> r1+B+r2
    rawResponse = this->keyEditor->cipher->decode(rawResponse, true);
    ui->leResponse->setText(rawResponse);
    QVector<QString> preparedResponse = responseParser(rawResponse);

    try {
        int r1 = preparedResponse[0].toInt();
        int r2 = preparedResponse[2].toInt();
        if (this->sessionRandom != r1){
            throw QString("Невалид. r1");
        }
        if (QString::compare(this->partner->getClientName(), preparedResponse[1], Qt::CaseInsensitive) != 0){
            throw QString("Невалид. clientName");
        }
//        if (this->partner->getClientName() != preparedResponse[1]){
//            throw QString("Невалид. clientName");
//        }
        QString tmpRequest = this->keyEditor->cipher->encode(QString::number(r1)+' '+QString::number(r2));
        bool success = this->partner->sessionResponse(tmpRequest);//r2 -> True/False
        if (!success){
            throw QString("Ошибка стороны А");
        }
        ui->lblStatus->setText("Успех");
    }
    catch (QString errmsg){
        this->partner->fall();
        ui->lblStatus->setText(errmsg);
    }
    this->setAuthControlEnabled(true);
}
bool CClient::isEve(){
    return this->eveFlag;
}
QString CClient::sessionRequest(QString rawInput){
    ui->lblStatus->setText("");
//    this->isSessionMaster = false;
    this->generateSessionRandom();
    this->setAuthControlEnabled(false);
//    QVector<QString> preparedInput = responseParser(rawInput);

    QString tmpRequest = rawInput+this->clientName+QString::number(this->sessionRandom);
    ui->leRequest->setText(tmpRequest);
    return this->keyEditor->cipher->encode(tmpRequest);
}
bool CClient::sessionResponse(QString rawInput){
//    this->isSessionMaster = false;
    rawInput = this->keyEditor->cipher->decode(rawInput, true);
    ui->leResponse->setText(rawInput);
    QVector<QString> preparedInput = responseParser(rawInput);
    try {
        int r2 = preparedInput[2].toInt();
        if (r2 != this->sessionRandom){
            throw QString("Невалид. r2");
        }
    }
    catch (QString errmsg){
        ui->lblStatus->setText(errmsg);
        return false;
    }
    this->setAuthControlEnabled(true);
    ui->lblStatus->setText("Успех");
    return true;
}
void CClient::fall(){
    if (ui->lblStatus->text().isEmpty()){
        ui->lblStatus->setText("Ошибка стороны B");
    }
    this->setAuthControlEnabled(true);
}



CAuthMaster::CAuthMaster(QWidget *parent):QMainWindow(parent),
    ui(new Ui::CAuthMaster){
    ui->setupUi(this);
    this->clients = new QVector<CClient *>();

    CAuthMaster::connect(ui->pbCreateClient, &QPushButton::clicked,
                         this, &CAuthMaster::createClient);
    CAuthMaster::connect(ui->pbCreateEve, &QPushButton::clicked, [&](){
        this->createClient(true);
    });
    CAuthMaster::connect(this, &CAuthMaster::clientsUpdated,
                         this, &CAuthMaster::updateClientsCombo);
}
CAuthMaster::~CAuthMaster(){
    delete ui;
    delete this->clients;
}
void CAuthMaster::createClient(bool isEve){
    CClient *client = new CClient(this, isEve);
    this->clients->append(client);
    ui->cmbClients->addItem(client->getClientName());
    client->show();

    CAuthMaster::connect(client, &CClient::closed, this, &CAuthMaster::removeClient);
    CAuthMaster::connect(client, &CClient::nameChanged, this, &CAuthMaster::updateClient);
    emit this->clientsUpdated(*this->clients);
}
void CAuthMaster::removeClient(CClient *client){
//    ui->cmbClients->removeItem(index);
    int index = this->clients->indexOf(client);
    if (index == -1){
        return;
    }
    this->clients->remove(index);
    emit this->clientsUpdated(*this->clients);
}
void CAuthMaster::updateClient(CClient *client){
    int index = this->clients->indexOf(client);
    if (index == -1){
        return;
    }
    emit this->clientsUpdated(*this->clients);
}
QVector<CClient *> CAuthMaster::getClientsList(){
    return *(this->clients);
}
void CAuthMaster::updateClientsCombo(){
    ui->cmbClients->clear();
    QString clientName{};
    for (int i = 0; i < this->clients->size(); i++){
        clientName.clear();
        if (this->clients->at(i)->isEve()){
            clientName.append("Eve:");
        }
        clientName.append(this->clients->at(i)->getClientName());
        ui->cmbClients->addItem(clientName);
    }
}
void CAuthMaster::closeEvent(QCloseEvent *event){
    for (auto val = this->clients->begin(); val != this->clients->end(); val++){
        CAuthMaster::disconnect((*val), &CClient::closed, this, &CAuthMaster::removeClient);
        (*val)->close();
        delete (*val);
    }
    event->accept();
}
