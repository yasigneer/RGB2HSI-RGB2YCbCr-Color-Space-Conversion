#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#define PI 3.1415926535897932384626433832795
#pragma pack(1)
struct ppm_header
{
	char pgmtype1;
	char pgmtype2;
	int pwidth;
	int pheight;
	int pmax;
};
struct ppm_file
{
	struct ppm_header *pheader;
	unsigned char *rdata,*gdata,*bdata;
};
struct YCbCr
{
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;
};
struct HSI
{
	float H;
	float S;
	float I;
};
void get_image_data(char *filename,struct ppm_file *image);
void write_image(char *filename,struct ppm_file *image);
struct YCbCr RGBToYCbCr(unsigned char red, unsigned char green, unsigned char blue);
struct HSI RGBToHSI(unsigned char red, unsigned char green, unsigned char blue);
void write_image_as_ycbcr(char *filename,struct ppm_file *image);
void write_image_as_hsi(char *filename,struct ppm_file *image);

void write_image(char *filename,struct ppm_file *image)
{
	FILE *fp;
	int i,max=0;
	fp=fopen(filename,"wb");
	fputc(image->pheader->pgmtype1,fp);
	fputc(image->pheader->pgmtype2,fp);
	fputc('\n',fp);
	fprintf(fp,"%d %d\n",image->pheader->pwidth,image->pheader->pheight);
	fprintf(fp,"%d\n",255/*max*/);
	for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
	{
		fwrite(&image->rdata[i],1,1,fp);
		fwrite(&image->gdata[i],1,1,fp);
		fwrite(&image->bdata[i],1,1,fp);
	}
	fclose(fp);
}
void get_image_data(char *filename, struct ppm_file *image ) 
{
	FILE* fp;
	int i=0;
	char temp[256];
	image->pheader=(struct ppm_header *)malloc(sizeof(struct ppm_header));
	fp = fopen(filename, "rb" );
	if (fp==NULL) 
	{
		printf("File is not opened: %s.\n\n", filename);
		exit(1);
	}
	printf ("The PPM File : %s...\n", filename);
	fscanf (fp, "%s", temp);
	if (strcmp(temp, "P6") == 0) 
	{
		image->pheader->pgmtype1=temp[0];
		image->pheader->pgmtype2=temp[1];
		fscanf (fp, "%s", temp);
		if (temp[0]=='#') 
		{
			while(fgetc(fp)!='\n');
			fscanf (fp, "%d %d\n",&image->pheader->pwidth,&image->pheader->pheight);
			fscanf (fp, "%d\n", &image->pheader->pmax);
		}
		else
		{
			sscanf (temp, "%d", &image->pheader->pwidth);
			fscanf (fp, "%d", &image->pheader->pheight);
			fscanf (fp, "%d", &image->pheader->pmax);
		}
		image->rdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		image->gdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		image->bdata=(unsigned char *)malloc(image->pheader->pheight*image->pheader->pwidth*sizeof(unsigned char));
		if (image->rdata==NULL) printf("Memory problem\n");
		for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
		{
			fread(&image->rdata[i],1,1,fp);
			fread(&image->gdata[i],1,1,fp);
			fread(&image->bdata[i],1,1,fp);
		}
	}
	else 
	{
		printf ("\nError! The file is not a PPM file");
		exit(1);
	}
	fclose(fp);
}
struct YCbCr RGBToYCbCr(unsigned char red, unsigned char green, unsigned char blue) {
	float fb = (float)red / 256;
	float fg = (float)green / 256;
	float fr = (float)blue / 256;
	
