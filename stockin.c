#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "stockin.h"
#include "date.h"

static int callback(void *data, int argc, char **argv, char **azColName){};

void stockIn() {
  system("cls");
  sqlite3 *db;
  sqlite3_stmt *stmt;
  int itemno;
  int con;
  int res;
  int qty;
  int stock;
  int result;
  char choice;
  char *date = getDate();
  char *zErrMsg = 0;
  char *insert_stock_logs;
  char *update_stock_quantity;
  const char* status = "STOCK-IN";    
    
  char * sql ="SELECT ITEM_NO FROM STOCK_ITEM";
  con = sqlite3_open("inventory.db", &db);
    
  if (con != SQLITE_OK) {fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));sqlite3_close(db);}
     
  con = sqlite3_prepare_v2(db, sql, strlen(sql)+1, &stmt, NULL);
  if (con != SQLITE_OK) {printf("Failed to fetch data from database %s\n\r",sqlite3_errstr(con));sqlite3_close(db);}
  
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

  insert_stock_logs = sqlite3_mprintf("INSERT INTO STOCK_LOGS (ITEM_NO,STATUS,DATE,QTY) values ('%d','%s','%s','%d');",itemno,status,date,qty ); 
  con = sqlite3_exec(db, insert_stock_logs, callback, 0, &zErrMsg);
  if( con != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "STOCKS SUCCESSFULLY ADDED!");
  }
  stock = getStockQty(itemno);
  result = stock + qty;
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
     main();
   } else {
     system("cls");
     goto display;
   }
    
}
