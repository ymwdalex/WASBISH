
//---"knVideo.cpp"---

//2010.12.15 Ver.1.00 for OpenNI
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"
#include "GL/glut.h"
#include "math.h"
#include "knVideo.h"

#include "XnCppWrapper.h"
using namespace xn;

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "Data/SamplesConfig.xml"
#define MAX_DEPTH 10000

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
float g_pDepthHist[MAX_DEPTH];

Context g_context;
DepthGenerator g_depth;
ImageGenerator g_image;
DepthMetaData g_depthMD;
ImageMetaData g_imageMD;

static unsigned char gColorBuffer[640 * 480 * 4];         // BGRA
static unsigned char gDepthColorBuffer[640 * 480 * 4];    // BGRA
static unsigned char gDepthImageBuffer[640 * 480 * 4];    // BGRA
static unsigned short gDepthRawBuffer[640 * 480];
static unsigned char gDepthRawBufferFloat[640 * 480];

int knVideoOpen(char *config)
{
  XnStatus rc;

  EnumerationErrors errors;
  rc = g_context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
  if (rc == XN_STATUS_NO_NODE_PRESENT)
  {
    XnChar strError[1024];
    errors.ToString(strError, 1024);
    printf("%s\n", strError);
    return (-1);
  }
  else if (rc != XN_STATUS_OK)
  {
    printf("Open failed: %s\n", xnGetStatusString(rc));
    return (-1);
  }

  rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
  rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);

  g_depth.GetMetaData(g_depthMD);
  g_image.GetMetaData(g_imageMD);

  // Hybrid mode isn't supported in this sample
  if (g_imageMD.FullXRes() != g_depthMD.FullXRes() || g_imageMD.FullYRes() != g_depthMD.FullYRes())
  {
    printf ("The device depth and image resolution must be equal!\n");
    return -1;
  }

  // RGB is the only image format supported.
  if (g_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
  {
    printf("The device image format must be RGB24\n");
    return -1;
  }

  g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);
  g_context.SetGlobalMirror(FALSE);   //画像を左右反転させたい場合はここをコメントアウトする。

  return 0;
};

int knVideoCapStart(void)
{
  //g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);
  return 0;
};


int knVideoDispOption(void)     //ダミー
{
  return 0;
};

int knVideoClose(void)
{
  g_context.Shutdown();
  return 0;
};

int knVideoCapStop(void)      //ダミー
{
  return 0;
};

int knVideoCapNext(void)      //ダミー
{
  return 0;
};


//KienectのDepth情報を色に変換して取得する
//この関数を使うときは、先に"knVideoGetImage()"関数を実行しておく必要がある。
unsigned char * knVideoGetDepthColor(void)
{
  g_depth.GetMetaData(g_depthMD);
  const XnDepthPixel* pDepth = g_depthMD.Data();

  // Calculate the accumulative histogram (the yellow display...)
  xnOSMemSet(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));

  unsigned int nNumberOfPoints = 0;
  for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
  {
    for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
    {
      if (*pDepth != 0)
      {
        g_pDepthHist[*pDepth]++;
        nNumberOfPoints++;
      }
    }
  }
  for (int nIndex=1; nIndex < MAX_DEPTH; nIndex++)  {
    g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
  }
 if (nNumberOfPoints)
  {
    for (int nIndex=1; nIndex < MAX_DEPTH; nIndex++)     {
      g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
    }
  }

  xnOSMemSet(gDepthColorBuffer, 0, sizeof(gDepthColorBuffer));

  const XnDepthPixel* pDepthRow = g_depthMD.Data();
  unsigned char* pBuffRow = gDepthColorBuffer + g_depthMD.YOffset();

  for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
  {
    const XnDepthPixel* pDepth = pDepthRow;
    unsigned char* pBuff = pBuffRow + g_depthMD.XOffset();

    for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
    {
      if (*pDepth != 0)
      {
        int nHistValue = g_pDepthHist[*pDepth];
        pBuff[2] = nHistValue;
        pBuff[1] = nHistValue;
        pBuff[0] = 0;
        pBuff[3] = 0;
      }
      pBuff += 4;
    }
    pDepthRow += g_depthMD.XRes();
    pBuffRow += g_depthMD.FullXRes()*4;
  }
  return gDepthColorBuffer;
}

