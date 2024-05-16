#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 30
#define MAX_GAMES_PER_USER 20
#define MAX_NAME_LENGTH 40
#define MAX_FILE_NAME_LENGTH 40
#define MAX_LINE_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int rating;
} Game;

typedef struct {
    char username[MAX_NAME_LENGTH];
    Game games[MAX_GAMES_PER_USER];
    int num_games;
} User;

User users[MAX_USERS];
int num_users = 0;
char filename[MAX_FILE_NAME_LENGTH];

void read_data_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File does not exist.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    User *current_user = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character from the line
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) {
            continue;
        }

        // If the line does not contain a space, it is a username
        if (strchr(line, ' ') == NULL) {
            if (num_users >= MAX_USERS) {
                printf("Max number of users reached. Cannot add more users.\n");
                break;
            }
            strcpy(users[num_users].username, line);
            users[num_users].num_games = 0;
            current_user = &users[num_users];
            num_users++;
        } else {
            if (current_user == NULL) {
                printf("Error: game entry without a user.\n");
                continue;
            }
            if (current_user->num_games >= MAX_GAMES_PER_USER) {
                printf("Max number of games reached for user %s. Cannot add more games.\n", current_user->username);
                continue;
            }
            char game_name[MAX_NAME_LENGTH];
            int rating;
            sscanf(line, "%s %d", game_name, &rating);
            strcpy(current_user->games[current_user->num_games].name, game_name);
            current_user->games[current_user->num_games].rating = rating;
            current_user->num_games++;
        }
    }

    fclose(file);
}



void write_data_to_file() {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
     for (int i = 0; i < num_users; i++) {
        fprintf(file, "%s\n", users[i].username);  
        for (int j = 0; j < users[i].num_games; j++) {
            fprintf(file, "%s %d\n", users[i].games[j].name, users[i].games[j].rating);  
        }
        if (i < num_users - 1) {
            fprintf(file, "\n");  
        }
    }

    

    fclose(file);
}

void start_menu();
void admin_menu();
void user_menu(User *user);
void add_user();
void remove_user();
void print_all_users();
void print_all_users_and_ratings();
void print_games(User *user);


