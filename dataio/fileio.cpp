#include "fileio.h"
#include <QFile>
#include <QTextStream>

Fileio::Fileio() {
    QFile f("/home/towdium/test.txt");

    f.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream fs(&f);
    int countY = 0;
    int countX = 0;
    while (!fs.atEnd()) {
        QString line = fs.readLine();
        QTextStream ss(&line);
        countX = 0;
        while (!ss.atEnd()) {
            float f;
            ss >> f;
            this->data << f;
            countX++;
        }
        countY++;
    }
    f.close();
    this->sizeX = countX;
    this->sizeY = countY;
}

const float *Fileio::getData() {
    return(this->data.constData());
}

int Fileio::getSizeX() {
    return(this->sizeX);
}

int Fileio::getSizeY() {
    return(this->sizeY);
}
