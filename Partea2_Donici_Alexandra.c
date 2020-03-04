#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
}pixel;

typedef struct
{
    unsigned int x;
    unsigned int y;
    double corelatie;
    pixel culoare;
}fereastra;

void dim( unsigned int *latime,unsigned int *inaltime,char * img_initiala)
{

 FILE *fin=fopen(img_initiala,"rb");
 if(fin==NULL)
 {
     printf("Imaginea nu a fost gasita.");
     return;
 }


   fseek(fin,18, SEEK_SET);
   fread(latime, sizeof(unsigned int), 1, fin);
   fread(inaltime, sizeof(unsigned int), 1, fin);

   fclose(fin);

}

int padding( unsigned int latime)
{
    if(latime%4!=0)
        return 4-(3*latime)%4;

    return 0;
}

 void header(char * img_initiala, unsigned char **h)
{

    int i;

    FILE *fin=fopen(img_initiala, "rb");
    if(fin==NULL)
 {
     printf("Imaginea nu a fost gasita.");
     return;
 }

    *h=(unsigned char *)malloc(55);
    if(h==NULL)
  {
      printf("Nu s-a putut aloca memorie pentru vectorul h.");
      return;
  }

    for(i=0;i<54;i++)
        fread(&(*h)[i],1,1,fin);

    fclose(fin);

}

void grayscale_image(char* nume_fisier_sursa, char* nume_fisier_destinatie)//transformă o imagine color într-o imagini în tonuri de gri (grayscale)
{
    FILE *fin, *fout;
    unsigned int latime, inaltime,i,j;
    unsigned char y, *h,z=0;
    pixel x;
    int p,q;

    header(nume_fisier_sursa, &h);
    dim(&latime, &inaltime, nume_fisier_sursa);
    p=padding(latime);

    fin = fopen(nume_fisier_sursa, "rb");
    if(fin == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc.");
   		return;
   	}

   	fout = fopen(nume_fisier_destinatie, "wb");
    if(fout == NULL)
   	{
   		printf("Nu am gasit fisierul binar asociat imaginii destinatie.");
   		return;
   	}

    for(i=0;i<54;i++)
        fwrite(&h[i],sizeof(unsigned char),1,fout);

    fseek(fin,54,SEEK_SET);

        for(i=0;i<inaltime;i++)
    {
        for(j=0;j<latime;j++)
        {
            fread(&x.blue,sizeof(unsigned char),1,fin);
            fread(&x.green,sizeof(unsigned char),1,fin);
            fread(&x.red,sizeof(unsigned char),1,fin);

            y=0.299*x.red+0.587*x.green+0.114*x.blue;//calcularea valorii grayscale de pe cele 3 canale

            x.blue=y;
            x.green=y;
            x.red=y;

            fwrite(&x.blue, sizeof(unsigned char),1,fout);
            fwrite(&x.green,sizeof(unsigned char),1,fout);
            fwrite(&x.red,sizeof(unsigned char),1,fout);
        }

        fseek(fin,p,SEEK_CUR);

        for(q=0;q<p;q++)
             fwrite(&z,1,1,fout);
    }

    fclose(fin);
    fclose(fout);
    free(h);

}

void matrice_imagine(char *cale_imagine, pixel ***v)//încarcă în memoria internă (matricea v) o imagine de tip BMP
{
    unsigned int latime, inaltime,i,j;
    int p;
    pixel x;

    dim(&latime,&inaltime, cale_imagine);

    *v=(pixel **)malloc(inaltime*sizeof(pixel *));
    if(*v==NULL)
  {
      printf("Nu s-a putut aloca memorie pentru matricea v.");
      return;
  }

    for(i=0;i<inaltime;i++)
        {(*v)[i]=(pixel *)malloc(latime*sizeof(pixel));
        if(*v==NULL)
  {
      printf("Nu s-a putut aloca memorie pentru matricea v.");
      return;
  }
        }

    FILE *fin=fopen(cale_imagine, "rb");
    if(fin==NULL)
 {
     printf("Imaginea nu a fost gasita.");
     return;
 }

    p=padding(latime);

    fseek(fin,54,SEEK_SET);

    for(i=0;i<inaltime;i++)
    {
        for(j=0;j<latime;j++)
        {
            fread(&x.blue,sizeof(unsigned char),1,fin);
            fread(&x.green,sizeof(unsigned char),1,fin);
            fread(&x.red,sizeof(unsigned char),1,fin);

            (*v)[inaltime-i-1][j]=x;//se tine cont de faptul ca imaginea este rasturnata in fisierul binar
        }

        fseek(fin,p,SEEK_CUR);//se sare peste octetii de padding
    }

    fclose(fin);
}

