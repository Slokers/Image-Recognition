/*
    Computer Science
    Final Year Project Maynooth University
    Rajendr Singh
    19402416
*/


#include <iostream>
#include <vector>
#include <limits>
#include <sstream>
// DBoW2
#include "DBoW2/DBoW2.h" 

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>


using namespace DBoW2;
using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// Use std::filesystem if the compiler supports it
#if __cplusplus >= 201703L && (!defined(__GNUC__) || (__GNUC__ * 100 + __GNUC_MINOR__) >= 800)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


void loadFeatures(vector<vector<cv::Mat>> &features, string path);
void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);
void vocabCreation(const vector<vector<cv::Mat>> &features);
void findBestMatch(const string &inputImagePath, const string &vocabPath, const vector<vector<cv::Mat>> &features, const vector<string> &buildingNames, string &output);
void loadBuildingNames(const string &csvPath, vector<string> &buildingNames);

int NIMAGES = 199;

int main() {
    vector<vector<cv::Mat>> features;
    string path;
    path = "/images/training";
    loadFeatures(features, path);
    vocabCreation(features);

    vector<string> buildingNames;
    string csvPath = "/build/trainingdata.csv";
    loadBuildingNames(csvPath, buildingNames);

    string vocabPath = "/build/raj_voc.yml.gz";
    
    //test for 5 different input images
    vector<string> inputImagePaths;
    for (int i = 1; i <= 5; ++i) {
        stringstream ss;
        ss << "/images/testing/testing" << i << ".jpg";
        inputImagePaths.push_back(ss.str());
    }

    ofstream outputFile("/build/output_results.csv");
    outputFile << "Score,Input image,Best match image index,Number of good matches,Building name" << endl;

    for (const string &inputImagePath : inputImagePaths) {
        string output;
        findBestMatch(inputImagePath, vocabPath, features, buildingNames, output);
        outputFile << output << endl;
    }

    outputFile.close();

    // Wait for a keypress event to close all windows
    //cv::waitKey(0);
    
    return 0;
}

void loadFeatures(vector<vector<cv::Mat>> &features, string path) {
    features.clear();
    features.reserve(NIMAGES);

    cv::Ptr<cv::ORB> orb = cv::ORB::create(10000); // Increase the number of keypoints to get better results


    cout << "Extracting ORB features..." << endl;
    for (int i = 1; i <= NIMAGES; ++i) {
        stringstream ss;
        ss << path << "/training" << i << ".jpg";
        cv::Mat image = cv::imread(ss.str(), 0);
        cv::Mat mask;
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        orb->detectAndCompute(image, mask, keypoints, descriptors);

        features.push_back(vector<cv::Mat>());
        changeStructure(descriptors, features.back());
    }
}

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out) {
    out.resize(plain.rows);
    for (int i = 0; i < plain.rows; ++i) {
        out[i] = plain.row(i);
    }
}

void vocabCreation(const vector<vector<cv::Mat>> &features) {
    const int k = 8;
    const int L = 3;
    const WeightingType weight = TF_IDF;
    const ScoringType scoring = L2_NORM; 
    OrbVocabulary voc(k, L, weight, scoring);
    cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
    voc.create(features);
    cout << endl << "Saving vocabulary..." << endl;
    voc.save("raj_voc.yml.gz");
    cout << "Vocabulary saved!" << endl;
    cout << "Vocabulary size: " << voc.size() << endl;
}

double loweRatio = 0.9;

bool isGoodMatch(const cv::DMatch &m, const vector<cv::DMatch> &matches) {
    if (matches.size() < 2) {
        return false;
    }

    if (m.distance < loweRatio * matches[1].distance) {
        return true;
    }

    return false;
}

void findBestMatch(const string &inputImagePath, const string &vocabPath, const vector<vector<cv::Mat>> &features, const vector<string> &buildingNames, string &output) {

    // Load input image and extract features
    cv::Mat inputImage = cv::imread(inputImagePath, 0);
    cv::Ptr<cv::ORB> orb = cv::ORB::create(); 
    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    orb->detectAndCompute(inputImage, mask, keypoints, descriptors);

    // Display the input image
    /*
    string inputImageWindowName = "Input Image: ";
    cv::imshow(inputImageWindowName, inputImage);
    */
    

    // Change feature structure
    vector<cv::Mat> inputFeatures;
    changeStructure(descriptors, inputFeatures);

    // Load the vocabulary
    OrbVocabulary voc(vocabPath);

    // Initialize FLANN matcher
    cv::FlannBasedMatcher matcher(new cv::flann::LshIndexParams(20, 10, 2));

    // Find the best match image
    int bestMatchIndex = -1;
    int bestMatchCount = 0;
    for (size_t i = 0; i < features.size(); ++i) {
        vector<vector<cv::DMatch>> matches;
        matcher.knnMatch(descriptors, features[i], matches, 2);

        int goodMatchesCount = 0;
        for (const auto &m : matches) {
            if (isGoodMatch(m[0], m)) {
                goodMatchesCount++;
            }
        }

        if (goodMatchesCount > bestMatchCount) {
            bestMatchCount = goodMatchesCount;
            bestMatchIndex = i;
        }
    }

    if (bestMatchIndex != -1) {
        double score = static_cast<double>(bestMatchCount) / keypoints.size();
    
    cout << "Score: " << score << endl;
    cout << "Input image: " << inputImagePath << endl; 
    cout << "Best match image index: " << bestMatchIndex+1 << endl;
    cout << "Number of good matches: " << bestMatchCount << endl;
    cout << "Building name: " << buildingNames[bestMatchIndex] << endl; 

    
    stringstream output_ss;
        output_ss << score << "," << inputImagePath << "," << bestMatchIndex+1 << "," << bestMatchCount << "," << buildingNames[bestMatchIndex];
        output = output_ss.str();
    } else {
        output = "No match found.";
    }

}

void loadBuildingNames(const string &csvPath, vector<string> &buildingNames) {
    buildingNames.clear();
    ifstream file(csvPath);

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        stringstream ss(line);
        string imageName, buildingName;
        getline(ss, imageName, ',');
        getline(ss, buildingName, ',');

        buildingNames.push_back(buildingName);
    }
}


