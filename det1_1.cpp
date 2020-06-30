
/* Implementuar nga Ahmet N Murati për lëndën "Procesimi i imixheve"

 Ky program i i zbret dy imixhe te tipit bitmap 24 bitëshe
 Sintaksa:
	Zbritja FileHyresNr1 FileHyresNr2 FileDales
	ku 
		FileHyresNr1 eshte emri i file-it hyres 1
		FileHyresNr2 eshte emri i file-it hyres 2
		FileDales eshte emri i file-it i cili fitohet pas zbritjes se imixhit 1 dhe 2
		
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

// I zbret bajtët e dy imixheve. 
// szOutputFile - emri i file-it dales
// szPixelsIm1 - pikselat e imixhit 1
// szPixelsIm2 - pikselat e imixhit 2
// nNumberOfPixels - numri i pikselave ne imixhin dales
// bfh - struktura BITMAPFILEHEADER
// bih - struktura BITMAPINFOHEADER
int ZbritjaPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
			  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Sintaksa:\n	Zbritja FileHyresNr1 FileHyresNr2 FileDales\n	ku\n 		FileHyresNr1 eshte emri i file-it hyres 1\n		FileHyresNr2 eshte emri i file-it hyres 2\n		FileDales eshte emri i file-it i cili fitohet \n                             pas zbritjes së imixhit 1 dhe 2\n");

		return -1;
	}
	argv[0] = strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);
	argv[3] = strupr(argv[3]);

	char szFirstInputFile[100], szSecondInputFile[100], szOutputFile[100];
	strcpy(szFirstInputFile, argv[1]);
	strcpy(szSecondInputFile, argv[2]);
	strcpy(szOutputFile, argv[3]);

	char *Extension;
	// Gjen paraqitjen e fundit te pikes ne emrin e datotekes dhe rezultatin e ruan
	// ne variablen Extension
	// kjo behet per file-in e pare hyres
	Extension	= strrchr(argv[1],'.');
	if (Extension == NULL)
	{
		strcat(szFirstInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	// Cakto ekstenzionin per file-in e dyte hyres
	Extension	= strrchr(argv[2],'.');
	if (Extension == NULL)
	{
		strcat(szSecondInputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	// Cakto ekstenzionin per file-in dales 
	Extension	= strrchr(argv[3],'.');
	if (Extension == NULL)
	{
		strcat(szOutputFile, ".BMP");
		Extension = ".BMP";
	}
	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}

	int a;
	
	// struktura BITMAPFILEHEADER per file-in e pare dhe te dyte hyres
	BITMAPFILEHEADER bfh1, bfh2;
	// struktura BITMAPINFOHEADER per file-in e pare dhe te dyte hyres
	BITMAPINFOHEADER bih1, bih2;
	
	// Thirret ky funksion per te kontrolluar a eshte imixhi i pare bitmap
	if (CheckIfBitmap(szFirstInputFile, &bfh1, &bih1))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfPixels1 = bfh1.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	
	// bufferi per pikselat e imixhit 1
	unsigned char *szPixels1 = new unsigned char [nNumberOfPixels1];
	
	// Bajtat e imixhit 1 ruhen ne variablen szPixels1
	if (PixelBytes(nNumberOfPixels1, szPixels1, szFirstInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// Thirret ky funksion per te kontrolluar a eshte imixhi i dyte bitmap
	if (CheckIfBitmap(szSecondInputFile, &bfh2, &bih2))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 2\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}
	
	// Kontrollo a eshte gjeresia e bitmapave hyres e njejte 
	if (bih1.biWidth != bih2.biWidth)
		if(bih1.biHeight != bih2.biHeight)
			{
			printf("Gjeresia dhe gjatësia e bitmapit 1 dhe 2 nuk janë të njejta\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}
	

	// Kontrollo a eshte gjatesia e bitmapave hyres e njejte 
	if (bih1.biHeight != bih2.biHeight)
	{
		printf("Gjatesia e bitmapit 1 dhe 2 nuk eshte e njejte\n");
		delete [] szPixels1;
		szPixels1 = NULL;
		return -1;
	}

	// numri i pikselave te imixhit 2 caktohet nga kjo shprehje
	long nNumberOfPixels2 = bfh2.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	// bufferi per pikselat e imixhit 2
	unsigned char *szPixels2 = new unsigned char [nNumberOfPixels2];
	if (PixelBytes(nNumberOfPixels2, szPixels2, szSecondInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szPixels1;
		delete [] szPixels2;
		szPixels1 = NULL;
		szPixels2 = NULL;
		return -1;
	}

	// nese madhesia e bitmapave eshte e njejte dhe te gjithe testet tjera kane
	// kaluar ne rregull, mund ta bejme mbledhjen e pikselave korrespondues
	if (ZbritjaPikselave(szOutputFile, szPixels1, szPixels2, nNumberOfPixels1, bfh1, bih1))
	{
		printf("Ka ardhe deri te ndonje gabim ne mbledhjen e pikselave te imixheve 1 dhe 2\n");
		return -1;
	}

	delete [] szPixels1;
	delete [] szPixels2;

	szPixels1 = NULL;
	szPixels2 = NULL;

	printf("Imixhi %s u krijua ne rregull\n", szOutputFile);
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
		a=1;
	}
	// verifiko 2 bajtat e pare te file-it
	if (bfh->bfType != 0x4d42)
	{
		fclose(fInputBitmap);
		printf("Datoteka %s nuk eshte bitmap file\n", szFileName);
		a=1;
	}
	if (bfh->bfReserved1 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 7 dhe 8 te datotekes %s duhet te jene 0\n", szFileName);
		a=1;
	}

	if (bfh->bfReserved2 != 0)
	{
		fclose(fInputBitmap);
		printf("Bajtat 9 dhe 10 te datotekes %s duhet te jene 0\n", szFileName);
		a=1;
	}

	// lexo bajtat e ardhshem prej file-it dhe rezultatin ruaje ne variablen tempBuffer
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(BITMAPINFOHEADER), fInputBitmap);
	memcpy((char *) bih, tempBuffer, sizeof(BITMAPINFOHEADER));
	// verifiko a eshte numri i bitave per piksel 8
	if (bih->biBitCount != 24)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 24\n");
		a=1;
	}
	
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		a=1;
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

	// Pasi imixhi eshte 24 bitësh me ngjyra, pikselat do te jene te vendosura pas strukturave
	// BITMAPFILEHEADER, BITMAPINFOHEADER pra secili piksel paraqitet me tri bajtë
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// ketu pointeri i file-it shkon pas struktures BITMAPFILEHEADER, BITMAPINFOHEADER 
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// lexo bajtat e file-it dhe rezultati ruhet ne variablen szPixels
	long nNumberRead = fread(szPixels, sizeof(char), nNumberOfPixels, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

int ZbritjaPikselave(char *szOutputFile, unsigned char *szPixelsIm1, unsigned char *szPixelsIm2, 
					  long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("Datoteka %s nuk ka mundur te krijohet\n", szOutputFile);
		return 1;
	}
	long i;
	// Variabla szOutputPixels ruan bajtat e file-it dales
	char *szOutputPixels = new char[nNumberOfPixels];
	// rezultati i zbritjes së pikselave te imixheve 1 dhe 2 ruhet ne kete variabel
	int tempPixel;
	for (i = 0; i < nNumberOfPixels; i++)
	{
		tempPixel = abs(szPixelsIm1[i] - szPixelsIm2[i]);
		
		
			szOutputPixels[i] = tempPixel;
		
	}



	// shkruaj ne file-in dales
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(szOutputPixels, sizeof(char), nNumberOfPixels, fOutputBitmap);

	delete [] szOutputPixels;
	szOutputPixels = NULL;
	fclose(fOutputBitmap);
	return 0;
}




