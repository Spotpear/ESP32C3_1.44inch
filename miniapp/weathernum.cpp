#include "weathernum.h"

#include <TJpg_Decoder.h>
//int numx;
//int numy;
//int numw;

//显示天气图标

void WeatherNum::printfweather1(int numx,int numy,String numw)
{
  if(numw == "01d" ||numw == "01n")
  {
    TJpgDec.drawJpg(numx,numy,t0, sizeof(t0));
  }
  
  else if(numw == "02d"|| numw == "03d"|| numw == "03n"||numw == "02n")
  {
    TJpgDec.drawJpg(numx,numy,t1, sizeof(t1));
  }
  else if(numw == "04d"||numw == "04n")
  {
    TJpgDec.drawJpg(numx,numy,t2, sizeof(t2));
  }
  else if(numw =="09d"||numw == "09n")
  {
    TJpgDec.drawJpg(numx,numy,t3, sizeof(t3));
  }
   else if(numw =="10d"||numw == "10n")
  {
    TJpgDec.drawJpg(numx,numy,t9, sizeof(t9));
  }
  else if(numw =="11d"||numw == "11n")
  {
    TJpgDec.drawJpg(numx,numy,t4, sizeof(t4));
  }
  else if(numw=="13d"||numw == "13n")
  {
    TJpgDec.drawJpg(numx,numy,t15, sizeof(t15));  
  }
  else if(numw="50d"||numw == "50n")
  {
    TJpgDec.drawJpg(numx,numy,t53, sizeof(t53));
  }
  // else if(numw="Sleet")
  // {
  //   TJpgDec.drawJpg(numx,numy,t6, sizeof(t6));
  // }
  // else if(numw="Light rain"||numw="Moderate rain"||numw="light to moderate rain"||numw="Moderate to heavy rain")
  // {
  //   TJpgDec.drawJpg(numx,numy,t7, sizeof(t7));
  // }
  // else if(numw==a heavy rain||numw="rainstorm"||numw="heavy rain"||nudmw="Rainstorm to heavy rainstorm")
  // {
  //   TJpgDec.drawJpg(numx,numy,t9, sizeof(t9));
  // }
  // else if(numw="Heavy rainstorm"||numw="Extremely heavy rainstorm"||numw="Heavy rainstorm to extremely heavy rainstorm"||numw="rain")
  // {
  //   TJpgDec.drawJpg(numx,numy,t11, sizeof(t11));
  // }
  // else if(numw="snow shower")
  // {
  //   TJpgDec.drawJpg(numx,numy,t13, sizeof(t13));
  // }
  // else if(numw==14||numw==26)
  // {
  //   TJpgDec.drawJpg(numx,numy,t14, sizeof(t14));
  // }
  // else if(numw==15||numw==27)
  // {
  //   TJpgDec.drawJpg(numx,numy,t15, sizeof(t15));
  // }
  // else if(numw==16||numw==17||numw==28||numw==302)
  // {
  //   TJpgDec.drawJpg(numx,numy,t16, sizeof(t16));
  // }
  // else if(numw==18)
  // {
  //   TJpgDec.drawJpg(numx,numy,t18, sizeof(t18));
  // }
  // else if(numw==19)
  // {
  //   TJpgDec.drawJpg(numx,numy,t19, sizeof(t19));
  // }
  // else if(numw==20)
  // {
  //   TJpgDec.drawJpg(numx,numy,t20, sizeof(t20));
  // }
  // else if(numw==29)
  // {
  //   TJpgDec.drawJpg(numx,numy,t29, sizeof(t29));
  // }
  // else if(numw==30)
  // {
  //   TJpgDec.drawJpg(numx,numy,t30, sizeof(t30));
  // }
  // else if(numw==31)
  // {
  //   TJpgDec.drawJpg(numx,numy,t31, sizeof(t31));
  // }
  // else if(numw==53||numw==32||numw==49||numw==54||numw==55||numw==56||numw==57||numw==58)
  // {
  //   TJpgDec.drawJpg(numx,numy,t53, sizeof(t53));
  // }
  else
  {
    TJpgDec.drawJpg(numx,numy,t99, sizeof(t99));
  }

}
