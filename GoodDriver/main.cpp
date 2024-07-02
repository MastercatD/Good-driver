#include "GoodDriver.h"

// Перехват клавиш
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);
//  Рисование дороги
void DisplayRoad(Vehicle *vehicle);
//  Дорожные правила
void RoadRules(Vehicle *vehicle,bool* violation1, bool* violation2, bool* turn,
               bool* turnSuccess, bool* turnFail);
//  Рисование
void Paint(Vehicle* vehicle);

// Глобальные переменные
Car car(-100, 0, 0);
GLfloat rideAngle = 70, turnAngle = 45;



// Рисование
void Paint(Vehicle *vehicle) {
  glClear(GL_COLOR_BUFFER_BIT);
  DisplayRoad(vehicle);
  vehicle->Display();
}

//  Рисование прямоугольника
void DrawRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height,
              GLfloat angle, GLfloat red, GLfloat green, GLfloat blue,
              GLfloat xOffset, GLfloat yOffset) {
  GLfloat step = 1.0f / 500.0f;

  glColor3f(red, green, blue);
  glPushMatrix();
  glScalef(step, step, 1);
  glTranslatef(x, y, 1.0f);
  glRotatef(angle, 0.0, 0.0, 1.0);

  glBegin(GL_QUADS);
  glVertex2f(-width / 2 + xOffset / 2, -height / 2 + yOffset / 2);
  glVertex2f(width / 2 + xOffset / 2, -height / 2 + yOffset / 2);
  glVertex2f(width / 2 + xOffset / 2, height / 2 + yOffset / 2);
  glVertex2f(-width / 2 + xOffset / 2, height / 2 + yOffset / 2);

  glEnd();
  glPopMatrix();
}

void DisplayRoad(Vehicle *vehicle) {
  Position pos = vehicle->GetPosition();
  DrawRect(-450, 0, 20, 1800, 0, 1, 1, 1);
  DrawRect(0, 0, 20, 1800, 0, 1, 1, 1);
  DrawRect(-225, 450 - pos.y, 20, 700, 0, 1, 1, 1);
  DrawRect(-225, -450 -pos.y, 20, 700, 0, 1, 1, 1);

}


void RoadRules(Vehicle *vehicle, bool* violation1, bool* violation2, bool* turn,
               bool* turnSuccess, bool* turnFail) {
  Position pos = car.GetPosition();
  if (pos.x < -450 || pos.x > 0)
    //  Выезд за дорогу
    *violation2 = true;
  else if (pos.y > -70 && pos.y < 70) {
    //  Разрыв
    if ((pos.x > -260 && pos.angle > turnAngle && pos.angle < 180) ||
        (pos.x < -190 && pos.angle > 180 + turnAngle))
      *turn = true;

  } else if ((pos.x > -260 && pos.angle > rideAngle &&
              pos.angle < 360 - rideAngle) ||
             (pos.x < -190 &&
              !(pos.angle > 180 - rideAngle && pos.angle < 180 + rideAngle))) {
    //  Встречное движение
    *violation1 = true;
    if (*turn) {
      //  Неудачный разворот
      *turn = false;
      *turnFail = true;
      glfwSetTime(0);
    }
  } else if (*turn) {
    //  Успешный разворот
    *turn = false;
    *turnSuccess = true;
    glfwSetTime(0);
  }
}

//  Перехват клавиатуры
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  switch (key) {
    // Движение
    case GLFW_KEY_W: {
      if (action == GLFW_PRESS)
        car.SetGasStatus(1);
      else if (action == GLFW_RELEASE)
        car.SetGasStatus(0);
    } break;
    case GLFW_KEY_S: {
      if (action == GLFW_PRESS)
        car.SetGasStatus(-1);
      else if (action == GLFW_RELEASE)
        car.SetGasStatus(0);
    } break;
    // Поворот
    case GLFW_KEY_D: {
      if (action == GLFW_PRESS)
        car.SetRotateStatus(-1);
      else if (action == GLFW_RELEASE)
        car.SetRotateStatus(0);
    } break;
    case GLFW_KEY_A: {
      if (action == GLFW_PRESS)
        car.SetRotateStatus(1);
      else if (action == GLFW_RELEASE)
        car.SetRotateStatus(0);
    } break;
  }
}


static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Основная программа
int main(int, char**) {

  //  Состояния событий
  bool violation1 = false, violation2 = false, turn = false;
  bool turnSuccess = false, turnFail = false;


  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(
      900, 900, "Good driver", nullptr, nullptr);
  if (window == nullptr) return 1;

  glfwSetKeyCallback(window, keyCallback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("Dudka Bold Italic.ttf", 16, NULL, io.Fonts->GetGlyphRangesCyrillic());
  (void)io;
 

  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);

  
  // Our state
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
#ifdef __EMSCRIPTEN__
  io.IniFilename = nullptr;
  EMSCRIPTEN_MAINLOOP_BEGIN
#else


  //  Цикл программы
  while (!glfwWindowShouldClose(window))
#endif
  {

    glfwPollEvents();

    violation2 = false;
    violation1 = false;
    //  Движение
    car.Move();
    //  Логика правил
    RoadRules(&car, &violation1, &violation2, &turn, &turnSuccess, &turnFail);
    //  Рисование
    Paint(&car);

    // Интерфейс
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //  Окно уведомлений
    if (violation1 || violation2 || turn || turnSuccess || turnFail) {
      ImGui::Begin(u8"Уведомление");
      if (violation1) ImGui::Text(u8"Выезд на встречную полосу");
      if (violation2) ImGui::Text(u8"Выезд за дорогу");
      if (turn) ImGui::Text(u8"Совершается разворот");
      if (turnSuccess) {
        ImGui::Text(u8"Разворот совершён успешно");
        if (glfwGetTime() > 2) turnSuccess = false;
      }
      if (turnFail) {
        ImGui::Text(u8"Разворот совершён неправильно");
        if (glfwGetTime() > 2) turnFail = false;
      }
      ImGui::End();
    }
    
    //  Окно настроек
    ImGui::Begin(u8"Настройки");
    car.Settings();
    ImGui::Text(u8"Угол начала разворота");
    ImGui::SliderFloat(u8"4", &turnAngle, 0.0f, 180.0f);
    ImGui::Text(u8"Допустимый угол езды");
    ImGui::SliderFloat(u8"5", &rideAngle, 0.0f, 180.0f);
    ImGui::End();
    
    // Отображение
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);


    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  // Очистка
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}