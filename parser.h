//PARSEADOR DE EXPRESIONES EN NOTACION INFIJA A POSTFIJA PARA MICROCONTROLADORES PIC
//Made by Bruno Fascendini!!! 2009 bfascendini@hotmail.com para uControl.
//Libre de uso y modificación siempre y cuando se mantengan estos créditos!

#include <string.h>

//#CASE

char const ParserVer[]="v0.1.0";

#DEFINE  BUFFER_SIZE       80                   //Tamaño maximo para contener la ecuacion! Agrandar de ser necesario(Consume mucha RAM)

#DEFINE  MAX_DIG_LENGTH    9                    //Cantidad maxima de caracteres q puede contener un numero(incluso el punto decimal(pd).)
#DEFINE  SEPARADOR         '$'                  //Indica cual es caracter que se utiliza para separar elementos de la pila(ej. "$2$3$+" lo cual son 3 tokens)
#DEFINE  ESPACIO           ' '                  //Indica cual es caracter que se utiliza para separar tokens(ej. "$2 3 +" lo cual es 1 solo token)

//******************************
//E N U M E R A C I O N E S
//******************************

enum Types{
   //Jerarquia segun tipo de Operadores
   Dummy,                  //Necesario! no quitar(tampoco molesto ni ocupa espacio alguno)...
   Prior0,                 //^
   Prior1,                 //* / %
   Prior2,                 //+ -
   CierreParentesis,       //)
   FuncionConParentesis,   //Muuuchas :) Todas las que lleven parentesis en el array de arriba o bien el parentesis solo mísmo...
   Espaciador,             //El caracter de espacio...

   //Tipos numericos
   Numero,                 //Numero, con punto decimal o sin el, pi,etc
   Variable,               //x y z(u otros)
      
   //Tipos de control
   Nada,                   //Para indicar q es el primero q se procesa...
   Invalido,               //Error en la ecuacion...
   Fin_Eq,                 //Encontrado el final de la ecuación
};

enum  Funciones{
   PUNTO='.',
   X='x',
   Y='y',
   Z='z',
   E='e',
   MAS='+',
   MENOS='-',
   DIVI='/',
   POR='*',
   POT='^',
   MOD='%',
   NPI='p',
   CIERRA=')',
   ABRE='(',
   LOGN='n',
   LOGA='l',
   ABSOLUTO='a',
   ROUND='r',
   RAIZ='q',
   SENO='s',
   COSENO='c',
   TANGENTE='t',
   SECANTE='n',
   COSECANTE=0xE0,
   COTANGENTE,
   SIGNO,
   ASENO,
   ACOSENO,
   ATANGENTE,
   ASECANTE,
   ACOSECANTE,
   ACOTANGENTE,
   SENOH,
   COSENOH,
   TANGENTEH,
   SECANTEH,
   COSECANTEH,
   COTANGENTEH,
   ASENOH, 
   ACOSENOH, 
   ATANGENTEH, 
   ASECANTEH, 
   ACOSECANTEH, 
   ACOTANGENTEH,
};

#DEFINE  FUNC_LENGTH       225                 //Util para acelerar proceso de busqueda de der a izq del array
//todos los caracteres y funciones reconocidas!
char const func[FUNC_LENGTH]={"x y z e + - / * ^ % pi ) ( ln( log( abs( rnd( sqrt( sen( sin( cos( tan( sec( csc( cot( sgn( asen( asin( acos( atan( asec( acsc( acot( senh( sinh( cosh( tanh( sech( csch( coth( asenh( asinh( acosh( atanh( asech( acsch( acoth("};