//KienectのDepthカメラに対応するカメラ画像を取得する
//この関数を使うときは、先に"knVideoGetImage()"関数を実行しておく必要がある。
unsigned char * knVideoGetDepthImage(void)
{
  xnOSMemSet(gDepthImageBuffer, 0, sizeof(gDepthImageBuffer));

  g_depth.GetMetaData(g_depthMD);
  const XnDepthPixel* pDepth = g_depthMD.Data();
  const XnDepthPixel* pDepthRow = g_depthMD.Data();
  unsigned char* pBuffRow = gDepthImageBuffer + g_depthMD.YOffset();
  unsigned char* pImgRow = gColorBuffer + g_depthMD.YOffset();

  for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
  {
    const XnDepthPixel* pDepth = pDepthRow;
    unsigned char* pBuff = pBuffRow + g_depthMD.XOffset();
    unsigned char* pImg = pImgRow + g_depthMD.XOffset();

    for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
    {
      if (*pDepth != 0)
      {
        pBuff[2] = pImg[2];
        pBuff[1] = pImg[1];
        pBuff[0] = pImg[0];
        pBuff[3] = 0;
      }
      pBuff += 4;
      pImg += 4;
    }
    pDepthRow += g_depthMD.XRes();
    pBuffRow += g_depthMD.FullXRes()*4;
    pImgRow += g_depthMD.FullXRes()*4;
  }
  return gDepthImageBuffer;
}

//Kienectのカメラ画像を取得する
unsigned char * knVideoGetImage(void)
{
  XnStatus rc = XN_STATUS_OK;

  // Read a new frame
  rc = g_context.WaitAnyUpdateAll();
  if (rc != XN_STATUS_OK)
  {
    printf("Read failed: %s\n", xnGetStatusString(rc));
    return NULL;
  }
  //g_depth.GetMetaData(g_depthMD);
  g_image.GetMetaData(g_imageMD);
  //const XnDepthPixel* pDepth = g_depthMD.Data();
  const XnUInt8* pImage = g_imageMD.Data();
 const XnRGB24Pixel* pImageRow = g_imageMD.RGB24Data();
  unsigned char* pBuffRow = gColorBuffer + g_imageMD.YOffset();

  for (XnUInt y = 0; y < g_imageMD.YRes(); ++y)
  {
    const XnRGB24Pixel* pImage = pImageRow;
    unsigned char* pBuff = pBuffRow + g_imageMD.XOffset();

    for (XnUInt x = 0; x < g_imageMD.XRes(); ++x, ++pImage)
    {
      pBuff[0] = pImage->nBlue;
      pBuff[1] = pImage->nGreen;
      pBuff[2] = pImage->nRed;
      pBuff[3] = 0;
      pBuff +=4;
    }
    pImageRow += g_imageMD.XRes();
    pBuffRow += g_imageMD.XRes()*4;
  }
  return gColorBuffer;
};

#if 0
//KienectのRawDepthデータを取得する
//この関数を使うときは、先に"knVideoGetImage()"関数を実行しておく必要がある。
float g_pDepthHistGrey[MAX_DEPTH];

