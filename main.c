#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h" 

static int callback();
void additem();

int main(int argc, char* argv[])
{
   additem();
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void additem() {

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   char itemname[BUFSIZ];
   char price[BUFSIZ];
   double result = 0.0;
   char choice[BUFSIZ];
   char yes[] = "Y";
   char no[] = "N";

   /* Open database */
   rc = sqlite3_open("stocks.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }
   
  additem:
  system("cls");
  printf("Item Name: ");
  fgets(itemname, BUFSIZ, stdin);

  inputprice:	
  printf("Price: ");
  if ( fgets( price, BUFSIZ, stdin ) != NULL )
	{
	  char *chk;
	  double tmp = strtod( price, &chk );
	  if ( isspace( *chk ) || *chk == 0 ) {
	     goto query;
	  } else {
	  	fprintf( stderr, "%s is not a valid floating-point number\n", price );
	    goto inputprice;
	  }
	    
	}
    
   /* Create SQL statement */
  query:
  sql = sqlite3_mprintf("INSERT INTO STOCKS (ITEMNAME, PRICE) values ('%s','%s');", itemname,price);
  
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "ITEM SUCCESSFULLY ADDED! \n");
   }
   
  display:
  printf("\n\nWould you like to add another item? (Y/N) ");
  fflush(stdout);
  if (fgets(choice, sizeof(choice), stdin)) {
    char *p = strchr(choice, '\n');
    if (p) *p = '\0';
    if (strcmp(yes, choice) == 0) {
    	system("cls");
        goto additem;
	}
    else if(strcmp(no, choice) == 0) {
    	sqlite3_close(db);
   		return 0;
    	//inventory();
	} else {
		printf("Invalid Input!");
		goto display;
	}
  }
    
}
