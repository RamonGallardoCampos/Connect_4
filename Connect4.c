#include <stdio.h>
#include <stdlib.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

#define Nfila 8
#define Ncol 8
#define Level 2
#define ConectWin 4
#define Jugador1 1
#define Jugador2 2
#define HUMA 1
#define MAQUINA 2
#define MAQUINA1 1
#define MAQUINA2 2
#define ESPAI_BUIT 0
#define TAULER_PLE 1
#define TAULER_NO_PLE 2
#define NO_EXISTEIX_COLUMNA 1
#define COLUMNA_PLENA -1
#define GUANYA 1
#define NO_GUANYA 2
#define NO_JUGA 1
#define FILA_PLENA -1
#define HUMA_VS_HUMA 1
#define HUMA_VS_MAQUINA 2
#define MAQUINA_VS_MAQUINA 3
#define SORTIR 4
#define TIRADA_EXITOSA 2
#define PARELL 1
#define IMPARELL 2
#define COLUMNA_MIG_PLENA 1
#define PROFUNDITAT 5
#define VICTORIA 9999
#define DERROTA -9999
#define ALPHA_0 -9999
#define BETA_0 9999

typedef struct node
{
    int tauler[Nfila][Ncol];
    int n_fills;
    struct node **fills;
    int valor;
    int level;
} Node;

int parell(int m)
{
    if (m % 2 == 0)
    {
        return PARELL;
    }
    else
        return IMPARELL;
}

// Ens adonem que nomes cal comptar en direccio a un semiespai ja que mitja translacions podem aconseguir totes les combinacions.
// Aquesta funcio agafa un punt de coordenades (x,y) i comensa a comptar un n�mero ConectWin (4 en aquest cas) de caselles per sobre, i anem
// baixant veient cuantes del mateix tipus estan alineades. Cal remarcar que no cal comen�ar a comptar un n�mero de caselles m�s gran que ConectWin
// ja que si hi hagu�s alguna cadena de longitud superior a ConectWin el joc ja hauria acabat. A m�s, el fet de comptar de adalt a abaix fa que
// que ens estalviem comptar per "fora" del tauler, ja que si comencessim a comptar de abaix cap adalt hauriem d'assegurarnos de no sortir del
// tauler a cada pas que fem, el que seria molt pesat de realitzar amb "if"s, fent alhora que l'algoritme sigui m�s r�pid. Tamb� tenim que
// no cal comptar cap abaix si ja comptem cap adalt, de la mateixa manera que no cal comptar cap a l'esquerra si ja comptem cap a la dreta. Les funcions
// de comptarDreta, comptarEsquerra, comptarDiagDreta i comptarDiagEsquerra s�n similars.

