#include "po.h"
#include "license.h"

static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    return 0;
}


static bool showPO = false;

bool& GetShowPOFlag() {
    return showPO;
}

void ClearInputPOFields(char* id, char* name, char* version, char* manufacturer, char* licenseExpiration) {
    memset(id, 0, sizeof(char) * 32);
    memset(name, 0, sizeof(char) * 32);
    memset(version, 0, sizeof(char) * 41);
    memset(manufacturer, 0, sizeof(char) * 32);
    memset(licenseExpiration, 0, sizeof(char) * 32);
}

void SavePODataToFile(const std::vector<POData>& data) {
    std::ofstream file("po_data.bin");
    if (file.is_open()) {
        for (const auto& entry : data) {
            file << entry.id << ' ' << entry.name << ' ' << entry.version << ' ' << entry.manufacturer << ' ' << entry.licenseExpiration << '\n';
        }
        file.close();
        std::cout << "PO data saved to file.\n";
    } else {
        std::cerr << "Failed to open PO file for writing.\n";
    }
}

void UpdatePODataFromFile() {
    std::vector<LicenseData> licenseData = LoadLicenseDataFromFile();
    std::vector<POData> poDataList = LoadPODataFromFile();

    for (const auto& license : licenseData) {
        bool poExists = false;
        for (auto& po : poDataList) {
            if (po.id == license.softwareId) {
                poExists = true;
                bool updated = false;
                if (updated) {
                    std::cout << "PO Data updated for softwareId: " << license.softwareId << std::endl;
                }
                break;
            }
        }
        if (!poExists) {
            POData newPO;
            newPO.id = license.softwareId;
            newPO.name = "default_name";
            newPO.version = "default_version";
            newPO.manufacturer = "default_manufacturer";
            newPO.licenseExpiration = "01.01.1970";
            poDataList.push_back(newPO);
        }
    }

    for (auto it = poDataList.begin(); it != poDataList.end(); ) {
        bool found = false;
        for (const auto& license : licenseData) {
            if (license.softwareId == it->id) {
                found = true;
                break;
            }
        }
        if (!found) {
            it = poDataList.erase(it);
        } else {
            ++it;
        }
    }

    SavePODataToFile(poDataList);
    std::cout << "PO Data processing complete" << std::endl;
}
std::vector<POData> LoadPODataFromFile() {
    std::vector<POData> data;
    std::ifstream file("po_data.bin");
    if (file.is_open()) {
        POData entry;
        while (file >> entry.id >> entry.name >> entry.version >> entry.manufacturer >> entry.licenseExpiration) {
            data.push_back(entry);
        }
        file.close();
        std::cout << "PO data loaded from file.\n";
    } else {
        std::cerr << "Failed to open PO file for reading.\n";
    }
    return data;
}

bool IsDateValid(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%d.%m.%Y");
    return !ss.fail();
}

