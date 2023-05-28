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

#include <glm/ext/matrix_transform.hpp>
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

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;
float angle_x = 0; // Aktualny kąt obrotu obiektu
float angle_y = 0; // Aktualny kąt obrotu obiektu
int canRotateWall = 0;
float wallAngle[6] = {0, 0, 0, 0, 0, 0}; // Aktualny kąt obrotu ściany
int chooseWall = 0; //wybór ściany
int checkAngle[6];

//tablica wektorów obrotu poszczególnych kosteczek

glm::vec3 rotKostki[3][3][3] = {
    {   // ściana gdzie środkowa kostka jest czerwona
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}, // poziomy rząd kostek nad czerwoną kostką
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)} // poziomy rząd kostek pod czerwoną kostką
    },
    {   //ściana pomiędzy
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}, // analogiczne jak wyżej
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}
    },
    {   // ściana gdzie środkowa kostka jest niebieska
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}, // analogicznie jak wyżej
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}
    }
};

ShaderProgram* sp;

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
    std::vector<float> verts;
    std::vector<unsigned int> groups;
    std::vector<float> norms;
    std::vector<float> texCoords;
    std::vector<unsigned int> faces;
    std::array<float, 4 * 7> wall_colors;

    void from_file(const char* filename);
    void draw();
    void set_wall_colors(std::array<float, 4 * 7> wall_colors);
};

void obj3dmodel::draw() {
    glUniform4fv(sp->u("wall_colors"), 4 * 7, wall_colors.data());

    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data());

    glEnableVertexAttribArray(sp->a("group"));
    glVertexAttribIPointer(sp->a("group"), 1, GL_UNSIGNED_INT, 0, groups.data());

    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 4);

    glDisableVertexAttribArray(
        sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
    glDisableVertexAttribArray(
        sp->a("color")); // Wyłącz przesyłanie danych do atrybutu color
    glDisableVertexAttribArray(
        sp->a("group")); // Wyłącz przesyłanie danych do atrybutu color
}

void obj3dmodel::set_wall_colors(std::array<float, 4 * 7> wall_colors) {
    this->wall_colors = wall_colors;
}

void obj3dmodel::from_file(const char* filename) {
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cerr << "Cannot open " << filename << std::endl;
        exit(1);
    }
    std::string line;
    std::array<float, 4> v = { 0, 0, 0, 1.0f };
    std::array<float, 2> t = { 0 };
    unsigned int group = 0;
    std::vector<std::array<float, 4>> ver;
    std::vector<std::array<float, 2>> vts;
    while (getline(in, line)) {
        auto l = line.substr(0, 2);
        std::istringstream ss(line.substr(2));
        if (l == "v ") {
            ss >> v[0];
            ss >> v[1];
            ss >> v[2];
            v[3] = 1;
            ver.push_back(v);
        }
        else if (l == "vn") {
            ss >> v[0];
            ss >> v[1];
            ss >> v[2];
            norms.insert(norms.end(), std::begin(v), std::end(v) - 1);
        }
        else if (l == "vt") {
            ss >> t[0]; // u
            ss >> t[1]; // v
            vts.push_back(t);
        }
        else if (l == "g ") {
            std::string s;
            ss >> s;
            if (s == "face") {
                ss >> group;
            }
            else if (s == "off") {
                group = 0;
            }
            else {
                throw("unrecognized group: \"" + s + "\"");
            }
        }
        else if (l == "f ") {
            ss.imbue(std::locale(std::locale(), new slash_plus_space_locale));
            for (int i = 0; i < 3; i++) {
                int v, vt, vn;
                ss >> v;
                ss >> vt;
                ss >> vn;
                faces.push_back(v - 1);
                verts.insert(verts.end(), ver[v - 1].begin(), ver[v - 1].end());
                texCoords.insert(texCoords.end(), vts[vt - 1].begin(),
                    vts[vt - 1].end());
                groups.push_back(group);
            }
        }
    }
    assert(this->texCoords.size() / 2 == this->verts.size() / 4);
    assert(this->groups.size() == this->verts.size() / 4);
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
        if (key == GLFW_KEY_LEFT) {
            canRotateWall = -1;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    rotKostki[i][0][j][1] += 0.25;
                }
            }
        }
        if (key == GLFW_KEY_RIGHT)
            canRotateWall = 1;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    rotKostki[i][0][j][1] += 0.25;
                }
            }
        if (key == GLFW_KEY_UP)
            chooseWall -= 1;
        if (key == GLFW_KEY_DOWN)
            chooseWall += 1;
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
void initOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać raz, na początku
    // programu************
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorEnterCallback(window, cursor_enter_callback); //ustaw sprawdzenie czy myszka jest w oknie
    glfwSetMouseButtonCallback(window, mouse_button_callback); //obsługa przycisków myszy

    kostka.from_file("kostka.obj");
    kostka.set_wall_colors({
        1.0, 1.0, 1.0, 0.0, // brzegi
        1.0, 0.0, 0.0, 0.0, // sciana 1
        0.0, 1.0, 0.0, 0.0, // sciana 2
        0.0, 0.0, 1.0, 0.0, // sciana 3
        1.0, 1.0, 0.0, 0.0, // sciana 4
        0.0, 1.0, 1.0, 0.0, // sciana 5
        1.0, 0.0, 1.0, 0.0, // sciana 6
        });
    sp = new ShaderProgram("v_simplest.glsl", NULL /*  "g_simplest.glsl" */,
        "f_simplest.glsl");
}

// Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu
    // pętli
    // głównej************
    delete sp;
}

void drawKostka(glm::mat4 M, glm::vec3 V) {
    bool canRot = 0;
    for (int i = 0; i < 3; i++)
    {
        if (V[i] != 0) {
            canRot = 1; 
            break;
        }
    }
    if(canRot) M = glm::rotate(M, glm::radians(360.0f),V);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    kostka.draw();
}

// Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
    //************Tutaj umieszczaj kod rysujący obraz******************l
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 V =
        glm::lookAt(glm::vec3(0, 0, -6), glm::vec3(0, 0, 0),
            glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz widoku

    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f,
        50.0f); // Wylicz macierz rzutowania

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_y,
        glm::vec3(1.0f, 0.0f, 0.0f)); // Wylicz macierz modelu
    M = glm::rotate(M, angle_x,
        glm::vec3(0.0f, 1.0f, 0.0f)); // Wylicz macierz modelu

    M = glm::scale(M, glm::vec3(0.5));

    sp->use(); // Aktywacja programu cieniującego
    // Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    kostka.draw();

    /*kostka.draw();

    M = glm::translate(M, glm::vec3(0, 2, 0));
    M = glm::rotate(M, glm::radians(wallAngle),
        glm::vec3(0, 1, 0));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    kostka.draw();*/
    if (chooseWall == 0 || chooseWall == 1) {
        glm::mat4 Mf = M;
        glm::mat4 Mjn = M;
        if (chooseWall == 0) {
            Mf = glm::translate(Mf, glm::vec3(0, 2, 0)); // obracana ściana fioletowa
            Mf = glm::rotate(Mf, glm::radians(wallAngle[chooseWall]),
                glm::vec3(0, 1, 0));

            drawKostka(Mf, rotKostki[1][0][1]);

            Mjn = glm::translate(Mjn, glm::vec3(0, -2, 0)); //ściana jasnoniebieska

            drawKostka(Mjn, rotKostki[1][2][1]);
        }
        else {
            Mjn = glm::translate(Mjn, glm::vec3(0, -2, 0)); // obracana ściana jasnoniebieska
            Mjn = glm::rotate(Mjn, glm::radians(wallAngle[chooseWall]),
                glm::vec3(0, 1, 0));

            drawKostka(Mjn, rotKostki[1][2][1]);

            Mf = glm::translate(Mf, glm::vec3(0, 2, 0)); //ściana fioletowa

            drawKostka(Mf, rotKostki[1][0][1]);
        }

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
        if (canRotateWall == -1) {
            wallAngle[chooseWall] -= 5;
            checkAngle[chooseWall] = static_cast<int>(wallAngle[chooseWall]);
            if (checkAngle[chooseWall] % 90 == 0) {
                canRotateWall = 0;
                wallAngle[chooseWall] = 0;
            }
        }
        else if (canRotateWall == 1){
            wallAngle[chooseWall] += 5;
            checkAngle[chooseWall] = static_cast<int>(wallAngle[chooseWall]);
            if (checkAngle[chooseWall] % 90 == 0) {
                canRotateWall = 0;
                wallAngle[chooseWall] = 0;
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