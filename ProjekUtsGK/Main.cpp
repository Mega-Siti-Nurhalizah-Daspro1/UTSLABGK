#include <iostream>
#include <freeglut.h>
#include <cmath>
#include <cstdlib>

// Variabel untuk posisi dan status lompat
float posX = 0.0f, posY = 0.0f; // Posisi karakter
bool isJumping = false;         // Apakah karakter sedang melompat
float jumpSpeed = 0.05f;        // Kecepatan lompatan
float gravity = 0.002f;         // Gravitasi

// Inisialisasi pengaturan latar belakang
void initScene() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);  // Langit berwarna biru muda
}

// Fungsi utilitas untuk menggambar lingkaran
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Menggambar matahari
void drawSun() {
    glColor3f(1.0f, 1.0f, 0.0f);  // Warna kuning
    drawCircle(0.8f, 0.8f, 0.1f, 50);  // Lingkaran matahari

    // Sinar matahari
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / 8;
        float x = cosf(angle) * 0.15f;
        float y = sinf(angle) * 0.15f;
        glVertex2f(0.8f, 0.8f);
        glVertex2f(0.8f + x, 0.8f + y);
    }
    glEnd();
}

// Menggambar awan
void drawCloud(float cx, float cy) {
    glColor3f(1.0f, 1.0f, 1.0f);  // Warna putih
    drawCircle(cx, cy, 0.1f, 50);    // Bagian tengah
    drawCircle(cx - 0.1f, cy, 0.08f, 50);  // Bagian kiri
    drawCircle(cx + 0.1f, cy, 0.08f, 50);  // Bagian kanan
}

// Menggambar pelangi menggunakan GL_TRIANGLE_STRIP untuk ketebalan dan warna
void drawRainbow() {
    float radius = 0.6f;  // Mengatur radius pelangi
    float colors[7][3] = {
        {1.0f, 0.0f, 0.0f},   // Merah
        {1.0f, 0.5f, 0.0f},   // Oranye
        {1.0f, 1.0f, 0.0f},   // Kuning
        {0.0f, 1.0f, 0.0f},   // Hijau
        {0.0f, 0.0f, 1.0f},   // Biru
        {0.5f, 0.0f, 0.5f},   // Indigo
        {0.7f, 0.0f, 1.0f}    // Violet
    };

    for (int i = 0; i < 7; i++) {
        glColor3fv(colors[i]);  // Mengatur warna untuk setiap strip pelangi
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= 100; j++) {
            float angle = 3.1415926f * j / 100;
            float x_inner = cos(angle) * (radius - i * 0.05f);
            float y_inner = sin(angle) * (radius - i * 0.05f);
            float x_outer = cos(angle) * (radius - (i + 1) * 0.05f);
            float y_outer = sin(angle) * (radius - (i + 1) * 0.05f);
            glVertex2f(x_inner, y_inner - 0.6f);  // Menggeser pelangi ke bawah (-0.6f)
            glVertex2f(x_outer, y_outer - 0.6f);
        }
        glEnd();
    }
}

// Menggambar latar belakang (area rumput)
void drawBackground() {
    glColor3f(0.2f, 0.8f, 0.2f);  // Warna rumput
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.6f);
    glVertex2f(1.0f, -0.6f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
}

// Menggambar kepala karakter (kepala, rambut, mata, dll.)
void drawHead() {
    glColor3f(1.0f, 0.8f, 0.6f); // Warna kulit
    drawCircle(0.0f, 0.0f, 0.2f, 50);  // Kepala

    // Rambut (setengah lingkaran)
    glColor3f(0.2f, 0.1f, 0.05f);  // Rambut cokelat
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 180; i++) {
        float angle = i * 3.14159 / 180;
        glVertex2f(cos(angle) * 0.22f, sin(angle) * 0.22f + 0.1f);  // Sedikit lebih besar dari kepala
    }
    glEnd();

    // Mata
    glColor3f(0.0f, 0.0f, 0.0f); // Warna hitam
    drawCircle(-0.05f, 0.05f, 0.02f, 50); // Mata kiri
    drawCircle(0.05f, 0.05f, 0.02f, 50);  // Mata kanan

    // Hidung
    glColor3f(0.9f, 0.7f, 0.5f); // Warna kulit untuk hidung
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.05f);
    glVertex2f(-0.03f, 0.0f);
    glVertex2f(0.03f, 0.0f);
    glEnd();

    // Mulut
    glColor3f(1.0f, 0.0f, 0.0f);  // Warna merah
    glBegin(GL_LINES);
    glVertex2f(-0.03f, -0.05f);
    glVertex2f(0.03f, -0.05f);
    glEnd();

    // Telinga
    glColor3f(1.0f, 0.8f, 0.6f); // Warna kulit untuk telinga
    drawCircle(-0.18f, 0.0f, 0.05f, 50); // Telinga kiri
    drawCircle(0.18f, 0.0f, 0.05f, 50);  // Telinga kanan
}

