#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 512

typedef struct {
  char** fields; // array of string*
  char** strs;   // array of string*
  int num_fields;  // length of fields
  int num_strs;    // length of strs
  int field_cap;
  int strs_cap;
} Page;

// Caller owns returned page.
Page* alloc_page(size_t str_size) {
  Page* page = (Page*) malloc(sizeof(Page));
  page->fields = (char**) malloc(sizeof(char*) * str_size);
  page->strs = (char**) malloc(sizeof(char*) * str_size);
  page->num_fields = page->num_strs = 0;
  page->field_cap = page->strs_cap = str_size;

  return page;
}

// more readable version of below
void page_add_field_old(Page* page, char* field) {
  // if out of space, realloc
  if (page->num_fields + 1 > page->field_cap) {
    int new_size = page->field_cap * 2;
    page->fields = (char**) realloc(page->fields, new_size);
    page->field_cap = new_size;
  }
  
  // add new field
  page->fields[++page->num_fields] = field;
}

// evil C programming
// basically makes the underlying actions of adding html or field generic
void page_add_gen(char*** change, int* num, int* cap, char* field) {
  // out of space? resize
  if (*num + 1 >= *cap) {
    printf("size = %d", *num * 2);
    int new_size = *num * 2;
    *change = (char**) realloc(*change, new_size);
    *cap = new_size;
  }
  
  // add field to array
  (*change)[*num] = field;
  *num = *num + 1;
}

// add a field to a page
void page_add_field(Page* page, char* field) {
  page_add_gen(&page->fields, &page->num_fields, &page->field_cap, field);
}

// add html to a page
void page_add_html(Page* page, char* html) {
  page_add_gen(&page->strs, &page->num_strs, &page->strs_cap, html);
}

// print a page to stdout
void print_page(Page* page) {
  printf("Fields (%d/%d): [", page->num_fields, page->field_cap);
  for (int i = 0; i < page->num_fields; i++) {
    printf("%s ", page->fields[i]);
  }
  printf("], HTML (%d/%d): [", page->num_strs, page->strs_cap);
  for (int i = 0; i < page->num_strs; i++) {
    printf("%s ", page->strs[i]);
  }
  printf("]\n");
}

// Caller owns returned page.
Page* parse_page(char* text) {
  // max number of $ chars = len(text)
  // parse tokes
  char delimiters[] = "$";

  int size = strlen(text);
  Page* page = alloc_page(size);

  print_page(page);

  char* tok_ptr = strtok(text, delimiters);
  for (int i = 0; tok_ptr != NULL; i++) {
    // do something with tok_ptr
    if (i % 2 == 0) {
      // HTML state
      page_add_html(page, tok_ptr);
    } else {
      // field state
      page_add_field(page, tok_ptr);
    }
    
    tok_ptr = strtok(NULL, delimiters);
  }

  print_page(page);

  return page;
}

int main(int argc, char** argv) {
  char* total_str = (char*) calloc(sizeof(char) * BUFFER_SIZE, sizeof(char) * BUFFER_SIZE);
  char* str = (char*) malloc(sizeof(char*) * BUFFER_SIZE);
  char* ret = fgets(str, BUFFER_SIZE, stdin);
  while (ret != NULL && strlen(total_str) + 1 < BUFFER_SIZE && strchr(ret, EOF) == NULL && ret == str) {
    strcat(total_str, str);
    ret = fgets(str, BUFFER_SIZE, stdin);
  }
  // char test[] = "<p>$0$</p><p>$1$</p>";
  
  free(parse_page(total_str));
  free(total_str);
  free(str);

  return EXIT_SUCCESS;
}