#DEFINE  TOKEN_QUANT       47                   //Para velocidad, indica la cantidad de funciones reconocidas
#DEFINE  PRI_ELE_CON_PAREN 12                   //Indica cual es el primer elemento que contiene una apertura de parentesis en su nombre. Para velocidad.
//Codigos para codificarlos
char const codig[TOKEN_QUANT]={
                  X,
                  Y,
                  Z,
                  E,
                  MAS,
                  MENOS,
                  DIVI,
                  POR,
                  POT,
                  MOD,  //%
                  NPI,  //pi
                  CIERRA,
                  ABRE,  //Primer funcion con parentesis abierto!
                  LOGN,  //ln(
                  LOGA,  //log(
                  ABSOLUTO,  //abs( 
                  ROUND,  //rnd(
                  RAIZ,  //sqrt( 
                  SENO,  //sen(
                  SENO,  //sin(
                  COSENO,  //cos(
                  TANGENTE,  //tan( 
                  SECANTE,  //sec( 
                  COSECANTE, //csc(
                  COTANGENTE, //cot( 
                  SIGNO, //sgn( 
                  ASENO, //asen( 
                  ASENO, //asin( 
                  ACOSENO, //acos( 
                  ATANGENTE, //atan( 
                  ASECANTE, //asec( 
                  ACOSECANTE, //acsc(
                  ACOTANGENTE, //acot( 
                  SENOH, //senh( 
                  SENOH, //sinh( 
                  COSENOH, //cosh( 
                  TANGENTEH, //tanh( 
                  SECANTEH, //sech( 
                  COSECANTEH, //csch( 
                  COTANGENTEH, //coth( 
                  ASENOH, //asenh( 
                  ASENOH, //asinh( 
                  ACOSENOH, //acosh(
                  ATANGENTEH, //atanh( 
                  ASECANTEH, //asech(
                  ACOSECANTEH, //acsch( 
                  ACOTANGENTEH  //acoth(
                  };

char StackNum[BUFFER_SIZE];   //Contiene los numeros a procesar
char StackNumSize;            //Indica el tamaño de la pila de numeros
char StackOpe[BUFFER_SIZE/3]; //Contiene los operadores a procesar
char StackOpeSize;            //Indica el tamaño de la pila de operadores

char  Largo;

//**********************************************************************************************
//**********************************************************************************************
//F U N C I O N E S ! ! !
//**********************************************************************************************
//**********************************************************************************************
void strCodificar(char* strIn);
short strPosFijar(char* CadenaIn,char* CadenaOut);
void strAppend(char* Str,char c);

void CopiarConstARAM(char Origen,char Desde,char CantCaracteres,char* Destino);

char GetPrevToken(char* inicio);
char GetPrevTokenFunc(char desde, char* StrOut);
char CountTokensInFunc();

Types GetNextTokenType(char* Inicio);
Types RevisarPilaDeOperadores(void);
Types AnalizarOperador(char Operador);

void CargarEnPilaDeNumeros(char* Desde,char CantCar);
void CargarEnPilaDeOperadores(char Operador);
char LeerUltimoElementoPilaDeOperadores();

char* QuitarDePilaDeNumeros(void);
char QuitarDePilaDeOperadores(void);
void QuitarOperadorPrioridadInferior();

//*****************************************************************************************
//Codifica la cadena pasada a funciones de un solo caracter.
//*****************************************************************************************
void strCodificar(char* strIn){
   char* inicio,inicioTmp,fin;
   char StrToken[MAX_DIG_LENGTH];   //Un pequeño string para contenter un Token temporalmente
   char i,j,nroTokens,nroTmp;
   Types res;

   nroTokens=CountTokensInFunc()-1;
   inicio=strIn;
   inicioTmp=inicio;
   fin=strlen(strIn);
   fin+=inicio;
   while(inicio<fin){
      j=0;
      nroTmp=nroTokens;
      while(j<FUNC_LENGTH){
         i=GetPrevTokenFunc(FUNC_LENGTH-j-2,StrToken);
         j+=i;
         j++;
         res=strncmp(inicio,StrToken,i);
         if(!res){
            //printf("Detectado: %s.Largo: %u.%c sera reemplazado por: %c(Token Nro: %u)\r\n",StrToken,i,*inicioTmp,codig[nroTmp],nroTmp);
            *inicioTmp=codig[nroTmp];
            inicio+=i;
            inicio--;
            break;
         }
         nroTmp--;
      }
      if(res) *inicioTmp=*inicio;
      inicio++;
      inicioTmp++;

  }
  *inicioTmp='\0';      //Finalizar Cadena modificada
}

//*****************************************************************************************

//*****************************************************************************************
char GetPrevToken(char* inicio){
   char i=0;
   
   while(*inicio!='\0' && *inicio!=ESPACIO){inicio--; i++;};
   return i;
}

