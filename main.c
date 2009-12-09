//Made by Bruno Fascendini!!! 2009 bfascendini@hotmail.com para uControl.
//Libre de uso y modificación siempre y cuando se mantengan estos créditos!

#include "main.h"
#include "parser.h"
#include "evaluador.h"
#include "HDM64GS12.c"

//3D:
//char EquIn[BUFFER_SIZE]="sin(x)*cos(y)";
//char EquIn[BUFFER_SIZE]="(x/4)^2-(y/4)^2";
//char EquIn[BUFFER_SIZE]="sqrt(20-x^2-y^2)";            //semiesfera
//char EquIn[BUFFER_SIZE]="16";                          //plano

//char EquIn[BUFFER_SIZE]="x^2+y^2";                     //paraboloide
//char EquIn[BUFFER_SIZE]="sqrt(5-x^2)-sqrt(9-y^2)";  //superficie discontinua...
char EquIn[BUFFER_SIZE]="sqrt(x^2-y^2)";

//2D:
//char EquIn[BUFFER_SIZE]="x^2";            //parabola.../2D)

float32 const RAIZ2SOBRE2  = 0.70710678118654752440084436210485;
float32 const RAIZ2TERCIOS = 0.81649658092772603273242802490196;
float32 const UNOSOBRERAIZ6 = 0.40824829046386301636621401245113;

struct graphic{
 float32 minX;      //minimum x axis value
 float32 minY;      //minimum y axis value
 float32 minZ;      //minimum z axis value
 float32 maxX;      //maximum x axis value
 float32 maxY;      //maximum y axis value
 float32 maxZ;      //maximum z axis value

 float32 minViewX;      //minimum x axis view value
 float32 minViewY;      //minimum y axis view value
 float32 minViewZ;      //minimum z axis view value
 float32 maxViewX;      //maximum x axis view value
 float32 maxViewY;      //maximum y axis view value
 float32 maxViewZ;      //maximum z axis view value

 int8 x1,y1;     //position(top left) in the LCD were the graphic will start             
 int8 x2,y2;     //position(bottom right) in the LCD were the graphic will end

 //will be calculated with previous values...
 float32 centerX;
 float32 centerY;
 float32 centerZ;

}graph;

 char  temp[20];

/*
(x1,y1)||||||||||||||||||||||||||||||||
       ||||||||||||||||||||||||||||||||
       ||||||||||||||||||||||||||||||||
       ||||||||||||||||||||||||||||||||
       ||||||||||||||||||||||||||||||||
       ||||||||||||||||||||||||||||||||       
       ||||||||||||||||||||||||||||||||(x2,y2)       

*/