void knVideoGetDepthRaw(unsigned short **depthShort, unsigned char** depthFloat)
{
  xnOSMemSet(gDepthRawBuffer, 0, sizeof(gDepthRawBuffer));

  g_depth.GetMetaData(g_depthMD);
  const XnDepthPixel* pDepth = g_depthMD.Data();

	// Calculate the accumulative histogram (the yellow display...)
	xnOSMemSet(g_pDepthHistGrey, 0, sizeof(g_pDepthHistGrey));

	unsigned int nNumberOfPoints = 0;
	for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
	{
		for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				g_pDepthHistGrey[*pDepth]++;
				nNumberOfPoints++;
			}
		}
	}
	for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
	{
		g_pDepthHistGrey[nIndex] += g_pDepthHistGrey[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			g_pDepthHistGrey[nIndex] = (unsigned char)(256 * (1.0f - (g_pDepthHistGrey[nIndex] / nNumberOfPoints)));
		}
	}

  const XnDepthPixel* pDepthRow = g_depthMD.Data();
  unsigned short* pBuffRow = gDepthRawBuffer + g_depthMD.YOffset();
  unsigned char *pBuffRowFloat = gDepthRawBufferFloat + g_depthMD.YOffset();

  for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
  {
    const XnDepthPixel* pDepth = pDepthRow;
    unsigned short* pBuff = pBuffRow + g_depthMD.XOffset();
    unsigned char* pBuffFloat = pBuffRowFloat + g_depthMD.XOffset();

    for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth, ++pBuff, ++pBuffFloat)
    {
      if (*pDepth != 0)
      {
        *pBuff = *pDepth;
        *pBuffFloat = (unsigned char)g_pDepthHistGrey[*pDepth];
      }
    }
    pDepthRow += g_depthMD.XRes();
    pBuffRow += g_depthMD.FullXRes();
    pBuffRowFloat += g_depthMD.FullXRes();
  }

  *depthShort = gDepthRawBuffer;
  *depthFloat = gDepthRawBufferFloat;

  return;
};
#else
unsigned short *knVideoGetDepthRaw(void)
{
  xnOSMemSet(gDepthRawBuffer, 0, sizeof(gDepthRawBuffer));

  g_depth.GetMetaData(g_depthMD);
  const XnDepthPixel* pDepth = g_depthMD.Data();
  const XnDepthPixel* pDepthRow = g_depthMD.Data();
  unsigned short* pBuffRow = gDepthRawBuffer + g_depthMD.YOffset();

  for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
  {
    const XnDepthPixel* pDepth = pDepthRow;
    unsigned short* pBuff = pBuffRow + g_depthMD.XOffset();

    for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth, ++pBuff)
    {
      if (*pDepth != 0)
      {
        *pBuff = *pDepth;
      }
    }
    pDepthRow += g_depthMD.XRes();
    pBuffRow += g_depthMD.FullXRes();
  }
  return gDepthRawBuffer;
};
#endif

int knVideoInqSize(int *x, int *y)
{
  *x = (int)g_depthMD.FullXRes();
  *y = (int)g_depthMD.FullYRes();
  return 0;
};

int knVideoSetMotorPosition(float MotorPosition)
{
  return -1;
}

int knVideoSetLedMode(int LedMode)
{
  return -1;
}

//////////////////////////////////////////////////////
//Depthカメラ(raw)->ワールド座標（単位mm）
//入力：
//　x:参照型 float :DepthカメラX座標 0～639
//　y:参照型 float :DepthカメラX座標 0～479
//　z:参照型 float :DepthカメラDepth 0～2047
//出力：
//　x:参照型 float :ワールド座標X（単位mm）
//　y:参照型 float :ワールド座標Y（単位mm）
//　z:参照型 float :ワールド座標Z（単位mm）
//////////////////////////////////////////////////////
void KinectDepthToWorld(float *x, float *y, float *z)
{
  XnPoint3D in[1],out[1];

  in[0].X = *x;
  in[0].Y = *y;
  in[0].Z = *z;
  g_depth.ConvertProjectiveToRealWorld (1,in,out);
  *x = out[0].X;
  *y =-out[0].Y; //ARToolKitのカメラ座標系は、Y軸が下向き
  *z = out[0].Z;
};

void KinectDepthToProject(float *x, float *y, float *z)
{
  XnPoint3D in[1],out[1];

  in[0].X = *x;
  in[0].Y = *y;
  in[0].Z = *z;
  g_depth.ConvertRealWorldToProjective(1,in,out);
  *x = out[0].X;
  *y =-out[0].Y; //ARToolKitのカメラ座標系は、Y軸が下向き
  *z = out[0].Z;
};
//---"knVideo.cpp"---ここまで-------------------------------------------------------