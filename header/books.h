#ifndef BOOKS_H
#define BOOKS_H

typedef struct {
    char title[100];
    char author[100];
    char isbn[20];
    int copies;
} Book;


void add_book(const char *isbn, const char *title, const char *author, int copies, char *response);
void delete_book(const char *isbn, char *response);
void modify_book(const char *isbn, const char *title, const char *author, int copies, char *response);
void search_book(const char *isbn, char *response);
void rent_book(const char *isbn, char *response);
int lock_file_read(int fd, char *response);
int lock_file_write(int fd, char *response);
int unlock_file(int fd, char *response);

#endif // BOOKS_H
