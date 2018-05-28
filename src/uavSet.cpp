/**	@file		uavset.cpp
 *	@note		gongchengdaxue.
 *	@brief		set the uavstep
 *
 *	@author		huangjingwei
 *	@date		2018/5/24
 *	@note		历史记录：
 *	@note		V1.0.0  
 *	@warning	
 */
#include "include/uavSet.h"
//#include <boost/concept_check.hpp>
#define MAX_PLANE_NUM 50

PLANE::PLANE()
{
}

PLANE::PLANE(MAP* map,FLAY_PLANE* pstFlayPlane,pathSearch* roadSearch):mmap(map),mpstFlayPlane(pstFlayPlane),mpathSearch(roadSearch)
{
    for(int i=0;i<MAX_PLANE_NUM;i++)
    PlanSetate.push_back(-1);
}


void PLANE::plane_up(int plane_num,int high,int flag)  //flag:0=back_search,flag=1:back_trans
{
    int step=0;
     if(flag)
    {
     while( mpstFlayPlane->astUav[plane_num].nZ<high)
    {
      mpstFlayPlane->astUav[plane_num].nZ+=1;
      break;
    }
	
    }
    else
    {
      while( mpstFlayPlane->astUav[plane_num].nZ<high)
      {
        for(int i;i<mpstFlayPlane->nUavNum;i++)
        {
          if(i==plane_num)
            continue;
          if((mpstFlayPlane->astUav[plane_num].nZ+1)==(mpstFlayPlane->astUav[i].nZ)&&(mpstFlayPlane->astUav[plane_num].nX)==(mpstFlayPlane->astUav[i].nX)&&(mpstFlayPlane->astUav[plane_num].nY)==(mpstFlayPlane->astUav[i].nY))
          {
            step++;
          // break;
          }
        }	
        if(!step)
          mpstFlayPlane->astUav[plane_num].nZ+=1;
          break;
      }
      mpstFlayPlane->astUav[plane_num].nGoodsNo=-1;
    }


}

void PLANE::plane_get(int plane_num,int high,int goods_no)
{
    if(goods_no==-1)
    return;
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	
  }
  int step=0;
  while( mpstFlayPlane->astUav[plane_num].nZ>high)
  {
    mpstFlayPlane->astUav[plane_num].nZ-=1;
    step++;
    break;
  }
  if(step==0&&mpstFlayPlane->astUav[plane_num].nZ==high)
  { 
    mpstFlayPlane->astUav[plane_num].nGoodsNo=goods_no;
  }
}
  
pair<int, int> PLANE::plane_search(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos)
{
  int uavX = mpstMatch->astWeUav[plane_num].nX;
  int uavY = mpstMatch->astWeUav[plane_num].nY;
  int uavZ = mpstMatch->astWeUav[plane_num].nZ;

  if(goods_no==-1)
    return make_pair(uavX,uavY);
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	  
  }
  

  int goodsXStart = mpstMatch->astGoods[goods_num].nStartX;
  int goodsYStart = mpstMatch->astGoods[goods_num].nStartY;

  printf("uavX=%3d,uavY=%3d,uavZ=%3d,goodsXStart=%d,goodsYStart=%d\r\n",uavX,uavY,uavZ,goodsXStart,goodsYStart);
  //mstar->findpath(uavX, uavY, uavZ, goodsXStart, goodsYStart,uavZ);
  vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(goodsXStart,goodsYStart),uavZ,obstaclePos);
  int uavNextX=uavX;
  int uavNextY=uavY;
  int uavNextZ=uavZ;
  
  mpathSearch->printPath(path);
  if(path.size()>1)
  {
    auto p=path.begin();
    
    p++;
    Node node = *p;

    uavNextX = node.x;
    uavNextY = node.y;
    uavNextZ = uavZ;
      
  }else if(uavZ < mmap->getMaxFlyHeight()-1)
  {
    uavNextZ = uavZ+1;

  }
  printf("uavNextX=%3d,uavNextY=%3d,uavNextZ=%3d\r\n",uavNextX,uavNextY,uavNextZ);

  
  if(uavNextZ>uavZ)
  {
    if(mmap->get_mappoint (uavX,uavY, uavNextZ)==0)
    {    
      uavNextX=uavX;
      uavNextY=uavY;
    }
    else
    {
      int flag=0;
      for(int i=-1;i<2;i++)
      {	
      	for(int j=-1;j<2;j++)
      	{
      	  if(i==0&&j==0)
      	    continue;
      	  if(mmap->get_mappoint (uavX+i,uavY+j, uavZ)==0)
      	  {
      	    uavNextX=uavX+i;
      	    uavNextY=uavY+j;
      	    uavNextY=uavZ;
      	    flag=1;
      	    break;
      	  }	 
      	}	
      	if(flag==1)
      	  break;
      }	   
    }
  }
    
  //if(mstar->getPathSize()>1)
  {
    mpstFlayPlane->astUav[plane_num].nY = uavNextY;
    mpstFlayPlane->astUav[plane_num].nX = uavNextX;
    mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
  }
  return make_pair(uavNextX,uavNextY);
}
  
