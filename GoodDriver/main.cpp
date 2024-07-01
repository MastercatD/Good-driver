#include <stdio.h>
#include <math.h>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>  

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#define PI 3.1415926535

struct Point2D {
  double x, y;
};

bool Interseсt(Point2D point11, Point2D point12, Point2D point21,
               Point2D point22) {
  bool result = false;
  double eps = 10e-10;
  //  Точки и векторы, лежащие на прямых
  double x01 = point11.x, x02 = point21.x;
  double vector11 = point12.x - point11.x,
         vector12 = point22.x - point21.x;
  double y01 = point11.y, y02 = point21.y;
  double vector21 = point12.y - y01,
         vector22 = point22.y - y02;
  double z01 = 1, z02 = 1;
  double vector31 = 0,
         vector32 = 0;
  //  Находим пересечение путём выражения параметров s и t из уравнений прямых в
  //  параметрическом виде
  double del =
      vector21 * vector12 / vector11 -
      vector22;  // Делитель выражения, если равен нулю, то прямые параллельные
  if (del >= eps && vector11 >= 0) {
    double s = -(y01 - y02 - (x01 - x02) / vector11) /
               (vector21 * vector12 / vector11 - vector22);
    double t = (x01 - vector12 * s - x02) / -vector11;
    double x = vector12 * s + x02;
    double y = vector22 * s + y02;
    double z = vector32 * s + z02;
    if (abs(x - (vector11 * t + x01)) < eps &&
        abs(y - (vector21 * t + y01)) < eps) //  Если система решена неправильно, то прямые скрещивающиеся
        {  // Проверка, чтобы точка лежала на обоих отрезках
      result = true;
    }
  }
  return result;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void DrawRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height,
              GLfloat angle, GLfloat red, GLfloat green, GLfloat blue,
              GLfloat xOffset = 0, GLfloat yOffset = 0);

void DisplayRoad();

void RoadRules();


struct Position {
  GLfloat x, y, angle;
};
// 
class Vehicle {
  Position pos;
 public:
  Position GetPosition() { return pos; }
  void SetPosition(Position pos) {
    this->pos.angle = pos.angle;
    this->pos.x = pos.x;
    this->pos.y = pos.y;
  }
  virtual void Display() = 0;
  virtual void Move() = 0;
};

class Car:public Vehicle {
  GLfloat speed = 0.0f, angularSpeed = 0.5, maxSpeed = 8.0f, acceleration = 0.1f;
  GLbyte gasStatus = 0, rotateStatus = 0;
 public:
  Car(GLfloat x, GLfloat y, GLfloat angle) {
    Position pos;
    pos.x = x;
    pos.y = y;
    pos.angle = angle;
    this->SetPosition(pos);
  }
   void SetGasStatus(GLbyte gasStatus) { this->gasStatus = gasStatus;
  }
  void SetRotateStatus(GLbyte rotateStatus) { this->rotateStatus = rotateStatus; }
  void Display() override {
    Position pos = GetPosition();
    DrawRect(pos.x, 0, 50, 100, pos.angle, 0.5f, 0.0f, 0.0f);
    DrawRect(pos.x, 0, 40, 60, pos.angle, 0.25f, 0.0f, 0.0f, 0, -20);
    DrawRect(pos.x, 0, 15, 5, pos.angle, 1.0f, 1.0f, 0.0f, 25, 95);
    DrawRect(pos.x, 0, 15, 5, pos.angle, 1.0f, 1.0f, 0.0f, -25, 95);
    DrawRect(pos.x, 0, 10, 5, pos.angle, 1.0f, 0.0f, 0.0f, 30, -95);
    DrawRect(pos.x, 0, 10, 5, pos.angle, 1.0f, 0.0f, 0.0f, -30, -95);
  }
  void Move() {
    Position pos = GetPosition();
    switch (gasStatus) {
      //  Ускорение вперёд
      case 1: { 
        speed += acceleration;
        if (speed >= maxSpeed) speed = maxSpeed;

      } break;
      //  Ускорение назад
      case -1: {
        speed -= acceleration;
        if (speed <= -maxSpeed) speed = -maxSpeed;

      } break;
      //  Замедление
      case 0: {
        speed /= 1.1;
      } break;
    }
    pos.angle += angularSpeed * rotateStatus * speed;
    if (pos.angle > 360)
      pos.angle -= 360;
    else if (pos.angle < 0)
      pos.angle += 360;
    pos.x -= speed * sin(pos.angle * PI / 180);
    pos.y += speed * cos(pos.angle * PI / 180);
    if (pos.y > 450) pos.y -= 900;
    if (pos.y < -450) pos.y += 900;
    SetPosition(pos);
  }

};



// Глобальные переменные
Car car(-100,0,0);


// Рисование
void Paint() {
  glClear(GL_COLOR_BUFFER_BIT);
  DisplayRoad();
  car.Display();
 // Position pos = car.GetPosition();
 // Point2D v1, v2, v3, v4;
 // v1.x = pos.x;
  //v1.y = pos.y;
 // v2.x = pos.x + ;
 // v2.y = pos.y;
}


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

void DisplayRoad() {
  Position pos = car.GetPosition();
  DrawRect(-450, 0, 20, 1800, 0, 1, 1, 1);
  DrawRect(0, 0, 20, 1800, 0, 1, 1, 1);
  DrawRect(-225, 450 - pos.y, 20, 700, 0, 1, 1, 1);
  DrawRect(-225, -450 -pos.y, 20, 700, 0, 1, 1, 1);
  //printf("\n%f", pos.y);

}


void RoadRules() {
  Position pos = car.GetPosition();
  if (pos.y > -70 && pos.y < 70) {
    //printf("\n%f",pos.x);

  } else if (pos.x > -225 && pos.angle > 70 && pos.angle < 290) {
    printf("Движение по встречке на правой стороне");
  }


}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  switch (key) {
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

// Main code
int main(int, char**) {
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
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+
  // only
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
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
#ifdef __EMSCRIPTEN__
  // For an Emscripten build we are disabling file-system access, so let's not
  // attempt to do a fopen() of the imgui.ini file. You may manually call
  // LoadIniSettingsFromMemory() to load settings from your own storage.
  io.IniFilename = nullptr;
  EMSCRIPTEN_MAINLOOP_BEGIN
#else
  while (!glfwWindowShouldClose(window))
#endif
  {



    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
                                      // and append into it.

      ImGui::Text("This is some useful text.");  // Display some text (you can
                                                 // use a format strings too)
      ImGui::Checkbox(
          "Demo Window",
          &show_demo_window);  // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat(
          "float", &f, 0.0f,
          1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3(
          "clear color",
          (float*)&clear_color);  // Edit 3 floats representing a color

      if (ImGui::Button(
              "Button"))  // Buttons return true when clicked (most widgets
                          // return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window) {
      ImGui::Begin(
          "Another Window",
          &show_another_window);  // Pass a pointer to our bool variable (the
                                  // window will have a closing button that will
                                  // clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me")) show_another_window = false;
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);


    car.Move();
    RoadRules();
    Paint();
    

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}