#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera() {
  view_matrix.SetIdentity();
  SetOrthographic(-1, 1, 1, -1, -1, 1);
}

Vector3 Camera::GetLocalVector(const Vector3 &v) {
  Matrix44 iV = view_matrix;
  if (iV.Inverse() == false)
    std::cout << "Matrix Inverse error" << std::endl;
  Vector3 result = iV.RotateVector(v);
  return result;
}

Vector3 Camera::ProjectVector(Vector3 pos) {
  Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
  Vector4 result = viewprojection_matrix * pos4;
  if (type == ORTHOGRAPHIC)
    return result.GetVector3();
  else
    return result.GetVector3() / result.w;
}

Vector4 Camera::ProjectVectorToClip(Vector3 pos) {
  Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
  Vector4 result = viewprojection_matrix * pos4;
  return result; // Return without perspective divide
}

void Camera::Rotate(float angle, const Vector3 &axis) {
  Matrix44 R;
  R.MakeRotationMatrix(angle, axis);
  Vector3 new_front = R * (center - eye);
  center = eye + new_front;
  UpdateViewMatrix();
}

void Camera::Move(Vector3 delta) {
  Vector3 localDelta = GetLocalVector(delta);
  eye = eye - localDelta;
  center = center - localDelta;
  UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom,
                             float near_plane, float far_plane) {
  type = ORTHOGRAPHIC;

  this->left = left;
  this->right = right;
  this->top = top;
  this->bottom = bottom;
  this->near_plane = near_plane;
  this->far_plane = far_plane;

  UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane,
                            float far_plane) {
  type = PERSPECTIVE;

  this->fov = fov;
  this->aspect = aspect;
  this->near_plane = near_plane;
  this->far_plane = far_plane;

  UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3 &eye, const Vector3 &center,
                    const Vector3 &up) {
  this->eye = eye;
  this->center = center;
  this->up = up;

  UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
	//SetExampleViewMatrix();
	
  // 1. Calcular vectors de la càmera (Ortonormals)
  Vector3 forward = (center - eye).Normalize();
  Vector3 right = forward.Cross(up).Normalize();
  Vector3 top = right.Cross(forward).Normalize();

  // 2. Construir Matriu de Rotació (Camera -> World)
  // Nota: En OpenGL la càmera mira cap a -Z, per això usem -forward a la
  // columna Z
  Matrix44 R;
  R.SetIdentity();

  // Columna 0 (Right / Eix X)
  R.m[0] = right.x;
  R.m[1] = right.y;
  R.m[2] = right.z;

  // Columna 1 (Top / Eix Y)
  R.m[4] = top.x;
  R.m[5] = top.y;
  R.m[6] = top.z;

  // Columna 2 (Back / Eix Z). Important: Z apunta en direcció contrària a
  // forward!
  R.m[8] = -forward.x;
  R.m[9] = -forward.y;
  R.m[10] = -forward.z;

  // Ara R defineix l'orientació de la càmera. La View Matrix és la inversa
  // (Rotació -> Transposada)
  R.Transpose();

  // 3. Construir Matriu de Traslació (moure el món perquè eye sigui l'origen)
  Matrix44 T;
  T.MakeTranslationMatrix(-eye.x, -eye.y, -eye.z);

  // 4. Combinar: Primer traslladem, després rotem
  view_matrix = R * T;
 
  UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix() {
  // Reset Matrix (Identity)
  projection_matrix.SetIdentity();

  // Comment this line to create your own projection matrix!
  //SetExampleProjectionMatrix();

  // Remember how to fill a Matrix4x4 (check framework slides)

  if (type == PERSPECTIVE) {
    // Convert FOV to radians
    float fov_rad = fov * DEG2RAD;
    float f = 1.0f / tan(fov_rad / 2.0f);

    // Column 0
    projection_matrix.m[0] = f / aspect;
    projection_matrix.m[1] = 0.0f;
    projection_matrix.m[2] = 0.0f;
    projection_matrix.m[3] = 0.0f;

    // Column 1
    projection_matrix.m[4] = 0.0f;
    projection_matrix.m[5] = f;
    projection_matrix.m[6] = 0.0f;
    projection_matrix.m[7] = 0.0f;

    // Column 2
    projection_matrix.m[8] = 0.0f;
    projection_matrix.m[9] = 0.0f;
    projection_matrix.m[10] =
        (far_plane + near_plane) / (near_plane - far_plane);
    projection_matrix.m[11] = -1.0f;

    // Column 3
    projection_matrix.m[12] = 0.0f;
    projection_matrix.m[13] = 0.0f;
    projection_matrix.m[14] =
        (2.0f * far_plane * near_plane) / (near_plane - far_plane);
    projection_matrix.m[15] = 0.0f;

  } else if (type == ORTHOGRAPHIC) {
    float rl = right - left;
    float tb = top - bottom;
    float fn = far_plane - near_plane;

    projection_matrix.m[0]  =  2.0f / rl;
    projection_matrix.m[5]  =  2.0f / tb;
    projection_matrix.m[10] = -2.0f / fn;

    projection_matrix.m[12] = -(right + left) / rl;
    projection_matrix.m[13] = -(top + bottom) / tb;
    projection_matrix.m[14] = -(far_plane + near_plane) / fn;

    projection_matrix.m[15] = 1.0f;
 }

  UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix() {
  viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix() {
  UpdateViewMatrix();
  UpdateProjectionMatrix();

  return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.
void Camera::SetExampleViewMatrix() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y,
            up.z);
  glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m);
}

void Camera::SetExampleProjectionMatrix() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (type == PERSPECTIVE)
    gluPerspective(fov, aspect, near_plane, far_plane);
  else
    glOrtho(left, right, bottom, top, near_plane, far_plane);

  glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m);
  glMatrixMode(GL_MODELVIEW);
}
