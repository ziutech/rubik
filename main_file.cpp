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

#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/matrix.hpp>
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
#include <glm/gtx/quaternion.hpp>
#include <vector>

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;
float angle_x = 0; // Aktualny kąt obrotu obiektu
float angle_y = 0; // Aktualny kąt obrotu obiektu
int canRotateWall = 0;
float wallAngle = 0; // Aktualny kąt obrotu ściany
int chooseWall = 0; //wybór ściany
int checkAngle;

//tablica wektorów obrotu poszczególnych kosteczek

glm::vec3 transKostki[27] = {
       // ściana fioletowa
        glm::vec3(1.94f, 1.94f, 1.94f), glm::vec3(0, 1.94f, 1.94f), glm::vec3(-1.94f, 1.94f, 1.94f),
        glm::vec3(1.94f, 1.94f, 0), glm::vec3(0, 1.94f, 0), glm::vec3(-1.94f, 1.94f, 0),
        glm::vec3(1.94f, 1.94f, -1.94f), glm::vec3(0, 1.94f, -1.94f), glm::vec3(-1.94f, 1.94f, -1.94f),
       //ściana pomiędzy
        glm::vec3(1.94f, 0, 1.94f), glm::vec3(0, 0, 1.94f), glm::vec3(-1.94f, 0, 1.94f),
        glm::vec3(1.94f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(-1.94f, 0, 0),
        glm::vec3(1.94f, 0, -1.94f), glm::vec3(0, 0, -1.94f), glm::vec3(-1.94f, 0, -1.94f),
       // ściana jasnoniebieska
        glm::vec3(1.94f, -1.94f, 1.94f), glm::vec3(0, -1.94f, 1.94f), glm::vec3(-1.94f, -1.94f, 1.94f),
        glm::vec3(1.94f, -1.94f, 0), glm::vec3(0, -1.94f, 0), glm::vec3(-1.94f, -1.94f, 0),
        glm::vec3(1.94f, -1.94f, -1.94f), glm::vec3(0, -1.94f, -1.94f), glm::vec3(-1.94f, -1.94f, -1.94f),
};

std::vector<std::vector<int>> posKostki{ {
    // ściana fioletowa
    {1, 1, 1}, {0, 1, 1}, {-1, 1, 1 },
    {1, 1, 0}, {0, 1, 0}, {-1, 1, 0},
    {1, 1, -1}, {0, 1, -1}, {-1, 1, -1},
    //ściana pomiędzy
   {1, 0, 1}, {0, 0, 1}, {-1, 0, 1},
   {1, 0, 0}, {0, 0, 0}, {-1, 0, 0},
   {1, 0, -1}, {0, 0, -1}, {-1, 0, -1},
   // ściana jasnoniebieska
    {1, -1, 1}, {0, -1, 1}, {-1, -1, 1},
    {1, -1, 0}, {0, -1, 0}, {-1, -1, 0},
    {1, -1, -1}, {0, -1, -1}, {-1, -1, -1},
} };

std::vector<std::vector<int>> newPosKostki;

glm::mat4 matKostki[27];

ShaderProgram *sp;
ShaderProgram *highlight;

glm::mat4 mulMat(glm::mat4 mat1, glm::mat4 mat2)
{
    glm::mat4 rslt;

   // printf("Multiplication of given two matrices is:\n");
  
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rslt[i][j] = 0;

            for (int k = 0; k < 4; k++) {
                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }

           // printf("%d\t", rslt[i][j]);
        }

       // printf("\n");
    }
    return rslt;
}

void createMatKostki(glm::mat4 M) //tworz macierze kostki
{
    for (int i = 0; i < 27; i++) matKostki[i] = M;
}

// Odkomentuj, żeby rysować kostkę
/*float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;*/

// Odkomentuj, żeby rysować czajnik
float* vertices = myTeapotVertices;
float* normals = myTeapotNormals;
float* texCoords = myTeapotTexCoords;
float* colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;

float wall_colors[] = {
    1.0, 1.0, 1.0, 0.0, // brzegi
    1.0, 0.0, 0.0, 0.0, // sciana 1
    0.0, 1.0, 0.0, 0.0, // sciana 2
    0.0, 0.0, 1.0, 0.0, // sciana 3
    1.0, 1.0, 0.0, 0.0, // sciana 4
    0.0, 1.0, 1.0, 0.0, // sciana 5
    1.0, 0.0, 1.0, 0.0, // sciana 6
};

