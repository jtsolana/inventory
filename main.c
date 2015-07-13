#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h" 
#include <math.h>
 
void tostring(char [], int);
int toint(char []);
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
     printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void menu();
void additem();
void viewstocks();
int stockin();
int main(int argc, char* argv[])
{
   //menu();
    stockin();
}

int stockin() {

	sqlite3 *db;
    sqlite3_stmt *stmt;
    int result;
    int rc;
    int itemno;
	int qty;
	char *zErrMsg = 0;
    int rc3;
    char *insert_stock_logs;
    const char* status = "STOCK-IN";    
    
    char * sql ="SELECT ITEM_NO FROM STOCKS";
    result = sqlite3_open("inventory.db", &db);
    
    if (result != SQLITE_OK) {fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));sqlite3_close(db);return 1;}
     
    result = sqlite3_prepare_v2(db, sql, strlen(sql)+1, &stmt, NULL);
	if (result != SQLITE_OK) {printf("Failed to fetch data from database %s\n\r",sqlite3_errstr(result));sqlite3_close(db);return 1;}
	
	validate_itemno:
	printf("Input Item No.: ");
    scanf("%d",&itemno);
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			rc = sqlite3_column_int(stmt,0);
			if(rc==itemno){
				goto validate_quantity;
			} 
		}
	} while (result == SQLITE_ROW);

	printf("Item No. not found in records \n");
	goto validate_itemno;
	
	validate_quantity: 
	printf("\nInput Quantity: ");
	scanf("%d",&qty);

    execute_stockin:
    insert_stock_logs = sqlite3_mprintf("INSERT INTO STOCK_LOGS (ITEM_NO,STATUS,QTY) values ('%d','%s','%d');",itemno,status,qty ); 
    rc3 = sqlite3_exec(db, insert_stock_logs, callback, 0, &zErrMsg);
    if( rc3 != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(stdout, "STOCKS SUCCESSFULLY ADDED!");
    }
	
    sqlite3_close(db);
    return 0;
    
}

/* MAIN MENU */
void menu() {
  char choice;
  menu:
  system("cls");
  printf(" ---------------------------------- \n");
  printf("|                                  |\n");
  printf("|             INVENTORY            |\n");
  printf("|              SYSTEM              |\n");
  printf("|                                  |\n");
  printf(" ---------------------------------- \n\n\n");
  printf("[1] Item Entry\n");
  printf("[2] View Stocks\n");
  printf("[3] Stock-in\n");
  printf("[4] Stock-out\n");
  printf("[0] Exit\n\n");
  printf("Enter Choice: ");
  scanf(" %c", &choice);
  switch (choice) {
    case '1' :
      additem();
      break;
    case '2' :
      viewstocks();
      break;
    case '0' :
      exit(0);
 	default:
  	  goto menu;
  }
  
}

/* ADD ITEM INTO DATABASE */
void additem() {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   char itemname[BUFSIZ];
   char price[BUFSIZ];
   char choice;

   rc = sqlite3_open("inventory.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }
   
   inputitem:
   system("cls");
   printf("Item Name: ");
   fgets(itemname, BUFSIZ, stdin);
   itemname[strlen(itemname) - 1] = '\0';
   if (itemname[0] == '\0') {
   	 goto inputitem;
   } 
   
   inputprice: 
   printf("Price: ");
   if(fgets( price,BUFSIZ,stdin)!=NULL){
      char *chk;
      double tmp = strtod( price, &chk );
      price[strlen(price) - 1] = '\0';
      if (price[0] == '\0') {
      	 printf("Please input a valid price\n");
   	     goto inputprice;
      } 
      if ( isspace( *chk ) || *chk == 0 ) {
         goto query;
      } else {
        fprintf( stderr, "%s is not a valid valid price\n", price );
        goto inputprice;
      }
   } 
  
   query:
   sql = sqlite3_mprintf("INSERT INTO STOCKS (ITEM_NAME,PRICE) values ('%s','%s');", itemname,price);
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "ITEM SUCCESSFULLY ADDED!");
   }
   
   display:
   printf("\n\nWould you like to add another item? (Y/N) ");
   scanf(" %c", &choice);
   if(choice == 'y' || choice == 'Y') {
  	 goto inputitem;
   } else if (choice == 'n' || choice == 'N') {
  	 menu();
   } else {
   	 system("cls");
   	 goto display;
   }
   
}

/* VIEW LIST OF ITEM STOCKS */
void viewstocks() {
   system("cls");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data;
   char choice;
   
   rc = sqlite3_open("inventory.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr," -----------------\n");
      fprintf(stderr,"| LIST OF STOCKS  |\n");
      fprintf(stderr," -----------------\n");
   }

   sql = sqlite3_mprintf("SELECT * FROM STOCKS"); 
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   sqlite3_close(db);
   
   printf("------------------------\n");
   display:
   printf("[1] Back to main menu\n");
   printf("[0] Exit Program: ");
   scanf(" %c", &choice);
   switch (choice) {
    case '1' :
      menu();
      break;
    case '0' :
      exit(0);
      break;
 	default:
  	  printf("Invalid Input\n");
  	  goto display;
   }
}

