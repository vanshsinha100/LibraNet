#ifndef USERS_H
#define USERS_H

typedef enum {
    ADMIN,
    USER
} UserType;

int login(UserType *userType);
void register_user();
void change_password(const char *username);

#endif // USERS_H
