#ifndef HUB_H
#define HUB_H

#include "includes.h"

struct HubData {
    std::string id;
    std::string hubName;
    std::string lead;
};

void ShowHubTable(std::vector<HubData>& hubData, int& numRows);
void SaveHubDataToFile(const std::vector<HubData>& data);
std::vector<HubData> LoadHubDataFromFile();
bool& GetShowHubFlag();
void UpdateHubDataFromFile();

#endif //HUB_H
