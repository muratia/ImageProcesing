/* Implementuar nga Ardian Grezda per lenden "Procesimi i imixheve"

 Ky program e ben numrimin e paraqitjes se pikselave ne 
	 nje imixh te tipit grayscale (imixhi eshte .bmp) per te gjithe pikselat
 Sintaksa:
	HistogramiGrayscale FileHyres
	ku 
		FileHyres - emri i file-it hyres
*/

#include <stdio.h>
#include <windows.h>

// Ky funksion verifikon se a eshte file-i hyres si bitmap file. Nese po, kthehet 0, 
// e nese nuk eshte bitmap, pason dalja nga programi
//
int CheckIfBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih);

// Ketu do te fitohen vetem pikselat per imixhet hyrese, te cilat ruhen ne variablen szPixels
// nNumberOfPixels- numri i pikselave ne file-in hyres
// szFileName - emri i file-it hyres
// Nese dalja eshte 0, qdo gje ka perfunduar ne rregull, perndryshe do te kemi ndonje gabim
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);

// Ky funksion e ben numrimin e paraqitjes se pikselave te imixhit, ku szPixels jane pikselat 
// ndersa nNumriPikselave eshte nje varg qe tregon numrin e pikselave qe paraqiten
// ne imixh per te gjithe pikselat prej 0 deri ne 255. 
// nNumberOfPixels eshte numri i pikselave per nje imixh
void NumroPikselat(unsigned char *szPixels, int * nNumriPikselave, long nNumberOfPixels);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Sintaksa: HistogramiGrayscale FileHyres\n");
		return -1;
	}
	argv[0] = strupr(argv[0]);

	char szInputFile[100];
	strcpy(szInputFile, argv[1]);

	char *Extension;
	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	// kjo behet per file-in e pare hyres
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szInputFile, ".BMP");
		Extension = ".BMP";
	}

	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	
	// Thirret ky funksion per te kontrolluar a eshte imixhi i pare bitmap
	if (CheckIfBitmap(szInputFile, &bfh, &bih))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfPixels = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
	;
	
	// bufferi per pikselat te imixhit
	unsigned char *szPixels = new unsigned char [nNumberOfPixels];
	
	// Bajtat e imixhit 1 ruhen ne variablen szPixels
	if (PixelBytes(nNumberOfPixels, szPixels, szInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szPixels;
		szPixels = NULL;
		return -1;
	}
	int nNumriPikselave[256];

	NumroPikselat(szPixels, nNumriPikselave, nNumberOfPixels);

	delete [] szPixels;
	szPixels = NULL;
	return 0;
}
int CheckIfGrayscaleBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih)
{
	unsigned char tempBuffer[2000];
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);
		return 1;
	}
	// kontrollo madhesine e file-it dhe rezultatin e ruan ne variablen lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// shko ne fillim te file-it
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// lexo bajtat per BITMAPFILEHEADER dhe ruaji ne var. tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// mbushe variablen tempBuffer me strukturen BITMAPFILEHEADER
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifiko madhesine e file-it
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s duhet te kete %ld bajte\n", szFileName, lFileSize);
		return 1;
	}
	// verifiko 2 bajtat e pare te file-it
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s nuk eshte bitmap file\n", szFileName);
		return 1;
	}
	if (bfh->bfReserved1 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 7 dhe 8 te datotekes %s duhet te jene 0\n", szFileName);
		return 1;
	}

	if (bfh->bfReserved2 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 9 dhe 10 te datotekes %s duhet te jene 0\n", szFileName);
		return 1;
	}

	// lexo bajtat e ardhshem prej file-it dhe rezultatin ruaje ne variablen tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifiko a eshte numri i bitave per piksel 8
	if (bih->biBitCount != 8)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 8\n");
		return 1;

	}
	
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		return 1;
	}

	
	
	fclose(fInputBitmap);
	return 0;
}

int CheckIfBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih)
{
	unsigned char tempBuffer[2000];
	FILE *fInputBitmap;
	int a=0;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);

	}
	// kontrollo madhesine e file-it dhe rezultatin e ruan ne variablen lFileSize
	long lFileSize;
	fseek(fInputBitmap, 0, SEEK_END);
	lFileSize = ftell(fInputBitmap);
	// shko ne fillim te file-it
	fseek(fInputBitmap, 0, SEEK_SET);
	int nNumberRead;
	// lexo bajtat per BITMAPFILEHEADER dhe ruaji ne var. tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPFILEHEADER), fInputBitmap);
	// mbushe variablen tempBuffer me strukturen BITMAPFILEHEADER
	memcpy((char *) bfh, tempBuffer, sizeof(BITMAPFILEHEADER));
	// verifiko madhesine e file-it
	if (bfh->bfSize != lFileSize)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s duhet te kete %ld bajte\n", szFileName, lFileSize);
		a=-1;
	}
	// verifiko 2 bajtat e pare te file-it
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s nuk eshte bitmap file\n", szFileName);
		a=-1;
	}
	if (bfh->bfReserved1 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 7 dhe 8 te datotekes %s duhet te jene 0\n", szFileName);
		a=-1;
	}

	if (bfh->bfReserved2 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 9 dhe 10 te datotekes %s duhet te jene 0\n", szFileName);
		a=-1;
	}

	// lexo bajtat e ardhshem prej file-it dhe rezultatin ruaje ne variablen tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifiko a eshte numri i bitave per piksel 8
	if (bih->biBitCount != 24)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 24\n");
		a=-1;
	}
	
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		a=-1;
	}
	return a;
}