// Menggambar badan, tangan, kaki, dan sepatu karakter
void drawBody() {
    glColor3f(0.2f, 0.6f, 1.0f); // Warna biru untuk pakaian
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(0.1f, -0.2f);
    glVertex2f(0.1f, -0.6f);
    glVertex2f(-0.1f, -0.6f);
    glEnd();
}

// Menggambar lengan
void drawArms() {
    glColor3f(1.0f, 0.8f, 0.6f); // Warna kulit untuk lengan
    glBegin(GL_QUADS);
    // Lengan kiri
    glVertex2f(-0.15f, -0.2f);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(-0.1f, -0.5f);
    glVertex2f(-0.15f, -0.5f);
    // Lengan kanan
    glVertex2f(0.1f, -0.2f);
    glVertex2f(0.15f, -0.2f);
    glVertex2f(0.15f, -0.5f);
    glVertex2f(0.1f, -0.5f);
    glEnd();
}
// Fungsi untuk menggambar kaki karakter
void drawLegs() {
    glColor3f(0.5f, 0.3f, 0.1f); // Warna cokelat untuk kaki (celana)
    glBegin(GL_QUADS);
    // Kaki kiri
    glVertex2f(-0.1f, -0.6f); // Kiri atas
    glVertex2f(-0.05f, -0.6f); // Kanan atas
    glVertex2f(-0.05f, -0.8f); // Kanan bawah
    glVertex2f(-0.1f, -0.8f);  // Kiri bawah
    // Kaki kanan
    glVertex2f(0.05f, -0.6f);  // Kiri atas
    glVertex2f(0.1f, -0.6f);   // Kanan atas
    glVertex2f(0.1f, -0.8f);   // Kanan bawah
    glVertex2f(0.05f, -0.8f);  // Kiri bawah
    glEnd();
}

// Fungsi untuk menggambar sepatu karakter
void drawShoes() {
    glColor3f(0.0f, 0.0f, 0.0f); // Warna sepatu (hitam)

    // Sepatu kiri (bentuk lebih besar dan lebar)
    glBegin(GL_QUADS);
    glVertex2f(-0.12f, -0.8f);  // Kiri atas sepatu kiri
    glVertex2f(-0.02f, -0.8f);  // Kanan atas sepatu kiri
    glVertex2f(-0.02f, -0.9f);  // Kanan bawah sepatu kiri
    glVertex2f(-0.12f, -0.9f);  // Kiri bawah sepatu kiri
    glEnd();

    // Sepatu kanan (bentuk lebih besar dan lebar)
    glBegin(GL_QUADS);
    glVertex2f(0.02f, -0.8f);   // Kiri atas sepatu kanan
    glVertex2f(0.12f, -0.8f);   // Kanan atas sepatu kanan
    glVertex2f(0.12f, -0.9f);   // Kanan bawah sepatu kanan
    glVertex2f(0.02f, -0.9f);   // Kiri bawah sepatu kanan
    glEnd();
}

// Menggambar karakter secara utuh
void drawCharacter() {
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);  // Menerapkan translasi posisi karakter
    drawHead();                      // Menggambar kepala karakter
    drawBody();                      // Menggambar badan karakter
    drawArms();                      // Menggambar lengan karakter
    drawLegs();                      // Menggambar kaki karakter
    drawShoes();                     // Menggambar sepatu karakter
    glPopMatrix();
}

// Fungsi tampilan utama
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Menggambar elemen latar belakang
    drawBackground();
    drawSun();
    drawRainbow();
    drawCloud(-0.5f, 0.7f);  // Awan kiri
    drawCloud(0.5f, 0.6f);   // Awan kanan

    // Menggambar karakter
    drawCharacter();

    glutSwapBuffers();
}

// Fungsi untuk mengupdate status lompat
void updateJump(int value) {
    if (isJumping) {
        posY += jumpSpeed;  // Naikkan karakter selama melompat
        jumpSpeed -= gravity;  // Mengurangi kecepatan seiring waktu (efek gravitasi)

        if (posY <= 0.0f) {  // Karakter kembali ke tanah
            posY = 0.0f;
            isJumping = false;
            jumpSpeed = 0.05f;  // Reset kecepatan lompatan
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, updateJump, 0);  // Panggil fungsi ini lagi setelah 16 ms (~60 FPS)
}

// Fungsi untuk menangani input keyboard
void handleKeys(unsigned char key, int x, int y) {
    if (key == 'a') {
        posX -= 0.05f;  // Gerakkan karakter ke kiri
    }
    else if (key == 'd') {
        posX += 0.05f;  // Gerakkan karakter ke kanan
    }
    else if (key == 's' && !isJumping) {
        isJumping = true;  // Melompat
    }
    glutPostRedisplay();
}

// Inisialisasi GLUT dan pengaturan jendela
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Karakter Background");
    initScene();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutTimerFunc(16, updateJump, 0);  // Memulai loop untuk update lompatan
    glutMainLoop();
    return 0;
}
