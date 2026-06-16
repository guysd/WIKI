#include <windows.h>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iostream>

// Dimensions globales de l'écran virtuel (couvrant tous les moniteurs)
int X_OFFSET = 0;
int Y_OFFSET = 0;
int WIDTH = 0;
int HEIGHT = 0;
int num_flocons = 2000; // Nombre dynamique ajusté selon la surface globale

// Structure pour représenter un flocon de neige
struct Flocon {
    float x;
    float y;
    float vitesse;
    float dériveVent; // Oscillation horizontale
    int taille;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // Masquer la console pour un effet magique immédiat
    HWND hConsole = GetConsoleWindow();
    if (hConsole) {
        ShowWindow(hConsole, SW_HIDE);
    }

    // 1. Récupération des dimensions de l'écran virtuel global (gestion multi-écrans)
    X_OFFSET = GetSystemMetrics(SM_XVIRTUALSCREEN);
    Y_OFFSET = GetSystemMetrics(SM_YVIRTUALSCREEN);
    WIDTH = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    HEIGHT = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Si les métriques virtuelles échouent, on bascule sur l'écran principal par sécurité
    if (WIDTH == 0 || HEIGHT == 0) {
        X_OFFSET = 0;
        Y_OFFSET = 0;
        WIDTH = GetSystemMetrics(SM_CXSCREEN);
        HEIGHT = GetSystemMetrics(SM_CYSCREEN);
    }

    // Calcul dynamique de la quantité de flocons pour garder une densité parfaite sur 3 écrans
    num_flocons = (WIDTH * HEIGHT) / 800; // Ajustement de la densité proportionnelle
    num_flocons = std::clamp(num_flocons, 1000, 6000); // Limite de sécurité pour préserver le CPU

    // Initialisation de notre mémoire tampon de pixels (Frame-buffer) à la bonne taille
    std::vector<uint32_t> pixelBuffer(WIDTH * HEIGHT, 0);

    // 2. Initialisation de la physique des flocons
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(WIDTH));
    std::uniform_real_distribution<float> distY(-static_cast<float>(HEIGHT), 0.0f);
    std::uniform_real_distribution<float> distVitesse(30.0f, 90.0f); 
    std::uniform_real_distribution<float> distVent(-0.5f, 0.5f);
    std::uniform_int_distribution<int> distTaille(1, 3);

    std::vector<Flocon> flocons(num_flocons);
    for (auto& f : flocons) {
        f.x = distX(gen);
        f.y = distY(gen);
        f.vitesse = distVitesse(gen);
        f.dériveVent = distVent(gen);
        f.taille = distTaille(gen);
    }

    // 3. Configuration de l'affichage Windows transparent de bas niveau
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ClasseNeigeDesktopMulti";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    // CRUCIAL : On utilise X_OFFSET et Y_OFFSET pour positionner correctement la fenêtre popup
    // sur le repère global de tes 3 écrans.
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, 
        wc.lpszClassName, 
        "Chute de Neige Desktop Multi-Ecran", 
        WS_POPUP | WS_VISIBLE, 
        X_OFFSET, Y_OFFSET, WIDTH, HEIGHT, 
        NULL, NULL, hInstance, NULL
    );

    // Appliquer la clé de couleur noire (RGB 0,0,0) comme canal alpha (100% transparent).
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    HDC hdc = GetDC(hwnd);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT; // Inversion Y
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    auto tempsPrecedent = std::chrono::high_resolution_clock::now();
    float tempsTotal = 0.0f;
    bool enCours = true;
    MSG msg = {};

    while (enCours) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) enCours = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto tempsActuel = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(tempsActuel - tempsPrecedent).count();
        tempsPrecedent = tempsActuel;
        tempsTotal += deltaTime;

        if (deltaTime > 0.05f) deltaTime = 0.05f;

        // =========================================================================
        // APPLICATION DES POINTEURS 1 : NETTOYAGE DE TOUT L'ECRAN VIRTUEL
        // =========================================================================
        uint32_t* pointeurPixelCourant = pixelBuffer.data();
        int totalPixels = WIDTH * HEIGHT;
        
        for (int i = 0; i < totalPixels; ++i) {
            *pointeurPixelCourant = 0; // Transparent
            pointeurPixelCourant++;
        }

        // =========================================================================
        // APPLICATION DES POINTEURS 2 : PHYSIQUE ET DESSIN DES FLOCONS
        // =========================================================================
        uint32_t* const debutMemoireEcran = pixelBuffer.data();

        for (auto& f : flocons) {
            f.y += f.vitesse * deltaTime;
            f.x += (f.dériveVent + sin(tempsTotal + f.y * 0.01f)) * 15.0f * deltaTime;

            if (f.y >= HEIGHT) {
                f.y = -10.0f;
                f.x = distX(gen);
            }
            if (f.x < 0) f.x = static_cast<float>(WIDTH - 1);
            if (f.x >= WIDTH) f.x = 0.0f;

            int sx = static_cast<int>(f.x);
            int sy = static_cast<int>(f.y);

            if (sy >= 0 && sy < HEIGHT) {
                uint32_t* pixelFlocon = debutMemoireEcran + (sy * WIDTH + sx);
                uint32_t blancFlocon = 0x00FFFFFF;

                if (f.taille == 1) {
                    *pixelFlocon = blancFlocon;
                } 
                else if (f.taille == 2) {
                    if (sx + 1 < WIDTH && sy + 1 < HEIGHT) {
                        *pixelFlocon = blancFlocon;         
                        *(pixelFlocon + 1) = blancFlocon;     
                        *(pixelFlocon + WIDTH) = blancFlocon; 
                        *(pixelFlocon + WIDTH + 1) = blancFlocon; 
                    }
                } 
                else {
                    if (sx - 1 >= 0 && sx + 1 < WIDTH && sy - 1 >= 0 && sy + 1 < HEIGHT) {
                        *pixelFlocon = blancFlocon;               
                        *(pixelFlocon - 1) = 0x00CCCCCC;          
                        *(pixelFlocon + 1) = 0x00CCCCCC;          
                        *(pixelFlocon - WIDTH) = 0x00CCCCCC;      
                        *(pixelFlocon + WIDTH) = 0x00CCCCCC;      
                    }
                }
            }
        }

        // Injection directe du buffer modifié au premier plan du bureau global
        StretchDIBits(hdc, 0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, 
                      pixelBuffer.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

        Sleep(8);
    }

    ReleaseDC(hwnd, hdc);
    return 0;
}
