/*******************************************************
  Verificar tabela de decisão 
  (c) LES/INF/PUC-Rio    Arndt von Staa
  versão 1.0 , 2015

  O programa executa na janela de comando
  Torne corrente o diretório em que se encontra o programa

  Para executar o programa use o comando de linha:

     ValidaTabelaDecisao  <nome arquivo>

     onde <nome arquivo> é o nome do arquivo com ou sem extensão
          a extensão default é:  tbdec  - será acrescentada caso não seja fornecida

          caso o arquivo da tabela não esteja no mesmo path
          que o programa, o nome do arquivo deve determinar o path,
          neste caso procure utilizar um path relativo.

O arquivo da tabela deve ser similar ao exemplo a seguir:

// Exemplo tabela de decisão

Usuario existe  : -  t  t  -  -  t  t  t  t 
Senha valida    : -  t  t  -  -  f  f  f  t 
Captcha correto : -  t  t  f  f  t  t  f  t 
Botao Cancela   : t  f  f  f  f  f  f  f  f 
Botao Login     : -  t  f  t  f  t  f  t  f  
Botao Troca     : -  -  t  -  t  -  t  -  t 
 
===============================================

Autoriza        :    x    
Erro usuário    :                x  x
Erro captcha    :          x  x        x  
Troca senha     :       x                 x
Cancela         : x
Impossivel      :

<EOF>

  Linhas em branco e comentários são ignorados
  Linhas de comentário iniciam com "//"

  O final da parte de definição das condições é sinalizado 
  por uma linha iniciando com "==="  

  O texto após o "===" é ignorado e pode conter a lista de ações ou oráculos

  Em cada linha de condições
     Os caracteres do início da linha até o caractere ':'  identificam a condição
     Os caracteres após o ':' informam os valores a serem usados na linha,
            devem ser um de 't', 'f', 'T', 'F', ou '-'

********************************************************/

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <ctype.h>
#include    <math.h>

/* Constantes internas do m¢dulo */

#define  DIM_BUFFER        512
#define  DIM_NOME_ARQ      256
#define  MAX_NUM_DECISAO   100

#define  DIM_CONDICAO       30

#define  ERRO_GRAVE          4

#define  DIRET_EXT         ".tbdec"

#define  SEP_EXT           '.'
#define  SEP_DIR           '\\'
#define  STR_SEP_DIR       "\\"

      typedef struct 
      {
         char condicao[ DIM_CONDICAO + 1 ] ;
         char linha[ MAX_NUM_DECISAO + 1 ] ;
      } tpLinha ; /* struct */
      static int DIM_LINHA = sizeof( tpLinha ) ;

      static int contaDecisoes ;
            /* Contador de decisões lidas */

      static tpLinha tabDecisao[ MAX_NUM_DECISAO ] ;
            /* Tabela de decisao lida */

      FILE * pArqTabela ;
            /* Arquivo de onde deve ler */

