// Графический интерфейс: Dear ImGui + GLFW + OpenGL
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "GuiApp.h"
#include "Matrix.h"
#include "Vector.h"
#include "Solver.h"
#include "MathError.h"

namespace {

const int MAXN = 6;    // максимальный размер матрицы / вектора в окне
const int CELL = 24;   // сколько символов помещается в одной ячейке

const ImVec4 ACCENT = ImVec4(0.36f, 0.62f, 1.00f, 1.00f);
const ImVec4 ERROR_COLOR = ImVec4(1.00f, 0.42f, 0.42f, 1.00f);

// ---------- Поле ввода матрицы / вектора ----------

struct GridEditor {
    int rows = 2;
    int cols = 2;
    char cells[MAXN][MAXN][CELL];
    char rhs[MAXN][CELL]; // столбец правых частей (для СЛАУ)

    GridEditor() { clear(); }

    void clear() {
        for (int i = 0; i < MAXN; i++) {
            std::strcpy(rhs[i], "0");
            for (int j = 0; j < MAXN; j++) {
                std::strcpy(cells[i][j], "0");
            }
        }
    }
};

// ---------- Результат операции ----------

struct Result {
    bool has = false;
    bool error = false;
    std::string title;
    std::string text;
    std::vector<std::vector<std::string>> table; // результат-матрица

    void fail(const std::string& message) {
        has = true;
        error = true;
        title.clear();
        text = message;
        table.clear();
    }

    void showText(const std::string& newTitle, const std::string& newText) {
        has = true;
        error = false;
        title = newTitle;
        text = newText;
        table.clear();
    }

    void showMatrix(const std::string& newTitle, Matrix m) {
        has = true;
        error = false;
        title = newTitle;
        text.clear();
        table.clear();
        for (size_t i = 0; i < m.rows; i++) {
            std::vector<std::string> row;
            for (size_t j = 0; j < m.columns; j++) {
                row.push_back(m.data[i][j].toString());
            }
            table.push_back(row);
        }
    }
};

// ---------- Перевод текста из ячеек в числа ----------

Fraction parseCell(const char* text, const std::string& label) {
    try {
        return Fraction::parse(text);
    }
    catch (MathError&) {
        throw MathError("Ячейка " + label + ": введите число или дробь (3, -2.5, 1/3)");
    }
}

Matrix gridToMatrix(const GridEditor& g, const std::string& name) {
    Matrix m(g.rows, g.cols);
    for (int i = 0; i < g.rows; i++) {
        for (int j = 0; j < g.cols; j++) {
            std::string label = name + "[" + std::to_string(i + 1) + "][" + std::to_string(j + 1) + "]";
            m.data[i][j] = parseCell(g.cells[i][j], label);
        }
    }
    return m;
}

Vector gridToVector(const GridEditor& g, const std::string& name) {
    Vector v(g.cols);
    for (int j = 0; j < g.cols; j++) {
        std::string label = name + "[" + std::to_string(j + 1) + "]";
        v.data[j] = parseCell(g.cells[0][j], label);
    }
    return v;
}

Vector gridToRhs(const GridEditor& g) {
    Vector v(g.rows);
    for (int i = 0; i < g.rows; i++) {
        v.data[i] = parseCell(g.rhs[i], "b[" + std::to_string(i + 1) + "]");
    }
    return v;
}

// запускает действие и превращает любую ошибку в сообщение
template <class F>
void run(Result& result, F action) {
    try {
        action();
    }
    catch (MathError& e) {
        result.fail(e.what());
    }
    catch (std::exception& e) {
        result.fail(e.what());
    }
}

std::string number(double x, const char* format) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), format, x);
    return buffer;
}

// ---------- Отрисовка ----------

