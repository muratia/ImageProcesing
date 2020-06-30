
/* Implementuar nga Ahmet N Murati për lëndën "Procesimi i imixheve"

 Ky program i përpunon një imazh te tipit bitmap binar
 Sintaksa:
	det12 FileHyresNr1  FileDales
	ku 
		FileHyresNr1 eshte emri i file-it hyres 1
		FileDales eshte emri i file-it i cili fitohet pas operacionit në imixhi hyrës
		
*/

#include <stdio.h>
#include <windows.h>
// Ky funksion verifikon se a eshte file-i hyres si bitmap file. Nese po, kthehet 0, 
// e nese nuk eshte bitmap, pason dalja nga programi
//
int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra);

// Ketu do te fitohen vetem pikselat per imixhet hyrese, te cilat ruhen ne variablen szPixels
// nNumberOfPixels- numri i pikselave ne file-in hyres
// szFileName - emri i file-it hyres
// Nese dalja eshte 0, qdo gje ka perfunduar ne rregull, perndryshe do te kemi ndonje gabim
int PixelBytes(long nNumberOfPixels, unsigned char *szPixels, char * szFileName);
int doIt4(int x);

// kryen operacionin bit për bit të imixhit hyrës
// szOutputFile - emri i file-it dales
// szPixelsIm1 - pikselat e imixhit 1
// nNumberOfPixels - numri i pikselave ne imixhin dales
// bfh - struktura BITMAPFILEHEADER
// bih - struktura BITMAPINFOHEADER
int operacioni(char *szOutputFile, unsigned char *szPixelsIm1, long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih);

void InvertBytes(unsigned char *szBytes, long nNumberOfBytes);

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
int main(int argc, char *argv[]){

	if (argc != 3)
	{
		printf("Sintaksa:\n	det12 FileHyresNr1 FileDales\n	ku\n 		FileHyresNr1 eshte emri i file-it hyres 1\n		FileDales eshte emri i file-it i cili fitohet \n                             pas operacioni t‰ kryer n‰ imixhin 1\n");
		return -1;
	}

	printf("ok1\n");
	argv[0] = strupr(argv[0]);
	argv[1] = strupr(argv[1]);
	argv[2] = strupr(argv[2]);
	

	char szFirstInputFile[100], szOutputFile[100];
	strcpy(szFirstInputFile, argv[1]);
	strcpy(szOutputFile, argv[2]);
	printf("ok2\n");
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
	printf("ok4\n");
	// Cakto ekstenzionin per file-in e dyte hyres
	Extension	= strrchr(argv[2],'.');

	Extension	= strupr(Extension);
	if (strcmp(Extension, ".BMP"))
	{
		printf("Bitmapi duhet te kete ekstenzion .bmp!\n");
		return -1;
	}printf("ok6\n");
	


	// struktura BITMAPFILEHEADER per file-in e pare dhe te dyte hyres
	BITMAPFILEHEADER bfh;
	// struktura BITMAPINFOHEADER per file-in e pare dhe te dyte hyres
	BITMAPINFOHEADER bih;
	
	int nNgjyra;

	// Thirret ky funksion per te kontrolluar a eshte imixhi i pare bitmap
	if (CheckIfBinaryBitmap(szFirstInputFile, &bfh, &bih, &nNgjyra))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		return -1;
	}

	// numri i pikselave caktohet nga kjo shprehje
	long nNumberOfBytes = bfh.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER)
		- sizeof(RGBQUAD[2]);
	
	// bufferi per pikselat e imixhit 1
	unsigned char *szBytes = new unsigned char [nNumberOfBytes];
	
	// Bajtat e imixhit 1 ruhen ne variablen szBytes1
	if (PixelBytes(nNumberOfBytes, szBytes, szFirstInputFile))
	{
		printf("Ka ardhe deri te ndonje gabim ne strukturen e bitmapit 1\n");
		delete [] szBytes;
		szBytes = NULL;
		return -1;
	}
	
	// nese tabela e ngjyrave eshte se pari ngjyra e bardhe (ff ff ff 00) e pastaj
	// ngjyra e zeze (00 00 00 00), bytat qe permbajne imixhin duhet te invertohen
	if (nNgjyra != 0)
	{
		InvertBytes(szBytes, nNumberOfBytes);

	}
	// nese madhesia e bitmapave eshte e njejte dhe te gjithe testet tjera kane
	// kaluar ne rregull, mund ta bejme AND-ing te ketyre dy imixheve
	if (operacioni(szOutputFile, szBytes, nNumberOfBytes, 
					  bfh, bih))
	{
		printf("Ka ardhe deri te ndonje gabim ne operatorin AND te imixheve 1 dhe 2\n");
		return -1;
	}

	delete [] szBytes;


	szBytes = NULL;


	printf("Imixhi %s u krijua ne rregull\n", szOutputFile);
	return 0;
}

