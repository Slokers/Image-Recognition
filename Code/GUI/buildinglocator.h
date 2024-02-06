#ifndef BUILDINGLOCATOR_H
#define BUILDINGLOCATOR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class BuildingLocator : public QWidget
{
    Q_OBJECT

public:
    explicit BuildingLocator(QWidget *parent = nullptr);

private slots:
    void onLocateButtonClicked();

private:
    QString getNameForImage(const QString &imageName);
    double randomDouble(double minValue, double maxValue);
    QHash<QString, QVariant> getDataForImage(const QString &imageName);

    QPushButton *locateButton;
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLabel *scoreLabel;

    QLabel *bestMatchImageIndexLabel;
    QLabel *numberOfGoodMatchesLabel;

    // Declare the missing variable
    QString selectedImagePath;
};

#endif // BUILDINGLOCATOR_H
