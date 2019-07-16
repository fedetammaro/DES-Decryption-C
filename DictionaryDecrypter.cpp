#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <crypt.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include "DictionaryDecrypter.h"

using namespace std;

DictionaryDecrypter::DictionaryDecrypter(vector<string> passwordsToCrack, string salt) {
    this->salt = salt;
    encryptedPasswords.reserve(passwordsToCrack.size());

    string passwordString;

    for (string & password : passwordsToCrack) {
        passwordString = crypt(password.c_str(), salt.c_str());
        encryptedPasswords.push_back(passwordString);
    }

    ifstream dictionary;
    string line;

    dictionary.open("passdict.txt");
    int index = 0;

    while (getline(dictionary, line)) {
        dictionaryVector.push_back(line);
        index++;
    }

    dictionary.close();
}

vector<long> DictionaryDecrypter::sequentialBruteforce(int runsPerPassword) {
    string line;
    vector<long> executionTimes;
    executionTimes.reserve(encryptedPasswords.size());
    vector<long> runTimes;
    runTimes.reserve((unsigned long)runsPerPassword);

    for (string& passwordToCrack : encryptedPasswords) {
        runTimes.clear();

        for (int index = 0; index < runsPerPassword; index++) {
            auto start = chrono::steady_clock::now();

            for (string& password : dictionaryVector) {
                string encryptionAttempt(crypt(password.c_str(), salt.c_str()));

                if (passwordToCrack == encryptionAttempt) {
                    cout << "Generated hash: " << encryptionAttempt << " - Wanted hash: " << passwordToCrack;
                    cout << " => Decrypted password: " << password << endl;
                    break;
                }
            }

            auto stop = chrono::steady_clock::now();
            auto elapsed_time = chrono::duration_cast<chrono::nanoseconds>(stop - start);
            runTimes.push_back(elapsed_time.count());
        }

        executionTimes.push_back(getMean(runTimes));
    }

    return executionTimes;
}

vector<long> DictionaryDecrypter::manualChunksBruteforce(int runsPerPassword, int numberOfThreads) {
    vector<long> executionTimes;
    executionTimes.reserve(encryptedPasswords.size());
    vector<long> runTimes;
    runTimes.reserve((unsigned long)runsPerPassword);

    for (string& passwordToCrack : encryptedPasswords) {
        runTimes.clear();

        for (int runIndex = 0; runIndex < runsPerPassword; runIndex++) {
            int elementsPerThread = static_cast<int>(ceil((double)dictionaryVector.size() / (double)numberOfThreads));
            volatile bool stopLoop = false;

            auto start = chrono::steady_clock::now();
            #pragma omp parallel num_threads(numberOfThreads) shared(stopLoop)
            {
                struct crypt_data bookkeepingData;
                bookkeepingData.initialized = 0;
                int numThread = omp_get_thread_num();

                for (int wordIndex = elementsPerThread * numThread; wordIndex < (numThread + 1) * elementsPerThread; wordIndex++) {
                    if (!stopLoop && wordIndex < dictionaryVector.size()) {
                        char *encryptionAttempt = crypt_r(dictionaryVector[wordIndex].c_str(), salt.c_str(),
                                                          &bookkeepingData);

                        if (strcmp(encryptionAttempt, passwordToCrack.c_str()) == 0) {
                            cout << "Generated hash: " << encryptionAttempt << " - Wanted hash: " << passwordToCrack;
                            cout << " => Decrypted password: " << dictionaryVector[wordIndex].c_str() << endl;
                            stopLoop = true;
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }
            auto stop = chrono::steady_clock::now();
            auto elapsed_time = chrono::duration_cast<chrono::nanoseconds>(stop - start);
            runTimes.push_back(elapsed_time.count());
        }

        executionTimes.push_back(getMean(runTimes));
    }

    return executionTimes;
}

vector<long> DictionaryDecrypter::parallelBruteforce(int runsPerPassword, int numberOfThreads) {

    vector<long> executionTimes;
    executionTimes.reserve(encryptedPasswords.size());
    vector<long> runTimes;
    runTimes.reserve((unsigned long)runsPerPassword);

    for (string& passwordToCrack : encryptedPasswords) {
        runTimes.clear();

        for (int runIndex = 0; runIndex < runsPerPassword; runIndex++) {
            volatile bool stopLoop = false;

            auto start = chrono::steady_clock::now();
            #pragma omp parallel num_threads(numberOfThreads)
            {
                struct crypt_data bookkeepingData;
                bookkeepingData.initialized = 0;

                #pragma omp for firstprivate(dictionaryVector)
                for (int wordIndex = 0; wordIndex < dictionaryVector.size(); wordIndex++) {
                    if (stopLoop) continue;

                    char *encryptionAttempt = crypt_r(dictionaryVector[wordIndex].c_str(), salt.c_str(),
                            &bookkeepingData);

                    if (strcmp(encryptionAttempt, passwordToCrack.c_str()) == 0) {
                        stopLoop = true;
                        cout << "Generated hash: " << encryptionAttempt << " - Wanted hash: " << passwordToCrack;
                        cout << " => Decrypted password: " << dictionaryVector[wordIndex] << endl;
                    }
                }
            }

            auto stop = chrono::steady_clock::now();
            auto elapsed_time = chrono::duration_cast<chrono::nanoseconds>(stop - start);
            runTimes.push_back(elapsed_time.count());
        }

        executionTimes.push_back(getMean(runTimes));
    }

    return executionTimes;
}

long DictionaryDecrypter::getMean(vector<long> values) {
    long sum = 0;

    for (long value : values) {
        sum += value;
    }

    return sum / values.size();
}

vector<float> DictionaryDecrypter::calculateSpeedup(vector<long> sequentialTimes, vector<long> parallelTimes) {
    vector<float> speedups;
    speedups.reserve(sequentialTimes.size());

    for (int index = 0; index < sequentialTimes.size(); index++) {
        speedups.push_back((float)sequentialTimes[index] / (float)parallelTimes[index]);
    }

    return speedups;
}