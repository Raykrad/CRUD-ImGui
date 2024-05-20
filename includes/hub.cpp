#include "hub.h"
#include "user.h"

static int TextEditCallback(ImGuiInputTextCallbackData* data) {
    return 0;
}

static bool showHub = false;

bool& GetShowHubFlag() {
    return showHub;
}

void ClearInputHubFields(char* id, char* hubName, char* lead) {
    memset(id, 0, sizeof(char) * 32);
    memset(hubName, 0, sizeof(char) * 50);
    memset(lead, 0, sizeof(char) * 32);
}

void SaveHubDataToFile(const std::vector<HubData>& data) {
    std::ofstream file("hub_data.bin");
    if (file.is_open()) {
        for (const auto& entry : data) {
            file << entry.id << ' ' << entry.hubName << ' ' << entry.lead << '\n';
        }
        file.close();
        std::cout << "Hub data saved to file.\n";
    } else {
        std::cerr << "Failed to open hub file for writing.\n";
    }
}

void UpdateHubDataFromFile() {
    std::vector<UserData> userData = LoadUserDataFromFile();
    std::vector<HubData> hubDataList = LoadHubDataFromFile();

    for (const auto& user : userData) {
        bool hubExists = false;
        for (const auto& hub : hubDataList) {
            if (hub.id == user.idNumber) {
                hubExists = true;
                break;
            }
        }
        if (!hubExists) {
            HubData newHub;
            newHub.id = user.idNumber;
            newHub.hubName = "Дефолтный";
            newHub.lead = "default_lead";
            hubDataList.push_back(newHub);
        }
    }

    for (auto it = hubDataList.begin(); it != hubDataList.end(); ) {
        bool found = false;
        for (const auto& user : userData) {
            if (user.idNumber == it->id) {
                found = true;
                break;
            }
        }
        if (!found) {
            it = hubDataList.erase(it);
        } else {
            ++it;
        }
    }

    SaveHubDataToFile(hubDataList);
    std::cout << "Hub Data updated" << std::endl;
}

std::vector<HubData> LoadHubDataFromFile() {
    std::vector<HubData> data;
    std::ifstream file("hub_data.bin");
    if (file.is_open()) {
        HubData entry;
        while (file >> entry.id >> entry.hubName >> entry.lead) {
            data.push_back(entry);
        }
        file.close();
        std::cout << "Hub data loaded from file.\n";
    } else {
        std::cerr << "Failed to open hub file for reading.\n";
    }
    return data;
}

void ShowHubTable(std::vector<HubData>& hubDataList, int& numRows) {
    static int selectedRow = -1;
    static char searchQuery[256] = "";
    static bool scrollToFirstResult = false;
    static bool searchButtonPressed = false;
    static int searchColumnIndex = 0;
    const char* searchColumns[] = { "ID", "Название", "Ответственный" };

    if (showHub) {

        ImGui::Begin("Узел");

        float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Закрыть окно").x;
        ImGui::SetCursorPosX(buttonPosX);
        if (ImGui::Button("Закрыть окно")) {
            showHub = false;
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

        ImGui::BeginTable("Узел", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV);

        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Название");
        ImGui::TableSetupColumn("Ответственный");

        ImGui::TableHeadersRow();

        for (int i = 0; i < numRows; ++i) {
            const HubData& hubRowData = hubDataList[i];
            bool matchFound = false;

            const char* searchColumn = "";
            switch (searchColumnIndex) {
                case 0: searchColumn = hubRowData.id.c_str(); break;
                case 1: searchColumn = hubRowData.hubName.c_str(); break;
                case 2: searchColumn = hubRowData.lead.c_str(); break;
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
            ImGui::Text("%s", hubRowData.id.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", hubRowData.hubName.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", hubRowData.lead.c_str());
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
            static char hubName[50] = "";
            static char lead[32] = "";
            static bool showError = false;

            ImGui::InputText("ID", id, IM_ARRAYSIZE(id), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Узел", hubName, IM_ARRAYSIZE(hubName), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Ответственный", lead, IM_ARRAYSIZE(lead), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("OK")) {
                showError = false;

                try {
                    int idValue = std::stoi(id);
                    if (strlen(hubName) > 49) {
                        showError = true;
                    }
                    int leadValue = std::stoi(lead);

                    bool idExists = false;
                    for (const auto& entry : hubDataList) {
                        if (entry.id == id) {
                            idExists = true;
                            break;
                        }
                    }

                    if (idExists) {
                        showError = true;
                    } else {
                        HubData newData;
                        newData.id = std::string(id);
                        newData.hubName = std::string(hubName);
                        newData.lead = std::string(lead);

                        hubDataList.push_back(newData);
                        numRows++;
                        ClearInputHubFields(id, hubName, lead);
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
                ClearInputHubFields(id, hubName, lead);
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
                    for (auto it = hubDataList.begin(); it != hubDataList.end(); ++it) {
                        if (std::stoi(it->id) == idToRemoveValue) {
                            it = hubDataList.erase(it);
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
        static HubData* dataToEdit = nullptr;
        static char newId[32] = "";
        static char newhubName[64] = "";
        static char newlead[41] = "";

        if (ImGui::BeginPopupModal("Редактировать строку", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!editMode) {
                ImGui::Text("Введите ID строки для редактирования:");
                ImGui::InputText("##IDToEdit", idToEdit, IM_ARRAYSIZE(idToEdit), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

                if (ImGui::Button("Найти")) {
                    try {
                        int idToEditValue = std::stoi(idToEdit);
                    for (auto& entry : hubDataList) {
                        if (std::stoi(entry.id) == idToEditValue) {
                            dataToEdit = &entry;
                            strncpy(newId, dataToEdit->id.c_str(), 32);
                            strncpy(newhubName, dataToEdit->hubName.c_str(), 32);
                            strncpy(newlead, dataToEdit->lead.c_str(), 41);
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
            ImGui::InputText("Название", newhubName, IM_ARRAYSIZE(newhubName), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);
            ImGui::InputText("Роль", newlead, IM_ARRAYSIZE(newlead), ImGuiInputTextFlags_CallbackEdit, TextEditCallback);

            if (ImGui::Button("Сохранить")) {
                dataToEdit->id = newId;
                dataToEdit->hubName = newhubName;
                dataToEdit->lead = newlead;
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
                    std::sort(hubDataList.begin(), hubDataList.end(), [](const HubData& a, const HubData& b) {
                        return std::stoi(a.id) < std::stoi(b.id);
                    });
                    break;
                case 1:
                    std::sort(hubDataList.begin(), hubDataList.end(), [](const HubData& a, const HubData& b) {
                        return std::stoi(a.hubName) < std::stoi(b.hubName);
                    });
                    break;
                case 2:
                    std::sort(hubDataList.begin(), hubDataList.end(), [](const HubData& a, const HubData& b) {
                        return a.lead < b.lead;
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


