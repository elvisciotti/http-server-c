#include <stdio.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h> //per timestamp file tmp php
#include <windows.h> //per la sleep
#include "funzioni.h" //personalizzate
#define BUFFER 10 /* dimensione volume buffer di invio ASCII in bytes */
#define VEL_DOWNLOAD 15/* Kbyte/sec per scaricare files, velocit� emulata */
//#define WEBROOT "c:\\webroot"
#define WEBROOT ".\\webroot"
#define ATTIVAPHP 1

#define DIMDATI 750 //dim buffer dati browser in ricezione


void main()
{

  char datiricevuti[DIMDATI];
  char datidainviare[DIMDATI];
  SOCKET Acceptsocket; //socket del server
  SOCKET serversocket;
  sockaddr_in serversockaddr; //sockaddr del server
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  int k;
  int addrlen;
  struct in_addr inaddr;
  char * * datiricevuti_spezz = NULL;
  FILE * pagina = NULL;
  FILE * log = NULL;
  char temp;
  char path[350];
  int size;
  int php = 0; //specifica se la pagina � php
  int scrivi;
  long handle;
  char * bufferinvio = NULL;
  char * ext = NULL;
  int n_passi, byte_rimasti, i, j;
  char tipo_file[35];
  int tipo_file_richiesto;
  char * lang;
  //  tm tc;
  char * timestamp;
  char * file_intero;
  char buffer[1250]; //dim max parola di pagina php
  coda_t * ingresso = NULL, * uscita = NULL; // per php
  int temp2;
  char * temp3;
  char temp4[250];
  char temp5[250];
  char temp6[250];
  char *temp7;


  //  time(&tc);

  //  printf("%d-%d-%d-", tc.tm_year, tc.tm_yday, tc.tm_hour, tc.tm_min, tc.tm_sec);

  /* 0=ascii, 1 =binario */

  //inizializzo
  wVersionRequested = MAKEWORD( 2, 2 );

  err = WSAStartup( wVersionRequested, & wsaData );
  if ( err != 0 )
  {
    {
      printf( "Esco al controllo 1\n" ); return;
    }
  }

  if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
  {
    WSACleanup();
    {
      printf( "Esco al controllo 2\n" ); return;
    }
  }
  //fine inizializzaz.

  /* riempio struct */
  serversockaddr.sin_family = AF_INET; serversockaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ); //localhost
  serversockaddr.sin_port = htons( 21000 );

  Acceptsocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); //crea socket

  //bind
  err = bind( Acceptsocket, ( sockaddr * ) & serversockaddr, sizeof( serversockaddr ) );
  if ( err != 0 ) printf( "Errore Bind\n" );

  //listen
  err = listen( Acceptsocket, 1 ); //server rimane in ascolto del client. Bloccante
  if ( err != 0 ) printf( "Errore listen\n" );

  while ( 1 )
  {
    serversocket = SOCKET_ERROR;
    while ( serversocket == SOCKET_ERROR )
      serversocket = accept( Acceptsocket, ( sockaddr * ) & serversockaddr, & addrlen );

    log = fopen( "log.txt", "a" );

    for ( i = 0; i < DIMDATI; i++ )
      datiricevuti[i] = '\0';

    recv( serversocket, datiricevuti, DIMDATI - 5, 0 );
    fprintf( log, "\n---------------------------------------------\nHo ricevuto: %s\n---- fine dati ricevuti-----",
         datiricevuti );

    datiricevuti_spezz = esplodi( datiricevuti, ' ', '\r' );
    /* if ( datiricevuti_spezz[1] [0] == '/' ) datiricevuti_spezz[1] [0] = '\\'; */


    fprintf( log, "\nComando:[%s]\nPagina:[%s]\nVersione: [%s]\n", datiricevuti_spezz[0], datiricevuti_spezz[1],
         datiricevuti_spezz[2] );

    if ( 1 || strcmp( "GET", datiricevuti_spezz[0] ) == 0 || strcmp( "POST", datiricevuti_spezz[0] ) == 0 )
    {
      if ( strcmp( datiricevuti_spezz[1], "/" ) == 0 ) strcpy( datiricevuti_spezz[1], "/index.htm" );

      sprintf( path, "%s%s", WEBROOT, rimpiazza_spazi( datiricevuti_spezz[1] ) );

      fprintf( log, "Apro il file richiesto[%s]\n", path );

      ext = ( char * ) calloc( 5, sizeof( char ) );
      ext = estrai_ext( datiricevuti_spezz[1] );

      //file binario o ascii ?
      if ( strcmp( ext, "html" ) == 0 || strcmp( ext, ".htm" ) == 0 || strcmp( ext, ".txt" ) == 0
           || strcmp( ext, ".css" ) == 0 || strcmp( ext, ".php" ) == 0 || strcmp( ext, ".log" ) == 0 )
           {
             tipo_file_richiesto = 0; //ascii
             pagina = fopen( path, "r" );
      }
      else
      {
        tipo_file_richiesto = 1; //bin
        pagina = fopen( path, "rb" );
      }

      //controllo che file sia autorizzato
      if ( strcmp( ext, ".mp3" ) == 0 || strcmp( ext, ".iso" ) == 0 )
      {
        sprintf( datidainviare, "HTTP/1.1 403 ERROR\r\n" );
        send( serversocket, datidainviare, strlen( datidainviare ), 0 );
      }
      else if ( pagina ) //se il file � stato aperto
      {
        //calcolo size in bytes leggendo il n. di char , anche per php
        if ( tipo_file_richiesto == 0 )
        {
          for ( size = 0; EOF != fgetc( pagina ); size++ );

          fseek( pagina, 0, SEEK_SET );
          // fclose( pagina );
          //  pagina = fopen( path, "r" );
        }
        //uso funziona calcola dimensione
        else if ( tipo_file_richiesto == 1 )
        {
          handle = _fileno( pagina );
          size = ( int )filelength( handle );
        }

        /* vedo se bisogna processare la pagina (se � php) */
        php = 0;
        if ( strcmp( ext, ".php" ) == 0 || strcmp( ext, ".PHP" ) == 0 )
          php = 1;

        if (ATTIVAPHP && php == 1 )
        {
          file_intero = ( char * ) calloc( size + 3, sizeof( char ) );
          // acquisisco da file
          for ( i = 0; ( file_intero[i] = fgetc( pagina ) ) != EOF; i++ );
          file_intero[i] = '\0'; //chiude stringa del file
          // metto il contenuto del file in coda (separando parole e tag inizio/fine php ) in modo da poterci
          //lavorare per sostituzione

          //          ingresso = NULL;           //file nuovo
          //          uscita = NULL;
          // while ( dequeue( & ingresso, & uscita ) );
          for ( i = j = 0; i < strlen( file_intero ) + 1; i++ )
          {
            if ( file_intero[i] == ' ' || file_intero[i] == '\n' || i == strlen( file_intero )
                 || ( file_intero[i] == '>' && file_intero[i - 1] == '?' )
                 || ( file_intero[i + 1] == '?' && file_intero[i + 2] == '>' )
                 || ( file_intero[i] == '?' && file_intero[i - 1] == '<' )
                 || ( file_intero[i + 1] == '<' && file_intero[i + 2] == '?' ) )
                 {
                   if ( !( file_intero[i] == ' ' || file_intero[i] == '\n' ) )
                     buffer[j++] = file_intero[i];
                   buffer[j] = '\0';
                   // chiude parola
                   enqueue( & ingresso, & uscita, buffer );
                   j = 0;
                   buffer[0] = '\0'; // azzera per uso futuro
            }
            else
              buffer[j++] = file_intero[i];
          }
          //free( file_intero ); //non mi serve pi�

          // rimpiazzamento variabili con i valori presi in post
          if ( strcmp( "POST", datiricevuti_spezz[0] ) == 0 )
          {
            // cerco dati inviati dal browser in post
            temp3 = strstr( datiricevuti, "\r\n\r\n" );
            if ( temp3 == NULL )
            {
              printf( "Errore post\n" ); break;
            }
            temp3 += 4; //mi sposto sui dati postati
            // cerco dimensioni dati post dette da browser
            temp2 = atoi( get_http_var( datiricevuti, "Content-Length" ) );
            temp3[temp2 + 2] = '&'; //semplifica algoritmo ricerca variabili
            //  printf( "[%s]\n\n\n\n", temp3 );
            //  temp4[0] = temp5[0] = '\0';
            // ricerco le coppie variabile=valore (separate da &) nella stringa di post del browser
            for ( i = j = 0, scrivi = 4; i < temp2 + 3; i++ )
            {
              if ( scrivi == 4 && temp3[i] == '=' )
              {
                temp4[j] = '\0'; scrivi = 5;
                j = -1;
              }
              if ( scrivi == 4 && temp3[i] != '=' ) temp4[j++] = temp3[i];
              if ( scrivi == 5 && temp3[i] == '&' )
              {
                temp5[j] = '\0'; scrivi = 4;
                sprintf( temp6, "$%s", temp4 ); //var php $
                // rimpiazzo i valori(temp5) delle variabili(temp6) nella pagina
               while (temp7=strchr(temp5,'+'))
                temp7[0]=' ';

                rimpiazza( & uscita, temp6, temp5 );
                // printf( "Rimpiazzo [%s] con [%s]\n", temp6, temp5 );
                temp4[0] = temp5[0] = '\0';
                j = 0;
              }
              if ( scrivi == 5 && temp3[i] != '&' ) temp5[j++] = temp3[i];
            }
            if ( temp3[i] == '=' ) temp4[j] = '\0';
            else
              temp4[j++] = temp3[i];
          }
          temp4[0] = temp5[0] = '\0';

          //sostituzioni di variabili predefinite php
          rimpiazza( & uscita, "$HTTP_USER_AGENT", get_http_var( datiricevuti, "User-Agent" ) );
          rimpiazza( & uscita, "$REMOTE_ADDR", get_http_var( datiricevuti, "Host" ) );
          rimpiazza( & uscita, "$HTTP_USER_LANGUAGE", get_http_var( datiricevuti, "Accept-Language" ) );

          //          attraversa_coda( uscita );

          //          free(file_intero);
          //          file_intero=(char *)calloc(5000,sizeof(char));
          file_intero = coda_in_stringa( uscita );
          size = strlen( file_intero ) /* - 2 */;
          //          printf( "%s", file_intero );
          php = 1;
        }
        /* FINE PHP */

        if ( strcmp( ext, "html" ) == 0 ) strcpy( tipo_file, "text/html" );
        else if ( strcmp( ext, ".htm" ) == 0 || strcmp( ext, ".php" ) == 0 || strcmp( ext, ".PHP" ) == 0 )
          strcpy( tipo_file, "text/html" );
        else if ( strcmp( ext, ".gif" ) == 0 ) strcpy( tipo_file, "image/gif" );
        else if ( strcmp( ext, ".jpg" ) == 0 ) strcpy( tipo_file, "image/jpeg" );
         else if ( strcmp( ext, ".asf" ) == 0 ) strcpy( tipo_file, "video/basic" );
        else
          strcpy( tipo_file, "unknow" );

        for ( i = 0; i < DIMDATI; i++ )
          datidainviare[i] = '\0';

        sprintf( datidainviare, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", tipo_file, size );
        fprintf( log, "---- Invio al browser: ----\n%s", datidainviare );
        send( serversocket, datidainviare, strlen( datidainviare ), 0 );

        if ( php != 1 )
        {
          bufferinvio = ( char * ) calloc( BUFFER, sizeof( char ) );
          n_passi = ( int )size / BUFFER;
          byte_rimasti = size % BUFFER;

          for ( i = 0; i < n_passi; i++ ) // passi
          {
            for ( j = 0; j < BUFFER; j++ ) //buffer interi di invio
                   bufferinvio[j] = fgetc( pagina );
            send( serversocket, bufferinvio, BUFFER, 0 ); //invio
            Sleep( ( int )BUFFER * 1000 / VEL_DOWNLOAD / 1024 ); //emula velocit� rete
          }
          for ( i = 0; i < byte_rimasti; i++ ) //resto
                 bufferinvio[i] = fgetc( pagina );
          //      bufferinvio[byte_rimasti] = '\0';
          send( serversocket, bufferinvio, byte_rimasti, 0 );
          //free( bufferinvio );
        }
        if ( php == 1 ) //file php
        {
          send( serversocket, file_intero, size, 0 );
          free( file_intero );

        }

        //fine dati
        strcpy( datidainviare, "\r\n\r\n" );
        send( serversocket, datidainviare, strlen( datidainviare ), 0 ); //fine
        fclose( pagina );
        fprintf( log, "dati omessi[...]\nFine invio\n" );
        while ( dequeue( & ingresso, & uscita ) );
      }
      else
      {
        fprintf( log, "Non riesco a aprire il file %s\n", path );
        sprintf( datidainviare, "HTTP/1.1 404 ERROR\r\n" );
        send( serversocket, datidainviare, strlen( datidainviare ), 0 );
      }
    }
    else
      fprintf( log, "Comando browser non riconosciuto %s\n", path );

    fclose( log );



    closesocket( serversocket );
  }
  closesocket( Acceptsocket );

  WSACleanup();
}