void imagine(char *cale_imagine, char *cale_imagine_finala, pixel **v)//salvează în memoria externă o imagine BMP stocată în memoria internă (matricea v)
{

    unsigned int i,j;
    pixel x;
    unsigned char *h,y=0;
    unsigned int latime, inaltime;
    int p,q;

    FILE *fout=fopen(cale_imagine_finala, "wb+");
    if(fout==NULL)
 {
     printf("Imaginea nu a fost gasita.");
     return;
 }

   dim(&latime,&inaltime,cale_imagine);
   p=padding(latime);
   header(cale_imagine,&h);

    for(i=0;i<54;i++)
        fwrite(&h[i],sizeof(unsigned char),1,fout);

    for(i=0;i<inaltime;i++)
       {
           for(j=0;j<latime;j++)
          {
              x=v[inaltime-i-1][j];//se tine cont de faptul ca imaginea este rasturnata in fisierul binar

              fwrite(&x.blue,sizeof(unsigned char),1,fout);
              fwrite(&x.green,sizeof(unsigned char),1,fout);
              fwrite(&x.red,sizeof(unsigned char),1,fout);
          }

             for(q=0;q<p;q++)
                 fwrite(&y,1,1,fout);//se adauga octeti egali cu 0 in cazul in care exista padding

       }

    fclose(fout);
    free(h);
}

double s_bar(pixel **s,unsigned int latime, unsigned int inaltime)//media valorilor intensităților grayscale a pixelilor în fereastra s
{
    double sb,sum=0;
    unsigned int i,j;

    for(i=0;i<inaltime;i++)
        for(j=0;j<latime;j++)
           sum=sum+s[i][j].red;

   sb=sum/(latime*inaltime);

   return sb;
}

double sigma_s(pixel **s, unsigned int latime, unsigned int inaltime)//deviația standard  a valorilor intensităților grayscale a pixelilor în șablonul s
{
    double sum=0,r;
    unsigned int i,j;

    for(i=0;i<inaltime;i++)
       for(j=0;j<latime;j++)
        sum=sum+(s[i][j].red-s_bar(s,latime,inaltime))*(s[i][j].red-s_bar(s,latime,inaltime));

    sum=sum/(latime*inaltime-1);
    r=sqrt(sum);

    return r;

}

double fI_bar(pixel **a, unsigned int latime, unsigned int inaltime, unsigned int pi, unsigned int pj)//media valorilor intensităților grayscale a pixelilor din fereastra f
{
    double sum=0,fb;
    unsigned int i,j;

    for(i=pi;i<pi+inaltime;i++)
        for(j=pj;j<pj+latime;j++)
           sum=sum+a[i][j].red;

   fb=sum/(latime*inaltime);

   return fb;

}

double sigma_fI(pixel **a, unsigned int latime, unsigned int inaltime, unsigned int pi, unsigned int pj)//deviația standard a valorilor intensităților grayscale a pixelilor în fereastra f
{
    double sum=0,r,fI;
    unsigned int i,j;

    fI=fI_bar(a,latime,inaltime,pi,pj);//media valorilor intensităților grayscale a pixelilor din fereastra f

    for(i=pi;i<pi+inaltime;i++)
        for(j=pj;j<pj+latime;j++)
        sum=sum+(a[i][j].red-fI)*(a[i][j].red-fI);

    sum=sum/(latime*inaltime-1);
    r=sqrt(sum);

    return r;

}

double corr(pixel **s, pixel **a, unsigned int pi, unsigned int pj, unsigned int latime, unsigned int inaltime)//calculează corelația dintre șablonul s și fereastra f care aparține inaginii a determinată de coordonatele pi și pj
{
    double sum=0,fI,sb,sigmaf,sigmas;
    unsigned int i,j;

    fI=fI_bar(a,latime,inaltime,pi,pj);//media valorilor intensităților grayscale a pixelilor din fereastra f
    sb=s_bar(s,latime,inaltime);//media valorilor intensităților grayscale a pixelilor în fereastra s
    sigmas=sigma_s(s,latime,inaltime);//deviația standard  a valorilor intensităților grayscale a pixelilor în șablonul s
    sigmaf=sigma_fI(a,latime,inaltime,pi,pj);//deviația standard a valorilor intensităților grayscale a pixelilor în fereastra f

    for(i=0;i<inaltime;i++)
        for(j=0;j<latime;j++)
           sum=sum+(double)(((a[pi+i][pj+j].red-fI)*(s[i][j].red-sb))/(sigmas*sigmaf));

    sum=sum/(inaltime*latime);

    return sum;

}