GLuint edgeAmbient, edgeBase, edgeHeight, edgeNormal, edgeRoughness;

// Procedura obsługi błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

class slash_plus_space_locale : public std::ctype<char> {
public:
    mask const* get_table() {
        static std::vector<std::ctype<char>::mask> table(
            classic_table(), classic_table() + table_size);
        table['/'] = (mask)space;
        return &table[0];
    }
    slash_plus_space_locale(size_t refs = 0)
        : std::ctype<char>(get_table(), false, refs) {}
};

struct obj3dmodel {

  std::vector<glm::vec4> verts;
  std::vector<int> groups;
  std::vector<glm::vec4> norms;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec4> modelC1;
  std::vector<glm::vec4> modelC2;
  std::vector<glm::vec4> modelC3;
  std::vector<unsigned int> faces;
  std::array<int, 7> wall_mapping;

  void from_file(const char *filename);
  void draw();
  void set_wall_mapping(std::array<int, 7> wall_colors);
};

void obj3dmodel::draw() {
  glUniform4fv(sp->u("wall_colors"), 4 * 7, wall_colors);
  glUniform1iv(sp->u("wall_mapping"), 7, wall_mapping.data());

    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data());


  glEnableVertexAttribArray(sp->a("group"));
  glVertexAttribIPointer(sp->a("group"), 1, GL_INT, 0, groups.data());

  glEnableVertexAttribArray(sp->a("normal"));
  glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data());

  glEnableVertexAttribArray(sp->a("c1"));
  glVertexAttribPointer(sp->a("c1"), 4, GL_FLOAT, false, 0, modelC1.data());
  glEnableVertexAttribArray(sp->a("c2"));
  glVertexAttribPointer(sp->a("c2"), 4, GL_FLOAT, false, 0, modelC2.data());
  glEnableVertexAttribArray(sp->a("c3"));
  glVertexAttribPointer(sp->a("c3"), 4, GL_FLOAT, false, 0, modelC3.data());

  glEnableVertexAttribArray(sp->a("texCoord"));
  glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0,
                        texCoords.data());

  glUniform1i(sp->u("edgeAmbient"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, edgeAmbient);

  glUniform1i(sp->u("edgeBase"), 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, edgeBase);

  glUniform1i(sp->u("edgeHeight"), 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, edgeHeight);

  glUniform1i(sp->u("edgeNormal"), 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, edgeNormal);

  glUniform1i(sp->u("edgeRoughness"), 4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, edgeNormal);

  glDrawArrays(GL_TRIANGLES, 0, verts.size());

  glDisableVertexAttribArray(
      sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
  glDisableVertexAttribArray(
      sp->a("texCoord")); // Wyłącz przesyłanie danych do atrybutu vertex
  glDisableVertexAttribArray(
      sp->a("normal")); // Wyłącz przesyłanie danych do atrybutu vertex
  glDisableVertexAttribArray(
      sp->a("group")); // Wyłącz przesyłanie danych do atrybutu color
}

void obj3dmodel::set_wall_mapping(std::array<int, 7> wall_mapping) {
  this->wall_mapping = wall_mapping;
}

void obj3dmodel::from_file(const char *filename) {
  std::ifstream in(filename, std::ios::in);
  if (!in) {
    std::cerr << "Cannot open " << filename << std::endl;
    exit(1);
  }
  std::string line;
  std::array<float, 4> v = {0, 0, 0, 1.0f};
  std::array<float, 2> t = {0};
  int group = 0;
  std::vector<glm::vec4> ver;
  std::vector<glm::vec4> nrm;
  std::vector<glm::vec2> vts;
  while (getline(in, line)) {
    auto l = line.substr(0, 2);
    std::istringstream ss(line.substr(2));
    if (l == "v ") {
      ss >> v[0];
      ss >> v[1];
      ss >> v[2];
      v[3] = 1;
      ver.push_back(glm::vec4(v[0], v[1], v[2], v[3]));
    } else if (l == "vn") {
      ss >> v[0];
      ss >> v[1];
      ss >> v[2];
      v[3] = 0;
      nrm.push_back(glm::vec4(v[0], v[1], v[2], v[3]));
    } else if (l == "vt") {
      ss >> t[0]; // u
      ss >> t[1]; // v
      vts.push_back(glm::vec2(t[0], t[1]));
    } else if (l == "g ") {
      std::string s;
      ss >> s;
      if (s == "face") {
        ss >> group;
      } else if (s == "edge") {
        group = -1;
      } else if (s == "off") {
        group = 0;
      } else {
        throw("unrecognized group: \"" + s + "\"");
      }
    } else if (l == "f ") {
      ss.imbue(std::locale(std::locale(), new slash_plus_space_locale));
      for (int i = 0; i < 3; i++) {
        int v, vt, vn;
        ss >> v;
        ss >> vt;
        ss >> vn;
        faces.push_back(v - 1);
        verts.push_back(ver[v - 1]);
        norms.push_back(nrm[vn - 1]);
        texCoords.push_back(vts[vt - 1]);
        groups.push_back(group);
      }
      unsigned int size = verts.size();
      auto v1 = verts[size - 1];
      auto v2 = verts[size - 2];
      auto v3 = verts[size - 3];
      auto v21 = v2 - v1;
      auto v31 = v3 - v1;

      auto c1 = texCoords[size - 1];
      auto c2 = texCoords[size - 2];
      auto c3 = texCoords[size - 3];

      auto c21 = c2 - c1;
      auto c31 = c3 - c1;

      float f = 1.0f / (c21.x * c31.y - c31.x * c21.y);

      glm::vec3 t;
      t.x = f * (c31.y * v21.x - c21.y * v31.x);
      t.y = f * (c31.y * v21.y - c21.y * v31.y);
      t.z = f * (c31.y * v21.z - c21.y * v31.z);
      glm::vec3 b;
      b.x = f * (-c31.x * v21.x + c21.x * v31.x);
      b.y = f * (-c31.x * v21.y + c21.x * v31.y);
      b.z = f * (-c31.x * v21.z + c21.x * v31.z);
      glm::vec3 n = glm::cross(t, b);

      t = glm::normalize(t);
      b = glm::normalize(b);
      n = glm::normalize(n);

      for (int i = 0; i < 3; i++) {
        modelC1.push_back(glm::vec4(t.x, b.x, n.x, 0));
        modelC2.push_back(glm::vec4(t.y, b.y, n.y, 0));
        modelC3.push_back(glm::vec4(t.z, b.z, n.z, 0));
      }
    }
  }
  assert(this->norms.size() == this->verts.size());
  assert(this->texCoords.size() == this->verts.size());
  assert(this->groups.size() == this->verts.size());
}

GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    // Wczytanie do pamięci komputera
    std::vector<unsigned char> image; // Alokuj wektor do wczytania obrazka
    unsigned width, height; // Zmienne do których wczytamy wymiary obrazka
    // Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);
    // Import do pamięci karty graficznej
    glGenTextures(1, &tex);            // Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); // Uaktywnij uchwyt
    // Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        (unsigned char*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
    int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) 
            canRotateWall = -1;
        
        if (key == GLFW_KEY_RIGHT) 
            canRotateWall = 1;

        if (key == GLFW_KEY_UP) {
            chooseWall -= 1;
            if (chooseWall == -1) chooseWall = 5;
        }
        if (key == GLFW_KEY_DOWN) {
            chooseWall += 1;
            if (chooseWall == 6) chooseWall = 0;
        }
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