void ShowPOTable(std::vector<POData>& poDataList, int& numRows) {
    static int selectedRow = -1;
    static char searchQuery[256] = "";
    static bool scrollToFirstResult = false;
    static bool searchButtonPressed = false;
    static int searchColumnIndex = 0;
    const char* searchColumns[] = { "ID", "Название", "Версия", "Производитель", "Срок истечения лицензии" };

    if (showPO) {

        ImGui::Begin("Программное обеспечение");

        float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Закрыть окно").x;
        ImGui::SetCursorPosX(buttonPosX);
        if (ImGui::Button("Закрыть окно")) {
            showPO = false;
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

        ImGui::BeginTable("Программное обеспечение", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV);

        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Название");
        ImGui::TableSetupColumn("Версия");
        ImGui::TableSetupColumn("Производитель");
        ImGui::TableSetupColumn("Срок истечения лицензии");

        ImGui::TableHeadersRow();

        for (int i = 0; i < numRows; ++i) {
            const POData& poRowData = poDataList[i];
            bool matchFound = false;

            const char* searchColumn = "";
            switch (searchColumnIndex) {
                case 0: searchColumn = poRowData.id.c_str(); break;
                case 1: searchColumn = poRowData.name.c_str(); break;
                case 2: searchColumn = poRowData.version.c_str(); break;
                case 3: searchColumn = poRowData.manufacturer.c_str(); break;
                case 4: searchColumn = poRowData.licenseExpiration.c_str(); break;
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
            ImGui::Text("%s", poRowData.id.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", poRowData.name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", poRowData.version.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", poRowData.manufacturer.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", poRowData.licenseExpiration.c_str());
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
            static char name[32] = "";
            static char version[41] = "";
            static char manufacturer[32] = "";
            static char licenseExpiration[11] = "";
            static bool showError = false;

            ImGui::InputText("ID", id, IM_ARRAYSIZE(id), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Название", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Версия", version, IM_ARRAYSIZE(version), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Производитель", manufacturer, IM_ARRAYSIZE(manufacturer), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Срок истечения лицензии", licenseExpiration, IM_ARRAYSIZE(licenseExpiration), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("OK")) {
                showError = false;

                try {
                    int idValue = std::stoi(id);
                    int nameValue = std::stoi(name);
                    if (strlen(version) > 40) {
                        showError = true;
                    }
                    int manufacturerValue = std::stoi(manufacturer);

                    bool idExists = false;
                    for (const auto& entry : poDataList) {
                        if (entry.id == id) {
                            idExists = true;
                            break;
                        }
                    }

                    if (idExists) {
                        showError = true;
                    } else if (!IsDateValid(licenseExpiration)) {
                        showError = true;
                    } else {
                        POData newData;
                        newData.id = std::string(id);
                        newData.name = std::string(name);
                        newData.version = std::string(version);
                        newData.manufacturer = std::string(manufacturer);
                        newData.licenseExpiration = std::string(licenseExpiration);

                        poDataList.push_back(newData);
                        numRows++;
                        ClearInputPOFields(id, name, version, manufacturer, licenseExpiration);
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
                ClearInputPOFields(id, name, version, manufacturer, licenseExpiration);
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
                    for (auto it = poDataList.begin(); it != poDataList.end(); ++it) {
                        if (std::stoi(it->id) == idToRemoveValue) {
                            it = poDataList.erase(it);
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
        static POData* dataToEdit = nullptr;
        static char newId[32] = "";
        static char newname[32] = "";
        static char newversion[41] = "";
        static char newmanufacturer[32] = "";
        static char newlicenseexpiration[11] = "";

        if (ImGui::BeginPopupModal("Редактировать строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!editMode) {
                ImGui::Text("Введите ID строки для редактирования:");
                ImGui::InputText("##IDToEdit", idToEdit, IM_ARRAYSIZE(idToEdit), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

                if (ImGui::Button("Найти")) {
                    try {
                        int idToEditValue = std::stoi(idToEdit);
                    for (auto& entry : poDataList) {
                        if (std::stoi(entry.id) == idToEditValue) {
                            dataToEdit = &entry;
                            strncpy(newId, dataToEdit->id.c_str(), 32);
                            strncpy(newname, dataToEdit->name.c_str(), 32);
                            strncpy(newversion, dataToEdit->version.c_str(), 41);
                            strncpy(newmanufacturer, dataToEdit->manufacturer.c_str(), 32);
                            strncpy(newlicenseexpiration, dataToEdit->licenseExpiration.c_str(), 11);
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
            ImGui::InputText("Название", newname, IM_ARRAYSIZE(newname), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Версия", newversion, IM_ARRAYSIZE(newversion), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Производитель", newmanufacturer, IM_ARRAYSIZE(newmanufacturer), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Срок истечения лицензии", newlicenseexpiration, IM_ARRAYSIZE(newlicenseexpiration), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("Сохранить")) {
                dataToEdit->id = newId;
                dataToEdit->name = newname;
                dataToEdit->version = newversion;
                dataToEdit->manufacturer = newmanufacturer;
                dataToEdit->licenseExpiration = newlicenseexpiration;
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
        const char* sortOptions[] = { "По ID", "По названию", "По версии", "По производителю", "По сроку истечения лицензии" };
        ImGui::Combo("##SortOption", &sortOptionIndex, sortOptions, IM_ARRAYSIZE(sortOptions));

        if (ImGui::Button("OK")) {
            switch (sortOptionIndex) {
                case 0:
                    std::sort(poDataList.begin(), poDataList.end(), [](const POData& a, const POData& b) {
                        return std::stoi(a.id) < std::stoi(b.id);
                    });
                    break;
                case 1:
                    std::sort(poDataList.begin(), poDataList.end(), [](const POData& a, const POData& b) {
                        return std::stoi(a.name) < std::stoi(b.name);
                    });
                    break;
                case 2:
                    std::sort(poDataList.begin(), poDataList.end(), [](const POData& a, const POData& b) {
                        return a.version < b.version;
                    });
                    break;
                case 3:
                    std::sort(poDataList.begin(), poDataList.end(), [](const POData& a, const POData& b) {
                        return std::stoi(a.manufacturer) < std::stoi(b.manufacturer);
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