//*****************************************************************************************
//*****************************************************************************************
//P RO G R A M A     P R I N C I P A L
//*****************************************************************************************
//*****************************************************************************************
void main(){
   float32* fltPtr;
   float32 varX,varY,varZ;
   float32 Xp,Yp;
   float32 fx,fy,fz;
   float incX;
   float incY;
   
   char x,y,z;
   char posicion;

   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_OFF|ADC_TAD_MUL_0);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_spi2(SPI_SS_DISABLED);
   setup_wdt(WDT_OFF);
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
   setup_timer_4(T4_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);

   set_tris_a(0x00);
   set_tris_b(0x00);
   set_tris_c(0xC0);
   set_tris_d(0x00);
   set_tris_e(0x00);

   DELAY_MS(200);

   output_b(0xFF);

   GLCD_init(1);

   DELAY_MS(500);

   output_b(0x00);

   //Setear propiedades de la grafica.
   graph.x1=0.0;
   graph.y1=0.0;
   graph.x2=127.0;
   graph.y2=63.0;

   graph.minX=-1.0;
   graph.maxX=1.0;
   graph.minY=-1.0;
   graph.maxY=1.0;

   graph.minViewX=-2.5;
   graph.maxViewX=2.5;
   graph.minViewY=-1.25;
   graph.maxViewY=1.25;

   printf("Hecho por Bruno Fascendini @ 2009 para uControl y Todopic\r\ncomo parte de proyecto calculadora cientifica con PIC del foro uControl\r\n");
   printf("Parseador version: %s Evaluador version: %s\r\n",ParserVer,EvaluadorVer);
   //menú:
   printf("Ingrese la superficie a graficar: ");

   posicion=0;
   do{
      EquIn[posicion]=getc();
      if(posicion>0 && EquIn[posicion]==8) posicion--; else posicion++;      //delete if delete key pressed...
      if(posicion==BUFFER_SIZE) break;
   }while(EquIn[posicion-1]!=13);
   EquIn[posicion]='\0';

   printf("Ha pedido que se grafique la ecuacion: %s",EquIn);

   printf("Ingrese valor minimo de x: ");
   posicion=0;
   do{
      temp[posicion]=getc();
      if(posicion>0 && temp[posicion]==8) posicion--; else posicion++;      //delete if delete key pressed...
      if(posicion==BUFFER_SIZE) break;
   }while(temp[posicion-1]!=13);
   temp[posicion]='\0';
   graph.minX=atof(temp);
   
   printf("Ingrese valor maximo de x: ");
   posicion=0;
   do{
      temp[posicion]=getc();
      if(posicion>0 && temp[posicion]==8) posicion--; else posicion++;      //delete if delete key pressed...
      if(posicion==BUFFER_SIZE) break;
   }while(temp[posicion-1]!=13);
   temp[posicion]='\0';
   graph.maxX=atof(temp);

   printf("Ingrese valor minimo de y: ");
   posicion=0;
   do{
      temp[posicion]=getc();
      if(posicion>0 && temp[posicion]==8) posicion--; else posicion++;      //delete if delete key pressed...
      if(posicion==BUFFER_SIZE) break;
   }while(temp[posicion-1]!=13);
   temp[posicion]='\0';
   graph.minY=atof(temp);
   
   printf("Ingrese valor maximo de y: ");
   posicion=0;
   do{
      temp[posicion]=getc();
      if(posicion>0 && temp[posicion]==8) posicion--; else posicion++;      //delete if delete key pressed...
      if(posicion==BUFFER_SIZE) break;
   }while(temp[posicion-1]!=13);
   temp[posicion]='\0';
   graph.maxY=atof(temp);

   graph.minViewX=graph.minX;
   graph.maxViewX=graph.maxX;
   graph.minViewY=graph.minY;
   graph.maxViewY=graph.maxY;

   //Calculating centers...
   graph.centerX= (graph.x2-graph.x1)/2;
   graph.centerY= (graph.y2-graph.y1)/2;

   incX=(graph.maxX-graph.minX)/50;       //set step cuantity for X axis
   incY=(graph.maxY-graph.minY)/50;       //set step cuantity for Y axis

   printf("Graficando...\r\n");

   //////////////////////////////////////////////////
   //PROCESO..............
   //comienzo del parseado de la ecuacion ingresada...
   //////////////////////////////////////////////////

   strlwr(EquIn);                        //1) PASAR EQUACION A MINUSCULAS

   //printf("Cadena en minusculas: %s\r\n",EquIn); 
                                        
   strCodificar(EquIn);                   // 2) REDUCIR ECUACION PARA OPTIMIZAR PROCESADO POSTERIOR

   printf("Cadena codificada: %s\r\n",EquIn);

   strPosFijar(EquIn,EquIn);              // 3) Pasar a notación PostFija

   printf("Cadena en notacion postfija: %s\r\n",EquIn);

   //cut unuseful zones...
   //if(graph.minX<graph.minViewX) graph.minX=graph.minViewX;
   //if(graph.maxX>graph.maxViewX) graph.maxX=graph.maxViewX;
   //if(graph.minY<graph.minViewY) graph.minY=graph.minViewY;
   //if(graph.maxY>graph.maxViewY) graph.maxY=graph.maxViewY;