bool hasEntered;//zmienna boolowska która mówi czy mysz jest w oknie

void cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        hasEntered = 1; //mysz w oknie
    }
    else
    {
        hasEntered = 0; //mysz poza oknem
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && hasEntered == true) { //jeśli wciśnięty lewy przycisk i mysz jest w oknie
        double xpos, ypos; //pozycja myszy
        glfwGetCursorPos(window, &xpos, &ypos);
        /*zapamiętanie wcześniejszej pozycji*/
        double firstXpos = xpos;
        double firstYpos = ypos;
        /*czytaj tak długo aż zmieni pozycje*/
        while (firstXpos == xpos && firstYpos == ypos) {
            glfwGetCursorPos(window, &xpos, &ypos);
            speed_x = 0;
            speed_y = 0;
        }
        //printf("%lf %lf : %lf %lf\n", xpos, ypos, firstXpos, firstYpos);

        /*edycja prędkości*/
        speed_x = xpos - firstXpos;


        speed_y = -(ypos - firstYpos);
    }
    else
    {
        speed_x = 0;
        speed_y = 0;
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0)
        return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}
obj3dmodel kostka;

// Procedura inicjująca

void initOpenGLProgram(GLFWwindow *window) {
  //************Tutaj umieszczaj kod, który należy wykonać raz, na początku
  // programu************
  glClearColor(1, 1, 1, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSetWindowSizeCallback(window, windowResizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorEnterCallback(window, cursor_enter_callback); //ustaw sprawdzenie czy myszka jest w oknie
  glfwSetMouseButtonCallback(window, mouse_button_callback); //obsługa przycisków myszy

  edgeBase = readTexture("higher_res/Plastic_Rough_001_basecolor.png");
  edgeAmbient =
      readTexture("higher_res/Plastic_Rough_001_ambientOcclusion.png");
  edgeHeight = readTexture("higher_res/Plastic_Rough_001_height.png");
  edgeNormal = readTexture("higher_res/Plastic_Rough_001_normal.png");
  edgeRoughness = readTexture("higher_res/Plastic_Rough_001_roughness.png");

  kostka.from_file("kostka.obj");
  kostka.set_wall_mapping({
      0,
      1,
      0,
      3,
      4,
      5,
      6,
  });
  sp = new ShaderProgram("v_simplest.glsl", NULL /*  "g_simplest.glsl" */,
                         "f_simplest.glsl");
  highlight = new ShaderProgram("v_highlight.glsl", NULL, "f_highlight.glsl");
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow *window) {
  //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu
  // pętli
  // głównej************
  delete sp;
  delete highlight;

}


bool firstInit = 1;

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
    //************Tutaj umieszczaj kod rysujący obraz******************l
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    glm::vec3 camera = glm::vec3(0, 0, -6);
  
    glm::mat4 V =
        glm::lookAt(camera, glm::vec3(0, 0, 0),
            glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz widoku

    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f,
        50.0f); // Wylicz macierz rzutowania

    glm::mat4 M = glm::mat4(1.0f);
    V = glm::rotate(V, angle_y,
        glm::vec3(1.0f, 0.0f, 0.0f)); // Wylicz macierz modelu
    V = glm::rotate(V, angle_x,
        glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz modelu

    M = glm::scale(M, glm::vec3(0.5));

    if (firstInit == 1) {
        createMatKostki(M);
        firstInit = 0;
    }

    sp->use(); // Aktywacja programu cieniującego
    // Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    //kostka.draw();

    for (int i = 0; i < 27; i++) {

        glm::mat4 Mk = matKostki[i];

        if (chooseWall == 0 && posKostki[i][1] == 1) { 
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(0, 1, 0));
            Mk = mulMat(Mk, Mp);
        }
        else if (chooseWall == 1 && posKostki[i][1] == -1) {
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(0, 1, 0));
            Mk = mulMat(Mk, Mp);
        }
        else if (chooseWall == 2 && posKostki[i][0] == 1) {
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(1, 0, 0));
            Mk = mulMat(Mk, Mp);
        }
        else if (chooseWall == 3 && posKostki[i][0] == -1) {
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(1, 0, 0));
            Mk = mulMat(Mk, Mp);
        }
        else if (chooseWall == 4 && posKostki[i][2] == 1) {
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(0, 0, 1));
            Mk = mulMat(Mk, Mp);
        }
        else if (chooseWall == 5 && posKostki[i][2] == -1) {
            glm::mat4 Mp = glm::mat4(1.0f);
            Mp = glm::rotate(Mp, glm::radians(wallAngle), glm::vec3(0, 0, 1));
            Mk = mulMat(Mk, Mp);
        }

        Mk = glm::translate(Mk, transKostki[i]);

        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));

        kostka.draw();
    }

    glfwSwapBuffers(window); // Przerzuć tylny bufor na przedni
}

