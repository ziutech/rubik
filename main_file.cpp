/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include "constants.h"
#include "lodepng.h"
#include "myCube.h"
#include "myTeapot.h"
#include "shaderprogram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

ShaderProgram *sp;

// Odkomentuj, żeby rysować kostkę
/*float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;*/

// Odkomentuj, żeby rysować czajnik
float *vertices = myTeapotVertices;
float *normals = myTeapotNormals;
float *texCoords = myTeapotTexCoords;
float *colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;

// Procedura obsługi błędów
void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

class slash_plus_space_locale : public std::ctype<char> {
public:
  mask const *get_table() {
    static std::vector<std::ctype<char>::mask> table(
        classic_table(), classic_table() + table_size);
    table['/'] = (mask)space;
    return &table[0];
  }
  slash_plus_space_locale(size_t refs = 0)
      : std::ctype<char>(get_table(), false, refs) {}
};

struct obj3dmodel {
  std::vector<float> verts;
  std::vector<float> norms;
  std::vector<float> tex;
  std::vector<unsigned int> faces;
  std::vector<float> colors;

  void from_file(const char *filename);
  void draw();
};

void obj3dmodel::draw() {
  glDrawElements(GL_TRIANGLES, this->faces.size(), GL_UNSIGNED_INT,
                 this->faces.data());
}

void obj3dmodel::from_file(const char *filename) {
  std::ifstream in(filename, std::ios::in);
  if (!in) {
    std::cerr << "Cannot open " << filename << std::endl;
    exit(1);
  }
  std::array<float, 4> v = {0, 0, 0, 1.0f};
  std::string line;
  while (getline(in, line)) {
    auto l = line.substr(0, 2);
    std::istringstream s(line.substr(2));
    if (l == "v ") {
      s >> v[0];
      s >> v[1];
      s >> v[2];
      verts.insert(verts.end(), v.begin(), v.end());
    } else if (l == "vn") {
      s >> v[0];
      s >> v[1];
      s >> v[2];
      norms.insert(norms.end(), std::begin(v), std::end(v) - 1);
    } else if (l == "f ") {
      s.imbue(std::locale(std::locale(), new slash_plus_space_locale));
      for (int i = 0; i < 3; i++) {
        int v, vt, vn;
        s >> v;
        s >> vt;
        s >> vn;
        faces.push_back(v - 1);
      }
    } else if (l == "vt") {
      std::array<float, 3> t = {0};
      s >> t[0]; // u
      s >> t[1]; // v
      t[2] = 0;  // w
      tex.insert(tex.end(), t.begin(), t.end());
    }
  }
}

obj3dmodel kostka;

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_LEFT)
      speed_x = -PI / 2;
    if (key == GLFW_KEY_RIGHT)
      speed_x = PI / 2;
    if (key == GLFW_KEY_UP)
      speed_y = PI / 2;
    if (key == GLFW_KEY_DOWN)
      speed_y = -PI / 2;
  }
  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_LEFT)
      speed_x = 0;
    if (key == GLFW_KEY_RIGHT)
      speed_x = 0;
    if (key == GLFW_KEY_UP)
      speed_y = 0;
    if (key == GLFW_KEY_DOWN)
      speed_y = 0;
  }
}

void windowResizeCallback(GLFWwindow *window, int width, int height) {
  if (height == 0)
    return;
  aspectRatio = (float)width / (float)height;
  glViewport(0, 0, width, height);
}