/*
//Ejes!
   varx=0.0;
   varz=0.0;
   for(vary=0.0;Xp>=0;vary+=0.4){
      Xp = RAIZ2SOBRE2 * (varX - varY) + CentroX;
      Yp = -(RAIZ2TERCIOS * varZ - UNOSOBRERAIZ6 * (varX + varY)) + CentroY;
      GLCD_pixel((int8)Xp,(int8)Yp,1);
   }

   vary=0.0;
   varz=0.0;
   for(varx=0.0;Xp<Radius+CentroX;varx+=0.4){
      Xp = RAIZ2SOBRE2 * (varX - varY) + CentroX;
      Yp = -(RAIZ2TERCIOS * varZ - UNOSOBRERAIZ6 * (varX + varY)) + CentroY;
      GLCD_pixel((int8)Xp,(int8)Yp,1);
   }

   varx=0.0;
   vary=0.0;
   for(varz=0.0;Yp>0;varz+=1.0){
      Xp = RAIZ2SOBRE2 * (varX - varY) + CentroX;
      Yp = -(RAIZ2TERCIOS * varZ - UNOSOBRERAIZ6 * (varX + varY)) + CentroY;
      GLCD_pixel((int8)Xp,(int8)Yp,1);
   }
*/


/*
//2D:
for(varX=graph.minX;varX<graph.maxX;varX+=0.1){
   fltPtr=strEvaluar(EquIn,StackNum,&varX,NULL,NULL);
   printf("X: %f Y: %f\r\n",varX,*fltPtr);

   Xp=varX+graph.centerX;
   Yp=graph.centerY-(*fltPtr);

   //ensure that pixel belongs to actual graph section...else do not show it!(out of bounds)
   if(Xp>=graph.x1 && Xp<=graph.x2 && Yp>=graph.y1 && Yp<=graph.y2) GLCD_pixel((int8)Xp,(int8)Yp,1);
}

printf("HECHO!\r\n");
while(1);
*/

//Proyeccion Isométrica...
   for(varY=graph.minY;varY<graph.maxY;varY+=incY){
      for(varX=graph.minX;varX<graph.maxX;varX+=incX){
         //indicate no error...
         errno=0;
         fltPtr=strEvaluar(EquIn,StackNum,&varX,&varY,NULL);
         //if errors during calculating...do not bother at all..
         if(errno) continue;
         
         //calculate isometric proyection
         varZ = *fltPtr;
         Xp = RAIZ2SOBRE2 * (varX - varY);
         Yp = (RAIZ2TERCIOS * varZ - UNOSOBRERAIZ6 * (varX + varY));
            
         //ensure that values are inside drawing zone...
         if(Xp>=graph.minViewX && Xp<=graph.maxViewX && Yp>=graph.minViewY && Yp<=graph.maxViewY){
            //printf("X: %f Y: %f Z: %f\r\n",varX,varY,varZ);
         
            //now let´s ubicate them inside actual graphic bounds...
            Xp=(Xp-graph.minViewX)*(float32)(graph.x2-graph.x1)/(graph.maxViewX-graph.minViewX)+(float32)graph.x1;
            Yp=(float32)graph.y2-((Yp-graph.minViewY)*(float32)(graph.y2-graph.y1)/(graph.maxViewY-graph.minViewY))+(float32)graph.y1; 

            //printf("XP: %f YP: %f\r\n",Xp,Yp);
            if(Xp>=graph.x1 && Xp<=graph.x2 && Yp>=graph.y1 && Yp<=graph.y2) GLCD_pixel((int8)Xp,(int8)Yp,1);            
         }
      }
   }

   printf("Proceso de graficacion finalizado.\r\n");

/*
//polares
  for(varY=-PI/2;varY<PI/2;varY+=PI/63){
      for(varX=-PI;varX<PI;varX+=2*PI/31){
         fX=Radius*cos(varX)*cos(varY);
         fY=Radius*cos(varX)*sin(varY);
         fZ=Radius*sin(varX);

         Xp=Sqrt(2.0) / 2.0 * (fX - fY) * Distance + CentroX;
         Yp = (Sqrt(2.0 / 3.0) * fZ - (1.0 / Sqrt(6.0)) * (fX + fY)) * Distance + CentroY;
         GLCD_pixel((int8)Xp,(int8)Yp,1);
       //printf("Xp: %f Yp: %f\r\n",Xp,Yp);
      }
  }
*/  
  
 //printf("EL resultado de la ecuacion es: %9f\r\n",*fltPtr);
   while(1);
}
