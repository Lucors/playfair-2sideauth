#include <QApplication>
#include "playfairauth.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CAuthMaster *authMaster = new CAuthMaster();
    authMaster->show();

//    Client *client = new Client();
//    client->show();

//    KeyEditor *key = new KeyEditor();
//    key->show();

    return app.exec();
}
