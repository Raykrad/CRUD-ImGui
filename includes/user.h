#ifndef USER_H
#define USER_H

#include "includes.h"

struct UserData {
    std::string id;
    std::string fio;
    std::string role;
    std::string idNumber;
    std::string password;
};

void ShowUserTable(std::vector<UserData>& userDataList, int& numRows);
void SaveUserDataToFile(const std::vector<UserData>& data);
std::vector<UserData> LoadUserDataFromFile();
bool& GetShowUserFlag();
void UpdateUserDataFromFile();

#endif // USER_H