void drawGrid(const char* id, GridEditor& g, const char* title, bool isVector, bool withRhs = false) {
    ImGui::PushID(id);
    ImGui::TextColored(ACCENT, "%s", title);

    ImGui::SetNextItemWidth(150);
    if (isVector) {
        ImGui::SliderInt("Размерность", &g.cols, 1, MAXN);
        g.rows = 1;
    } else {
        ImGui::SliderInt(withRhs ? "Уравнений" : "Строк", &g.rows, 1, MAXN);
        ImGui::SetNextItemWidth(150);
        ImGui::SliderInt(withRhs ? "Неизвестных" : "Столбцов", &g.cols, 1, MAXN);
    }
    ImGui::Spacing();

    for (int i = 0; i < g.rows; i++) {
        for (int j = 0; j < g.cols; j++) {
            if (j > 0) ImGui::SameLine();
            ImGui::PushID(i * MAXN + j);
            ImGui::SetNextItemWidth(66);
            ImGui::InputText("##cell", g.cells[i][j], CELL, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::PopID();
        }
        if (withRhs) {
            ImGui::SameLine();
            ImGui::TextDisabled("|");
            ImGui::SameLine();
            ImGui::PushID(1000 + i);
            ImGui::SetNextItemWidth(66);
            ImGui::InputText("##rhs", g.rhs[i], CELL, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    if (ImGui::SmallButton("Очистить")) {
        g.clear();
    }
    ImGui::PopID();
}

void drawResult(const Result& r) {
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(ACCENT, "Результат");
    ImGui::Spacing();

    if (!r.has) {
        ImGui::TextDisabled("Введите данные и нажмите кнопку операции");
        return;
    }

    if (r.error) {
        ImGui::PushStyleColor(ImGuiCol_Text, ERROR_COLOR);
        ImGui::TextWrapped("Ошибка: %s", r.text.c_str());
        ImGui::PopStyleColor();
        return;
    }

    if (!r.title.empty()) {
        ImGui::Text("%s", r.title.c_str());
    }

    if (!r.table.empty()) {
        ImGui::Spacing();
        int columns = (int)r.table[0].size();
        if (ImGui::BeginTable("result", columns,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
            for (size_t i = 0; i < r.table.size(); i++) {
                ImGui::TableNextRow();
                for (int j = 0; j < columns; j++) {
                    ImGui::TableNextColumn();
                    ImGui::Text("  %s  ", r.table[i][j].c_str());
                }
            }
            ImGui::EndTable();
        }
    }

    if (!r.text.empty()) {
        ImGui::Spacing();
        ImGui::TextWrapped("%s", r.text.c_str());
    }
}

// ---------- Вкладка "Матрицы" ----------

void matrixTab() {
    static GridEditor A, B;
    static char k[CELL] = "2";
    static Result result;

    ImGui::Spacing();
    ImGui::BeginGroup();
    drawGrid("A", A, "Матрица A", false);
    ImGui::EndGroup();
    ImGui::SameLine(0, 50);
    ImGui::BeginGroup();
    drawGrid("B", B, "Матрица B", false);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(ACCENT, "Операции");
    ImGui::Spacing();

    if (ImGui::Button("A + B")) {
        run(result, [&] { result.showMatrix("A + B", gridToMatrix(A, "A").add(gridToMatrix(B, "B"))); });
    }
    ImGui::SameLine();
    if (ImGui::Button("A - B")) {
        run(result, [&] { result.showMatrix("A - B", gridToMatrix(A, "A").subtract(gridToMatrix(B, "B"))); });
    }
    ImGui::SameLine();
    if (ImGui::Button("A × B")) {
        run(result, [&] { result.showMatrix("A × B", gridToMatrix(A, "A").multiply(gridToMatrix(B, "B"))); });
    }
    ImGui::SameLine(0, 30);
    if (ImGui::Button("Транспонировать A")) {
        run(result, [&] { result.showMatrix("Транспонированная A", gridToMatrix(A, "A").transpose()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Определитель A")) {
        run(result, [&] { result.showText("Определитель A", gridToMatrix(A, "A").determinant().toString()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Ранг A")) {
        run(result, [&] { result.showText("Ранг A", std::to_string(gridToMatrix(A, "A").rank())); });
    }

    ImGui::Spacing();
    ImGui::SetNextItemWidth(90);
    ImGui::InputText("число k", k, CELL, ImGuiInputTextFlags_AutoSelectAll);
    ImGui::SameLine();
    if (ImGui::Button("k × A")) {
        run(result, [&] {
            Fraction number = parseCell(k, "k");
            result.showMatrix("k × A", gridToMatrix(A, "A").multiplyByNumber(number));
        });
    }

    drawResult(result);
}

// ---------- Вкладка "Векторы" ----------

void vectorTab() {
    static GridEditor a, b, c;
    static bool first = true;
    if (first) {
        a.cols = b.cols = c.cols = 3;
        first = false;
    }
    static char k[CELL] = "2";
    static Result result;

    ImGui::Spacing();
    ImGui::BeginGroup();
    drawGrid("a", a, "Вектор a", true);
    ImGui::EndGroup();
    ImGui::SameLine(0, 50);
    ImGui::BeginGroup();
    drawGrid("b", b, "Вектор b", true);
    ImGui::EndGroup();
    ImGui::SameLine(0, 50);
    ImGui::BeginGroup();
    drawGrid("c", c, "Вектор c (для смешанного)", true);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(ACCENT, "Операции");
    ImGui::Spacing();

    if (ImGui::Button("a + b")) {
        run(result, [&] { result.showText("a + b", gridToVector(a, "a").add(gridToVector(b, "b")).toString()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("a - b")) {
        run(result, [&] { result.showText("a - b", gridToVector(a, "a").subtract(gridToVector(b, "b")).toString()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Скалярное a · b")) {
        run(result, [&] { result.showText("Скалярное произведение", gridToVector(a, "a").dot(gridToVector(b, "b")).toString()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Векторное a × b")) {
        run(result, [&] { result.showText("Векторное произведение", gridToVector(a, "a").cross(gridToVector(b, "b")).toString()); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Смешанное (a, b, c)")) {
        run(result, [&] {
            result.showText("Смешанное произведение",
                            gridToVector(a, "a").mixed(gridToVector(b, "b"), gridToVector(c, "c")).toString());
        });
    }

    if (ImGui::Button("Длина |a|")) {
        run(result, [&] { result.showText("Длина a", number(gridToVector(a, "a").length(), "%.6g")); });
    }
    ImGui::SameLine();
    if (ImGui::Button("Угол между a и b")) {
        run(result, [&] { result.showText("Угол между a и b", number(gridToVector(a, "a").angle(gridToVector(b, "b")), "%.4f") + "°"); });
    }

    ImGui::SameLine(0, 30);
    ImGui::SetNextItemWidth(90);
    ImGui::InputText("число k", k, CELL, ImGuiInputTextFlags_AutoSelectAll);
    ImGui::SameLine();
    if (ImGui::Button("k × a")) {
        run(result, [&] {
            Fraction number = parseCell(k, "k");
            result.showText("k × a", gridToVector(a, "a").multiplyByNumber(number).toString());
        });
    }

    drawResult(result);
}

// ---------- Вкладка "СЛАУ" ----------

std::string describeSolution(Solution s) {
    if (s.type == SolutionType::None) {
        return "Решений нет";
    }
    if (s.type == SolutionType::Unique) {
        std::string text = "Единственное решение:\n";
        for (size_t i = 0; i < s.x.size; i++) {
            text += "x" + std::to_string(i + 1) + " = " + s.x.data[i].toString() + "\n";
        }
        return text;
    }

    std::string text = "Бесконечно много решений.\nОбщее решение:\nx = " + s.x.toString();
    for (size_t i = 0; i < s.basis.size(); i++) {
        Vector basis = s.basis[i];
        text += " + t" + std::to_string(i + 1) + " * " + basis.toString();
    }
    text += "\nгде t - любые числа:";
    for (size_t i = 0; i < s.freeCols.size(); i++) {
        text += " t" + std::to_string(i + 1) + " = x" + std::to_string(s.freeCols[i] + 1);
        if (i + 1 < s.freeCols.size()) text += ",";
    }
    return text;
}

void slaeTab() {
    static GridEditor system;
    static Result result;
    static bool first = true;
    if (first) {
        system.rows = system.cols = 3;
        first = false;
    }

    ImGui::Spacing();
    ImGui::TextDisabled("Система A * x = b: слева коэффициенты, справа после черты - правые части");
    ImGui::Spacing();
    drawGrid("system", system, "Расширенная матрица", false, true);

    ImGui::Spacing();
    if (ImGui::Button("Решить методом Гаусса")) {
        run(result, [&] {
            Solution s = solveGauss(gridToMatrix(system, "A"), gridToRhs(system));
            result.showText("Решение системы", describeSolution(s));
        });
    }

    drawResult(result);
}

// ---------- Оформление ----------

void setupStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0.0f;
    s.FrameRounding = 7.0f;
    s.GrabRounding = 7.0f;
    s.TabRounding = 7.0f;
    s.PopupRounding = 7.0f;
    s.FramePadding = ImVec2(12, 7);
    s.ItemSpacing = ImVec2(10, 9);
    s.WindowPadding = ImVec2(22, 16);
    s.FrameBorderSize = 0.0f;
    s.TabBarBorderSize = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]        = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
    c[ImGuiCol_FrameBg]         = ImVec4(0.15f, 0.17f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBgHovered]  = ImVec4(0.20f, 0.23f, 0.30f, 1.00f);
    c[ImGuiCol_FrameBgActive]   = ImVec4(0.22f, 0.26f, 0.34f, 1.00f);
    c[ImGuiCol_Button]          = ImVec4(0.20f, 0.38f, 0.72f, 1.00f);
    c[ImGuiCol_ButtonHovered]   = ImVec4(0.28f, 0.48f, 0.88f, 1.00f);
    c[ImGuiCol_ButtonActive]    = ImVec4(0.18f, 0.34f, 0.66f, 1.00f);
    c[ImGuiCol_Tab]             = ImVec4(0.14f, 0.16f, 0.21f, 1.00f);
    c[ImGuiCol_TabHovered]      = ImVec4(0.28f, 0.48f, 0.88f, 1.00f);
    c[ImGuiCol_TabSelected]     = ImVec4(0.20f, 0.38f, 0.72f, 1.00f);
    c[ImGuiCol_SliderGrab]      = ImVec4(0.36f, 0.62f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrabActive]= ImVec4(0.50f, 0.72f, 1.00f, 1.00f);
    c[ImGuiCol_Separator]       = ImVec4(0.22f, 0.25f, 0.32f, 1.00f);
    c[ImGuiCol_TableBorderStrong] = ImVec4(0.30f, 0.34f, 0.44f, 1.00f);
    c[ImGuiCol_TableBorderLight]  = ImVec4(0.22f, 0.25f, 0.32f, 1.00f);
}

// ищет в системе шрифт с русскими буквами
std::string findFont() {
    const char* candidates[] = {
#ifdef _WIN32
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
#elif defined(__APPLE__)
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
#endif
    };
    for (const char* path : candidates) {
        std::error_code error;
        if (std::filesystem::exists(path, error)) {
            return path;
        }
    }
    return "";
}

} // namespace

int runGui() {
    if (!glfwInit()) {
        std::cerr << "Не удалось запустить GLFW\n";
        return 1;
    }

#ifdef __APPLE__
    const char* glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow* window = glfwCreateWindow(1180, 780, "Матричный калькулятор", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Не удалось создать окно (нужна видеокарта с OpenGL 3). "
                     "Можно запустить с параметром --console\n";
        glfwTerminate();
        return 1;
    }

#ifdef _WIN32
    FreeConsole(); // окно открылось - чёрная консоль больше не нужна
#endif

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // синхронизация с частотой монитора

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // не сохранять положение окон в файл

    setupStyle();

    // шрифты с кириллицей: обычный и крупный для заголовка
    ImFont* titleFont = nullptr;
    std::string fontPath = findFont();
    if (!fontPath.empty()) {
        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 19.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        titleFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 30.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // одно окно на весь экран
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("main", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::PushFont(titleFont);
        ImGui::TextColored(ACCENT, "Матричный калькулятор");
        ImGui::PopFont();
        ImGui::TextDisabled("Матрицы, векторы и системы уравнений. Числа можно вводить дробями: 1/3, -2.5, 7");
        ImGui::Spacing();

        if (ImGui::BeginTabBar("tabs")) {
            if (ImGui::BeginTabItem("Матрицы")) {
                matrixTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Векторы")) {
                vectorTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Системы уравнений (СЛАУ)")) {
                slaeTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::Render();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.09f, 0.10f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
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
