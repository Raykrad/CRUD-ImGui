#include "license.h"
#include "po.h"
#include "user.h"


static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    return 0;
}

static bool showLicense = false;
static bool licenseEdit = false;

bool& GetShowLicenseFlag() {
    return showLicense;
}

bool& GetLicenseEdit() {
    return licenseEdit;
}

void GetLicenseBack() {
    licenseEdit = false;
}

void ClearInputLicenseFields(char* id, char* softwareId, char* licenseNumber, char* userId) {
    memset(id, 0, sizeof(char) * 32);
    memset(softwareId, 0, sizeof(char) * 32);
    memset(licenseNumber, 0, sizeof(char) * 41);
    memset(userId, 0, sizeof(char) * 32);
}

void SaveLicenseDataToFile(const std::vector<LicenseData>& data) {
    std::ofstream file("license_data.bin");
    if (file.is_open()) {
        for (const auto& entry : data) {
            file << entry.id << ' ' << entry.softwareId << ' ' << entry.licenseNumber << ' ' << entry.userId << ' ' << entry.status << '\n';
        }
        file.close();
        std::cout << "License data saved to file.\n";
        UpdatePODataFromFile();
    } else {
        std::cerr << "Failed to open license file for writing.\n";
    }
}

std::vector<LicenseData> LoadLicenseDataFromFile() {
    std::vector<LicenseData> data;
    std::ifstream file("license_data.bin");
    if (file.is_open()) {
        LicenseData entry;
        while (file >> entry.id >> entry.softwareId >> entry.licenseNumber >> entry.userId >> entry.status) {
            data.push_back(entry);
        }
        file.close();
        std::cout << "License data loaded from file.\n";
    } else {
        std::cerr << "Failed to open license file for reading.\n";
    }
    return data;
}

