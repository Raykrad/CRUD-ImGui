#include "license.h"
#include "po.h"
#include "user.h"
#include "hub.h"

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(codepoint);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1024, 768, "База данных", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    glfwMakeContextCurrent(window);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    glfwSetCharCallback(window, char_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    GetShowLicenseFlag() = false;
    GetShowPOFlag() = false;
    GetShowUserFlag() = false;
    GetShowHubFlag() = false;
    int numRowsLicense = 0;
    int numRowsPO = 0;
    int numRowsUser = 0;
    int numRowsHub = 0;

    std::vector<LicenseData> licenseData = LoadLicenseDataFromFile();
    numRowsLicense = static_cast<int>(licenseData.size());
    std::cout << "First license data." << std::endl;
    std::vector<POData> POData = LoadPODataFromFile();
    numRowsPO = static_cast<int>(POData.size());
    std::cout << "First po data." << std::endl;
	std::vector<UserData> userData = LoadUserDataFromFile();
    numRowsUser = static_cast<int>(userData.size());
    std::cout << "First user data." << std::endl;
    std::vector<HubData> hubData = LoadHubDataFromFile();
    numRowsHub = static_cast<int>(hubData.size());
    std::cout << "First hub data." << std::endl;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Панель взаимодействий");
        if (ImGui::Button("Лицензии") && !GetShowLicenseFlag()) {
            GetShowLicenseFlag() = true;
        }
        if (ImGui::Button("Программное обеспечение") && !GetShowPOFlag()) {
            GetShowPOFlag() = true;
        }
        if (ImGui::Button("Пользователи") && !GetShowUserFlag()) {
            GetShowUserFlag() = true;
        }
        if (ImGui::Button("Узел") && !GetShowHubFlag()) {
            GetShowHubFlag() = true;
        }
        ImGui::End();

        if (GetLicenseEdit()) {
            std::cout << "Loaded and get back to false." << std::endl;

            SaveLicenseDataToFile(licenseData);
            licenseData = LoadLicenseDataFromFile();
            UpdatePODataFromFile();
            UpdateUserDataFromFile();
            UpdateHubDataFromFile();

            POData = LoadPODataFromFile();
			numRowsPO = static_cast<int>(POData.size());

            userData = LoadUserDataFromFile();
            numRowsUser = static_cast<int>(userData.size());

            hubData = LoadHubDataFromFile();
            numRowsHub = static_cast<int>(hubData.size());

            GetLicenseBack();
        }

        if (GetShowLicenseFlag()) {
            ShowLicenseTable(licenseData, numRowsLicense);
        }

        if (GetShowPOFlag()) {
            ShowPOTable(POData, numRowsPO);
        }
		
		if (GetShowUserFlag()) {
            ShowUserTable(userData, numRowsUser);
        }

        if (GetShowHubFlag()) {
            ShowHubTable(hubData, numRowsHub);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    SaveLicenseDataToFile(licenseData);
    SavePODataToFile(POData);
    SaveUserDataToFile(userData);
    SaveHubDataToFile(hubData);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
