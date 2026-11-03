#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// ======================================================
// ABSTRACT BASE CLASS
// ======================================================

class Document {
protected:
    string filename;
    string content;

public:
    Document(string file) {
        filename = file;
        loadFile();
    }

    void loadFile() {
        ifstream file(filename);

        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filename);
        }

        string line;

        while (getline(file, line)) {
            content += line + " ";
        }

        file.close();
    }

    virtual void preprocess() = 0;
    virtual vector<string> tokenize() = 0;

    string getFilename() {
        return filename;
    }

    virtual ~Document() {}
};


// ======================================================
// TEXT DOCUMENT CLASS
// ======================================================

class TextDocument : public Document {

public:

    TextDocument(string file) : Document(file) {}

    void preprocess() override {

        // Convert to lowercase
        transform(
            content.begin(),
            content.end(),
            content.begin(),
            [](unsigned char c) {
                return tolower(c);
            }
        );

        // Remove punctuation
        for (char &c : content) {

            if (ispunct(
                static_cast<unsigned char>(c))) {

                c = ' ';
            }
        }
    }

    vector<string> tokenize() override {

        vector<string> words;

        string word = "";

        for (char c : content) {

            if (isspace(
                static_cast<unsigned char>(c))) {

                if (!word.empty()) {
                    words.push_back(word);
                    word = "";
                }

            } else {
                word += c;
            }
        }

        if (!word.empty()) {
            words.push_back(word);
        }

        return words;
    }
};


// ======================================================
// ABSTRACT SIMILARITY CLASS
// ======================================================

class SimilarityAlgorithm {

public:

    virtual double calculate(
        vector<string> doc1,
        vector<string> doc2
    ) = 0;

    virtual string getName() = 0;

    virtual ~SimilarityAlgorithm() {}
};


// ======================================================
// COSINE SIMILARITY
// ======================================================

class CosineSimilarity : public SimilarityAlgorithm {

public:

    double calculate(
        vector<string> doc1,
        vector<string> doc2
    ) override {

        map<string, int> freq1;
        map<string, int> freq2;

        // Count words in document 1
        for (string word : doc1) {
            freq1[word]++;
        }

        // Count words in document 2
        for (string word : doc2) {
            freq2[word]++;
        }

        // Store all unique words
        set<string> allWords;

        for (auto &x : freq1) {
            allWords.insert(x.first);
        }

        for (auto &x : freq2) {
            allWords.insert(x.first);
        }

        double dotProduct = 0;
        double magnitude1 = 0;
        double magnitude2 = 0;

        for (string word : allWords) {

            double x = freq1[word];
            double y = freq2[word];

            dotProduct += x * y;

            magnitude1 += x * x;
            magnitude2 += y * y;
        }

        magnitude1 = sqrt(magnitude1);
        magnitude2 = sqrt(magnitude2);

        if (magnitude1 == 0 || magnitude2 == 0) {
            return 0;
        }

        return dotProduct /
               (magnitude1 * magnitude2);
    }

    string getName() override {
        return "Cosine Similarity";
    }
};


// ======================================================
// PLAGIARISM DETECTOR
// ======================================================

class PlagiarismDetector {

private:

    SimilarityAlgorithm *algorithm;

public:

    PlagiarismDetector(
        SimilarityAlgorithm *alg) {

        algorithm = alg;
    }

    double check(
        TextDocument &doc1,
        TextDocument &doc2) {

        doc1.preprocess();
        doc2.preprocess();

        vector<string> words1 =
            doc1.tokenize();

        vector<string> words2 =
            doc2.tokenize();

        return algorithm->calculate(
            words1,
            words2
        );
    }

    string getAlgorithmName() {
        return algorithm->getName();
    }
};


// ======================================================
// REPORT GENERATOR
// ======================================================

class ReportGenerator {

public:

    void generate(
        string file1,
        string file2,
        double similarity) {

        double percentage =
            similarity * 100;

        cout << "\n";
        cout << "============================================\n";
        cout << "          PLAGIARISM DETECTION REPORT\n";
        cout << "============================================\n";

        cout << "\nFile 1 : " << file1;
        cout << "\nFile 2 : " << file2;

        cout << "\n\nSimilarity Score : "
             << fixed
             << setprecision(2)
             << percentage
             << "%";

        cout << "\nClassification    : ";

        if (percentage >= 80) {

            cout << "VERY HIGH SIMILARITY";

        } else if (percentage >= 60) {

            cout << "HIGH SIMILARITY";

        } else if (percentage >= 40) {

            cout << "MODERATE SIMILARITY";

        } else if (percentage >= 20) {

            cout << "LOW SIMILARITY";

        } else {

            cout << "VERY LOW SIMILARITY";
        }

        cout << "\n\n============================================\n";

        cout << "\nNote:";
        cout << "\nSimilarity indicates matching content";
        cout << "\nbased on the selected algorithm.";
        cout << "\nIt is not by itself proof of plagiarism.";

        cout << "\n\n============================================\n";
    }
};


// ======================================================
// MAIN FUNCTION
// ======================================================

int main() {

    cout << "\n";
    cout << "============================================\n";
    cout << "          PLAGICHECK - OOP PROJECT\n";
    cout << "============================================\n";

    cout << "\nThis program compares two text files.\n";

    string file1;
    string file2;

    cout << "\nEnter path of first file: ";
    cin >> file1;

    cout << "Enter path of second file: ";
    cin >> file2;

    try {

        // Create document objects
        TextDocument document1(file1);
        TextDocument document2(file2);

        // Create similarity algorithm
        CosineSimilarity cosine;

        // Create plagiarism detector
        PlagiarismDetector detector(&cosine);

        // Check similarity
        double similarity =
            detector.check(
                document1,
                document2
            );

        // Generate report
        ReportGenerator report;

        report.generate(
            document1.getFilename(),
            document2.getFilename(),
            similarity
        );

    }
    catch (exception &e) {

        cout << "\nERROR: "
             << e.what()
             << "\n";
    }

    return 0;
}