/***** Protótipos das funções encapuladas no módulo *****/

   static int TemExtensao( char * NomeArq ) ;
   static int CompararColuna( int col1 , int col2 ) ;


   int main( int    numArgs   ,
             char * pArqs[ ] )
   {

      char    nomeArqTabela[ DIM_NOME_ARQ ] ;
      char    buffer[ DIM_BUFFER + 1 ]  ;
      int     tamBuffer ,
              i , j , k ;
      int     temCondicao ;
      int     col ;
      int     numCols ;
      int     contaErro ;
      int     contaFalta ;
      int     num  ;
      int     ehIgual ;
      int     maxCol ;
      tpLinha corrLinha ;

      /* Apresentar identifica‡„o */

         printf( "\n\nVerificador de tabela de decisao" ) ;
         printf(   "\n   Laboratorio de Engenharia de Software "   ) ;
         printf(   "\n   versao 1.00 (c) 2015 " ) ;
         printf(   "\n" ) ;

      /* Abrir o arquivo de diretivas */

         pArqTabela = NULL ;

         if ( numArgs >= 2 )
         {
            strcpy( nomeArqTabela , pArqs[ 1 ] ) ;
            if ( TemExtensao( nomeArqTabela ) == -1 )
            {
               strcat( nomeArqTabela , DIRET_EXT ) ;
            } /* if */

            pArqTabela = fopen( nomeArqTabela , "r" ) ;
            if ( pArqTabela == NULL )
            {
               printf( "\n%s nao abre" , nomeArqTabela ) ;
            } /* if */
         }  /* if */

         if ( pArqTabela == NULL )
         {
            printf( "\nFaltou o arquivo da tabela a verificar" ) ;
            printf( "\n   Uso: ValidaTabDecis  <arquivoTabela>.tbdec" ) ;
            return ERRO_GRAVE ;
         } /* if */

      /* Ler todas as linhas do arquivo */

         contaDecisoes = 0 ;
         fgets( buffer , DIM_BUFFER , pArqTabela ) ;

         numCols = -1 ;
     
         while ( ! feof( pArqTabela ) )
         {
         /* ler a parte da condição */

            memset( &corrLinha , 0 , DIM_LINHA ) ;
            tamBuffer = strlen( buffer ) ;

     /* controlar fim da parte de linhas de condições */
     
            if ( memcmp( buffer , "===" , 3 ) == 0 )
            {
               break ;
            } /* if */

     /* saltar o nome da condição na linha */

            for ( i = tamBuffer - 2 ; i > 0 ; i-- )
            {
               if ( buffer[ i ] != ' ' )
               {
                  break ;
               } /* if */
            } /* for */
 
            buffer[ i + 2 ] = 0 ;
            tamBuffer = i + 1 ;

            temCondicao = 0 ;

            if ( ( tamBuffer > 5 )
              && ( memcmp( buffer , "//" , 2 )) )
            {
               for ( i = 0 ; i < tamBuffer ; i++ )
               {
                   if ( buffer[ i ] != ':' )
                   {
                      corrLinha.condicao[ i ] = buffer[ i ] ;
                   } else
                   {
                      break ;
                   } /* if */
               } /* for */

               temCondicao = 1 ;
               for ( j = i - 1 ; j >= 0 ; j-- )
               {
                  if ( corrLinha.condicao[ j ] != ' ' )
                  { 
                     break ;
                  } /* if */
               } /* for */
               corrLinha.condicao[ j + 1 ] = 0 ;

            /* Ler a lista de condições da linha */
             
               col = 0 ;
               for ( ; i < tamBuffer ; i++ )
               {
                  if ( strchr( "tfTF-" , buffer[ i ] ) != NULL )
                  {
                     corrLinha.linha[ col ] = buffer[ i ] ;
                     col ++ ;
                  } /* if */
               } /* for */

            /* verificar a linha lida */

               if ( temCondicao == 0 )
               {
                  printf( "\n*** Faltou o campo de condicao na linha: %s\n   cancelado" , buffer ) ;
                  exit( ERRO_GRAVE ) ;
               } /* if */

               if ( contaDecisoes == 0 )
               {
                  numCols = col ;
               } else
               {
                  if ( numCols != col )
                  {
                     printf( "\n*** Linha nao contem %d colunas: %s\n   cancelado" , numCols , buffer ) ;
                     exit( ERRO_GRAVE ) ;
                  } /* if */
               } /* if */

               memcpy( &tabDecisao[ contaDecisoes ] , &corrLinha , DIM_LINHA ) ;
               contaDecisoes ++ ;
            } /* if */

            fgets( buffer , DIM_BUFFER , pArqTabela ) ;
         } /* while */ 

      /* Imprimir a labela lida */

         printf( "\nTabela lida do arquivo %s\n" , nomeArqTabela ) ;
         printf( "\n                                 " ) ;
         for ( k = 0 ; k < numCols ; k++)
         {
            printf( "%3d" , k + 1 ) ;
         } /* for */

         for ( i = 0 ; i < contaDecisoes ; i ++ )
         {
             printf( "\n%30s : " , tabDecisao[ i ].condicao ) ;
             for ( j = 0 ; j < numCols ; j++)
             {
                printf( "  %c" , tabDecisao[ i ].linha[ j ] ) ;
             } /* for */
         } /* for */

         printf( "\n" ) ;

      /* Verificar ambiguidade */

         contaErro = 0 ;
         for ( i = 0 ; i < numCols - 1 ; i++ )
         {
            for ( j = i + 1 ; j < numCols  ; j ++ )
            {
               if ( CompararColuna( i , j ) == 0 )
               {
                  contaErro++ ;
                  printf( "\n***   Colunas %d e %d sao logicamente iguais" , i + 1 , j + 1 ) ;
               } /* if */
            } /* for */
         } /* for */

         if ( contaErro > 0 )
         {
            printf( "\n\n*** Tabela eh ambigua" ) ;
         } /* if */

      /* verificar completeza */

         contaFalta = 0 ;

         #pragma warning(disable:4244)
         maxCol =  pow( 2.0 , contaDecisoes ) ;
         #pragma warning(default:4244)

         for ( i = 0 ; i < maxCol ; i ++ )
         {
            num = i ;
         /* gera coluna para i */
            for ( j = 0 ; j < contaDecisoes ; j ++ )
            {
               if ( ( num & 1 ) == 0 )
               {   
                  tabDecisao[ j ].linha[ MAX_NUM_DECISAO - 2 ] = 't' ;
               } else
               {
                  tabDecisao[ j ].linha[ MAX_NUM_DECISAO - 2 ] = 'f' ;
               } /* if */
               num = num / 2 ;
            } /* for */

         /* verifica se existe alguma coluna que satisfaz a gerada */   
 
            ehIgual = 0 ;
            for ( j = 0 ; j < numCols ; j++ )
            {
               if ( CompararColuna( j , MAX_NUM_DECISAO - 2 ) == 0 )
               {
                  ehIgual++ ;
                  break ;
               } /* if */
            } /* for */

            if ( ehIgual == 0 )
            {
               printf( "\n***   Condicao %3d nao eh tratada   " , i ) ;
               for ( k = 0 ; k < contaDecisoes ; k++ )
               {
                  printf( " %c" , tabDecisao[ k ].linha[ MAX_NUM_DECISAO - 2 ] ) ;
               } /* for */
               contaErro ++ ;
               contaFalta ++ ;
            } /* if */
         } /* for maxCol */

       /* Controlar o resultado final */

         if ( contaFalta > 0 )
         {
            printf( "\n\n*** Tabela esta incompleta" ) ;
         } /* if */
         if ( contaErro > 0 )
         {
            printf( "\n\n*** Tabela nao eh valida\n" ) ;
            return 4 ;
         } else
         {

         /* Imprimir oraculo */

            printf( "\n\n===============" ) ;

            fgets( buffer , DIM_BUFFER , pArqTabela ) ;
     
            while ( ! feof( pArqTabela ) )
            {
               tamBuffer = strlen( buffer ) ;

            /* imprimir linha que contém oráculo */

               for ( i = 0 ; i < tamBuffer ; i++ )
               {
                   if ( buffer[ i ] == ':' )
                   {
                      buffer[ i ] = 0 ;
                      for ( j = i - 1 ; j > 0  ; j-- )
                      {
                         if ( buffer[ j ] == ' ' )
                         {
                            buffer[ j ] = 0 ;
                         } else
                         {
                            break ;
                         } /* if */  
                      } /* for */
                      if ( buffer[ tamBuffer - 1 ] == '\x0D' || buffer[ tamBuffer - 1 ] == '\x0A' )
                      { 
                         buffer[ tamBuffer - 1 ] = 0 ;
                         if ( buffer[ tamBuffer - 2 ] == '\x0D' || buffer[ tamBuffer - 2 ] == '\x0A' )
                         { 
                            buffer[ tamBuffer - 2 ] = 0 ;
                         } /* if */
                      } /* if */
                      buffer[ tamBuffer - 1 ] = 0 ;
                      printf( "\n%30s :  %s" , buffer , buffer + i + 1 ) ;
                      break ;
                   } /* if */
               } /* for */

               fgets( buffer , DIM_BUFFER , pArqTabela ) ;

            } /* while imprimir oráculo*/ 

            fclose( pArqTabela ) ;
          
            printf( "\n\n--- Tabela eh valida\n" ) ;
            return 0 ;
         } /* if */

   } /* fim da função main */