void ShowLicenseTable(std::vector<LicenseData>& licenseData, int& numRows) {
    static int selectedRow = -1;
    static char searchQuery[256] = "";
    static bool scrollToFirstResult = false;
    static bool searchButtonPressed = false;
    static int searchColumnIndex = 0;
    const char* searchColumns[] = { "ID", "ID ПО", "Номер лицензии", "ID Пользователя", "Статус" };

    if (showLicense) {

        ImGui::Begin("Лицензия");

        float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Закрыть окно").x;
        ImGui::SetCursorPosX(buttonPosX);
        if (ImGui::Button("Закрыть окно")) {
            showLicense = false;
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

        ImGui::BeginTable("Лицензия", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV);

        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("ID ПО");
        ImGui::TableSetupColumn("Номер лицензии");
        ImGui::TableSetupColumn("ID Пользователя");
        ImGui::TableSetupColumn("Статус");

        ImGui::TableHeadersRow();

        for (int i = 0; i < numRows; ++i) {
            const LicenseData& rowData = licenseData[i];
            bool matchFound = false;

            const char* searchColumn = "";
            switch (searchColumnIndex) {
                case 0: searchColumn = rowData.id.c_str(); break;
                case 1: searchColumn = rowData.softwareId.c_str(); break;
                case 2: searchColumn = rowData.licenseNumber.c_str(); break;
                case 3: searchColumn = rowData.userId.c_str(); break;
                case 4: searchColumn = rowData.status.c_str(); break;
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
            ImGui::Text("%s", rowData.id.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", rowData.softwareId.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", rowData.licenseNumber.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", rowData.userId.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", rowData.status.c_str());
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
            static char softwareId[32] = "";
            static char licenseNumber[41] = "";
            static char userId[32] = "";
            static int statusIndex = 0;
            static bool showError = false;

            ImGui::InputText("ID", id, IM_ARRAYSIZE(id), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("ID ПО", softwareId, IM_ARRAYSIZE(softwareId), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Номер лицензии", licenseNumber, IM_ARRAYSIZE(licenseNumber), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("ID Пользователя", userId, IM_ARRAYSIZE(userId), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            const char* statusOptions[] = { "Активна", "Неактивна" };
            ImGui::Combo("Статус", &statusIndex, statusOptions, IM_ARRAYSIZE(statusOptions));

            if (ImGui::Button("OK")) {
                showError = false;

                try {
                    int idValue = std::stoi(id);
                    int softwareIdValue = std::stoi(softwareId);
                    if (strlen(licenseNumber) > 40) {
                        showError = true;
                    }
                    int userIdValue = std::stoi(userId);

                    bool idExists = false;
                    for (const auto& entry : licenseData) {
                        if (entry.id == id) {
                            idExists = true;
                            break;
                        }
                    }

                    if (idExists) {
                        showError = true;
                    } else {
                        LicenseData newData;
                        newData.id = std::string(id);
                        newData.softwareId = std::string(softwareId);
                        newData.licenseNumber = std::string(licenseNumber);
                        newData.userId = std::string(userId);
                        newData.status = statusOptions[statusIndex];

                        licenseData.push_back(newData);
                        numRows++;
                        ClearInputLicenseFields(id, softwareId, licenseNumber, userId);
                        ImGui::CloseCurrentPopup();
                        std::cout << "New license data added. licenseEdit set to true." << std::endl;
                        licenseEdit = true;
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
                ClearInputLicenseFields(id, softwareId, licenseNumber, userId);
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
                    for (auto it = licenseData.begin(); it != licenseData.end(); ++it) {
                        if (std::stoi(it->id) == idToRemoveValue) {
                            it = licenseData.erase(it);
                            numRows--;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        showError = true;
                    } else {
                        licenseEdit = true;
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
        static LicenseData* dataToEdit = nullptr;
        static char newId[32] = "";
        static char newSoftwareId[32] = "";
        static char newLicenseNumber[41] = "";
        static char newUserId[32] = "";
        static int statusIndex = 0;

        if (ImGui::BeginPopupModal("Редактировать строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!editMode) {
                ImGui::Text("Введите ID строки для редактирования:");
                ImGui::InputText("##IDToEdit", idToEdit, IM_ARRAYSIZE(idToEdit), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

                if (ImGui::Button("Найти")) {
                    try {
                        int idToEditValue = std::stoi(idToEdit);
                    for (auto& entry : licenseData) {
                        if (std::stoi(entry.id) == idToEditValue) {
                            dataToEdit = &entry;
                            strncpy(newId, dataToEdit->id.c_str(), 32);
                            strncpy(newSoftwareId, dataToEdit->softwareId.c_str(), 32);
                            strncpy(newLicenseNumber, dataToEdit->licenseNumber.c_str(), 41);
                            strncpy(newUserId, dataToEdit->userId.c_str(), 32);
                            statusIndex = (dataToEdit->status == "Активна") ? 0 : 1;
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
            ImGui::InputText("ID", newId, IM_ARRAYSIZE(newId), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("ID ПО", newSoftwareId, IM_ARRAYSIZE(newSoftwareId), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Номер лицензии", newLicenseNumber, IM_ARRAYSIZE(newLicenseNumber), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("ID Пользователя", newUserId, IM_ARRAYSIZE(newUserId), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            const char* statusOptions[] = { "Активна", "Неактивна" };
            ImGui::Combo("Статус", &statusIndex, statusOptions, IM_ARRAYSIZE(statusOptions));

            if (ImGui::Button("Сохранить")) {
            try {
                int newIdValue = std::stoi(newId);
                int newSoftwareIdValue = std::stoi(newSoftwareId);
                if (strlen(newLicenseNumber) > 40) {
                    showError = true;
                }
                int newUserIdValue = std::stoi(newUserId);

                bool idExists = false;
                for (const auto& entry : licenseData) {
                    if (&entry != dataToEdit && entry.id == newId) {
                        idExists = true;
                        break;
                    }
                }

                if (idExists) {
                    showError = true;
                } else {
                    int oldSoftwareId = std::stoi(dataToEdit->softwareId);
                    if (oldSoftwareId != newSoftwareIdValue) {
                        std::vector<POData> poDataList = LoadPODataFromFile();
                        for (auto& po : poDataList) {
                            if (std::stoi(po.id) == oldSoftwareId) {
                                po.id = newSoftwareIdValue;
                                break;
                            }
                        }
                        SavePODataToFile(poDataList);
                        licenseEdit = true;
                    }
                    dataToEdit->id = newId;
                    dataToEdit->softwareId = newSoftwareId;
                    dataToEdit->licenseNumber = newLicenseNumber;
                    dataToEdit->userId = newUserId;
                    dataToEdit->status = statusOptions[statusIndex];
                    editMode = false;
                    ImGui::CloseCurrentPopup();
                }
            } catch (std::invalid_argument&) {
                showError = true;
            } catch (std::out_of_range&) {
                showError = true;
            }
        }

        if (showError) {
            ImGui::Text("Ошибка: Проверьте введённые данные.");
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
        const char* sortOptions[] = { "По ID", "По ID ПО", "По номеру лицензии", "По ID пользователя", "По статусу" };
        ImGui::Combo("##SortOption", &sortOptionIndex, sortOptions, IM_ARRAYSIZE(sortOptions));

        if (ImGui::Button("OK")) {
            switch (sortOptionIndex) {
                case 0:
                    std::sort(licenseData.begin(), licenseData.end(), [](const LicenseData& a, const LicenseData& b) {
                        return std::stoi(a.id) < std::stoi(b.id);
                    });
                    break;
                case 1:
                    std::sort(licenseData.begin(), licenseData.end(), [](const LicenseData& a, const LicenseData& b) {
                        return std::stoi(a.softwareId) < std::stoi(b.softwareId);
                    });
                    break;
                case 2:
                    std::sort(licenseData.begin(), licenseData.end(), [](const LicenseData& a, const LicenseData& b) {
                        return a.licenseNumber < b.licenseNumber;
                    });
                    break;
                case 3:
                    std::sort(licenseData.begin(), licenseData.end(), [](const LicenseData& a, const LicenseData& b) {
                        return std::stoi(a.userId) < std::stoi(b.userId);
                    });
                    break;
                case 4:
                    std::sort(licenseData.begin(), licenseData.end(), [](const LicenseData& a, const LicenseData& b) {
                        return a.status < b.status;
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


