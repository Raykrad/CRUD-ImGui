#ifndef PO_H
#define PO_H

#include "includes.h"

struct POData {
    std::string id;
    std::string name;
    std::string version;
    std::string manufacturer;
    std::string licenseExpiration;
};

void ShowPOTable(std::vector<POData>& poDataList, int& numRows);
void SavePODataToFile(const std::vector<POData>& data);
std::vector<POData> LoadPODataFromFile();
bool& GetShowPOFlag();
void UpdatePODataFromFile();

#endif //PO_H
