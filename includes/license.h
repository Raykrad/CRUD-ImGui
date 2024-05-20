#ifndef LICENSE_H
#define LICENSE_H

#include "includes.h"

struct LicenseData {
    std::string id;
    std::string softwareId;
    std::string licenseNumber;
    std::string userId;
    std::string status;
};

void ShowLicenseTable(std::vector<LicenseData>& licenseData, int& numRows);
void SaveLicenseDataToFile(const std::vector<LicenseData>& data);
std::vector<LicenseData> LoadLicenseDataFromFile();
bool& GetShowLicenseFlag();
bool& GetLicenseEdit();
void GetLicenseBack();

#endif // LICENSE_H
