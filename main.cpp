#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Zdefiniowanie stb_image.h

// Sta³e do obs³ugi rozmiaru okna
const int windowWidth = 800;
const int windowHeight = 600;

// Wspó³rzêdne kamery
float cameraX = 0.0f;
float cameraY = 1.75f;
float cameraZ = 5.0f;

// K¹ty obrotu kamery
float cameraYaw = 0.0f;   // w poziomie
float cameraPitch = 0.0f; // w pionie

// Przechowywanie poprzednich pozycji myszy
int lastMouseX = windowWidth / 2;
int lastMouseY = windowHeight / 2;

// ID tekstury
GLuint floorTextureID;

// Funkcja inicjuj¹ca
void initializeGL() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // Ustawienie koloru t³a na szary

    glEnable(GL_DEPTH_TEST); // W³¹czenie testu g³êbokoœci
    glDepthFunc(GL_LEQUAL);  // Ustawienie funkcji testu g³êbokoœci

    // Inicjalizacja GLEW
    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "Nie mo¿na zainicjowaæ GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Ukrycie kursora myszy
    glutSetCursor(GLUT_CURSOR_NONE);

    // Za³aduj teksturê pod³ogi
    int textureWidth, textureHeight, numChannels;
    unsigned char* data = stbi_load("stone.png", &textureWidth, &textureHeight, &numChannels, 0);
    if (!data) {
        std::cerr << "Nie mo¿na za³adowaæ tekstury stone.png" << std::endl;
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &floorTextureID);
    glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

// Funkcja rysuj¹ca
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Ustawienie kamery
    gluLookAt(cameraX, cameraY, cameraZ,
        cameraX + sin(cameraYaw), cameraY + sin(cameraPitch), cameraZ - cos(cameraYaw),
        0.0f, 1.0f, 0.0f);

    // Rysowanie pod³ogi z tekstur¹
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 0.0f, -5.0f); // Lewy dolny róg
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f, 0.0f, 5.0f);  // Lewy górny róg
    glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0f, 0.0f, 5.0f);   // Prawy górny róg
    glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0f, 0.0f, -5.0f);  // Prawy dolny róg
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

// Funkcja obs³uguj¹ca zmiany rozmiaru okna
void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

// Funkcja obs³uguj¹ca ruch mysz¹
void mouseMovement(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    // Zmiana k¹tów obrotu kamery na podstawie ruchu myszy
    cameraYaw += deltaX * 0.005f;
    cameraPitch -= deltaY * 0.005f;

    // Ograniczenie k¹ta pionowego patrzenia kamery
    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

// Funkcja obs³uguj¹ca poruszanie kamer¹
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        // Poruszanie w lewo
        cameraX -= cos(cameraYaw) * 0.1f;
        cameraZ -= sin(cameraYaw) * 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        // Poruszanie w prawo
        cameraX += cos(cameraYaw) * 0.1f;
        cameraZ += sin(cameraYaw) * 0.1f;
        break;
    case GLUT_KEY_UP:
        // Poruszanie do przodu
        cameraX += sin(cameraYaw) * 0.1f;
        cameraZ -= cos(cameraYaw) * 0.1f;
        break;
    case GLUT_KEY_DOWN:
        // Poruszanie do ty³u
        cameraX -= sin(cameraYaw) * 0.1f;
        cameraZ += cos(cameraYaw) * 0.1f;
        break;
    }

    glutPostRedisplay();
}

// Obs³uga zmiany na pe³ny ekran
void toggleFullscreen() {
    static bool fullscreen = false;
    fullscreen = !fullscreen;
    if (fullscreen) {
        glutFullScreen();
    }
    else {
        glutReshapeWindow(windowWidth, windowHeight);
        glutPositionWindow((glutGet(GLUT_SCREEN_WIDTH) - windowWidth) / 2,
            (glutGet(GLUT_SCREEN_HEIGHT) - windowHeight) / 2);
    }
}

// Funkcja obs³uguj¹ca klawisze klawiatury
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'f':
    case 'F':
        toggleFullscreen();
        break;
    case 27: // Escape key
        if (glutGet(GLUT_WINDOW_WIDTH) == windowWidth && glutGet(GLUT_WINDOW_HEIGHT) == windowHeight) {
            glutLeaveMainLoop();
        }
        else {
            toggleFullscreen();
        }
        break;
    }
}

// Funkcja obs³uguj¹ca aktualizacjê kamery i ruch
void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    // Inicjalizacja GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL White Floor");

    // Rejestracja funkcji
    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutPassiveMotionFunc(mouseMovement);
    glutKeyboardFunc(keyboard);

    // Inicjalizacja OpenGL i GLEW
    initializeGL();

    // Rozpoczêcie pêtli g³ównej GLUT
    glutTimerFunc(16, update, 0); // Timer do aktualizacji klatki
    glutMainLoop();

    return 0;
}
