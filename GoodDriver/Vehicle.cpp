#include "Vehicle.h"




Position Vehicle::GetPosition() { return pos; }

void Vehicle::SetPosition(Position pos) {
  this->pos.angle = pos.angle;
  this->pos.x = pos.x;
  this->pos.y = pos.y;
}

  Car::Car(GLfloat x, GLfloat y, GLfloat angle) {
  Position pos;
  pos.x = x;
  pos.y = y;
  pos.angle = angle;
  this->SetPosition(pos);
}

void Car::SetGasStatus(GLbyte gasStatus) { this->gasStatus = gasStatus; }

void Car::SetRotateStatus(GLbyte rotateStatus) {
  this->rotateStatus = rotateStatus;
}

void Car::Settings() {
  ImGui::Text(u8"Максимальная скорость");
  ImGui::SliderFloat(u8"1", &maxSpeed, 1.0f, 30.0f);
  ImGui::Text(u8"Ускорение");
  ImGui::SliderFloat(u8"2", &acceleration, 0.0f, 0.1f);
  ImGui::Text(u8"Скорость поворота");
  ImGui::SliderFloat(u8"3", &angularSpeed, 0.01f, 1.0f);
}

void Car::Display() {
  Position pos = GetPosition();
  DrawRect(pos.x, 0, 50, 100, pos.angle, 0.5f, 0.0f, 0.0f);
  DrawRect(pos.x, 0, 40, 60, pos.angle, 0.25f, 0.0f, 0.0f, 0, -20);
  DrawRect(pos.x, 0, 15, 5, pos.angle, 1.0f, 1.0f, 0.0f, 25, 95);
  DrawRect(pos.x, 0, 15, 5, pos.angle, 1.0f, 1.0f, 0.0f, -25, 95);
  DrawRect(pos.x, 0, 10, 5, pos.angle, 1.0f, 0.0f, 0.0f, 30, -95);
  DrawRect(pos.x, 0, 10, 5, pos.angle, 1.0f, 0.0f, 0.0f, -30, -95);
}
void Car::Move() {
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
      speed /= 1.02;
    } break;
  }
  pos.angle += angularSpeed * rotateStatus * speed;
  if (pos.angle > 360)
    pos.angle -= 360;
  else if (pos.angle < 0)
    pos.angle += 360;
  //  Движение машины
  pos.x -= speed * sin(pos.angle * PI / 180);
  pos.y += speed * cos(pos.angle * PI / 180);
  //  Зацикливание координаты y
  if (pos.y > 450) pos.y -= 900;
  if (pos.y < -450) pos.y += 900;
  //  Левая и правая границы
  if (pos.x < 400 && pos.x > -500) SetPosition(pos);
}