pair<int, int> PLANE::plane_tran(int plane_num,int goods_no, vector<pair<int, int>> obstaclePos)
{
  int uavX = mpstMatch->astWeUav[plane_num].nX;
  int uavY = mpstMatch->astWeUav[plane_num].nY;
  int uavZ = mpstMatch->astWeUav[plane_num].nZ;

  if(goods_no==-1)
    return make_pair(uavX,uavY);
  int goods_num;
  for(goods_num=0;goods_num<mpstMatch->nGoodsNum;goods_num++)
  {
    if(goods_no==mpstMatch->astGoods[goods_num].nNO)
      break;	  
  }

  int goodsXStart = mpstMatch->astGoods[goods_num].nEndX;
  int goodsYStart = mpstMatch->astGoods[goods_num].nEndY;

  printf("uavX=%3d,uavY=%3d,uavZ=%3d,goodsXStart=%d,goodsYStart=%d\r\n",uavX,uavY,uavZ,goodsXStart,goodsYStart);
  //mstar->findpath(uavX, uavY, uavZ, goodsXStart, goodsYStart,uavZ);

  vector<Node> path = mpathSearch->createGraph(make_pair(uavX,uavY),make_pair(goodsXStart,goodsYStart),uavZ,obstaclePos);
  int uavNextX=uavX;
  int uavNextY=uavY;
  int uavNextZ=uavZ;
  if(path.size()>1)
  {
    auto p=path.begin();
    
    p++;
    Node node = *p;
    uavNextX = node.x;
    uavNextY = node.y;
    uavNextZ = uavZ;
      
  }else if(uavZ < mmap->getMaxFlyHeight()-1)
  {
    uavNextZ = uavZ+1;

  }
  printf("uavNextX=%3d,uavNextY=%3d,uavNextZ=%3d\r\n",uavNextX,uavNextY,uavNextZ);
  
    if(uavNextZ<uavZ)
  {
    if(mmap->get_mappoint (uavX,uavY, uavNextZ)==0)
    {    
      uavNextX=uavX;
      uavNextY=uavY;
    }
    else
    {
      int flag=0;
      for(int i=-1;i<2;i++)
      {	
	for(int j=-1;j<2;j++)
	{
	  if(i==0&&j==0)
	    continue;
	  if(mmap->get_mappoint (uavX+i,uavY+j, uavZ)==0)
	  {
	    uavNextX=uavX+i;
	    uavNextY=uavY+j;
	    uavNextY=uavZ;
	    flag=1;
	    break;
	  }	 
	}	
	if(flag==1)
	  break;
      }	   
    }
  }

  //if(mstar->getPathSize()>1)
  {
    mpstFlayPlane->astUav[plane_num].nY = uavNextY;
    mpstFlayPlane->astUav[plane_num].nX = uavNextX;
    mpstFlayPlane->astUav[plane_num].nZ = uavNextZ;
  }
  return  make_pair(uavNextX,uavNextY);
}
  
void PLANE::plane_put(int plane_num,int high)
{
  int step=0;
  while( mpstFlayPlane->astUav[plane_num].nZ>high)
  {
    mpstFlayPlane->astUav[plane_num].nZ-=1;
    step++;
    break;
  }
}

