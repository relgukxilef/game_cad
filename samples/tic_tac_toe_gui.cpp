#include <cstdint>
#include <bit>
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

const char *results[] = {"X wins", "Draw", "O wins"};

struct tic_tac_toe {
    uint16_t marks[2] = {0};
    const char *result = nullptr;

    unsigned current_player() {
        uint16_t occupied = marks[0] | marks[1];
        auto player = std::popcount(occupied) % 2;
        return player;
    }

    void update(unsigned choice) {
        if (result) {
            return;
        }

        uint16_t occupied = marks[0] | marks[1];
        auto player = current_player();

        if (occupied & (1 << choice)) {
            return;
        }
        marks[player] |= (1 << choice);
        occupied = marks[0] | marks[1];

        uint16_t board = 0b111'111'111;
        uint16_t
            vertical = 0b001'001'001,
            horizontal = 0b111,
            diagonal_0 = 0b001'010'100,
            diagonal_1 = 0b100'010'001;

        int score = 1; // 1 = draw
        for (int player = 0; player < 2; player++) {
            auto p = marks[player];
            for (int i = 0; i < 3; i++) {
                if (
                    (((p >> i) & vertical) == vertical) |
                    (((p >> i * 3) & horizontal) == horizontal)
                ) {
                    score = 2 * player;
                }
            }
            if (
                ((p & diagonal_0) == diagonal_0) |
                ((p & diagonal_1) == diagonal_1)
            ) {
                score = 2 * player;
            }
        }
        
        if (score != 1 || !(board & ~occupied)) {
            result = results[score];
            return;
        }
    }
};

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Game GUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    tic_tac_toe game;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (int i = 0; i < 3; i++) {
            ImGui::PushID(i);
            ImGui::BeginGroup();
            for (int j = 0; j < 3; j++) {
                auto symbol = "";
                auto index = (i * 3 + j);
                auto bit = 1 << index;
                if (game.marks[0] & bit)
                    symbol = "X";
                else if (game.marks[1] & bit)
                    symbol = "O";
                ImGui::PushID(j);
                if (ImGui::Button(symbol, {100, 100})) {
                    game.update(index);
                }
                ImGui::PopID();
                ImGui::SameLine();
            }
            ImGui::EndGroup();
            ImGui::PopID();
        }
    
        if (game.result) {
            ImGui::TextUnformatted("Game over.");
            ImGui::TextUnformatted(game.result);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