//*****************************************************************************************
//Vuelca en un string de salida<StrOut> el proximo Token inmediatamente anterior a la posicion <desde> del array <func>. Devuelve la cantidad de caracteres que tiene la cadena encontrada
//*****************************************************************************************
char GetPrevTokenFunc(char desde, char* StrOut){
   char i,j;

   while(func[desde]==ESPACIO) desde--;
   for(i=0; ; i++,desde--){
      if(desde==255)break;
      if(func[desde]==ESPACIO || func[desde]=='\0') break;
   }

   for(j=0;j<i;StrOut++,j++){
      desde++;
      *StrOut=func[desde];
   }  
   *StrOut='\0';
   return i;
}

//*****************************************************************************************

//*****************************************************************************************
char CountTokensInFunc(){
   char i,j,tmp;

   j=0;
   for(i=0;;i++){
      tmp=func[i];
      if(tmp=='\0') break;
      if(!i)j++;
      if(tmp==ESPACIO) j++;
   }
   
   return j;
}

//*****************************************************************************************

//*****************************************************************************************
short strPosFijar(char* CadenaIn,char* CadenaOut){
   Types Res,Anterior;
   char Ope;
   short Inhibir;
   
   StackNumSize=0;
   StackNumSize=0;
   Anterior=Nada;                      //Indicar que no hay nada previamente procesado...
   do{
      Inhibir=FALSE;
      Res=GetNextTokenType(CadenaIn);
      Ope=*CadenaIn;                   //Guardar POSIBLE operador en variable temporal
      //printf("Analizado: %c. Devuelto Tipo: %u\r\n",*CadenaIn,res);
      switch(Res){
         case FuncionConParentesis:
         case Variable:
            if(Anterior==Numero || Anterior==CierreParentesis){
               Ope=POR;
               Res=Prior1;             //Indicar que es un *
               Inhibir=TRUE;
            }else{
               if(Res==FuncionConParentesis){
                  CargarEnPilaDeOperadores(Ope);
                  //printf("La pila de operadores es ahora: %s\r\n",StackOpe);
               }
               if(Res==Variable){
                  CargarEnPilaDeNumeros(CadenaIn,Largo);
                  //printf("La pila de numeros es ahora: %s\r\n",StackNum);
               }
               break;
            }
         case Prior0:
         case Prior1:
         case Prior2:
         case CierreParentesis:
            if(!StackOpeSize && Res!=CierreParentesis){
               CargarEnPilaDeOperadores(Ope);
            }else{
               //printf("El ultimo operador es de tipo: %u.Res vale: %u\r\n",RevisarPilaDeOperadores(),res);
               if(Res==Prior0) Res--;                       //Si es un ^, ni siquiera se saca a si mísmo, decrementar su prioridad para que no permitir la igualdad del while()
               while(RevisarPilaDeOperadores()<=Res){
                  QuitarOperadorPrioridadInferior();
                  //printf("La pila de numeros es ahora: %s\r\n",StackNum);
                  //printf("La pila de operadores es ahora: %s\r\n",StackOpe);
               }
               if(Res!=CierreParentesis){
                  CargarEnPilaDeOperadores(Ope);
               }else{
                  if(LeerUltimoElementoPilaDeOperadores()==ABRE){
                     QuitarDePilaDeOperadores();                     //Si quedó un '(' quitarlo de la pila
                  }else{
                     QuitarOperadorPrioridadInferior();
                  }
               }
            }
            //printf("La pila de operadores es ahora: %s\r\n",StackOpe);
         break;

         case Numero:
            CargarEnPilaDeNumeros(CadenaIn,Largo);
            //printf("La pila de numeros es ahora: %s\r\n",StackNum);
         break;
      }
      
      Anterior=Res;
      if(!Inhibir) CadenaIn+=Largo;
   }while(Res!=Invalido && Res!=Fin_Eq);

   while(StackOpeSize){                      //Terminar Operaciones pendientes...
      QuitarOperadorPrioridadInferior();
   }

   strcpy(CadenaOut,StackNum+1);

   if(Res==Fin_Eq) return TRUE; else return FALSE;
}

//*****************************************************************************************

//*****************************************************************************************
Types GetNextTokenType(char* Inicio){
   char i,Tmp;
   short pd;

   Largo=0;
   Tmp=*Inicio;
   if(Tmp==ESPACIO)  return Espaciador;
   if(Tmp=='\0')     return Fin_Eq;
   if((Tmp>='0' && Tmp<='9') || Tmp==PUNTO){                //es un número?
      for(i=0,pd=0;i<MAX_DIG_LENGTH;Inicio++,i++,Largo++){  //Eso parece...
         if(*Inicio==PUNTO){
            if(!pd) pd=TRUE; else break;
         }else{
            if(*Inicio<'0' || *Inicio>'9') return Numero;
        }
      }
   }else{                                                   //Es posiblemente un Operador...
      Largo=1;
      return AnalizarOperador(Tmp);
   }
   
   return Invalido;
}