int main(void) {
  
    GLFWwindow* window; // Wskaźnik na obiekt reprezentujący okno

    glfwSetErrorCallback(error_callback); // Zarejestruj procedurę obsługi błędów

    if (!glfwInit()) { // Zainicjuj bibliotekę GLFW
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(
        1000, 1000, "OpenGL", NULL,
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
/*sprawdzenie czy kąt obrotu osi y jest za mały lub za duży*/
        if (angle_y > 0.5)
            angle_y = 0.5;
        else if (angle_y < -0.5)
            angle_y = -0.5;

        //printf("%f\n", angle_y);
        //rotacja i edycja
        if (canRotateWall == -1) { // w lewo
            wallAngle -= 5;
            checkAngle = static_cast<int>(wallAngle);
            if (checkAngle % 90 == 0) {
                canRotateWall = 0;
                wallAngle = 0;
                newPosKostki = posKostki;
                for (int i = 0; i < 27; i++) {
                    if (chooseWall == 0 && posKostki[i][1] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(0, 1, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, 1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { - 1, 1, 0};
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, 1, 1 };
                    }
                    else if (chooseWall == 1 && posKostki[i][1] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(0, 1, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, -1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 0 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, -1, 1 };
                    }
                    else if (chooseWall == 2 && posKostki[i][0] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1) newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1) newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, 0 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 1, 0, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, 0 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 1, 0, -1 };
                       // printf("%d, %d, %d : %d, %d, %d \n", posKostki[i][0], posKostki[i][1], posKostki[i][2], newPosKostki[i][0], newPosKostki[i][1], newPosKostki[i][2]);
                    }
                    else if (chooseWall == 3 && posKostki[i][0] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1) newPosKostki[i] = { -1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1) newPosKostki[i] = { -1, -1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { -1, 0, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { -1, 0, -1 };
                    }
                    else if (chooseWall == 4 && posKostki[i][2] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(0, 0, 1));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 0, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 0, -1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 0, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 0, 1, 1 };
                    }
                    else if (chooseWall == 5 && posKostki[i][2] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(0, 0, 1));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 0, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 0, -1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 0, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 0, 1, -1 };
                    }
                }
                posKostki = newPosKostki;
                printf("\n");
            }
        }
        else if (canRotateWall == 1){ //w prawo
            wallAngle += 5;
            checkAngle = static_cast<int>(wallAngle);
            if (checkAngle % 90 == 0) {
                canRotateWall = 0;
                wallAngle = 0;
                newPosKostki = posKostki;
                for (int i = 0; i < 27; i++) {
                    if (chooseWall == 0 && posKostki[i][1] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0, 1, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { - 1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = {- 1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, 1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, 1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, 0 };
                    }
                    else if (chooseWall == 1 && posKostki[i][1] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0, 1, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, -1 };
                        else  if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, -1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 0, -1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 0 };
                    }
                    else if (chooseWall == 2 && posKostki[i][0] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1, 0, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { 1, 0, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 0 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { 1, 0, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, 0 };
                    }
                    else if (chooseWall == 3 && posKostki[i][0] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1, 0, 0));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 0)  newPosKostki[i] = { -1, 0, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 0 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 0)  newPosKostki[i] = { -1, 0, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, 0 };
                    }
                    else if (chooseWall == 4 && posKostki[i][2] == 1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0, 0, 1));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, -1, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, -1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 1, 1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 0, 1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == 1)  newPosKostki[i] = { 1, 0, 1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == 1)  newPosKostki[i] = { 0, -1, 1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == 1)  newPosKostki[i] = { -1, 0, 1 };
                    }
                    else if (chooseWall == 5 && posKostki[i][2] == -1) {
                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0, 0, 1));
                        matKostki[i] = mulMat(matKostki[i], M);
                        if (posKostki[i][0] == 1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, -1, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, -1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 1, -1 };
                        else if (posKostki[i][0] == 1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 0, 1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == -1 && posKostki[i][2] == -1)  newPosKostki[i] = { 1, 0, -1 };
                        else if (posKostki[i][0] == -1 && posKostki[i][1] == 0 && posKostki[i][2] == -1)  newPosKostki[i] = { 0, -1, -1 };
                        else if (posKostki[i][0] == 0 && posKostki[i][1] == 1 && posKostki[i][2] == -1)  newPosKostki[i] = { -1, 0, -1 };
                    }
                }
                posKostki = newPosKostki;
            }
        }

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
