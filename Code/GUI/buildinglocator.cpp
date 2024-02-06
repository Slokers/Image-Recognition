#include "buildinglocator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QHash>

BuildingLocator::BuildingLocator(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *imageLayout = new QHBoxLayout();
    mainLayout->addLayout(imageLayout);

    QStringList imagePaths;
    for (int i = 1; i <= 5; i++) {
        QString imagePath = QString("/images/testing/testing%1.jpg").arg(i);
        imagePaths.append(imagePath);
    }

    for (const QString &imagePath : imagePaths) {
        QPushButton *imageButton = new QPushButton(this);
        QPixmap pixmap(imagePath);
        QIcon icon(pixmap.scaled(200, 200, Qt::KeepAspectRatio));
        imageButton->setIcon(icon);
        imageButton->setIconSize(QSize(200, 200));
        imageLayout->addWidget(imageButton);

        connect(imageButton, &QPushButton::clicked, this, [this, imagePath]() {
            selectedImagePath = imagePath;
        });
    }

    locateButton = new QPushButton("Locate", this);
    mainLayout->addWidget(locateButton);
    connect(locateButton, &QPushButton::clicked, this, &BuildingLocator::onLocateButtonClicked);

    QHBoxLayout *imageLabelLayout = new QHBoxLayout();
    imageLabel = new QLabel(this);
    imageLabelLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(imageLabelLayout);

    QGroupBox *resultGroupBox = new QGroupBox("Result", this);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroupBox);
    mainLayout->addWidget(resultGroupBox);

    nameLabel = new QLabel(this);
    resultLayout->addWidget(nameLabel, 0, Qt::AlignCenter);

    scoreLabel = new QLabel(this);
    resultLayout->addWidget(scoreLabel, 0, Qt::AlignCenter);

    bestMatchImageIndexLabel = new QLabel(this);
    resultLayout->addWidget(bestMatchImageIndexLabel, 0, Qt::AlignCenter);

    numberOfGoodMatchesLabel = new QLabel(this);
    resultLayout->addWidget(numberOfGoodMatchesLabel, 0, Qt::AlignCenter);

    }

void BuildingLocator::onLocateButtonClicked() {
    if (!selectedImagePath.isEmpty()) {

        // Get the building name, best match image index, and number of good matches from the CSV data
        QHash<QString, QVariant> data = getDataForImage(selectedImagePath);
        nameLabel->setText("Building name: " + data["building_name"].toString());

        double score = data["score"].toDouble();
        scoreLabel->setText(QString("Score: %1").arg(score, 0, 'f', 2));

        int bestMatchImageIndex = data["best_match_image_index"].toInt();
        bestMatchImageIndexLabel->setText(QString("Best match image index: %1").arg(bestMatchImageIndex));

        int numberOfGoodMatches = data["number_of_good_matches"].toInt();
        numberOfGoodMatchesLabel->setText(QString("Number of good matches: %1").arg(numberOfGoodMatches));

        // Construct the output image path using the best match image index
        QString outputImagePath = QString("/images/training/training%1.jpg").arg(bestMatchImageIndex);
        QPixmap pixmap(outputImagePath);
        imageLabel->setPixmap(pixmap.scaled(400, 400, Qt::KeepAspectRatio));

    }
}

QHash<QString, QVariant> BuildingLocator::getDataForImage(const QString &imageName) {
    QFile file("\build\\output_restults.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        return QHash<QString, QVariant>();
    }

    QTextStream in(&file);
    QString headerLine = in.readLine();
    QStringList header = headerLine.split(',');

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() == header.size()) {
            if (fields[1].trimmed() == imageName) {
                QHash<QString, QVariant> data;
                data["score"] = fields[0].toDouble();
                data["input_image"] = fields[1].trimmed();
                data["best_match_image_index"] = fields[2].toInt();
                data["number_of_good_matches"] = fields[3].toInt();
                data["building_name"] = fields[4].trimmed();
                return data;
            }
        }
    }

    return QHash<QString, QVariant>();
}
