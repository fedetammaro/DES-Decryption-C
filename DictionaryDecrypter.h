#ifndef OPENMP_VERSION_DICTIONARYBRUTEFORCEUTILITY_H
#define OPENMP_VERSION_DICTIONARYBRUTEFORCEUTILITY_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class DictionaryDecrypter {
public:
    DictionaryDecrypter(vector<string> passwordsToCrack, string salt);
    vector<long> sequentialBruteforce(int runsPerPassword);
    vector<long> parallelBruteforce(int runsPerPassword, int numberOfThreads);
    vector<long> manualChunksBruteforce(int runsPerPassword, int numberOfThreads);
    long getMean(vector<long> values);
    vector<float> calculateSpeedup(vector<long> sequentialTimes, vector<long> parallelTimes);

private:
    vector<string> encryptedPasswords;
    string salt;
    vector<string> dictionaryVector;
};


#endif //OPENMP_VERSION_DICTIONARYBRUTEFORCEUTILITY_H
