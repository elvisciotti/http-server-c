/* lista delle parole della pagina php separate da spazi o \n o tag php fine/inizio */
typedef struct coda
{
  char * el;

  /* parola effettiva */
  int tag;
  /* se=1 la parola è fra tag php, se=0 no */
  struct coda * succ_p;
}
coda_t;



/* esplode l'array a (fino al terminatore) in base al separatore in un array bidimensionale che restituisce */
extern char * * esplodi( char * a, char separatore, char terminatore )
{
  char temp[255], * * b;
  int i, j, riga;

  b = ( char * * ) calloc( 5, sizeof( char * ) ); //array di puntatori
  for ( riga = 0; riga < 5; riga++ )
    b[riga] = ( char * ) calloc( 255, sizeof( char ) ); //alloco mem
  i = riga = j = 0;
  while ( a[i] != terminatore && i < strlen( a ) )
  {
    if ( a[i] != separatore ) //se carattere valido
    {
      b[riga] [j++] = a[i]; //accumula in riga
    }
    if ( a[i] == separatore || a[i] == terminatore ) //fine parola
    {
      b[riga++] [j] = '\0'; //chiude e avanza riga
      j = 0; //lettera riportata a zero per parola successiva
    }
    i++;
  }
  return b;
}

/* estrae gli ultimi 4 caratteri della stringa (ext file) */
extern char * estrai_ext( char * inputstr )
{
  char * ext;
  int len;

  ext = ( char * ) calloc( 5, sizeof( char ) );
  len = strlen( inputstr );
  if ( len < 5 ) return "";
  ext[0] = inputstr[len - 4];
  ext[1] = inputstr[len - 3];
  ext[2] = inputstr[len - 2];
  ext[3] = inputstr[len - 1];
  ext[4] = '\0';
  return ext;
}


/* rimpiazza i %20 del browser con spazi per accedere al file corretto */
extern char * rimpiazza_spazi( char * inputstr )
{
  char * copia;
  int len = strlen( inputstr );
  int i, j;

  copia = ( char * ) calloc( len, sizeof( char ) );
  for ( i = j = 0; i < len + 1; i++ )
  {
    if ( i < len - 2 && inputstr[i] == '%' && inputstr[i + 1] == '2' && inputstr[i + 2] == '0' )
    {
      copia[j++] = ' '; i += 2;
    }
    else
      copia[j++] = inputstr[i];
  }
  //  copia[j+1] = '\0';
  return copia;
}


/* data la stringa di risposta del browser, restituisce il valore delle variabili http */
/* es: Accept-Language, Referer, User-Agent, Host... */
extern char * get_http_var( char * datiricevuti, char * var )
{
  char * punt;
  char * ret;
  int i = 0;

  ret = ( char * ) calloc( 2500, sizeof( char ) );
  punt = strstr( datiricevuti, var );
  if ( punt != NULL )
  {
    punt += ( strlen( var ) + 2 );
    for ( i = 0; punt[i] != '\r'; i++ )
      ret[i] = punt[i];
    ret[i + 1] = '\0';
    return ret;
  }
  else
    return "<ERRORE>";
}


/* stampa le stringe e i tag della coda (cioè della pagina php) per controllo */
extern void attraversa_coda( coda_t * uscita_p )
{
  coda_t * punt;
  for ( punt = uscita_p; ( punt != NULL ); punt = punt->succ_p )
    printf( "%s\n", punt->el, punt->tag );
}


/* rimpiazza temp4 con temp5 nella pagina php */
/* la pagina php è nella coda */
extern void rimpiazza( coda_t * * uscita_p, char * temp4, char * temp5 )
{
  coda_t * punt; //copia del puntatore passato per indirizzo
  /* scorro la coda e rimpiazzo le variabili con i valori se sono fra tag php */
  for ( punt = * uscita_p; ( punt != NULL ); punt = punt->succ_p )
    if ( strcmp( punt->el, temp4 ) == 0 && punt->tag == 1 )
    {
      /* sostituisco solo in aree delimitate da tag php */
      punt->el = ( char * ) calloc( strlen( temp5 ) + 2, sizeof( char ) );
      strcpy( punt->el, temp5 );
    }
}

extern char * coda_in_stringa( coda_t * uscita_p )
{
  coda_t * punt; //copia del puntatore passato per indirizzo
  char * totale;

  totale = ( char * ) calloc( 15800, sizeof( char ) );
  totale[0] = '\0';

  for ( punt = uscita_p; ( punt != NULL ); punt = punt->succ_p )
  {
    if ( punt != NULL && strcmp( punt->el, "<?" ) != 0
         && strcmp( punt->el, "?>" ) != 0  && (strcmp( punt->el, "print" ) != 0 || punt->tag==0 ))
         {
           strcat( totale, punt->el );
           if (1 || strcmp( punt->el, "" ) == 0 ) strcat( totale, " " );
    }
  }


  return totale;
}


/* accoda str e crea i tag a seconda che siano delimitate da tag php */
extern void enqueue( coda_t * * ingresso_p, coda_t * * uscita_p, char * str )
{
  coda_t * nuovo_p;
  static tagphp = 0;

  nuovo_p = ( coda_t * ) malloc( sizeof( coda_t ) );
  nuovo_p->el = ( char * ) calloc( strlen( str ) + 2, sizeof( char ) );
  strcpy( nuovo_p->el, str );
  if ( strcmp( str, "<?" ) == 0 ) tagphp = 1; //inizio zona php
  else if ( strcmp( str, "?>" ) == 0 ) tagphp = 0;
  nuovo_p->tag = tagphp;
  nuovo_p->succ_p = NULL;
  if ( * ingresso_p == NULL )
    * uscita_p = nuovo_p;
  else
    ( * ingresso_p )->succ_p = nuovo_p;
  * ingresso_p = nuovo_p;
}


int dequeue(coda_t **ingresso_p, coda_t **uscita_p)
{
  int ris;
  coda_t *succ_p;
  if (*uscita_p == NULL)
    ris = 0;
  else
  {
    succ_p = (*uscita_p)->succ_p;
    free(*uscita_p);
    *uscita_p = succ_p;
    if (*uscita_p == NULL) *ingresso_p = NULL;
  }
  return (ris);
}