int CheckIfBinaryBitmap(char * szFileName, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *ngjyra)
{
	unsigned char tempBuffer[200];
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
	if ((unsigned)bfh->bfSize != (unsigned)lFileSize)
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
	// verifiko a eshte numri i bitave per piksel 1
	if (bih->biBitCount != 1)
	{
		fclose(fInputBitmap);
		printf("Numri i bitave per piksel duhet te jete 1\n");
		return 1;

	}
	// verifiko a eshte bitmapi i kompresuar
	if (bih->biCompression != BI_RGB)
	{
		fclose(fInputBitmap);
		printf("Bitmapi eshte i kompresuar\n");
		return 1;
	}

	RGBQUAD rgb[2];
	// lexo tabelen e ngjyrave, e ketu tabela e ngjyrave ka dy elemente, ku elementi
	// i pare i ka komponentet R=G=B =0, ndersa elementi i dyte i ka komponentat 
	// R = G = B = 255
	rgb[0].rgbBlue=0;
	rgb[0].rgbGreen=0;
	rgb[0].rgbRed=0;
	rgb[1].rgbBlue=255;
	rgb[1].rgbGreen=255;
	rgb[1].rgbRed=255;


	int nNgjyra1;
	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb), fInputBitmap);
	nNgjyra1 = tempBuffer[0];
	if (nNgjyra1 != tempBuffer[1])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}
	else if (nNgjyra1 != tempBuffer[2])
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}

	nNumberRead = fread(tempBuffer, sizeof(char), sizeof(rgb[0]), fInputBitmap);
	

	if (nNgjyra1 != 0 && nNgjyra1 != 255)
	{
		printf("Tabela e ngjyrave nuk eshte korrekte\n");
		fclose(fInputBitmap);
		return 1;
	}
	
	

	*ngjyra = nNgjyra1;
	fclose(fInputBitmap);
	return 0;
}

void InvertBytes(unsigned char *szBytes, long nNumberOfBytes)
{
	for (int i = 0; i < nNumberOfBytes; i++)
	{
		szBytes[i] = 255 - szBytes[i];
	}
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
	int nOffsetBytes = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD[2]);
	// ketu pointeri i file-it shkon pas struktures BITMAPFILEHEADER, BITMAPINFOHEADER dhe tabeles se ngjyrave
	fseek(fInputBitmap, nOffsetBytes, SEEK_SET);
	// lexo bajtat e file-it dhe rezultati ruhet ne variablen szPixels
	long nNumberRead = fread(szPixels, sizeof(char), nNumberOfPixels, fInputBitmap);
	fclose(fInputBitmap);
	return 0;
}

int operacioni(char *szOutputFile, unsigned char *szPixelsIm1, long nNumberOfPixels, BITMAPFILEHEADER bfh, BITMAPINFOHEADER bih)
{
	FILE *fOutputBitmap;
	if ((fOutputBitmap = fopen(szOutputFile, "w+b")) == NULL)
	{
		printf("Datoteka %s nuk ka mundur te krijohet\n", szOutputFile);
		return 1;
	}
	long i;
	unsigned char *szOutputBytes	= new unsigned char[nNumberOfPixels];
	




	// ne fillim te gjithe bajtat jane zero
	for (i = 0; i < nNumberOfPixels; i++)
	{
		szOutputBytes[i] = 0;
	}


	for(long currentPixel=0; currentPixel<nNumberOfPixels;currentPixel++){
		szOutputBytes[currentPixel] = szPixelsIm1[currentPixel]&0xEF;
	}
	
	for(currentPixel=0; currentPixel<nNumberOfPixels;currentPixel++)
			szOutputBytes[currentPixel] = szPixelsIm1[currentPixel]|0x8;

	
	// mbushe strukturen RGBQUAD ashtu qe komponentet R,G,B jane te njejta
	RGBQUAD rgb[2];
	rgb[0].rgbBlue		= 0;
	rgb[0].rgbGreen		= 0;
	rgb[0].rgbRed		= 0;
	rgb[0].rgbReserved	= 0;

	rgb[1].rgbBlue		= 255;
	rgb[1].rgbGreen		= 255;
	rgb[1].rgbRed		= 255;
	rgb[1].rgbReserved	= 255;


	// shkruaj ne file-in dales
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fOutputBitmap);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fOutputBitmap);
	fwrite(&rgb, sizeof(rgb), 1, fOutputBitmap);
	fwrite(szPixelsIm1, sizeof(char), nNumberOfPixels, fOutputBitmap);

	delete [] szOutputBytes;
	
	szOutputBytes = NULL;
	
	fclose(fOutputBitmap);
	return 0;
}


	