// Procedura inicjująca
void initOpenGLProgram(GLFWwindow *window) {
  //************Tutaj umieszczaj kod, który należy wykonać raz, na początku
  // programu************
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glfwSetWindowSizeCallback(window, windowResizeCallback);
  glfwSetKeyCallback(window, keyCallback);

  kostka.from_file("kostka.obj");
  float color[4] = {
      1.0f,
      0.5f,
      0.5f,
      1.0f,
  };
  for (int i = 0; i < kostka.verts.size(); i++) {
    kostka.colors.insert(kostka.colors.end(), std::begin(color),
                         std::end(color));
  }
  sp = new ShaderProgram("v_simplest.glsl", "g_simplest.glsl",
                         "f_simplest.glsl");
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow *window) {
  //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli
  // głównej************

  delete sp;
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow *window, float angle_x, float angle_y) {
  //************Tutaj umieszczaj kod rysujący obraz******************l
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 V =
      glm::lookAt(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0),
                  glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz widoku

  glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f,
                                 50.0f); // Wylicz macierz rzutowania

  glm::mat4 M = glm::mat4(1.0f);
  M = glm::rotate(M, angle_y,
                  glm::vec3(1.0f, 0.0f, 0.0f)); // Wylicz macierz modelu
  M = glm::rotate(M, angle_x,
                  glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz modelu

  sp->use(); // Aktywacja programu cieniującego
  // Przeslij parametry programu cieniującego do karty graficznej
  glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
  glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

  glEnableVertexAttribArray(
      sp->a("vertex")); // Włącz przesyłanie danych do atrybutu vertex
  glVertexAttribPointer(
      sp->a("vertex"), 4, GL_FLOAT, false, 0,
      kostka.verts.data()); // Wskaż tablicę z danymi dla atrybutu vertex

  glEnableVertexAttribArray(
      sp->a("color")); // Włącz przesyłanie danych do atrybutu vertex
  glVertexAttribPointer(
      sp->a("color"), 4, GL_FLOAT, false, 0,
      kostka.colors.data()); // Wskaż tablicę z danymi dla atrybutu color

  kostka.draw();

  glDisableVertexAttribArray(
      sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
  glDisableVertexAttribArray(
      sp->a("color")); // Wyłącz przesyłanie danych do atrybutu color

  glfwSwapBuffers(window); // Przerzuć tylny bufor na przedni
}

int main(void) {
  GLFWwindow *window; // Wskaźnik na obiekt reprezentujący okno

  glfwSetErrorCallback(error_callback); // Zarejestruj procedurę obsługi błędów

  if (!glfwInit()) { // Zainicjuj bibliotekę GLFW
    fprintf(stderr, "Nie można zainicjować GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window = glfwCreateWindow(
      500, 500, "OpenGL", NULL,
      NULL); // Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

  if (!window) // Jeżeli okna nie udało się utworzyć, to zamknij program
  {
    fprintf(stderr, "Nie można utworzyć okna.\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(
      window); // Od tego momentu kontekst okna staje się aktywny i polecenia
               // OpenGL będą dotyczyć właśnie jego.
  glfwSwapInterval(
      1); // Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

  if (glewInit() != GLEW_OK) { // Zainicjuj bibliotekę GLEW
    fprintf(stderr, "Nie można zainicjować GLEW.\n");
    exit(EXIT_FAILURE);
  }

  initOpenGLProgram(window); // Operacje inicjujące

  // Główna pętla
  float angle_x = 0; // Aktualny kąt obrotu obiektu
  float angle_y = 0; // Aktualny kąt obrotu obiektu
  glfwSetTime(0);    // Zeruj timer
  while (!glfwWindowShouldClose(
      window)) // Tak długo jak okno nie powinno zostać zamknięte
  {
    angle_x +=
        speed_x *
        glfwGetTime(); // Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i
                       // czasu jaki upłynał od poprzedniej klatki
    angle_y +=
        speed_y *
        glfwGetTime(); // Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i
                       // czasu jaki upłynał od poprzedniej klatki
    glfwSetTime(0);                      // Zeruj timer
    drawScene(window, angle_x, angle_y); // Wykonaj procedurę rysującą
    glfwPollEvents(); // Wykonaj procedury callback w zalezności od zdarzeń
                      // jakie zaszły.
  }

  freeOpenGLProgram(window);

  glfwDestroyWindow(window); // Usuń kontekst OpenGL i okno
  glfwTerminate();           // Zwolnij zasoby zajęte przez GLFW
  exit(EXIT_SUCCESS);
}
