#include <QApplication>
#include "buildinglocator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BuildingLocator buildingLocator;
    buildingLocator.show();

    return app.exec();
}