void colorare(pixel **a, unsigned int pi, unsigned int pj,unsigned int latime, unsigned int inaltime, pixel c)//colorează utilizând culoarea c, conturul ferestrei f din imaginea a
{
    unsigned int i,j;

    for(i=pi;i<pi+inaltime;i++)
    {
        a[i][pj]=c;
        a[i][pj+latime-1]=c;
    }

    for(j=pj;j<pj+latime;j++)
    {
        a[pi][j]=c;
        a[pi+inaltime-1][j]=c;
    }
}

void culori (pixel **c)//completează vectorul c de tip pixel cu octeții pentru cele 10 culori folosite la încadrarea detecțiilor șabloanelor.
{
     (*c)=(pixel *)malloc(10*sizeof(pixel));
     if(c==NULL)
        {
           printf("Nu s-a putut aloca memorie pentru vectorul c.");
           return ;
        }

    (*c)[0].red=255;
    (*c)[0].green=0;
    (*c)[0].blue=0;

    (*c)[1].red=255;
    (*c)[1].green=255;
    (*c)[1].blue=0;

    (*c)[2].red=0;
    (*c)[2].green=255;
    (*c)[2].blue=0;

    (*c)[3].red=0;
    (*c)[3].green=255;
    (*c)[3].blue=255;

    (*c)[4].red=255;
    (*c)[4].green=0;
    (*c)[4].blue=255;

    (*c)[5].red=0;
    (*c)[5].green=0;
    (*c)[5].blue=255;

    (*c)[6].red=192;
    (*c)[6].green=192;
    (*c)[6].blue=192;

    (*c)[7].red=255;
    (*c)[7].green=140;
    (*c)[7].blue=0;

    (*c)[8].red=128;
    (*c)[8].green=0;
    (*c)[8].blue=128;

    (*c)[9].red=128;
    (*c)[9].green=0;
    (*c)[9].blue=0;

}
void template_matching(char * cale_imagine, char * cale_sablon, double ps, fereastra **d,unsigned int *l, pixel colour)//detectează toate suprapunerile șablonului cale_sablon cu ferestrele din imaginea cale_imagine care au corelația mai mare decât pragul ps
{
    pixel **a, **s;
    double c;
    unsigned int i,j,latime_img,inaltime_img,latime_sb, inaltime_sb,p,q;

    matrice_imagine(cale_imagine,&a);
    matrice_imagine(cale_sablon,&s);

    dim(&latime_img, &inaltime_img,cale_imagine);
    dim(&latime_sb, &inaltime_sb,cale_sablon);

    p=inaltime_img-inaltime_sb;
    q=latime_img-latime_sb;

    for(i=0;i<p;i++)
        for(j=0;j<q;j++)
        {
            c=corr(s,a,i,j,latime_sb,inaltime_sb);

            if(c>ps)//s-a gasit o detectie
            {
                *d=(fereastra *)realloc(*d,((*l)+1)*sizeof(fereastra));
                if(*d==NULL)
                    {
                        printf("Nu s-a putut realoca memorie pentru vectorul d.");
                        return;
                    }

                (*d)[(*l)].x=i;
                (*d)[(*l)].y=j;
                (*d)[(*l)].corelatie=c;
                (*d)[(*l)].culoare=colour;

                (*l)++;
            }
        }

        for(i=0;i<inaltime_img;i++)
            free(a[i]);
        free(a);

        for(i=0;i<inaltime_sb;i++)
            free(s[i]);
        free(s);

}


int cmp_descrescator (const void *a, const void *b)
{
    fereastra va=*(fereastra *)a;
    fereastra vb=*(fereastra *)b;

    if(va.corelatie<vb.corelatie)
        return 1;

    if(va.corelatie>vb.corelatie)
        return -1;

    return 0;

}


