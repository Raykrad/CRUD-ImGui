#include "user.h"
#include "license.h"
#include "po.h"

static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    return 0;
}

static bool showUser = false;

bool& GetShowUserFlag() {
    return showUser;
}

void ClearInputUserFields(char* id, char* fio, char* role, char* idNumber, char* password) {
    memset(id, 0, sizeof(char) * 32);
    memset(fio, 0, sizeof(char) * 32);
    memset(role, 0, sizeof(char) * 41);
    memset(idNumber, 0, sizeof(char) * 32);
    memset(password, 0, sizeof(char) * 32);
}

void SaveUserDataToFile(const std::vector<UserData>& data) {
    std::ofstream file("user_data.bin");
    if (file.is_open()) {
        for (const auto& entry : data) {
            file << entry.id << ' ' << entry.fio << ' ' << entry.role << ' ' << entry.idNumber << ' ' << entry.password << '\n';
        }
        file.close();
        std::cout << "User data saved to file.\n";
    } else {
        std::cerr << "Failed to open User file for writing.\n";
    }
}

void UpdateUserDataFromFile() {
    std::vector<LicenseData> licenseData = LoadLicenseDataFromFile();
    std::vector<UserData> userDataList = LoadUserDataFromFile();

    if(!userDataList.empty()) {
        userDataList = LoadUserDataFromFile();
    }

    for (const auto& license : licenseData) {
        bool userExists = false;
        for (const auto& user : userDataList) {
            if (user.id == license.userId) {
                userExists = true;
                bool updated = false;
                if (updated) {
                    std::cout << "User data updated for softwareId: " << license.userId << std::endl;
                }
                break;
            }
        }
        if (!userExists) {
            UserData newUser;
            newUser.id = license.userId;
            newUser.fio = "И.И.Иванов";
            newUser.role = "default_role";
            newUser.idNumber = "default_idNumber";
            newUser.password = "00000000";
            userDataList.push_back(newUser);
        }
    }

    for (auto it = userDataList.begin(); it != userDataList.end(); ) {
        bool found = false;
        for (const auto& license : licenseData) {
            if (license.userId == it->id) {
                found = true;
                break;
            }
        }
        if (!found) {
            it = userDataList.erase(it);
        } else {
            ++it;
        }
    }

    SaveUserDataToFile(userDataList);
    std::cout << "User Data updated" << std::endl;
}

std::vector<UserData> LoadUserDataFromFile() {
    std::vector<UserData> data;
    std::ifstream file("user_data.bin");
    if (file.is_open()) {
        UserData entry;
        while (file >> entry.id >> entry.fio >> entry.role >> entry.idNumber >> entry.password) {
            data.push_back(entry);
        }
        file.close();
        std::cout << "User data loaded from file.\n";
    } else {
        std::cerr << "Failed to open User file for reading.\n";
    }
    return data;
}