//*****************************************************************************************

//*****************************************************************************************
void CargarEnPilaDeNumeros(char* Desde,char CantCar){
   StackNum[StackNumSize++]=SEPARADOR;
   while(CantCar && *Desde!='\0'){
      StackNum[StackNumSize++]=*Desde;
      Desde++;
      CantCar--;
   }
   StackNum[StackNumSize]='\0';        //Finalizar string
}

//*****************************************************************************************

//*****************************************************************************************
void CargarEnPilaDeOperadores(char Operador){
   StackOpe[StackOpeSize++]=Operador;
   StackOpe[StackOpeSize]='\0';        //Finalizar string
}

//*****************************************************************************************

//*****************************************************************************************
char* QuitarDePilaDeNumeros(void){
   char* Recorre;
  
   Largo=0;
   if(!StackNumSize) return 0x0000;       //Si no hay nada en la pila, devolver puntero nulo
   Recorre=StackNum+StackNumSize;         //Apuntar al ultimo elemento de la pila
   while(StackNumSize && *Recorre!=SEPARADOR){
      Recorre--;
      Largo++;
      StackNumSize--;
   }
   *(Recorre)='\0';
   return Recorre+1;
}

//*****************************************************************************************

//*****************************************************************************************
char QuitarDePilaDeOperadores(void){
   char Tmp;

   Largo=0;
   if(!StackOpeSize) return 0x00;       //Si no hay nada en la pila, devolver caracter nulo
   
   Largo=1;
   StackOpeSize--;
   Tmp=*(StackOpe+StackOpeSize);
   *(StackOpe+StackOpeSize)='\0';         //Borrar elemento
   return Tmp;                            //Devoler elemento pedido
}

//*****************************************************************************************

//*****************************************************************************************
Types RevisarPilaDeOperadores(void){
   if(StackOpeSize)return AnalizarOperador(*(StackOpe+StackOpeSize-1)); else return 0xFF;
}

//*****************************************************************************************

//*****************************************************************************************
Types AnalizarOperador(char Operador){
   char i;

   for(i=0;i<TOKEN_QUANT;i++){
         if(Operador==codig[i]){
            if(Operador==MAS || Operador==MENOS)                  return Prior2;
            if(Operador==POR || Operador==DIVI || Operador==MOD)  return Prior1;
            if(Operador==POT)                                     return Prior0;
         
            if(Operador==CIERRA)                                  return CierreParentesis;
            if(i>=PRI_ELE_CON_PAREN)                              return FuncionConParentesis;
            if(Operador==X || Operador==Y || Operador==Z)         return  Variable;
            if(Operador==E || Operador==NPI)                      return  Numero;   
         }
   }
   return Invalido;
}

//*****************************************************************************************

//*****************************************************************************************
char LeerUltimoElementoPilaDeOperadores(){
  if(StackOpeSize) return *(StackOpe+StackOpeSize-1); else return 0x00;
}

//*****************************************************************************************

//*****************************************************************************************
void strAppend(char* Str,char c){
   char* Tmp;

   Tmp=Str;
   Tmp+=strlen(Str);
   *Tmp=c;
   Tmp++;
   Tmp='\0';
}

//*****************************************************************************************

//*****************************************************************************************
void QuitarOperadorPrioridadInferior(){

   char* ptrNum;
   char ope;

   //printf("StackNum sin mod: %s\r\n",StackNum);
   ptrNum=StackNum+StackNumSize;
   while(*ptrNum!=SEPARADOR) ptrNum--;
   if(ptrNum!=StackNum) *ptrNum=ESPACIO;
   //printf("StackNum con separador 1: %s\r\n",StackNum);
   ope=QuitarDePilaDeOperadores();
   if(ope!=ABRE){
      ptrNum=StackNum+StackNumSize;
      *ptrNum=ESPACIO;
      //printf("StackNum con separador 2: %s\r\n",StackNum);
      ptrNum++;
      *ptrNum='\0';
      strAppend(StackNum,ope);
      StackNumSize+=2;
 }
   //printf("StackNum final: %s\r\n",StackNum);
}