int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName)
{
	FILE *fInputBitmap;
	if ((fInputBitmap = fopen(szFileName, "r+b")) == NULL)
	{
		printf("Datoteka %s nuk ekziston apo nuk mund te hapet\n", szFileName);
		return 1;
	}

	// Pasi imixhi eshte grayscale, pikselat do te jene te vendosura pas strukturave
	// BITMAPFILEHEADER, BITMAPINFOHEADER si dhe pas tabeles se ngjyrave RGBQUAD[256]
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// ketu pointeri i file-it shkon pas struktures BITMAPFILEHEADER, BITMAPINFOHEADER dhe tabeles se ngjyrave
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// lexo bajtat e file-it dhe rezultati ruhet ne variablen szPixels
	long nNumberRead = fread(szPixels, sizeof(char), nNumberOfPixels, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

void NumroPikselat(unsigned char *szPixels, int * nNumriPikselave, long nNumberOfPixels)
{
	long i;
	int rPixNo[256];
	int gPixNo[256];
	int bPixNo[256];

	// ne fillim per te gjithe pikselat numri i paraqitjes se tyre eshte zero
	for (i = 0; i < 256; i++) {	rPixNo[i]=0;	gPixNo[i]=0;	bPixNo[i]=0; }

	int tempRedPixelValue=0;
	int tempGreenPixelValue=0;
	int tempBluePixelValue=0;

	for (i = 0; i < nNumberOfPixels; i+=3)
	{
		tempRedPixelValue = szPixels[i];
		bPixNo[tempRedPixelValue]++;
	}

	for (i = 1; i < nNumberOfPixels-1; i+=3){
		tempGreenPixelValue = szPixels[i];
		gPixNo[tempGreenPixelValue]++;
	}
	
	for (i = 2; i < nNumberOfPixels-2; i+=3){
		tempBluePixelValue = szPixels[i];
		rPixNo[tempBluePixelValue]++;
	}	
	// paraqiti rezultatet ne ekran
	printf("Numri i paraqitjes se pikselave  me ngjyre te kuqe ne kete imixh eshte:\n");
	printf("\n");
	printf("--------------------------------------------------------------------------\n");
	printf("Pikseli\tNumri\t\tPikseli\tNumri\t\tPikseli\tNumri\t\t\n");
	printf("--------------------------------------------------------------------------\n");
	for (i = 0; i < 256; i++)
	{
		if ((i % 3) == 0 && i != 0)
			printf("\n");
		printf("%d\t%d\t\t", i, rPixNo[i]);
	}
	printf("\n--------------------------------------------------------------------------\n");

	// paraqiti rezultatet ne ekran
	printf("Numri i paraqitjes se pikselave  me ngjyre te blerte ne kete imixh eshte:\n");
	printf("\n");
	printf("--------------------------------------------------------------------------\n");
	printf("Pikseli\tNumri\t\tPikseli\tNumri\t\tPikseli\tNumri\t\t\n");
	printf("--------------------------------------------------------------------------\n");
	for (i = 0; i < 256; i++)
	{
		if ((i % 3) == 0 && i != 0)
			printf("\n");
		printf("%d\t%d\t\t", i, gPixNo[i]);
	}
	printf("\n--------------------------------------------------------------------------\n");

	// paraqiti rezultatet ne ekran
	printf("Numri i paraqitjes se pikselave  me ngjyre te kalter ne kete imixh eshte:\n");
	printf("\n");
	printf("--------------------------------------------------------------------------\n");
	printf("Pikseli\tNumri\t\tPikseli\tNumri\t\tPikseli\tNumri\t\t\n");
	printf("--------------------------------------------------------------------------\n");
	for (i = 0; i < 256; i++)
	{
		if ((i % 3) == 0 && i != 0)
			printf("\n");
		printf("%d\t%d\t\t", i, bPixNo[i]);
	}
	printf("\n--------------------------------------------------------------------------\n");
}

