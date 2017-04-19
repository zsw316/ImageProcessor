#include <stdio.h>  
#include <unistd.h>
#include <cstdlib>
#include "def.h"
#include "ImageHelper.h"

typedef enum {
    CROP_OP,
    ZOOM_OP,
    ROTATE_OP,
    SHOWEXIF_OP,
    INVALID_OP
} ImageOperationType;

void printUsage()
{
    print_info("------ Image Processor Usage ------\n");
    print_info(" ImageProcessor: \n");
    print_info(" -h : print help infomation\n");
    print_info(" -c 150x150 -s srcImage -o destImage: Crop the source image to size 150x150, output as destImage\n");
    print_info(" -z 0.5 -s srcImage -o destImage: Zoom in/out the source image in a specific scale, output as destImage\n");
    print_info(" -r 270 -s srcImage -o destImage: Rotate clockwise the srcImage in 270 degree, output as destImage\n");
    print_info(" -d srcImage: Show the exif information of specified image\n");
    print_info( "----------------------------------\n");
}

bool isValidSize(char * sizeStr, uint32_t& width, uint32_t& height)
{
    int subStrNum = 0;
    int CORRECT_SUBSTR_NUM = 2;
    char *p[CORRECT_SUBSTR_NUM+1];
    char *buf = sizeStr;
    char *outer_ptr;

    while((p[subStrNum] = strtok_r(buf, "x", &outer_ptr))!=NULL)   
    {  
        subStrNum++; 
        if (subStrNum>CORRECT_SUBSTR_NUM) break; 
        buf = NULL;
    }

    if (subStrNum != CORRECT_SUBSTR_NUM) 
    {
        error_log("Invalid size str: %s", sizeStr);
        return false;
    }

    width = atoi(p[0]);
    height = atoi(p[1]);

    return true;
}

void cropImage(char* sizeStr, const char* srcImage, const char* destImage, CImageHelper *helper)
{
    uint32_t width = 0, height = 0;
    if (srcImage==NULL || destImage==NULL)
    {
        error_log("Invalid image name: srcImage: %s, destImage:%s", srcImage, destImage);
        return;
    }

    if (!isValidSize(sizeStr, width, height)) 
    {
        error_log("Invalid size str: %s", sizeStr);
        return;
    }

    helper->CropImage(srcImage, destImage, width, height, false);
}

void zoomImage(char* scaleStr, const char* srcImage, const char* destImage, CImageHelper *helper)
{
    double scale = atof(scaleStr);
    if(scale<0.1 || scale>64)
    {
        error_log("Invalid scale: %f (should be between 1 and 64)", scale);
        return;
    }

    helper->ZoomImage(srcImage, destImage, scale, false);
}

void rotateImage(char* degreeStr, const char* srcImage, const char* destImage, CImageHelper *helper)
{ 
    uint32_t degree = atoi(degreeStr);
    if (degree<=0 || degree>=360)
    {
        error_log("Invalid rotation degree: %u", degree);
        return;
    }

    helper->RotateCounterClockwise(srcImage, destImage, degree);
}

void showImageExif(const char* srcImage, CImageHelper *helper)
{
    helper->ShowExif(srcImage);
}

int main(int argc, char *argv[]) 
{
    if(argc<2)
    {
        printUsage();
        exit(1);
    }
    
    char temp;
    char* srcImage = NULL;
    char* destImage = NULL;
    char* scaleStr = NULL;
    char* sizeStr = NULL;
    char* degreeStr = NULL;
    ImageOperationType opType = INVALID_OP;

    while((temp=getopt(argc,argv,"c:z:s:o:r:d:h"))!=-1)   
    {   
        switch (temp)   
        {
            case 'c':    
            {
                sizeStr = optarg; 
                opType = CROP_OP; 
                break; 
            }
            
            case 's':   
            {
                srcImage = optarg;  
                break;  
            }
            
            case 'o':   
            {
                destImage = optarg;  
                break; 
            }
            
            case 'z':
            {
                scaleStr = optarg;
                opType = ZOOM_OP;
                break;
            }

            case 'r':   
            {
                opType = ROTATE_OP;    
                degreeStr = optarg;   
                break; 
            }
            
            case 'd':
            {
                opType = SHOWEXIF_OP;
                srcImage = optarg; 
                break; 
            }

            case 'h':   
            {
                printUsage(); 
                exit(0); 
            } 
        }   
    }

    CImageHelper *helper = new CImageHelper();
    helper->Init();

    if (opType == INVALID_OP) {
        printUsage();
        exit(1); 
    }
    else if (opType == CROP_OP) 
    {
        cropImage(sizeStr, srcImage, destImage, helper);
    }
    else if (opType == ZOOM_OP) 
    {
        zoomImage(scaleStr, srcImage, destImage, helper);
    }
    else if (opType == ROTATE_OP)
    {
        rotateImage(degreeStr, srcImage, destImage, helper);
    }
    else if (opType == SHOWEXIF_OP)
    {
        showImageExif(srcImage, helper);
    }

    delete helper;

    return 0;
}