void PLANE:: planePathCorretion(void)
{
  int uavX=0 ,uavNextX=0;
  int uavY=0 ,uavNextY=0;
  int uavZ=0 ,uavNextZ=0;
  
  int uavX1=0 ,uavNextX1=0;
  int uavY1=0 ,uavNextY1=0;
  int uavZ1=0 ,uavNextZ1=0;

  int correctState=0;

  for(int uavnum=0;uavnum< mpstMatch->nUavWeNum;uavnum++)//判断交叉飞行存不存在，撞击风险
  {
    if(mpstMatch->astWeUav[uavnum].nStatus!=1)//飞机有效
    {
      uavX      = mpstMatch->astWeUav[uavnum].nX;
      uavY      = mpstMatch->astWeUav[uavnum].nY;
      uavZ      = mpstMatch->astWeUav[uavnum].nZ;
      uavNextX  = mpstFlayPlane->astUav[uavnum].nX; 
      uavNextY  = mpstFlayPlane->astUav[uavnum].nY;
      uavNextZ  = mpstFlayPlane->astUav[uavnum].nZ;
      if(uavZ == uavNextZ)//同一平面
      {
        int tmpX = abs(uavNextX - uavX);
        int tmpY = abs(uavNextY - uavY);
        if((tmpX==1)&&(tmpY==1))//走的对角线
        {
          for(int uavTemp=0;uavTemp< mpstMatch->nUavWeNum;uavTemp++)
          {
            if(uavTemp != uavnum)
            {
              if(mpstMatch->astWeUav[uavTemp].nStatus!=1)//飞机有效
              {
                uavX1      = mpstMatch->astWeUav[uavTemp].nX;
                uavY1      = mpstMatch->astWeUav[uavTemp].nY;
                uavZ1      = mpstMatch->astWeUav[uavTemp].nZ;
                uavNextX1  = mpstFlayPlane->astUav[uavTemp].nX; 
                uavNextY1  = mpstFlayPlane->astUav[uavTemp].nY;
                uavNextZ1  = mpstFlayPlane->astUav[uavTemp].nZ;
                if(uavNextZ1 == uavNextZ)
                {
                  int tmpX1 = abs(uavNextX1 - uavX1);
                  int tmpY1 = abs(uavNextY1 - uavY1);  
                  
                  if((tmpX1 ==1 )&& (tmpY1 ==1))//走的对角线
                  {
                    int tmpX2 = abs(uavNextX1 - uavNextX);
                    int tmpY2 = abs(uavNextY1 - uavNextY);  

                    if((tmpX2<=1)&&(tmpY2<=1))//存在交叉问题
                    {
                    // printf("\n");
                    // printf("uavX=%d,uavY=%d,uavNextX=%d,uavNextY=%d\n",uavX,uavY,uavNextX,uavNextY);
                    // printf("uavX1=%d,uavY1=%d,uavNextX1=%d,uavNextY1=%d\n",uavX1,uavY1,uavNextX1,uavNextY1);
                    // printf("\n");
                    // string response;
                    // cin >> response;
                      correctState = 1;//改变了原来的计划路线
                      if(mpstFlayPlane->astUav[uavnum].nZ< mmap->getMaxFlyHeight())
                      {
                        mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                        mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                        mpstFlayPlane->astUav[uavnum].nZ++;
                      }else{
                        if(1 != mmap->get_mappoint(mpstFlayPlane->astUav[uavnum].nX,mpstFlayPlane->astUav[uavnum].nY,mpstFlayPlane->astUav[uavnum].nZ-1))
                        {
                          mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                          mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                          mpstFlayPlane->astUav[uavnum].nZ--;
                        }  
                        else{
                          mpstFlayPlane->astUav[uavnum].nX = mpstMatch->astWeUav[uavnum].nX;
                          mpstFlayPlane->astUav[uavnum].nY = mpstMatch->astWeUav[uavnum].nY;
                          mpstFlayPlane->astUav[uavnum].nZ = mpstMatch->astWeUav[uavnum].nZ;
                        }
                      }
                    }
                  }

                }
              }
            }
            
          }

        }
      }
      

    }
  }


  while(correctState == 1)//如果改变了飞行计划，则需要校准一下飞行计划，防止自我撞击
  {
    
    correctState=0;
    for(int uavnum=0;uavnum< mpstMatch->nUavWeNum;uavnum++)//判断飞行存不存在，撞击风险
    {
      if(mpstMatch->astWeUav[uavnum].nStatus!=1)//飞机有效
      {
        uavX      = mpstMatch->astWeUav[uavnum].nX;
        uavY      = mpstMatch->astWeUav[uavnum].nY;
        uavZ      = mpstMatch->astWeUav[uavnum].nZ;
        uavNextX  = mpstFlayPlane->astUav[uavnum].nX; 
        uavNextY  = mpstFlayPlane->astUav[uavnum].nY;
        uavNextZ  = mpstFlayPlane->astUav[uavnum].nZ;
        for(int uavTmp=0; uavTmp< mpstMatch->nUavWeNum;uavTmp++)//判断飞行存不存在，撞击风险
        {
          if(mpstMatch->astWeUav[uavTmp].nStatus!=1)//飞机有效
          {
            uavNextX1  = mpstFlayPlane->astUav[uavTmp].nX; 
            uavNextY1  = mpstFlayPlane->astUav[uavTmp].nY;
            uavNextZ1  = mpstFlayPlane->astUav[uavTmp].nZ;   
            if(uavNextX==uavNextX1&&\
               uavNextY==uavNextX1&&\
               uavNextZ==uavNextZ1)   //两架飞机目标点相同，需要处理
               {
                 mpstFlayPlane->astUav[uavnum].nX = uavX;
                 mpstFlayPlane->astUav[uavnum].nY = uavY;
                 mpstFlayPlane->astUav[uavnum].nZ = uavZ;
                 correctState=1;
               }      
          }
        }
      }
    }

  }


}

void PLANE::set_newmatch(MATCH_STATUS * pstMatch)
{
  mpstMatch=pstMatch;
}
  
FLAY_PLANE* PLANE::renew()
{
  return mpstFlayPlane;
}




  