/***********************************************************************
*
*  $FC Função: Comparar coluna 
*        return 0 - colums are logically equal,  1 are logically different
*
***********************************************************************/
   int CompararColuna( int col1 , int col2 ) 
   {
       int i ;
       for ( i = 0 ; i < contaDecisoes ; i ++ )
       {
          if ( ( tabDecisao[ i ].linha[ col1 ] == '-' )
            || ( tabDecisao[ i ].linha[ col2 ] == '-' ))
          {
          } else
          if ( tolower( tabDecisao[ i ].linha[ col1 ] ) != 
                    tolower( tabDecisao[ i ].linha[ col2 ] ))
          {   
             return 1 ;
          } /* if */
       } /* for */

       return 0 ;

   } /* fim comparar coluna */
/***********************************************************************
*
*  $FC Função: Ver se nome de arquivo tem extensão
*
***********************************************************************/

   int TemExtensao( char * nomeArq )
   {

      int encontrou ,
          i ;

      encontrou = -1 ;
      for( i = strlen( nomeArq ) - 1 ; i > 0  ; i -- )
      {
         if ( nomeArq[ i ] == SEP_EXT )
         {
            encontrou = i ;
            break ;
         } else if ( nomeArq[ i ] == SEP_DIR )
         {
            break ;
         } /* if */
      } /* for */
      return encontrou ;

   } /* Fim função: Ver se nome de arquivo tem extens„o */