int main() {
    printf("Welcome to board game ratings.\n");

    printf("Enter the file name: ");
    scanf("%s", filename);

    read_data_from_file(filename);

    start_menu();

    write_data_to_file();

    printf("Goodbye!\n");

    return 0;
}
int find_user_index(const char *username) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}
void user_menu(User *user) {
    while (1) {
        printf("\n%s's board games\n", user->username);
        printf("1) Print games\n");
        printf("2) Add game\n");
        printf("3) Search games\n");
        printf("4) Remove game\n");
        printf("5) Exit\n");
        printf("Select: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            printf("\nGames for %s:\n", user->username);
            if (user->num_games == 0) {
                printf("No games registered\n");
                return;
            }
            for (int i = 0; i < user->num_games; i++) {
                printf("%s: %d\n", user->games[i].name, user->games[i].rating);
            }
        } else if (choice == 2) {
            if (user->num_games >= MAX_GAMES_PER_USER) {
                printf("Your game register is full.\n");
                continue;
            }
            char game_name[MAX_NAME_LENGTH];
            int rating;
            printf("Register new boardgame (q to quit): ");
            scanf("%s", game_name);
            if (strcmp(game_name, "q") == 0) continue;
            int game_exists = 0;
            for (int i = 0; i < user->num_games; i++) {
                if (strcmp(user->games[i].name, game_name) == 0) {
                    game_exists = 1;
                    break;
                }
            }
            if (game_exists) {
                printf("Board game already added.\n");
                continue;
            }
            while (1) {
                printf("Rating (1-10): ");
                scanf("%d", &rating);
                if (rating >= 1 && rating <= 10) break;
                printf("Illegal value!\n");
            }
            strcpy(user->games[user->num_games].name, game_name);
            user->games[user->num_games].rating = rating;
            user->num_games++;
        } else if (choice == 3) {
            char search_term[MAX_NAME_LENGTH];
            printf("Search (q to quit): ");
            scanf("%s", search_term);
            if (strcmp(search_term, "q") == 0) continue;
            printf("\nSearch results:\n");
            for (int i = 0; i < user->num_games; i++) {
                if (strstr(user->games[i].name, search_term) != NULL) {
                    printf("%s: %d\n", user->games[i].name, user->games[i].rating);
                }
            }
        } else if (choice == 4) {
            if (user->num_games == 0) {
                printf("No games registered.\n");
                continue;
            }
            char search_term[MAX_NAME_LENGTH];
            printf("Search boardgame to remove (q to quit): ");
            scanf("%s", search_term);
            if (strcmp(search_term, "q") == 0) continue;
            int found_index = -1;
            for (int i = 0; i < user->num_games; i++) {
                if (strstr(user->games[i].name, search_term) != NULL) {
                    if (found_index == -1) {
                        found_index = i;
                    } else {
                        found_index = -2;
                        break;
                    }
                }
            }
            if (found_index == -1) {
                printf("No matching board game found.\n");
            } else if (found_index == -2) {
                printf("You did not find one unique board game.\n");
            } else {
                printf("%s: %d\n", user->games[found_index].name, user->games[found_index].rating);
                printf("Do you want to remove this game (y/n): ");
                char confirmation;
                scanf(" %c", &confirmation); 
                if (confirmation == 'y' || confirmation == 'Y') {
                    for (int i = found_index; i < user->num_games - 1; i++) {
                        user->games[i] = user->games[i + 1];
                    }
                    user->num_games--;
                    printf("Game removed.\n");
                } else {
                    printf("Removal cancelled.\n");
                }
            }
        } else if (choice == 5) {
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
}


void start_menu() {
    char choice[MAX_NAME_LENGTH];

    while (1) {
        printf("Please enter username, admin, or quit: ");
        scanf("%s", choice);

       
        
        if (strcmp(choice, "admin") == 0) {
            admin_menu();
        } else if (strcmp(choice, "quit") == 0) {
            write_data_to_file();
            break;
        } else {
            int user_index = find_user_index(choice);
            if (user_index != -1) {
                user_menu(&users[user_index]);
            } else {
                printf("User does not exist.\n");
            }
    }
}
}



void admin_menu() {
    int admin_choice;
    while (1) {
        printf("\nAdministration\n");
        printf("Choose:\n");
        printf("1) Add user\n");
        printf("2) Remove user\n");
        printf("3) Print all users\n");
        printf("4) Print all users and all their ratings\n");
        printf("5) Exit\n");

        printf("Please enter your choice: ");
        scanf("%d", &admin_choice);

        switch (admin_choice) {
            case 1:
                add_user();
                break;
            case 2:
                remove_user();
                break;
            case 3:
                print_all_users();
                break;
            case 4:
                print_all_users_and_ratings();
                break;
            case 5:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void add_user() {
    char new_username[MAX_NAME_LENGTH];

    while (1) {
        printf("Register new user (q to quit): ");
        scanf("%s", new_username);

        if (strcmp(new_username, "q") == 0) {
            return;
        }

        int user_exists = 0;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(users[i].username, new_username) == 0) {
                user_exists = 1;
                break;
            }
        }

        if (user_exists) {
            printf("Username exists! Please choose another.\n");
        } else if (num_users >= MAX_USERS) {
            printf("The register is full!\n");
            return;
        } else {
            strcpy(users[num_users].username, new_username);
            users[num_users].num_games = 0;
            num_users++;
            printf("User added successfully.\n");
            return;
        }
    }
}

void remove_user() {
    char user_to_remove[MAX_NAME_LENGTH];
    char confirm;

    while (1) {
        printf("Remove user (q to quit): ");
        scanf("%s", user_to_remove);

        if (strcmp(user_to_remove, "q") == 0) {
            return;
        }

        int user_index = -1;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(users[i].username, user_to_remove) == 0) {
                user_index = i;
                break;
            }
        }

        if (user_index == -1) {
            printf("User does not exist! Please choose another.\n");
        } else {
            printf("Warning: User has rated games.\n");
            printf("Do you still want to remove %s (y/n)? ", user_to_remove);
            scanf(" %c", &confirm);

            if (confirm == 'y' || confirm == 'Y') {
                for (int i = user_index; i < num_users - 1; i++) {
                    strcpy(users[i].username, users[i + 1].username);
                    users[i].num_games = users[i + 1].num_games;
                    for (int j = 0; j < users[i + 1].num_games; j++) {
                        strcpy(users[i].games[j].name, users[i + 1].games[j].name);
                        users[i].games[j].rating = users[i + 1].games[j].rating;
                    }
                }
                num_users--;
                printf("Removed.\n");
            } else {
                printf("Operation canceled.\n");
            }

            return;
        }
    }
}

void print_all_users() {
    if (num_users == 0) {
        printf("No users registered.\n");
        return;
    }

    printf("Users:\n");
    printf("________________________________________\n");
    for (int i = 0; i < num_users; i++) {
        printf("%s\n", users[i].username);
    }
}

void print_all_users_and_ratings() {
    if (num_users == 0) {
        printf("No users registered.\n");
        return;
    }

    printf("Users and board games:\n");
    printf("_____________________________________________________________\n");
    printf("| %-10s | %-30s | %-8s |\n", "User", "Board Game", "Rating");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < users[i].num_games; j++) {
            printf("| %-10s | %-30s | %-8d |\n", users[i].username, users[i].games[j].name, users[i].games[j].rating);
        }
    }
    printf("-------------------------------------------------------------\n");
}

void print_games(User *user) {
    if (user->num_games == 0) {
        printf("No games registered\n");
        return;
    }
    for (int i = 0; i < user->num_games; i++) {
        printf("%s: %d\n", user->games[i].name, user->games[i].rating);
    }
}



  

