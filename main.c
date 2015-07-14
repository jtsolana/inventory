#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"
#include "date.h"
#include "add_item.h"
#include "get_quantity.h"
#include "stockin.h"
#include "viewstock.h"


int main(int argc, char* argv[]) {
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