	struct YCbCr ycbcr;
	ycbcr.Y = (unsigned char)(16 + 65.738*fr + 129.057*fg + 25.064*fb);
	ycbcr.Cb = (unsigned char)(128 -  37.945*fr - 74.494*fg + 112.439*fb);
	ycbcr.Cr = (unsigned char)(128 + 112.439*fr - 94.154*fg - 18.285* fb);
	return ycbcr;
}
struct HSI RGBToHSI(unsigned char red, unsigned char green, unsigned char blue) {
	float fb = (float)red / 255;
	float fg = (float)green / 255;
	float fr = (float)blue / 255;
	float min,alfa;
	int i;
	alfa = acosf((((fr-fg)+(fr-fb))/2.0)/(sqrt((fr-fg)*(fr-fg) + (fr-fb)*(fg-fb))+0.0001));
	if( fr <= fg){
		if( fr <= fb ) min = fr;
		else min = fb;
	}
	else{
		if(fg <= fb) min = fg;
		else min = fb;
	}
	struct HSI hsi;
	if(fb <= fg) {
		hsi.H = alfa;
	}
	else {
		hsi.H = 2*PI - alfa;
	}
	hsi.H =  (hsi.H*180 / PI)*255/360;;
	hsi.S = (1.0 - (3.0/(fr + fg + fb + 0.0001))*min);
	hsi.S = hsi.S*255.0;;
	hsi.I = ((red + green + blue)/(3.0*255.0));
	hsi.I = hsi.I*255.0;
	return hsi;
}
void write_image_as_ycbcr(char *filename,struct ppm_file *image)
{
	FILE *fp;
	int i,max=0;
	struct YCbCr ycbcr;
	fp=fopen(filename,"wb");
	fputc(image->pheader->pgmtype1,fp);
	fputc(image->pheader->pgmtype2,fp);
	fputc('\n',fp);
	fprintf(fp,"%d %d\n",image->pheader->pwidth,image->pheader->pheight);
	fprintf(fp,"%d\n",255/*max*/);
	for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
	{
		ycbcr = RGBToYCbCr(image->rdata[i],image->gdata[i],image->bdata[i]);
		fwrite(&ycbcr.Y,1,1,fp);
		fwrite(&ycbcr.Cb,1,1,fp);
		fwrite(&ycbcr.Cr,1,1,fp);
	}
	fclose(fp);
}
void write_image_as_hsi(char *filename,struct ppm_file *image)
{
	FILE *fp;
	int i,max=0;
	struct HSI hsi;
	fp=fopen(filename,"wb");
	fputc(image->pheader->pgmtype1,fp);
	fputc(image->pheader->pgmtype2,fp);
	fputc('\n',fp);
	fprintf(fp,"%d %d\n",image->pheader->pwidth,image->pheader->pheight);
	fprintf(fp,"%d\n",255/*max*/);
	for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
	{
		unsigned char h = (unsigned char) hsi.H;
		unsigned char s = (unsigned char) hsi.S;
		unsigned char i = (unsigned char) hsi.I;
		fwrite(&hsi.H,1,1,fp);
		fwrite(&s,1,1,fp);
		fwrite(&i,1,1,fp);
	}	
	fclose(fp);
}
float calculate_psnr(struct ppm_file *image,int mod){
	int i;
	float mse=0.0,psnr;
	struct HSI hsi;
	struct YCbCr ycbcr;
	for(i=0;i<image->pheader->pwidth*image->pheader->pheight;i++)
	{
		if(mod == 0){
			hsi = RGBToHSI(image->rdata[i],image->gdata[i],image->bdata[i]);
			mse+= (float)((image->rdata[i]+image->gdata[i]+image->bdata[i])/3 - (hsi.H+hsi.S+hsi.I)/3)*((image->rdata[i]+image->gdata[i]+image->bdata[i])/3 - (hsi.H+hsi.S+hsi.I)/3);
		}
		else if( mod == 1){
			ycbcr = RGBToYCbCr(image->rdata[i],image->gdata[i],image->bdata[i]);
			mse+= (float)((image->rdata[i]+image->gdata[i]+image->bdata[i])/3 - (ycbcr.Y+ycbcr.Cb+ycbcr.Cr)/3)*((image->rdata[i]+image->gdata[i]+image->bdata[i])/3 - (ycbcr.Y+ycbcr.Cb+ycbcr.Cr)/3);
		}
	}
	mse = mse/(image->pheader->pwidth*image->pheader->pheight);
	psnr = 10*logf((image->pheader->pmax)*(image->pheader->pmax)/mse);
	return psnr;
}
int main()
{	
	struct ppm_file picture;
	float psnr_hsi,psnr_ycbcr;
	get_image_data("mandrill.ppm",&picture);
	//Information of image
	printf("pgmtype...=%c%c\n",picture.pheader->pgmtype1,picture.pheader->pgmtype2);
	printf("width...=%d\n",picture.pheader->pwidth);
	printf("height...=%d\n",picture.pheader->pheight);
	printf("max gray level...=%d\n",picture.pheader->pmax);
	psnr_hsi = calculate_psnr(&picture,0);
	psnr_ycbcr = calculate_psnr(&picture,1);
	printf("HSI PSNR Value		=%f\nYCbCr PSNR Value	=%f\n",psnr_hsi,psnr_ycbcr);
	write_image_as_ycbcr("ycbcr.ppm",&picture);
	write_image_as_hsi("hsi.ppm",&picture);
	
	return 0;
} 
