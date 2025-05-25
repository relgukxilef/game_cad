#include <cstdint>
#include <bit>
#include <iostream>

#include <gcad/players2.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

using namespace gcad;

// TODO: avoid code duplication
const char *results[] = {"Loss", "Draw", "Win"};

struct tic_tac_toe {
    uint16_t marks[2] = {0};

    void update(players2_t &players) {
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
        
        uint16_t occupied = marks[0] | marks[1];
        auto player = std::popcount(occupied) % 2;

        if (score != 1 || !(board & ~occupied)) {
            players[0].score(results[2 - score], 2 - score);
            players[1].score(results[score], score);
            return;
        }

        if (auto choice = players[player].choice("Cell index", 9)) {
            if (occupied & (1 << *choice)) {
                players[player].grid(1);
                players[player].label("Invalid move");
                return;
            }
            marks[player] |= (1 << *choice);

            players[player].grid(3);
            uint16_t m[2] = {marks[0], marks[1]};
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    if (m[0] & 1) {
                        players[1 - player].label("o");
                    } else if (m[1] & 1) {
                        players[1 - player].label("x");
                    } else {
                        players[1 - player].label("_");
                    }
                    m[0] >>= 1;
                    m[1] >>= 1;
                }
            }
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
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // TODO

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