void ShowUserTable(std::vector<UserData>& userDataList, int& numRows) {
    static int selectedRow = -1;
    static char searchQuery[256] = "";
    static bool scrollToFirstResult = false;
    static bool searchButtonPressed = false;
    static int searchColumnIndex = 0;
    const char* searchColumns[] = { "ID", "Имя", "Роль", "Идентификационный номер", "Пароль" };

    if (showUser) {

        ImGui::Begin("Пользователи");

        float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Закрыть окно").x;
        ImGui::SetCursorPosX(buttonPosX);
        if (ImGui::Button("Закрыть окно")) {
            showUser = false;
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Поиск");
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::InputText("##Search", searchQuery, IM_ARRAYSIZE(searchQuery));
        ImGui::SameLine();
        ImGui::Text(" в ");
        ImGui::SameLine();
        ImGui::PushItemWidth(140);
        ImGui::Combo("##SearchIn", &searchColumnIndex, searchColumns, IM_ARRAYSIZE(searchColumns));

        if (ImGui::Button("Найти")) {
            searchButtonPressed = true;
            scrollToFirstResult = false;
        }

        ImGui::BeginTable("Пользователи", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV);

        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Имя");
        ImGui::TableSetupColumn("Роль");
        ImGui::TableSetupColumn("Идентификационный номер");
        ImGui::TableSetupColumn("Пароль");

        ImGui::TableHeadersRow();

        for (int i = 0; i < numRows; ++i) {
            const UserData& userRowData = userDataList[i];
            bool matchFound = false;

            const char* searchColumn = "";
            switch (searchColumnIndex) {
                case 0: searchColumn = userRowData.id.c_str(); break;
                case 1: searchColumn = userRowData.fio.c_str(); break;
                case 2: searchColumn = userRowData.role.c_str(); break;
                case 3: searchColumn = userRowData.idNumber.c_str(); break;
                case 4: searchColumn = userRowData.password.c_str(); break;
            }

            if (searchButtonPressed &&
                strstr(searchColumn, searchQuery)) {
                matchFound = true;
            }

            if (matchFound) {
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                if (!scrollToFirstResult) {
                    ImGui::SetScrollHereY();
                    scrollToFirstResult = true;
                }
            } else if (!searchButtonPressed) {
                ImGui::TableNextRow();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", userRowData.id.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", userRowData.fio.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", userRowData.role.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", userRowData.idNumber.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", userRowData.password.c_str());
        }

        ImGui::EndTable();

        if (ImGui::Button("Сортировать")) {
            ImGui::OpenPopup("Сортировка");
        }
        ImGui::SameLine();
        if (ImGui::Button("Добавить строку")) {
            ImGui::OpenPopup("Добавить новую строку");
        }
        ImGui::SameLine();
        if (ImGui::Button("Удалить строку")) {
            ImGui::OpenPopup("Удалить строку");
        }
        ImGui::SameLine();
        if (ImGui::Button("Редактировать строку")) {
            ImGui::OpenPopup("Редактировать строку");
        }

        if (ImGui::BeginPopupModal("Добавить новую строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char id[32] = "";
            static char fio[64] = "";
            static char role[41] = "";
            static char idNumber[32] = "";
            static char password[32] = "";
            static bool showError = false;

            ImGui::InputText("ID", id, IM_ARRAYSIZE(id), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Название", fio, IM_ARRAYSIZE(fio), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Роль", role, IM_ARRAYSIZE(role), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Идентификационный номер", idNumber, IM_ARRAYSIZE(idNumber), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Пароль", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("OK")) {
                showError = false;

                try {
                    int idValue = std::stoi(id);
                    if (strlen(fio) > 60) {
                        showError = true;
                    }
                    if (strlen(role) > 40) {
                        showError = true;
                    }
                    int idNumberValue = std::stoi(idNumber);
                    if (strlen(password) > 31) {
                        showError = true;
                    }
                    bool idExists = false;
                    for (const auto& entry : userDataList) {
                        if (entry.id == id) {
                            idExists = true;
                            break;
                        }
                    }

                    if (idExists) {
                        showError = true;
                    } else {
                        UserData newData;
                        newData.id = std::string(id);
                        newData.fio = std::string(fio);
                        newData.role = std::string(role);
                        newData.idNumber = std::string(idNumber);
                        newData.password = std::string(password);

                        userDataList.push_back(newData);
                        numRows++;
                        ClearInputUserFields(id, fio, role, idNumber, password);
                        ImGui::CloseCurrentPopup();
                    }
                } catch (std::invalid_argument&) {
                    showError = true;
                } catch (std::out_of_range&) {
                    showError = true;
                }
            }

            if (showError) {
                ImGui::Text("Ошибка: Проверьте введённые данные. Возможно, выбранный ID уже существует.");
            }

            ImGui::SameLine();
            if (ImGui::Button("Отмена")) {
                ClearInputUserFields(id, fio, role, idNumber, password);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Удалить строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char idToRemove[32] = "";
            static bool showError = false;

            ImGui::Text("Введите ID строки для удаления:");
            ImGui::InputText("##IDToRemove", idToRemove, IM_ARRAYSIZE(idToRemove), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("Удалить")) {
                try {
                    int idToRemoveValue = std::stoi(idToRemove);
                    bool found = false;
                    for (auto it = userDataList.begin(); it != userDataList.end(); ++it) {
                        if (std::stoi(it->id) == idToRemoveValue) {
                            it = userDataList.erase(it);
                            numRows--;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        showError = true;
                    } else {
                        ImGui::CloseCurrentPopup();
                    }
                } catch (std::invalid_argument&) {
                    showError = true;
                } catch (std::out_of_range&) {
                    showError = true;
                }
            }

            if (showError) {
                ImGui::Text("Ошибка: Строка с указанным ID не найдена.");
            }

            ImGui::SameLine();
            if (ImGui::Button("Отмена")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        static char idToEdit[32] = "";
        static bool showError = false;
        static bool editMode = false;
        static UserData* dataToEdit = nullptr;
        static char newId[32] = "";
        static char newfio[64] = "";
        static char newrole[41] = "";
        static char newidNumber[32] = "";
        static char newpassword[32] = "";

        if (ImGui::BeginPopupModal("Редактировать строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!editMode) {
                ImGui::Text("Введите ID строки для редактирования:");
                ImGui::InputText("##IDToEdit", idToEdit, IM_ARRAYSIZE(idToEdit), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

                if (ImGui::Button("Найти")) {
                    try {
                        int idToEditValue = std::stoi(idToEdit);
                    for (auto& entry : userDataList) {
                        if (std::stoi(entry.id) == idToEditValue) {
                            dataToEdit = &entry;
                            strncpy(newId, dataToEdit->id.c_str(), 32);
                            strncpy(newfio, dataToEdit->fio.c_str(), 32);
                            strncpy(newrole, dataToEdit->role.c_str(), 41);
                            strncpy(newidNumber, dataToEdit->idNumber.c_str(), 32);
                            strncpy(newpassword, dataToEdit->password.c_str(), 11);
                            editMode = true;
                            break;
                        }
                    }
                    if (!editMode) {
                        showError = true;
                    }
                } catch (std::invalid_argument&) {
                    showError = true;
                } catch (std::out_of_range&) {
                    showError = true;
                }
            }

            if (showError) {
                ImGui::Text("Ошибка: Строка с указанным ID не найдена.");
            }

            ImGui::SameLine();
            if (ImGui::Button("Отмена")) {
                ImGui::CloseCurrentPopup();
            }
        } else {
            ImGui::InputText("ID", newId, IM_ARRAYSIZE(newId), ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackCharFilter, TextEditCallback);
            ImGui::InputText("Название", newfio, IM_ARRAYSIZE(newfio), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Роль", newrole, IM_ARRAYSIZE(newrole), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Идентификационный номер", newidNumber, IM_ARRAYSIZE(newidNumber), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Пароль", newpassword, IM_ARRAYSIZE(newpassword), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("Сохранить")) {
                dataToEdit->id = newId;
                dataToEdit->fio = newfio;
                dataToEdit->role = newrole;
                dataToEdit->idNumber = newidNumber;
                dataToEdit->password = newpassword;
                editMode = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Отмена")) {
                editMode = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
        }

if (ImGui::BeginPopupModal("Сортировка", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static int sortOptionIndex = 0;
        const char* sortOptions[] = { "По ID", "По имени", "По роли", "По идентификационному номеру", "По паролю" };
        ImGui::Combo("##SortOption", &sortOptionIndex, sortOptions, IM_ARRAYSIZE(sortOptions));

        if (ImGui::Button("OK")) {
            switch (sortOptionIndex) {
                case 0:
                    std::sort(userDataList.begin(), userDataList.end(), [](const UserData& a, const UserData& b) {
                        return std::stoi(a.id) < std::stoi(b.id);
                    });
                    break;
                case 1:
                    std::sort(userDataList.begin(), userDataList.end(), [](const UserData& a, const UserData& b) {
                        return std::stoi(a.fio) < std::stoi(b.fio);
                    });
                    break;
                case 2:
                    std::sort(userDataList.begin(), userDataList.end(), [](const UserData& a, const UserData& b) {
                        return a.role < b.role;
                    });
                    break;
                case 3:
                    std::sort(userDataList.begin(), userDataList.end(), [](const UserData& a, const UserData& b) {
                        return std::stoi(a.idNumber) < std::stoi(b.idNumber);
                    });
                    break;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Отмена")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    }

    ImGui::End();
}


