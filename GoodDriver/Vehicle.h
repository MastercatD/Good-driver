#include <GLFW/glfw3.h>

#include <math.h>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define PI 3.1415926535


void DrawRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height,
              GLfloat angle, GLfloat red, GLfloat green, GLfloat blue,
              GLfloat xOffset = 0, GLfloat yOffset = 0);

//  ��������� � ���������� ������
struct Position {
  GLfloat x, y, angle;
};

// ����� ������������� ��������
class Vehicle {
  Position pos;

 public:
  Position GetPosition();
  void SetPosition(Position pos);
  //  ���������
  virtual void Settings() = 0;
  //  ���������
  virtual void Display() = 0;
  //  ��������
  virtual void Move() = 0;
};

//  ����� ����������
class Car : public Vehicle {
  GLfloat speed = 0.0f, angularSpeed = 0.5, maxSpeed = 8.0f,
          acceleration = 0.1f;
  GLbyte gasStatus = 0, rotateStatus = 0;

 public:
  Car(GLfloat x, GLfloat y, GLfloat angle);
  void SetGasStatus(GLbyte gasStatus);
  void SetRotateStatus(GLbyte rotateStatus);
  //  ���������
  void Settings() override;
  //  ���������
  void Display() override;
  //  ��������
  void Move() override;
};
