#pragma once

#include <pthread.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

class ZipfianGenerator {
   public:
    ZipfianGenerator(uint64_t min, uint64_t max)
        : items(max - min + 1),
          base(min),
          zipfianconstant(0.99),
          theta(0.99),
          gen(rd()),
          dis(0, 1) {
        zetan = zeta(0, max - min + 1, zipfianconstant, 0);
        zeta2theta = zeta(0, 2, theta, 0);
        alpha = 1.0 / (1.0 - theta);
        eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / zetan);
        countforzeta = items;
        nextValue(items);
        pthread_rwlock_init(&lock, NULL);
    }

    ZipfianGenerator(uint64_t min, uint64_t max, double thet)
        : items(max - min + 1),
          base(min),
          zipfianconstant(thet),
          theta(thet),
          gen(rd()),
          dis(0, 1) {
        zetan = zeta(0, max - min + 1, zipfianconstant, 0);
        zeta2theta = zeta(0, 2, theta, 0);
        alpha = 1.0 / (1.0 - theta);
        eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / zetan);
        countforzeta = items;
        nextValue(items);
        pthread_rwlock_init(&lock, NULL);
    }

    ZipfianGenerator(uint64_t min, uint64_t max, double thet, double zet)
        : items(max - min + 1),
          base(min),
          zipfianconstant(thet),
          theta(thet),
          zetan(zet),
          gen(rd()),
          dis(0, 1) {
        zeta2theta = zeta(0, 2, theta, 0);
        alpha = 1.0 / (1.0 - theta);
        eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / zetan);
        countforzeta = items;
        nextValue(items);
        pthread_rwlock_init(&lock, NULL);
    }

    ~ZipfianGenerator() { pthread_rwlock_destroy(&lock); }

    inline double zeta(uint64_t st, uint64_t n, double theta, double initialsum) {
        countforzeta = n;
        double sum = initialsum;
        for (size_t i = st; i < n; i++) {
            sum += 1 / (pow(i + 1, theta));
        }
        return sum;
    }

    inline uint64_t nextValue(uint64_t item_cnt) {
        if (item_cnt != countforzeta) {
            // have to recompute zetan and eta, since they depend onitem_cnt
            pthread_rwlock_wrlock(&lock);
            if (item_cnt > countforzeta) {
                // we have added more items. can compute zetan incrementally, which is
                // cheaper
                zetan = zeta(countforzeta, item_cnt, theta, zetan);
                eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / zetan);
            } else {
                std::cout << "WARNING: Recomputing Zipfian distribtion. This is slow and "
                                 "should be avoided. "
                              << "(item_cnt=" << item_cnt << " countforzeta=" << countforzeta
                              << ")";
                // zetan = zeta(0, item_cnt, theta, 0);
                // eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / zetan);
            }
            pthread_rwlock_unlock(&lock);
        }
        // from "Quickly Generating Billion-Record Synthetic Databases", Jim Gray
        // et al, SIGMOD 1994
        double u = dis(gen);
        double uz = u * zetan;
        if (uz < 1.0)
            return base;
        if (uz < 1.0 + pow(0.5, theta))
            return base + 1;
        uint64_t ret = base + (uint64_t)(item_cnt * pow(eta * u - eta + 1, alpha));
        return ret;
    }
    inline uint64_t nextValue() { return nextValue(items); }

   private:
    // Number of items.
    uint64_t items;
    // Min item to generate.
    uint64_t base;
    uint64_t countforzeta;
    // The zipfian constant to use.
    double zipfianconstant;
    // Computed parameters for generating the distribution.
    double theta, zetan, zeta2theta, alpha, eta;
    pthread_rwlock_t lock;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
};

extern ZipfianGenerator* zipfianGenerator;