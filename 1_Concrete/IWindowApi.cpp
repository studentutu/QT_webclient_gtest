//
// Created by rfedor on 25.12.2021.
//

#include <QApplication>
#include "../0_Abstract/IWindowApi.h"

QApplication* actualQtApplication = nullptr;

void IWindowApi::CreateWindow(int argc, char *argv[]) {

    if(actualQtApplication == nullptr)
    {
        actualQtApplication = new QApplication(argc, argv);
    }
}