int comptarAdalt(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int y0 = (y - ConectWin > -1) ? y - ConectWin + 1 : 0;
    int p = 0;
    while (y0 < y + 1)
    {
        if (tauler[x][y0] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        y0++;
    }
    return p;
}

int comptarDreta(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int x0 = (x + ConectWin < Ncol) ? x + ConectWin - 1 : Ncol - 1;
    int p = 0;
    while (x <= x0)
    {
        if (tauler[x][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        x++;
    }
    return p;
}

int comptarDiagDretaAdalt(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int y0 = (y - ConectWin > -1) ? y - ConectWin + 1 : 0;
    int x0 = (x + ConectWin < Ncol) ? x + ConectWin - 1 : Ncol - 1;

    int p = 0;
    while ((y0 <= y) && (x <= x0))
    {
        if (tauler[x][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        y--;
        x++;
    }
    return p;
}

int comptarDiagDretaAbaix(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int y0 = (y + ConectWin < Nfila) ? y + ConectWin - 1 : Nfila - 1;
    int x0 = (x + ConectWin < Ncol) ? x + ConectWin - 1 : Ncol - 1;

    int p = 0;
    while ((y <= y0) && (x <= x0))
    {
        if (tauler[x][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        y++;
        x++;
    }
    return p;
}

int calculaFilaTirar(int tauler[Nfila][Ncol], int col)
{
    int j = -1;
    while (j + 1 < Nfila && tauler[j + 1][col] == ESPAI_BUIT)
    {
        j++;
    }
    return j;
}

int taulerPle(int tauler[Nfila][Ncol])
{
    int p = 0;
    for (int i = 0; i < Ncol; i++)
    {
        if (tauler[0][i] == ESPAI_BUIT)
        {
            p++;
        }
    }
    if (p == 0)
    {
        return TAULER_PLE;
    }
    else
    {
        return TAULER_NO_PLE;
    }
}

void aplicarTirada(int tauler[Nfila][Ncol], int fila, int col, int jugador)
{
    tauler[fila][col] = jugador;
}

char printSimbol(int tauler[Nfila][Ncol], int x, int y)
{
    if (tauler[x][y] == 1)
    {
        return 'X';
    }
    if (tauler[x][y] == 2)
    {
        return 'O';
    }
    if (tauler[x][y] == 0)
    {
        return ' ';
    }
    return 'a';
}

int PrintTauler(int tauler[Nfila][Ncol])
{
    printf("\n");
    for (int i = 0; i < Ncol; i++)
    {
        printf("|%d", i + 1);
        if (i + 1 == Ncol)
        {
            printf("|");
        }
    }
    printf("\n");
    for (int i = 0; i < Nfila; i++)
    {
        for (int j = 0; j < Ncol; j++)
        {
            printf("|%c", printSimbol(tauler, i, j));
            if (j + 1 == Ncol)
            {
                printf("|");
            }
        }
        printf("\n");
    }
    return 0;
}

void netejaTauler(int tauler[Nfila][Ncol])
{
    for (int i = 0; i < Nfila; i++)
    {
        for (int j = 0; j < Ncol; j++)
        {
            tauler[i][j] = ESPAI_BUIT;
        }
    }
}

int calcularNumFills(int tauler[Nfila][Ncol])
{
    int p = 0;
    for (int i = 0; i < Ncol; i++)
    {
        if (tauler[0][i] == ESPAI_BUIT)
        {
            p++;
        }
    }
    return p;
}

void copiaTauler(int pTauler[Nfila][Ncol], int pareTauler[Nfila][Ncol])
{
    for (int i = 0; i < Nfila; i++)
    {
        for (int j = 0; j < Ncol; j++)
        {
            pTauler[i][j] = pareTauler[i][j];
        }
    }
}

int cambiaTorn(int jugador)
{
    if (jugador == 1)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

int promptJugadorHuma()
{
    printf("Select a column to play: ");
    int columna;
    scanf("%d", &columna);
    columna--;
    return columna;
}

int TirarPesa(int tauler[Nfila][Ncol], int jugador, int columna)
{
    if (columna < 0 || columna > Ncol - 1)
    {
        return NO_EXISTEIX_COLUMNA;
    }
    else if (calculaFilaTirar(tauler, columna) == COLUMNA_PLENA)
    {
        return COLUMNA_PLENA;
    }
    else if (jugador == Jugador1)
    {
        tauler[calculaFilaTirar(tauler, columna)][columna] = Jugador1;
        return TIRADA_EXITOSA;
    }
    else
    {
        tauler[calculaFilaTirar(tauler, columna)][columna] = Jugador2;
        return TIRADA_EXITOSA;
    }
}

int acaba(int tauler[Nfila][Ncol], int jugador)
{
    for (int y = 0; y < Nfila; y++)
    {
        for (int x = 0; x < Ncol; x++)
        {
            if (comptarAdalt(tauler, x, y, jugador) >= ConectWin)
            {
                return GUANYA;
            }
            else if (comptarDreta(tauler, x, y, jugador) >= ConectWin)
            {
                return GUANYA;
            }
            else if (comptarDiagDretaAdalt(tauler, x, y, jugador) >= ConectWin)
            {
                return GUANYA;
            }
            else if (comptarDiagDretaAbaix(tauler, x, y, jugador) >= ConectWin)
            {
                return GUANYA;
            }
        }
    }
    return NO_GUANYA;
}

int calculaColumna(int tauler[Nfila][Ncol], int fill_index)
{
    int p = 0, c = 0;
    for (int i = 0; i <= fill_index; i++)
    {
        if (tauler[0][c] != ESPAI_BUIT)
        {
            p++;
            i--;
        }
        c++;
    }
    int col_fill_index = fill_index + p;
    return col_fill_index;
}

int radi3(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int xDreta = (x + ConectWin - 1 < Ncol) ? x + ConectWin - 1 : Ncol - 1;
    int xEsquerra = (x - ConectWin + 1 > -1) ? x - ConectWin + 1 : 0;
    int yAdalt = (y - ConectWin + 1 > -1) ? y - ConectWin + 1 : 0;
    int yAbaix = (y + ConectWin - 1 < Nfila) ? y + ConectWin - 1 : Nfila - 1;

    int comptador = 0;
    int p = 0;
    int sum = 0;
    while (x + comptador < xDreta)
    {
        if (tauler[x + comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y + comptador < yAbaix))
    {
        if (tauler[x + comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y + comptador < yAbaix)
    {
        if (tauler[x][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y + comptador < yAbaix))
    {
        if (tauler[x - comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (x - comptador > xEsquerra)
    {
        if (tauler[x - comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y - comptador > yAdalt))
    {
        if (tauler[x - comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y - comptador > yAdalt)
    {
        if (tauler[x][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y - comptador > yAdalt))
    {
        if (tauler[x + comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p;
        comptador++;
    }
    return sum;
}

int radi2(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int xDreta = (x + ConectWin - 1 < Ncol) ? x + ConectWin - 1 : Ncol - 1;
    int xEsquerra = (x - ConectWin + 1 > -1) ? x - ConectWin + 1 : 0;
    int yAdalt = (y - ConectWin + 1 > -1) ? y - ConectWin + 1 : 0;
    int yAbaix = (y + ConectWin - 1 < Nfila) ? y + ConectWin - 1 : Nfila - 1;

    int comptador = 0;
    int p = 0;
    int sum = 0;
    while (x + comptador < xDreta)
    {
        if (tauler[x + comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y + comptador < yAbaix))
    {
        if (tauler[x + comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y + comptador < yAbaix)
    {
        if (tauler[x][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y + comptador < yAbaix))
    {
        if (tauler[x - comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (x - comptador > xEsquerra)
    {
        if (tauler[x - comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y - comptador > yAdalt))
    {
        if (tauler[x - comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y - comptador > yAdalt)
    {
        if (tauler[x][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y - comptador > yAdalt))
    {
        if (tauler[x + comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p * p;
        comptador++;
    }
    return sum;
}

int radi(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int xDreta = (x + ConectWin - 1 < Ncol) ? x + ConectWin - 1 : Ncol - 1;
    int xEsquerra = (x - ConectWin + 1 > -1) ? x - ConectWin + 1 : 0;
    int yAdalt = (y - ConectWin + 1 > -1) ? y - ConectWin + 1 : 0;
    int yAbaix = (y + ConectWin - 1 < Nfila) ? y + ConectWin - 1 : Nfila - 1;

    int comptador = 0;
    int p = 0;
    int sum = 0;
    while (x + comptador < xDreta)
    {
        if (tauler[x + comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y + comptador < yAbaix))
    {
        if (tauler[x + comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y + comptador < yAbaix)
    {
        if (tauler[x][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y + comptador < yAbaix))
    {
        if (tauler[x - comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (x - comptador > xEsquerra)
    {
        if (tauler[x - comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador > xEsquerra) && (y - comptador > yAdalt))
    {
        if (tauler[x - comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y - comptador > yAdalt)
    {
        if (tauler[x][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador < xDreta) && (y - comptador > yAdalt))
    {
        if (tauler[x + comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    return sum;
}

int radi4(int tauler[Nfila][Ncol], int x, int y, int jugador)
{
    int xDreta = (x + ConectWin < Ncol) ? x + ConectWin : Ncol - 1;
    int xEsquerra = (x - ConectWin > -1) ? x - ConectWin : 0;
    int yAdalt = (y - ConectWin > -1) ? y - ConectWin : 0;
    int yAbaix = (y + ConectWin < Nfila) ? y + ConectWin : Nfila - 1;

    int comptador = 0;
    int p = 0;
    int sum = 0;
    while (x + comptador <= xDreta)
    {
        if (tauler[x + comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador <= xDreta) && (y + comptador <= yAbaix))
    {
        if (tauler[x + comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y + comptador <= yAbaix)
    {
        if (tauler[x][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador >= xEsquerra) && (y + comptador <= yAbaix))
    {
        if (tauler[x - comptador][y + comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (x - comptador >= xEsquerra)
    {
        if (tauler[x - comptador][y] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x - comptador >= xEsquerra) && (y - comptador >= yAdalt))
    {
        if (tauler[x - comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while (y - comptador >= yAdalt)
    {
        if (tauler[x][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    p = 0;
    comptador = 0;

    while ((x + comptador <= xDreta) && (y - comptador >= yAdalt))
    {
        if (tauler[x + comptador][y - comptador] == jugador)
        {
            p++;
        }
        else
        {
            p = 0;
        }
        sum = sum + p * p;
        comptador++;
    }
    return sum;
}

int ColumnaAcaba(int tauler[Nfila][Ncol], int jugador)
{
    int taulerProba[Nfila][Ncol];
    for (int i = 0; i < Ncol; i++)
    {
        copiaTauler(taulerProba, tauler);
        int estat = TirarPesa(taulerProba, jugador, i);

        if (estat == TIRADA_EXITOSA)
        {
            int guanyador = acaba(taulerProba, jugador);
            if (guanyador == GUANYA)
            {
                return i;
            }
        }
    }
    return NO_GUANYA;
}

// int ColumnaMig(int tauler[Nfila][Ncol], int jugador) {
//   int taulerProba[Nfila][Ncol];
//   copiaTauler(taulerProba, tauler);
//   if (parell(Ncol) == PARELL) {
//     int mig = Ncol / 2;
//     int estat = TirarPesa(taulerProba, jugador, mig);
//     if (estat == TIRADA_EXITOSA) {
//       return mig;
//     } else {
//       int mig = (Ncol - 1) / 2;
//       int estat = TirarPesa(taulerProba, jugador, mig);
//       if (estat == TIRADA_EXITOSA) {
//         return mig;
//       }
//     }
//   } else {
//     int mig = (Ncol - 1) / 2;
//     int estat = TirarPesa(taulerProba, jugador, mig);
//     if (estat == TIRADA_EXITOSA) {
//       return mig;
//     }
//   }
//   return COLUMNA_MIG_PLENA;
// }

int Puntuatje(int tauler[Nfila][Ncol], int jugador)
{
    int SUM = 0;
    for (int i = 0; i < Nfila; i++)
    {
        for (int j = 0; j < Ncol; j++)
        {
            if (tauler[i][j] == jugador)
            {
                SUM = SUM + radi(tauler, i, j, jugador);
            }
        }
    }
    return SUM;
}

int heuristica(int tauler[Nfila][Ncol], int jugador)
{
    int contrincant = cambiaTorn(jugador);
    if (acaba(tauler, jugador) == GUANYA)
    {
        return VICTORIA;
    }
    else if (acaba(tauler, contrincant) == GUANYA)
    {
        return DERROTA;
    }
    else
    {
        return Puntuatje(tauler, jugador) - Puntuatje(tauler, contrincant);
    }
}

int heuristica2(int tauler[Nfila][Ncol], int jugador, int nivell)
{
    int contrincant = cambiaTorn(jugador);
    if (acaba(tauler, jugador) == GUANYA)
    {
        return VICTORIA - nivell;
    }
    else if (acaba(tauler, contrincant) == GUANYA)
    {
        return DERROTA + nivell;
    }
    else
    {
        return Puntuatje(tauler, jugador) - Puntuatje(tauler, contrincant);
    }
}

void esborraArbre(Node *pare)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        esborraArbre(pare->fills[i]);
    }
    free(pare);
}

int ColumnaMaxValorsFills(Node *node)
{
    int maxim = node->fills[0]->valor;
    int p = 0;
    for (int i = 0; i < node->n_fills; i++)
    {
        if (maxim < node->fills[i]->valor)
        {
            maxim = MAX(node->fills[i]->valor, maxim);
            p = i;
        }
    }
    return calculaColumna(node->tauler, p);
}

int MaxValorsFills(Node *node)
{
    int maxim = node->fills[0]->valor;
    for (int i = 0; i < node->n_fills; i++)
    {
        maxim = MAX(node->fills[i]->valor, maxim);
    }
    return maxim;
}

int MinValorsFills(Node *node)
{
    int minim = node->fills[0]->valor;
    for (int i = 0; i < node->n_fills; i++)
    {
        minim = MIN(node->fills[i]->valor, minim);
    }
    return minim;
}

int minimax(Node *pare, int nivell)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        minimax(pare->fills[i], nivell + 1);
    }
    if (nivell == PROFUNDITAT)
        return 0;
    else if (nivell == 0)
    {
        return ColumnaMaxValorsFills(pare);
    }
    else if (parell(nivell) == PARELL)
    {
        pare->valor = MaxValorsFills(pare);
    }
    else
    {
        pare->valor = MinValorsFills(pare);
    }
    return 0;
}

int alpha_beta(Node *pare, int nivell, int alpha, int beta)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        alpha_beta(pare->fills[i], nivell + 1, alpha, beta);
    }
    if (nivell == PROFUNDITAT)
        return 0;
    else if (nivell == 0)
    {
        return ColumnaMaxValorsFills(pare);
    }
    else if (parell(nivell) == PARELL)
    {
        pare->valor = MaxValorsFills(pare);
        alpha = MAX(alpha, pare->valor);
        if (beta <= alpha)
        {
            return 0;
        }
    }
    else
    {
        pare->valor = MinValorsFills(pare);
        beta = MIN(beta, pare->valor);
        if (beta <= alpha)
        {
            return 0;
        }
    }
    return 0;
}

int cosetes(int nivell)
{
    if (parell(nivell) == 1)
    {
        return MAQUINA;
    }
    else
    {
        return HUMA;
    }
}

Node *crearNode2(Node *pare, int fill_index, int nivell, int jugador)
{
    Node *p = malloc(sizeof(Node));
    copiaTauler(p->tauler, pare->tauler);
    int col = calculaColumna(p->tauler, fill_index);
    int fila = calculaFilaTirar(p->tauler, col);
    aplicarTirada(p->tauler, fila, col, jugador);
    if ((nivell + 1 < PROFUNDITAT) && (acaba(p->tauler, cosetes(nivell + 1)) == NO_GUANYA))
    {
        p->n_fills = calcularNumFills(pare->tauler);
        p->fills = malloc(p->n_fills * sizeof(Node *));
    }
    else
    { // Node fulla
        p->level = nivell + 1;
        p->n_fills = 0;
        p->fills = NULL;
        p->valor = heuristica2(p->tauler, jugador, nivell + 1);
    }
    return p;
}

int alpha_beta2(Node *pare, int nivell, int alpha, int beta)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        alpha_beta2(pare->fills[i], nivell + 1, alpha, beta);
    }
    if (nivell == pare->level)
        return 0;
    else if (nivell == 0)
    {
        return ColumnaMaxValorsFills(pare);
    }
    else if (parell(nivell) == PARELL)
    {
        pare->valor = MaxValorsFills(pare);
        alpha = MAX(alpha, pare->valor);
        if (beta <= alpha)
        {
            return 0;
        }
    }
    else
    {
        pare->valor = MinValorsFills(pare);
        beta = MIN(beta, pare->valor);
        if (beta <= alpha)
        {
            return 0;
        }
    }
    return 0;
}

Node *crearNode(Node *pare, int fill_index, int nivell, int jugador)
{
    Node *p = malloc(sizeof(Node));
    copiaTauler(p->tauler, pare->tauler);
    int col = calculaColumna(p->tauler, fill_index);
    int fila = calculaFilaTirar(p->tauler, col);
    aplicarTirada(p->tauler, fila, col, jugador);
    if (nivell + 1 < PROFUNDITAT)
    {
        p->n_fills = calcularNumFills(pare->tauler);
        p->fills = malloc(p->n_fills * sizeof(Node *));
    }
    else
    { // Node fulla
        p->n_fills = 0;
        p->fills = NULL;
        p->valor = heuristica(p->tauler, jugador);
    }
    return p;
}

void crear1Nivell(Node *pare, int nivell, int jugador)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        pare->fills[i] = crearNode(pare, i, nivell, jugador);
    }
}

void crearArbre(Node *arrel, int nivell, int jugador)
{
    crear1Nivell(arrel, nivell, jugador);
    jugador = cambiaTorn(jugador);
    for (int i = 0; i < arrel->n_fills; i++)
    {
        crearArbre(arrel->fills[i], nivell + 1, jugador);
    }
}

int decisio(Node *pare, int tauler[Nfila][Ncol], int jugador)
{
    int columna;
    int contrincant = cambiaTorn(jugador);

    columna = ColumnaAcaba(tauler, jugador);
    if (columna != NO_GUANYA)
        return columna;

    columna = ColumnaAcaba(tauler, contrincant);
    if (columna != NO_GUANYA)
        return columna;

    else
    {
        columna = minimax(pare, 0);
        return columna;
    }
}

int decisio2(Node *pare, int tauler[Nfila][Ncol], int jugador)
{
    int columna;
    int estat1;
    int j;
    int contrincant = cambiaTorn(jugador);
    int TaulerProba[Nfila][Ncol];

    columna = ColumnaAcaba(tauler, jugador);
    if (columna != NO_GUANYA)
    {
        return columna;
    }

    columna = ColumnaAcaba(tauler, contrincant);
    if (columna != NO_GUANYA)
    {
        return columna;
    }

    else
    {
        for (int i = 0; i < pare->n_fills; i++)
        {
            copiaTauler(TaulerProba, tauler);
            j = calculaColumna(TaulerProba, i);
            estat1 = TirarPesa(TaulerProba, jugador, j);
            if (estat1 == TIRADA_EXITOSA)
            {
                columna = ColumnaAcaba(TaulerProba, jugador);
                if (columna != NO_GUANYA)
                    return columna;

                columna = ColumnaAcaba(TaulerProba, contrincant);
                if (columna != NO_GUANYA)
                    return columna;
            }
        }
    }

    columna = minimax(pare, 0);
    return columna;
}

int decisio3(Node *pare, int tauler[Nfila][Ncol], int jugador)
{
    int columna;
    int estat1;
    int j;
    int contrincant = cambiaTorn(jugador);
    int TaulerProba[Nfila][Ncol];

    columna = ColumnaAcaba(tauler, jugador);
    if (columna != NO_GUANYA)
    {
        return columna;
    }

    columna = ColumnaAcaba(tauler, contrincant);
    if (columna != NO_GUANYA)
    {
        return columna;
    }

    else
    {
        for (int i = 0; i < pare->n_fills; i++)
        {
            copiaTauler(TaulerProba, tauler);
            j = calculaColumna(TaulerProba, i);
            estat1 = TirarPesa(TaulerProba, jugador, j);
            if (estat1 == TIRADA_EXITOSA)
            {
                columna = ColumnaAcaba(TaulerProba, jugador);
                if (columna != NO_GUANYA)
                    return columna;

                columna = ColumnaAcaba(TaulerProba, contrincant);
                if (columna != NO_GUANYA)
                    return columna;
            }
        }
    }

    columna = alpha_beta(pare, 0, ALPHA_0, BETA_0);
    return columna;
}

void juga(int mode)
{
    int torn = 1;
    int jugador;
    int columna;
    int n0;
    int tauler[Nfila][Ncol];
    netejaTauler(tauler); // Per si de cas

    if (mode == HUMA_VS_HUMA)
    {
        printf("Who starts ? Player 1 (select 1) or Player 2 (select 2)?:  \n");
        scanf("%d", &n0);
        while (n0 != Jugador1 && n0 != Jugador2)
        {
            printf("The player you selected does not exist, please select between 1 and 2: \n");
            scanf("%d", &n0);
            continue;
        }
        jugador = n0;
        printf(
            "Player %d starts \n"
            "--------------------- \n",
            jugador);

        while (taulerPle(tauler) != TAULER_PLE)
        {
            printf(
                "TURN: %d \n"
                "Player %d turn \n",
                torn, jugador);
            PrintTauler(tauler);
            columna = promptJugadorHuma();

            int estatTauler = TirarPesa(tauler, jugador, columna);
            while (estatTauler == NO_EXISTEIX_COLUMNA)
            {
                printf("The column where you want to play does not exist: \n");
                columna = promptJugadorHuma();
                estatTauler = TirarPesa(tauler, jugador, columna);
                continue;
            }
            while (estatTauler == COLUMNA_PLENA)
            {
                printf("The column where you want to play is full: \n");
                columna = promptJugadorHuma();
                estatTauler = TirarPesa(tauler, jugador, columna);
                continue;
            }
            int aux = acaba(tauler, jugador);
            if (aux == GUANYA)
            {
                PrintTauler(tauler);
                printf("CONGRATULATIONS, PLAYER %d HAS WON ! \n", jugador);
                break;
            }
            else if (taulerPle(tauler) == TAULER_PLE)
            {
                PrintTauler(tauler);
                printf("DRAW !! \n");
                break;
            }

            jugador = cambiaTorn(jugador);
            torn++;
        }
    }

    else if (mode == HUMA_VS_MAQUINA)
    {
        printf("Who starts ? Human player (select 1) or CPU (select 2)?: \n");
        scanf("%d", &n0);
        while (n0 != HUMA && n0 != MAQUINA)
        {
            printf("The player you selected does not exist, please select between 1 and 2: \n");
            scanf("%d", &n0);
            continue;
        }
        if (n0 == HUMA)
        {
            printf(
                "Player starts\n"
                "-------------- \n");
        }
        else if (n0 == MAQUINA)
        {
            printf(
                "CPU starts\n"
                "------------------ \n");
        }
        jugador = n0;
        while (taulerPle(tauler) != TAULER_PLE)
        {
            Node *pare = malloc(sizeof(Node));
            copiaTauler(pare->tauler, tauler);
            printf("TORN %d: \n", torn);
            PrintTauler(tauler);
            if (jugador == MAQUINA)
            {
                /*printf("La Maquina esta pensant... \n");*/
                pare->n_fills = calcularNumFills(pare->tauler);
                pare->fills = malloc(pare->n_fills * sizeof(Node *));
                crearArbre(pare, 0, jugador);
                columna = decisio3(pare, tauler, jugador);
                /*printf("despues de alfa beta \n");*/
                esborraArbre(pare);
                /*printf("despues de borrar \n");*/
                int fila = calculaFilaTirar(tauler, columna);
                /*printf("despues de calculaFilaTirar \n");*/
                aplicarTirada(tauler, fila, columna, jugador);
                /*printf("despues d'aplicarTirada \n");*/
            }
            else
            {
                columna = promptJugadorHuma();
                int estatTauler = TirarPesa(tauler, jugador, columna);
                while (estatTauler == NO_EXISTEIX_COLUMNA)
                {
                    printf("The column where you want to play does not exist \n");
                    columna = promptJugadorHuma();
                    estatTauler = TirarPesa(tauler, jugador, columna);
                    continue;
                }
                while (estatTauler == COLUMNA_PLENA)
                {
                    printf("The column where you want to play is full \n");
                    columna = promptJugadorHuma();
                    estatTauler = TirarPesa(tauler, jugador, columna);
                    continue;
                }
            }
            /*printf("abans de aux \n");*/
            int aux = acaba(tauler, jugador);
            /*printf("despues de aux \n");*/
            if (aux == GUANYA)
            {
                PrintTauler(tauler);
                if (jugador == HUMA)
                {
                    printf("CONGRATULATIONS, YOU HAVE WON !! \n");
                }
                else
                {
                    printf("CPU HAS WON!! \n");
                }
                break;
            }
            else if (taulerPle(tauler) == TAULER_PLE)
            {
                PrintTauler(tauler);
                printf("DRAW !! \n");
                break;
            }
            /*printf("abans de cambiaTorn \n");*/
            jugador = cambiaTorn(jugador);
            torn++;
        }
    }
    else if (mode == MAQUINA_VS_MAQUINA)
    {
        printf("Who starts ? CPU 1 (select 1) or CPU 2 (select 2)?: \n");
        scanf("%d", &n0);
        while (n0 != MAQUINA1 && n0 != MAQUINA2)
        {
            printf("The player you selected does not exist, select a player between 1 and 2 \n");
            scanf("%d", &n0);
            continue;
        }
        if (n0 == MAQUINA1)
        {
            printf(
                "CPU 1 starts \n"
                "-------------- \n");
        }
        else if (n0 == MAQUINA2)
        {
            printf(
                "CPU 2 starts \n"
                "------------------ \n");
        }
        jugador = n0;

        while (taulerPle(tauler) != TAULER_PLE)
        {
            Node *pare = malloc(sizeof(Node));
            copiaTauler(pare->tauler, tauler);
            printf("TURN %d: \n", torn);
            PrintTauler(tauler);
            printf("CPU %d is thinking ... \n", jugador);
            pare->n_fills = calcularNumFills(pare->tauler);
            pare->fills = malloc(pare->n_fills * sizeof(Node *));
            crearArbre(pare, 0, jugador);
            columna = decisio3(pare, tauler, jugador);
            esborraArbre(pare);
            int fila = calculaFilaTirar(tauler, columna);
            aplicarTirada(tauler, fila, columna, jugador);

            int aux = acaba(tauler, jugador);
            if (aux == GUANYA)
            {
                PrintTauler(tauler);
                printf("CPU %d HAS WON !! \n", jugador);
                break;
            }
            else if (taulerPle(tauler) == TAULER_PLE)
            {
                PrintTauler(tauler);
                printf("DRAW !! \n");
                break;
            }
            jugador = cambiaTorn(jugador);
            torn++;
        }
    }
}

void recorreNode(Node *node, int fill_index, int nivell)
{
    for (int i = 0; i < nivell; i++)
    {
        printf("  ");
    }
    printf("%d\n", (int)node->valor);
}
void recorre1Nivell(Node *pare, int nivell)
{
    for (int i = 0; i < pare->n_fills; i++)
    {
        recorreNode(pare->fills[i], i, nivell);
    }
}
void recorreArbreRecursiva(Node *node, int nivell)
{
    for (int i = 0; i < nivell; i++)
    {
        printf("  ");
    }
    printf("%d\n", (int)node->valor);
    for (int i = 0; i < node->n_fills; i++)
    {
        recorreArbreRecursiva(node->fills[i], nivell + 1);
    }
}

int main()
{
    int mode;
    printf(
        "SELECT A GAME MODE: \n"
        "1) Player 1 vs Player 2 \n"
        "2) Human Player vs CPU \n"
        "3) CPU 1 vs CPU 2 \n"
        "4) EXIT \n");
    scanf("%d", &mode);

    if (mode < 1 || mode > 4)
    {
        printf("There does not exist a mode with that selection number\n");
        return NO_JUGA;
    }
    else if (mode > 0 && mode < 4)
    {
        juga(mode);
    }
    return 0;
}
