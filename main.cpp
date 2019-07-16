#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "DictionaryDecrypter.h"

using namespace std;

void printExecutionSpeedups(vector<string> passwords, vector<long> sequentialTimes, vector<long> parallelTimes,
                            vector<float> speedups, string method);

int main(int argc, char** argv) {
    if (argc != 4) {
        if (argc < 2) {
            cout << "Too few arguments! Please input number of runs per experiment and number of threads for parallel execution." << endl;
            return 1;
        }
    }

    int runsPerPassword;
    runsPerPassword = atoi(argv[1]);

    if (runsPerPassword <= 0) {
        cout << "Number of runs per experiment must be an integer > 0" << endl;
        return 1;
    }

    vector<string> passwordsToCrack = {"password", "mara1992", "Maple800", "vjht1051"};
    string salt = "aa";

    DictionaryDecrypter dictionaryDecrypter(passwordsToCrack, salt);

    vector<long> sequentialTimes, firstMethodParallelTimes, secondMethodParallelTimes;
    vector<float> firstMethodSpeedups, secondMethodSpeedups;
    vector<int> numberOfThreads = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    sequentialTimes = dictionaryDecrypter.sequentialBruteforce(runsPerPassword);

    for(int threads : numberOfThreads) {
        cout << "==========Using " << threads << " threads==========" << endl;

        firstMethodParallelTimes = dictionaryDecrypter.parallelBruteforce(runsPerPassword, threads);
        firstMethodSpeedups = dictionaryDecrypter.calculateSpeedup(sequentialTimes, firstMethodParallelTimes);
        printExecutionSpeedups(passwordsToCrack, sequentialTimes, firstMethodParallelTimes, firstMethodSpeedups, "First");
        usleep(5000000);

        secondMethodParallelTimes = dictionaryDecrypter.manualChunksBruteforce(runsPerPassword, threads);
        secondMethodSpeedups = dictionaryDecrypter.calculateSpeedup(sequentialTimes, secondMethodParallelTimes);
        printExecutionSpeedups(passwordsToCrack, sequentialTimes, secondMethodParallelTimes, secondMethodSpeedups, "Second");
        usleep(5000000);
    }

    return 0;
}

void printExecutionSpeedups(vector<string> passwords, vector<long> sequentialTimes, vector<long> parallelTimes,
                            vector<float> speedups, string method) {
    cout << "---" << method << " method statistics---" << endl;

    for (int index = 0; index < passwords.size(); index++) {
        cout << "Password #" << index+1 << ": " << passwords[index] << " - ";
        cout << "Seq. time " << sequentialTimes[index] << " ns - Par. time " << parallelTimes[index] << " ns - ";
        cout << " Speedup " << fixed << setprecision(7) << speedups[index] << endl;
    }
}