double suprapunere(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int latime, unsigned int inaltime)//calculează suprapunerea spațială dintre două detecții determinate de coordonatele (x1,y1) și (x2,y2)
{
    unsigned int l,h,a_reuniune,a_intersectie,x;
    double s;

    x=2*latime*inaltime;

    if(x1==x2 && y1==y2)
    {
        l=latime;
        h=inaltime;
    }
    else
        if(x1==x2 && y2>y1)
    {
        if(y2-y1>=latime)
            return 0;

        l=y1+latime-y2;
        h=inaltime;
    }
    else
        if(x1==x2 && y2<y1)
    {
          if(y1-y2>=latime)
            return 0;

        l=y2+latime-y1;
        h=inaltime;
    }
    else
        if(y1==y2 && x1<x2)
    {
        if(x2-x1>=inaltime)
            return 0;

        l=latime;
        h=x1+inaltime-x2;
    }
    else
        if(y1==y2 && x1>x2)
    {
        if(x1-x2>=inaltime)
            return 0;

        l=latime;
        h=x2+inaltime-x1;
    }
    else
        if(x1<x2 && y1<y2)
    {
        if(x2-x1>=inaltime)
            return 0;

        if(y2-y1>=latime)
            return 0;

        l=y1+latime-y2;
        h=x1+inaltime-x2;
    }
    else
        if(x1>x2 && y1>y2)
    {
         if(x1-x2>=inaltime)
            return 0;

        if(y1-y2>=latime)
            return 0;

        l=y2+latime-y1;
        h=x2+inaltime-x1;
    }
    else
        if(x1<x2 && y1>y2)
    {
         if(x2-x1>=inaltime)
            return 0;

        if(y1-y2>=latime)
            return 0;

        l=y2+latime-y1;
        h=x1+inaltime-x2;
    }
    else
        if(x1>x2 && y1<y2)
    {
         if(x1-x2>=inaltime)
            return 0;

        if(y2-y1>=latime)
            return 0;

        l=y1+latime-y2;
        h=x2+inaltime-x1;
    }

    a_intersectie=l*h;//aria intersectiei
    a_reuniune=x-a_intersectie;//aria reuniunii
    s=(double)a_intersectie/a_reuniune;//suprapunerea

    return s;



}
void eliminare_non_maxime(fereastra **d,unsigned int *l, unsigned int latime_sablon, unsigned int inaltime_sablon )//elimină detecțiile d[j] care au o suprapunere sup mai mare de 0.2 cu detecția d[i], unde i<j
{
    unsigned int i,j,k;
    double sup;

    for(i=0;i<(*l)-1;i++)
    for(j=i+1;j<(*l);j++)
    {
        sup=suprapunere((*d)[i].x,(*d)[i].y,(*d)[j].x,(*d)[j].y,latime_sablon,inaltime_sablon);

        if(sup>0.2)
        { for(k=j;k<(*l)-1;k++)
             (*d)[k]=(*d)[k+1];

        (*l)--;

        (*d)=(fereastra *)realloc((*d), (*l)*sizeof(fereastra));
        if(*d==NULL)
            {
                 printf("Nu s-a putut realoca memorie pentru vectorul d.");
                 return;
            }

        j--;

        }
    }
}
int main()
{
    pixel **a,*c;
    unsigned int latime_imagine, inaltime_imagine, latime_sablon,inaltime_sablon,i,l=0;
    fereastra *d;
    d=NULL;
    double ps=0.5;
    char sablon[101],sablon_grayscale[101],imagine_test[101],imagine_grayscale[101],imagine_rezultat[101];

    FILE *fin=fopen("denumiri.txt","r");
    if(fin==NULL)
 {
     printf("Fisierul text nu a fost gasit.");
     return 0;
 }

    fgets(imagine_test, 101, fin);
    imagine_test[strlen(imagine_test) - 1] = '\0';

    fgets(imagine_grayscale, 101,fin);
    imagine_grayscale[strlen(imagine_grayscale) - 1] = '\0';

    fgets(imagine_rezultat, 101,fin);
    imagine_rezultat[strlen(imagine_rezultat) - 1] = '\0';

    matrice_imagine(imagine_test,&a);
    grayscale_image(imagine_test, imagine_grayscale);

    culori(&c);

    for(i=0;i<10;i++)
    {
          fgets(sablon, 101,fin);
          sablon[strlen(sablon) - 1] = '\0';

          fgets(sablon_grayscale, 101, fin);
          sablon_grayscale[strlen(sablon_grayscale) - 1] = '\0';

          grayscale_image(sablon,sablon_grayscale);
          template_matching(imagine_grayscale,sablon_grayscale,ps,&d,&l,c[i]);//se apeleaza pentru fiecare sablon si se creeaza vectorul d
          remove(sablon_grayscale);
            printf(" %d ",l);

    }

    qsort(d,l,sizeof(fereastra),cmp_descrescator);//se sorteaza descrescator vectorul d in functie de corelatie
    dim(&latime_sablon, &inaltime_sablon,sablon);
    dim(&latime_imagine, &inaltime_imagine, imagine_test);
    eliminare_non_maxime(&d,&l,latime_sablon, inaltime_sablon);//se elimina suprapunerile mai mari de 0,2

      for(i=0;i<l;i++)
            colorare(a,d[i].x,d[i].y,11,15,d[i].culoare);//se coloreaza detectiile ramase
       printf(" %d ",l);
    imagine(imagine_test,imagine_rezultat,a);//se incarcă imaginea in memoria externa

    fclose(fin);
    remove(imagine_grayscale);
    free(c);
    free(d);

    for(i=0;i<inaltime_imagine;i++)
        free(a[i]);
    free(a);

    return 0;
}
