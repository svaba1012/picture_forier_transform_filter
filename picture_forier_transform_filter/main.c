#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <conio.h>

#define PI 3.1415

typedef unsigned char BYTE;

void dft(double complex* x, double complex* y, int n);
void idft(double complex* x, double complex* y, int n);
BYTE* ucitajBmp(BYTE* podaci, int* visina, int* sirina);
void sacuvajNoviBmp(BYTE* pikseli, BYTE* podaci, int visina, int sirina);
void forierTranPic(BYTE* pikseli, int l);
void getSvaPic(BYTE* podaci);

int main(){
    int visina, sirina;
    BYTE* pikseli, podaci[0x36];
    pikseli = ucitajBmp(podaci, &visina, &sirina);
    int l = sirina * visina;
    //uncomment this to make compressed version of picture
    forierTranPic(pikseli, l);
    //sacuvajNoviBmp(pikseli, podaci, visina, sirina);

    getSvaPic(podaci); //decompress sva picture
    printf("Picture size %dx%d", visina, sirina);
    return 0;
}

void dft(double complex* x, double complex* y, int n){
    static int k = 1;
    static int per = 0;
    int percentage = 0;
    for(int i = 0; i < n; i++){
        y[i] = 0;
        for(int j = 0; j < n; j++){
            y[i] += x[j] * cexp(-I * i * j * 2 * PI / n);

        }
        percentage = (float)(i)/(n * 3) * 100 + per;
        if(percentage >= k * 5){
            system("cls");
            printf("Transforming picture.\nProgress: %d%%\n",percentage);
            k++;
        }

        y[i] /= n;
    }
    per = percentage;
}

void idft(double complex* x, double complex* y, int n){
    for(int i = 0; i < n; i++){
        y[i] = 0;
        for(int j = 0; j < n; j++){
            y[i] += x[j] * cexp(I * i * j * 2 * PI / n);
        }
        //y[i] /= n;
    }
}

BYTE* ucitajBmp(BYTE* podaci, int* visina, int* sirina){
    FILE* fSlika;
    BYTE d[4];
    BYTE h[4];
    BYTE* pikseli;
    fSlika = fopen(".\\probna1.bmp", "rb");//change this if you want to open another picture
    fseek(fSlika, 0x12, SEEK_CUR);
    fread(d, 1, 4, fSlika);
    fread(h, 1, 4, fSlika);
    *sirina = 0;
    *visina = 0;
    for(int i = 4; i >= 0; i--){
        *sirina *= 0x100;
        *sirina += d[i];
        *visina *= 0x100;
        *visina += h[i];
    }
    pikseli = malloc(*visina * (*sirina) * 3);
    //podaci = malloc(0x36);
    fseek(fSlika, 0, SEEK_SET);
    fread(podaci, 1, 0x36, fSlika);
    fread(pikseli, 1, *visina * (*sirina) * 3, fSlika);
    fclose(fSlika);
    return pikseli;
}

void sacuvajNoviBmp(BYTE* pikseli, BYTE* podaci, int visina, int sirina){
    FILE* novaBmp;
    novaBmp = fopen(".\\nova.bmp", "wb");
    int numPiks;
    numPiks = visina * sirina * 3;
    for(int i = 0; i < numPiks; i += 3){
        pikseli[i + 1] = pikseli[i] = pikseli[i + 2];
    }
    fwrite(podaci, 1, 0x36, novaBmp);
    fwrite(pikseli, 1, numPiks, novaBmp);
    fclose(novaBmp);

}

void forierTranPic(BYTE* pikseli, int l){
    double complex* x;
    double complex* x1;
    double complex* x2;
    double complex* y;
    double complex* y1;
    double complex* y2;
    x = malloc(sizeof(double complex) * l);
    y = malloc(sizeof(double complex) * l);
    x1 = malloc(sizeof(double complex) * l);
    y1 = malloc(sizeof(double complex) * l);
    x2 = malloc(sizeof(double complex) * l);
    y2 = malloc(sizeof(double complex) * l);
    for(int i = 0; i < l; i++){
        x[i] = (double)pikseli[3 * i];
        x1[i] = (double)pikseli[3 * i + 1];
        x2[i] = (double)pikseli[3 * i + 2];
    }
    dft(x, y, l);
    dft(x1, y1, l);
    dft(x2, y2, l);
    FILE* forierPic;
    forierPic = fopen(".\\forier1.b", "wb");
    char sva[4] = "SVA";
    fwrite(sva, 1, 4, forierPic);
    fwrite(&l, 4, 1, forierPic);
    fwrite(y, sizeof(double complex), l, forierPic);
    fwrite(y1, sizeof(double complex), l, forierPic);
    fwrite(y2, sizeof(double complex), l, forierPic);
    fclose(forierPic);
}

void getSvaPic(BYTE* podaci){
    FILE* forierPic;
    int l, broj;
    double complex* y;
    double complex* y1;
    double complex* y2;
    double complex* x;
    double complex* x1;
    double complex* x2;
    BYTE* pikseli;
    forierPic = fopen(".\\forier1.b", "rb");
    fseek(forierPic, 4, SEEK_SET);
    fread(&l, 4, 1, forierPic);
    y = malloc(sizeof(double complex) * l);
    x = malloc(sizeof(double complex) * l);
    y1 = malloc(sizeof(double complex) * l);
    x1 = malloc(sizeof(double complex) * l);
    y2 = malloc(sizeof(double complex) * l);
    x2 = malloc(sizeof(double complex) * l);
    pikseli = malloc(3 * l);
    fread(y, sizeof(double complex), l, forierPic);
    fread(y1, sizeof(double complex), l, forierPic);
    fread(y2, sizeof(double complex), l, forierPic);
    printf("Enter the cut off frequency for picture filter: ");
    scanf("%d", &broj);
    FILE* novaBmp;
    char str[60];
    int redni = 1;
    double help;
    for(int i = broj; i < l; i++){
        y[i] = 0;
        y1[i] = 0;
        y2[i] = 0;
    }
    printf("Inverse transforming the pic.\nProcessing:\n");
    idft(y, x, l);
    printf("Done 1/3\n");
    idft(y1, x1, l);
    printf("Done 2/3\n");
    idft(y2, x2, l);
    printf("Finished transforming\n");
    for(int i = 0; i < l; i++){
        help = creal(x[i]);
        pikseli[3 * i] = (BYTE)help;
        help = creal(x1[i]);
        pikseli[3 * i + 1] = (BYTE)help;
        help = creal(x2[i]);
        pikseli[3 * i + 2] = (BYTE)help;
    }
    //sprintf(str, "C:\\Users\\svaba1012\\Desktop\\Forier boja\\nova%d.bmp", broj);
    novaBmp = fopen(".\\nova1.bmp", "wb");
    fwrite(podaci, 1, 0x36, novaBmp);
    fwrite(pikseli, 1, 3 * l, novaBmp);
    fclose(novaBmp);
    //printf("%d. gotova\n", redni++);

}
