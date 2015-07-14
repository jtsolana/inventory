#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

// Global variables
int itemno;
char choice;

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
     printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void mainMenu();
void addItem();
void viewStocks();
int stockIn();
int getStockQty();
int main(int argc, char* argv[])
{
    mainMenu();
}

void mainMenu() {

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
      addItem();
      break;
    case '2' :
      viewStocks();
      break;
    case '3' :
      stockIn();
      break;
  default:
      goto menu;
  }
  
}

/* ADD ITEM INTO DATABASE */
void addItem() {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   char itemname[BUFSIZ];
   char price[BUFSIZ];

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
   sql = sqlite3_mprintf("INSERT INTO STOCK_ITEM (ITEM_NAME,PRICE) values ('%s','%s');", itemname,price);
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
     mainMenu();
   } else {
     system("cls");
     goto display;
   }
   
}

/* VIEW LIST OF ITEM STOCKS */
void viewStocks() {
   system("cls");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data;
   
   rc = sqlite3_open("inventory.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr," -----------------\n");
      fprintf(stderr,"| LIST OF STOCKS  |\n");
      fprintf(stderr," -----------------\n");
   }

   sql = sqlite3_mprintf("SELECT * FROM STOCK_ITEM"); 
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
      mainMenu();
      break;
    case '0' :
      exit(0);
      break;
  default:
      printf("Invalid Input\n");
      goto display;
   }
}

int getStockQty(int qty) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int con;
    int con2;
    int stock;
    char* str1;
    char str2[BUFSIZ];
    char *zErrMsg = 0;
    int num = itemno;
    str1 = "SELECT QTY FROM STOCK_ITEM WHERE ITEM_NO =";
    itoa(num, str2, 10);
    char * sql = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
    strcpy(sql, str1);
    strcat(sql, str2);
      
    con = sqlite3_open("inventory.db", &db);
    if (con != SQLITE_OK) {fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));sqlite3_close(db);return 1;}
    
    con = sqlite3_prepare_v2(db,sql, -1, &stmt, 0);    
    if (con != SQLITE_OK) {fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));sqlite3_close(db);return 1;}    
    
    con = sqlite3_step(stmt);
    
    if (con == SQLITE_ROW) {
        stock = sqlite3_column_int (stmt, 0);
        qty = stock + qty;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return(qty);
}

int stockIn() {
  system("cls");
  sqlite3 *db;
  sqlite3_stmt *stmt;
  int con;
  int res;
  int qty;
  int result;
  char *zErrMsg = 0;
  char *insert_stock_logs;
  char *update_stock_quantity;
  const char* status = "STOCK-IN";    
    
  char * sql ="SELECT ITEM_NO FROM STOCK_ITEM";
  con = sqlite3_open("inventory.db", &db);
    
  if (con != SQLITE_OK) {fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));sqlite3_close(db);return 1;}
     
  con = sqlite3_prepare_v2(db, sql, strlen(sql)+1, &stmt, NULL);
  if (con != SQLITE_OK) {printf("Failed to fetch data from database %s\n\r",sqlite3_errstr(con));sqlite3_close(db);return 1;}
  
  validate_itemno:
  printf("Input Item No.: ");
  scanf("%d",&itemno);
  do {
    con = sqlite3_step(stmt);
    if (con == SQLITE_ROW) { /* can read data */
      res = sqlite3_column_int(stmt,0);
      if(res==itemno){
        goto validate_quantity;
      } 
    }
  } while (con == SQLITE_ROW);

  printf("Item No. not found in records \n");
  goto validate_itemno;
  
  validate_quantity: 
  printf("\nInput Quantity: ");
  scanf("%d",&qty);

  insert_stock_logs = sqlite3_mprintf("INSERT INTO STOCK_LOGS (ITEM_NO,STATUS,QTY) values ('%d','%s','%d');",itemno,status,qty ); 
  con = sqlite3_exec(db, insert_stock_logs, callback, 0, &zErrMsg);
  if( con != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "STOCKS SUCCESSFULLY ADDED!");
    result = getStockQty(qty);
  }
  
  update_stock_quantity = sqlite3_mprintf("UPDATE STOCK_ITEM SET QTY = %d WHERE ITEM_NO = %d ;",result,itemno);  
  /* Execute SQL statement */
  con = sqlite3_exec(db, update_stock_quantity, callback, 0, &zErrMsg);
  if( con != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    sqlite3_close(db);
  }

  display:
   printf("\n\nWould you like to stock another item? (Y/N) ");
   scanf(" %c", &choice);
   if(choice == 'y' || choice == 'Y') {
     stockIn();
   } else if (choice == 'n' || choice == 'N') {
     mainMenu();
   } else {
     system("cls");
     goto display;
   }
    
}
